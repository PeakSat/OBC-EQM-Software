#include "FreeRTOSTasks/HousekeepingTask.hpp"
#include "TaskInitialization.hpp"

void HousekeepingTask::execute() {

    LOG_DEBUG << "Runtime init: " << this->TaskName;
    vTaskDelay(pdMS_TO_TICKS(500));
    auto &housekeeping = Services.housekeeping;
    uint32_t nextCollection = 0;
    uint32_t timeBeforeDelay = 0;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (true) {
//        LOG_DEBUG << "Runtime entered: " << this->TaskName;
        while(!takeSemaphoreGroup(smphr_groups::GROUP_A)){
            LOG_DEBUG << "HouseKeeping found the semaphore locked";
            vTaskDelay(500);
        }
        nextCollection = housekeeping.reportPendingStructures(xTaskGetTickCount(), timeBeforeDelay, nextCollection);
        timeBeforeDelay = xTaskGetTickCount();
//        LOG_DEBUG << "Runtime exit: " << this->TaskName;
        releaseSemaphoreGroup(smphr_groups::GROUP_A);
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(nextCollection));
    }
}
