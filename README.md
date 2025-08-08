## Introduction

This demo project is the integration of Infineon's [Imagimob-ready model deployment for PSoC&trade; 6](https://github.com/Infineon/mtb-example-ml-imagimob-deploy-ready-model/tree/release-v1.0.0)
and Avnet's [/IOTCONNECT ModusToolbox&trade; Basic Sample](https://github.com/avnet-iotconnect/avnet-iotc-mtb-basic-example/tree/release-v7.0.2). 
It demonstrates the ModusToolbox&trade; Machine Learning (MTBML) with Imagimob ready models 
using sensor data to evaluate the following:

* Pulse-code modulation (PDM/PCM) microphone audio detection:
  * Siren
  * Baby Cry
  * Alarm
  * Cough
  * Snore
* Gesture detection using the radar sensor (only on CY8CKIT-062S2-AI):
  * Push
  * Swipe Up
  * Swipe Down
  * Swipe Left
  * Swipe Right

While the gesture model detection is built into a single model, audio detection requires a corresponding model to be built,
hence only one sound can be detected at a time, depending on the compile-time setting.

The project supports Eclipse and VSCode with GCC_ARM compiler.

## QuickStart Guide

To quickly evaluate this project without compiling code follow the step-by-step instructions in the [QuickStart Guide](QUICKSTART.md).

## Supported Toolchains (make variable 'TOOLCHAIN')

* GNU Arm® Embedded Compiler (GCC_ARM) Version 11.3.1 (14 should work as well) - Default value of TOOLCHAIN

> [!IMPORTANT]
> As of 8/8/2025 the older version v1.0.0 of this application and older will not work with any MTB Tools versions 
> Due to a problem with incompatibility with retarget-io and the latest BSP.
> Please use v1.1.0 and later versions.

## Supported Boards

The code has been developed and tested with MTB 3.5, with VsCode, and the board(s) below:

### Supported kits (make variable 'TARGET')

- [PSoC&trade; 6 AI Evaluation Kit](https://www.infineon.com/CY8CKIT-062S2-AI) (`CY8CKIT-062S2-AI`) – Default value of `TARGET`
- [PSoC&trade; 62S2 Wi-Fi Bluetooth&reg; Pioneer Kit](https://www.infineon.com/CY8CKIT-062S2-43012) (`CY8CKIT-062S2-43012`) **audio models only**

## Building the Project

[Watch an overview video](https://saleshosted.z13.web.core.windows.net/media/ifx/videos/IFX%20Modus%20with%20IoTConnect.mp4) of creating a new project with /IOTCONNECT in ModusToolbox&trade; then follow the steps below.

To build the project, please refer to the 
[/IOTCONNECT ModusToolbox&trade; Basic Sample Developer Guide](https://github.com/avnet-iotconnect/avnet-iotc-mtb-basic-example/tree/release-v7.0.1/DEVELOPER_GUIDE.md) 
and note the following:
- Once ModusToolbox has been installed, the [ModusToolbox&trade; for Machine Learning](https://softwaretools.infineon.com/tools/com.ifx.tb.tool.modustoolboxpackmachinelearning) software should be installed as well.
- Modify the [Makefile](Makefile#L166) ```MODEL_SELECTION``` variable to use the desired model.
- Over-the-air updates are not currently supported.
- Use the [psoc6airm-device-template.json Device Template](https://raw.githubusercontent.com/avnet-iotconnect/avnet-iotc-mtb-ai-baby-monitor/main/files/psoc6airm-device-template.json) instead of the Basic Sample's template.
  **Note:** Right-click the link and select "Save Link As" to download the file.


## Running the Demo

For audio models, once the board connects to /IOTCONNECT, it will start processing microphone input and attempt to detect the corresponding sound. 
This can be tested by placing the board in such way so that the microphone close to the PC speaker.
For best results, the microphone should be placed very close and pointed directly towards the speaker.

The following YouTube sound clips can be used for testing (sorted by recognition quality):
  * [Siren](https://www.youtube.com/watch?v=s5bwBS27A1g)
  * [Alarm](https://www.youtube.com/watch?v=hFIJaB6kVzk)
  * [Snore](https://www.youtube.com/watch?v=dXKCapH-vx8)
  * [Cough](https://www.youtube.com/watch?v=Qp09X74kjBc)
  * [Baby Cry](https://www.youtube.com/watch?v=Rwj1_eWltJQ&t=227s)

For radar models, please refer to the [Operation Section](https://github.com/Infineon/mtb-example-ml-imagimob-deploy-ready-model/blob/master/README.md#operation) of the original Infineon's project,
and see the gesture animations at the bottom of the section.
Note that for best detection, the 062S2-AI board **should not be placed on, or near a flat surface**. The surface
behind the board will likely interfere with the sensor. The best way to test gestures on this board
is to hold the board by the USB cable, like shown in the animations. 

When the appropriate sound or gesture is recognized in-between telemetry reporting events,
the *class* telemetry value will be reported as a string with the name of the last detected class (label).

To make it easier to observe instant gesture detections in a user interface, the last gesture will "linger"
for some time even when no detection occurs. This application behavior can be controlled with the
*set-linger-interval* command (see commands below). 

One can also supply a shorter interval (eg. 500 ms) as a parameter to the **set-reporting-interval** command 
at runtime to increase the reporting interval. this is especially useful for gesture detections.

The following commands can be sent to the device using the /IOTCONNECT Web UI:

| Command                  | Argument Type     | Description                                                                                                                                                                 |
|:-------------------------|-------------------|:----------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `board-user-led`         | String (on/off)   | Turn the board LED on or off                                                                                                                                                |
| `set-reporting-interval` | Number (eg. 4000) | Set telemetry reporting interval in milliseconds.  By default, the application will report gestures every 1000ms and Audio every 2500ms                                     |
| `set-linger-interval`    | Number (eg. 4000) | Set linger interval in milliseconds. By default, the gestures will linger for 5 seconds and audio detection will not linger. Set to 1 if you wish to disable this behavior. |
| `demo-mode`              | String (on/off)   | Enable demo mode. In this mode the application will send telemetry to /IOTCONNECT for a longer period                                                                        |


## Other /IOTCONNECT-enabled Infineon Kits
See the list [here](https://avnet-iotconnect.github.io/partners/infineon/)
