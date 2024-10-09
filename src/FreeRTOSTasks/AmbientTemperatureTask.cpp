#include "AmbientTemperatureTask.hpp"
#include "semphr.h"
#include "TaskInitialization.hpp"

void AmbientTemperatureTask::execute() {
    while(!takeSemaphoreGroupA()){
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
        releaseSemaphoreGroupA();
        vTaskSuspend(taskHandle);
    }
    LOG_DEBUG<<"No errors on ambient temperatures";

    releaseSemaphoreGroupA();


    while (true) {
        LOG_DEBUG << "Runtime entered: " << this->TaskName;
        while(!takeSemaphoreGroupA()){
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
        releaseSemaphoreGroupA();
        vTaskDelay(pdMS_TO_TICKS(DelayMs));
    }
}
