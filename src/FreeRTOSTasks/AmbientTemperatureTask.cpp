#include "AmbientTemperatureTask.hpp"
#include "semphr.h"
#include "TaskInitialization.hpp"

void AmbientTemperatureTask::execute() {
    while(!takeSemaphoreGroup(smphr_groups::GROUP_A)){
        LOG_DEBUG << "AMB_TEMP Found semaphore Locked";
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    LOG_DEBUG << "Runtime init: " << this->TaskName;
    uint8_t numberOfDisconnectedSensors = 0;

    for (auto &sensor: sensors) {
        if (not sensor.isDeviceConnected()) {
            LOG_ERROR << "Temperature sensor with address " << sensor.getI2CUserAddress()
                      << " was disconnected";
            numberOfDisconnectedSensors++;
        }
    }

    if (numberOfDisconnectedSensors == NumberOfTemperatureSensors) {
        LOG_ERROR << "Suspending ambient temperature task";
        releaseSemaphoreGroup(smphr_groups::GROUP_A);
        vTaskSuspend(taskHandle);
    }
    LOG_DEBUG<<"No errors on ambient temperatures";

    releaseSemaphoreGroup(smphr_groups::GROUP_A);


    while (true) {
        LOG_DEBUG << "Runtime entered: " << this->TaskName;
        while(!takeSemaphoreGroup(smphr_groups::GROUP_A)){
            LOG_DEBUG << "AMB_TEMP Found semaphore Locked";
            vTaskDelay(pdMS_TO_TICKS(500));
        }
        for (uint8_t sensorCounter = 0; sensorCounter < NumberOfTemperatureSensors; sensorCounter++) {
            if (not sensors[sensorCounter].isDeviceConnected()) {
                LOG_ERROR << "Temperature sensor with address " << sensors[sensorCounter].getI2CUserAddress()
                          << " was disconnected";
                continue;
            }
            ambientTemperature[sensorCounter] = sensors[sensorCounter].getTemperature();
            LOG_INFO << "Sensor with address" << sensors[sensorCounter].getI2CUserAddress() <<
                " responded with ambient temperature = " << ambientTemperature[sensorCounter];
        }

        CommonParameters::boardTemperature1.setValue(ambientTemperature[0]);
        CommonParameters::boardTemperature2.setValue(ambientTemperature[1]);
        LOG_DEBUG << "Exiting Amb temperature";
        releaseSemaphoreGroup(smphr_groups::GROUP_A);
        vTaskDelay(pdMS_TO_TICKS(DelayMs));
    }
}
