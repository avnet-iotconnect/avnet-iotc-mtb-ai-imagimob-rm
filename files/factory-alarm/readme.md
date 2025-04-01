# Smart Manufacturing: Siren and Alarm Detection with DEEPCRAFT™ and /IOTCONNECT (Acoustic)

## Background
In large manufacturing environments, alarms can be triggered by various sources such as machinery malfunctions or emergency situations. The DEEPCRAFT™ Ready Model continuously listens and accurately identifies these alarms, automatically triggering cloud-based responses via the IOTCONNECT platform. When an alarm is detected, the system immediately activates visual alerts (e.g., red LEDs) throughout the facility and notifies the centralized control center through real-time dashboard updates.

The control center receives instant updates through IOTCONNECT transformation widgets indicating active alarms. Concurrently, IOTCONNECT logs events, triggers UI alerts, and notifies designated personnel via email, SMS, or other push notification mechanisms. Once the issue is resolved, the control center can centrally deactivate alarms using the dashboard interface, disabling the alarm system and turning off visual indicators.
![Factory Alarm Dashboard](../factory-alarm/factory_alarm_dashboard.png)
## Getting Started

1. Follow the onboarding steps outlined in the [Quickstart Guide](https://github.com/avnet-iotconnect/avnet-iotc-mtb-ai-imagimob-rm/blob/main/QUICKSTART.md) to ensure your device is properly set up.
2. Import the Factory Alarm dashboard (`dashboard.json`) from this folder into your IOTCONNECT environment.
3. Configure IOTCONNECT device rules to automate notifications and control device responses.
4. Validate and test your setup using Infineon's IM69D130 MEMS microphone to ensure accurate detection and response to alarms.

For additional information or troubleshooting, please refer to the main [Avnet IOTCONNECT Imagimob AI Examples README](../README.md).

## Technical Specifications
- **Audio Event Detection**: Measures alarm and siren sounds longer than 1.2 seconds.
- **Detection Accuracy**: Detects approximately 90% of alarms and sirens in factory environments.
- **Signal-to-Noise Ratio (SNR)**: Minimum 10 dB required.
- **Robustness**: Designed to be effective across varying distances and angles from sound sources, and against diverse factory background noises.

## Model Specifications
- **Platform Optimization**: Optimized for real-time operation on Infineon PSOC™ 6.
- **Memory Footprint**: RAM: 27 kB, Flash: 32 kB.
- **Inference Time**: 144 ms every 800 ms.
- **Audio Data Specs**: Capable of processing sound data at 16000 Hz sample rate, 1-channel mono, 16-bit bit depth.

## Possible Use Case
This model is ideal for alerting workers who might be wearing headphones or protective gear, ensuring they are promptly notified when an alarm sounds.

## IOTCONNECT Features
- **Real-time Notifications**: Immediate alerts when alarms are detected.
- **Centralized Device Management**: Remote activation or deactivation of alarms.
- **Event Logging**: Comprehensive tracking of device statuses, alarm events, and activities.
- **Cloud-Based Rules**: Automated response triggers upon alarm detection for effective incident management.

## From Sound to Action: Infineon XENSIV MEMS Microphone
The DEEPCRAFT™ acoustic model utilizes Infineon's IM69D130 MEMS Microphone, providing:
- High sensitivity and low self-noise
- Exceptional far-field and directional performance
- High dynamic range for accurate sound detection

This integration ensures reliable acoustic detection performance, critical for effective alarm and siren identification in industrial environments.

## IOTCONNECT Device Rules
Set up IOTCONNECT device rules to automate actions and notifications:
- **Device Commands**: Remotely control alarms and visual indicators (e.g., LED activation).
- **Notification Types**: Configure notifications via email, push notifications, UI alerts, or webhooks based on detected alarms.

## Getting Started

1. Follow the onboarding steps outlined in the [Quickstart Guide](https://github.com/avnet-iotconnect/avnet-iotc-mtb-ai-imagimob-rm/blob/main/QUICKSTART.md) to ensure your device is properly set up.
2. Import the Factory Alarm dashboard (`dashboard.json`) from this folder into your IOTCONNECT environment.
3. Configure IOTCONNECT device rules to automate notifications and control device responses.
4. Validate and test your setup using Infineon's IM69D130 MEMS microphone to ensure accurate detection and response to alarms.

For additional information or troubleshooting, please refer to the main [Avnet IoTConnect Imagimob AI Examples README](../README.md).

