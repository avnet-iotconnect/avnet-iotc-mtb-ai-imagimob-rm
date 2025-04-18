/******************************************************************************
* File Name:   audio.h
*
* Description: This file contains the function prototypes and variables
*              used in audio.c.
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

#ifndef AUDIO_H_
#define AUDIO_H_

#include <stdlib.h>
#include "cybsp.h"
#include "cyhal.h"
#include "cy_result.h"
#ifdef COUGH_MODEL
#include "cough_lib.h"
#endif
#ifdef ALARM_MODEL
#include "alarm_siren_lib.h"
#endif
#ifdef BABYCRY_MODEL
#include "babycry_lib.h"
#endif
#ifdef SIREN_MODEL
#include "siren_lib.h"
#endif
#ifdef SNORE_MODEL
#include "snore_lib.h"
#endif

#include "stdio.h"


/*******************************************************************************
* Function Prototypes
********************************************************************************/
cy_rslt_t create_audio_task(void);

/* Returns the detected label/class. NULL if nothing was detected */
const char* get_last_detected_label(void);

#endif /* AUDIO_H_ */
