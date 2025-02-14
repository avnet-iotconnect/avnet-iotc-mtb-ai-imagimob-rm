/******************************************************************************
* File Name:   audio.c
*
* Description: This file implements the interface with the PDM, as
*              well as the PDM ISR to feed the pre-processor.
*
* Related Document: See README.md
*
*******************************************************************************
* Copyright 2024, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/

#include "audio.h"
#include <time.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/*******************************************************************************
* Macros
********************************************************************************/
/* Define how many samples in a frame */
#define FRAME_SIZE                  (1024)

/* Noise threshold hysteresis */
#define THRESHOLD_HYSTERESIS        3u

/* Volume ratio for noise and print purposes */
#define VOLUME_RATIO                (4*FRAME_SIZE)

/* Desired sample rate. Typical values: 8/16/22.05/32/44.1/48kHz */
#define SAMPLE_RATE_HZ              16000u

/* Decimation Rate of the PDM/PCM block. Typical value is 64 */
#define DECIMATION_RATE             64u

/* Audio Subsystem Clock. Typical values depends on the desire sample rate:
- 8/16/48kHz    : 24.576 MHz
- 22.05/44.1kHz : 22.579 MHz */
#define AUDIO_SYS_CLOCK_HZ          24576000u

/* PDM/PCM Pins */
#define PDM_DATA                    P10_5
#define PDM_CLK                     P10_4

/* RTOS tasks */
#define AUDIO_TASK_NAME                      "audio_task"
#define AUDIO_TASK_STACK_SIZE                (2048) // Use fixed size instead of (configMINIMAL_STACK_SIZE * 10)
#define AUDIO_TASK_PRIORITY                  (2) // Use low priority instead of (configMAX_PRIORITIES - 1)

/*******************************************************************************
* Function Prototypes
********************************************************************************/
void pdm_pcm_isr_handler(void *arg, cyhal_pdm_pcm_event_t event);
void clock_init(void);

/*******************************************************************************
* Global Variables
********************************************************************************/
/* Interrupt flags */
volatile bool pdm_pcm_flag = true;
volatile long tick1 = 0;

/* HAL Object */
cyhal_pdm_pcm_t pdm_pcm;
cyhal_clock_t   audio_clock;

/* Model Output variable */
int data_out[IMAI_DATA_OUT_COUNT] = {0};
static const char* LABELS[IMAI_DATA_OUT_COUNT] = IMAI_SYMBOL_MAP;

/* Task handler */
static TaskHandle_t audio_task_handler;

/* Detection tracking */
static bool was_ever_detected = false;

const char* get_last_detected_label(void) {
    const char* ret = was_ever_detected ? LABELS[1] : NULL;
    was_ever_detected = false; // Unless the next detection triggers at some time...
    return ret;
}

/*******************************************************************************
* Function Name: audio_init
********************************************************************************
* Summary:
*    A function used to initialize and configure the PDM based on the shield
*    selected in the Makefile. Starts an asynchronous read which triggers an
*    interrupt when completed.
*
* Parameters:
*   None
*
* Return:
*     The status of the initialization.
*
*
*******************************************************************************/
cy_rslt_t audio_init(void)
{
    /* HAL PDM Configuration */
    const cyhal_pdm_pcm_cfg_t pdm_pcm_cfg =
    {
        .sample_rate     = SAMPLE_RATE_HZ,
        .decimation_rate = DECIMATION_RATE,
        .mode            = CYHAL_PDM_PCM_MODE_LEFT,
        .word_length     = 16,  /* bits */
        .left_gain       = 0,   /* dB */
        .right_gain      = 0,   /* dB */
    };

    /* Init the clocks */
    clock_init();

 
    /* Initialize the PDM/PCM block */
    cyhal_pdm_pcm_init(&pdm_pcm, PDM_DATA, PDM_CLK, &audio_clock, &pdm_pcm_cfg);
    cyhal_pdm_pcm_register_callback(&pdm_pcm, pdm_pcm_isr_handler, NULL);
    cyhal_pdm_pcm_enable_event(&pdm_pcm, CYHAL_PDM_PCM_ASYNC_COMPLETE, CYHAL_ISR_PRIORITY_DEFAULT, true);
    cyhal_pdm_pcm_start(&pdm_pcm);


    /* Initialize Imagimob pre-processing library */
    IMAI_AED_init();
    return 0;
}


