#!/bin/bash

# this script can be used to automatically build the application with MTB command line

set -e

if [ -z "${APP_PATH}" ]; then
  APP_PATH=${PWD}
fi

if [ -z "${BUILD_DIR}" ]; then
  BUILD_DIR=/tmp/build
fi

if [ -z "${ARTIFACTS_DIR}" ]; then
  ARTIFACTS_DIR=${APP_PATH}/artifacts
fi

mkdir -p "${BUILD_DIR}"
mkdir -p "${ARTIFACTS_DIR}"

#############################
cd "${BUILD_DIR}"

project-creator-cli --board-id CY8CKIT-062S2-AI --app-path "${APP_PATH}" --user-app-name 062S2-AI-app

cd 062S2-AI-app/

###############
make MODEL_SELECTION=GESTURE_MODEL build
cp ./build/last_config/avnet-iotc-mtb-ai-imagimob-rm.hex "${ARTIFACTS_DIR}"/062S2-AI-imagimob-gesture.hex

make MODEL_SELECTION=SIREN_MODEL build
cp ./build/last_config/avnet-iotc-mtb-ai-imagimob-rm.hex "${ARTIFACTS_DIR}"/062S2-AI-imagimob-siren.hex

make MODEL_SELECTION=BABYCRY_MODEL build
cp ./build/last_config/avnet-iotc-mtb-ai-imagimob-rm.hex "${ARTIFACTS_DIR}"/062S2-AI-imagimob-baby-cry.hex

make MODEL_SELECTION=ALARM_MODEL build
cp ./build/last_config/avnet-iotc-mtb-ai-imagimob-rm.hex "${ARTIFACTS_DIR}"/062S2-AI-imagimob-alarm.hex

make MODEL_SELECTION=COUGH_MODEL build
cp ./build/last_config/avnet-iotc-mtb-ai-imagimob-rm.hex "${ARTIFACTS_DIR}"/062S2-AI-imagimob-cough.hex

make MODEL_SELECTION=SNORE_MODEL build
cp ./build/last_config/avnet-iotc-mtb-ai-imagimob-rm.hex "${ARTIFACTS_DIR}"/062S2-AI-imagimob-snore.hex


#############################
cd "${BUILD_DIR}"

project-creator-cli --board-id CY8CKIT-062S2-43012 --app-path "${APP_PATH}" --user-app-name 062S2-43012-app
# --app-path <path/to/custom/application>

cd 062S2-43012-app/

###############
make MODEL_SELECTION=SIREN_MODEL build
cp ./build/last_config/avnet-iotc-mtb-ai-imagimob-rm.hex "${ARTIFACTS_DIR}"/062S2-43012-imagimob-siren.hex

make MODEL_SELECTION=BABYCRY_MODEL build
cp ./build/last_config/avnet-iotc-mtb-ai-imagimob-rm.hex "${ARTIFACTS_DIR}"/062S2-43012-imagimob-baby-cry.hex

make MODEL_SELECTION=ALARM_MODEL build
cp ./build/last_config/avnet-iotc-mtb-ai-imagimob-rm.hex "${ARTIFACTS_DIR}"/062S2-43012-imagimob-alarm.hex

make MODEL_SELECTION=COUGH_MODEL build
cp ./build/last_config/avnet-iotc-mtb-ai-imagimob-rm.hex "${ARTIFACTS_DIR}"/062S2-43012-imagimob-cough.hex

make MODEL_SELECTION=SNORE_MODEL build
cp ./build/last_config/avnet-iotc-mtb-ai-imagimob-rm.hex "${ARTIFACTS_DIR}"/062S2-43012-imagimob-snore.hex


if [ "$GITHUB_ACTIONS" != "true" ]; then
  echo "GitHub Action Environment not detected. Creating the zip..."
  cd "${ARTIFACTS_DIR}"
  rm -f avnet-iotc-mtb-ai-imagimob-rm-firmware.zip
  zip avnet-iotc-mtb-ai-imagimob-rm-firmware.zip -- *.hex
  rm -f -- *.hex
  echo "Done."
fi