/*******************************************************************************
* Function Name: audio_task
********************************************************************************
* Summary:
* This is the main task.
*    1. Initializes the PDM/PCM block.
*    2. Wait for the frame data available for process.
*    3. Runs the model and provides the result.
* Parameters:
*  pvParameters : unused
*
* Return:
*  none
*
*******************************************************************************/
void audio_task(void *pvParameters)
{
    cy_rslt_t rslt;

    rslt = audio_init();
    if(rslt != 0)
    {
        CY_ASSERT(0);
    }
    int16_t  audio_frame[FRAME_SIZE] = {0};
    for(;;)
    {
        /* Check if any microphone has data to process */
        if (pdm_pcm_flag)
        {
            /* Clear the PDM/PCM flag */
            pdm_pcm_flag = 0;

            /* Calculate the volume by summing the absolute value of all the
             * audio data from a frame */
            for (uint32_t index = 0; index < FRAME_SIZE; index++)
            {

                /*convert int to float*/
                int16_t val_temp = audio_frame[index];
                float data_in = ((float) val_temp) / 32767.0f;

                /* scale for multiply the audio value */
                float scale_factor = 20.0f;

                data_in = data_in*scale_factor;

                if (data_in > 1.0)
                {
                    data_in = 1.0f;
                }
                if (data_in < -1.0)
                {
                   data_in = -1.0f;
                }

                /* Pass audio data for enqueue */
                IMAI_AED_enqueue(&data_in);

                if (IMAI_AED_dequeue(data_out) == IMAI_RET_SUCCESS)
                {                    
                    if (data_out[1] == 1)
                    {
                        /* print triggered class and the triggered time since IMAI Initial. */
                        printf("Detected %s\r\n", LABELS[1]);
                        was_ever_detected = true;
                    }
                }
            }
        } else {
            taskYIELD()
        }
                   
        /* Setup to read the next frame */
        cyhal_pdm_pcm_read_async(&pdm_pcm, audio_frame, FRAME_SIZE);
    }

}


/*******************************************************************************
 * Function Name: create_audio_task
 ********************************************************************************
 * Summary:
 *  Function that creates the audio task.
 *
 * Parameters:
 *  None
 *
 * Return:
 *  CY_RSLT_SUCCESS upon successful creation of the radar sensor task, else a
 *  non-zero value that indicates the error.
 *
 *******************************************************************************/
cy_rslt_t create_audio_task(void)
{
    BaseType_t status;
    printf("****************** IMAGIMOB Ready Model %s Code Example ****************** \r\n\n", LABELS[1]);

    /* Create the RTOS task */
    status = xTaskCreate(audio_task, AUDIO_TASK_NAME, AUDIO_TASK_STACK_SIZE, NULL, AUDIO_TASK_PRIORITY, &audio_task_handler);


    return (pdPASS == status) ? CY_RSLT_SUCCESS : (cy_rslt_t) status;
}


/*******************************************************************************
* Function Name: pdm_pcm_isr_handler
********************************************************************************
* Summary:
*  PDM/PCM ISR handler. A flag is set when the interrupt is generated.
*
* Parameters:
*  arg: not used
*  event: event that occurred
*
*******************************************************************************/
void pdm_pcm_isr_handler(void *arg, cyhal_pdm_pcm_event_t event)
{
    (void) arg;
    (void) event;

    /* when the flag is set, data from the sensor is read in the main function */
    pdm_pcm_flag = true;
}


/*******************************************************************************
* Function Name: clock_init
********************************************************************************
* Summary:
*    A function used to initialize and configure PDM clocks.
*
* Parameters:
*   None
*
* Return:
*     None
*
*
*******************************************************************************/
void clock_init(void)
{
    /* HAL Object */
    cyhal_clock_t   pll_clock;

    /* Initialize the PLL */
    cyhal_clock_reserve(&pll_clock, &CYHAL_CLOCK_PLL[1]);
    cyhal_clock_set_frequency(&pll_clock, AUDIO_SYS_CLOCK_HZ, NULL);
    cyhal_clock_set_enabled(&pll_clock, true, true);

    /* Initialize the audio subsystem clock (CLK_HF[1])
    * The CLK_HF[1] is the root clock for the I2S and PDM/PCM blocks */
    cyhal_clock_reserve(&audio_clock, &CYHAL_CLOCK_HF[1]);

    /* Source the audio subsystem clock from PLL */
    cyhal_clock_set_source(&audio_clock, &pll_clock);
    cyhal_clock_set_enabled(&audio_clock, true, true);
}
