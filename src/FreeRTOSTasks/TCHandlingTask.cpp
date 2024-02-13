#include "TCHandlingTask.hpp"
#include <Logger.hpp>
#include <definitions.h>
#include <MessageParser.hpp>
#include "COBS.hpp"
#include "task.h"
#include "CAN/ApplicationLayer.hpp"
#include "LCLDefinitions.hpp"
#include "AmbientTemperatureTask.hpp"
#include "CANGatekeeperTask.hpp"
#include "CANTestTask.hpp"
#include "HousekeepingTask.hpp"
#include "MCUTemperatureTask.hpp"
#include "MRAMTask.hpp"
#include "NANDTask.hpp"
#include "TimeBasedSchedulingTask.hpp"
#include "TimeKeepingTask.hpp"
#include "UARTGatekeeperTask.hpp"
#include "LCL.hpp"


TCHandlingTask::TCHandlingTask() : Task("TCHandling") {
    messageQueueHandle = xQueueCreateStatic(TCQueueCapacity, sizeof(etl::string<MaxUsartTCSize>),
                                            messageQueueStorageArea,
                                            &messageQueue);
    configASSERT(messageQueueHandle);

    UART0_ReadCallbackRegister([](uintptr_t object) -> void {
        TCHandlingTask *task = reinterpret_cast<TCHandlingTask * >(object);

        if (UART0_ReadCountGet() == 0) {
            UART_ERROR uartError = UART0_ErrorGet();
        } else {
            task->ingress();
        }

        UART0_Read(&(task->byteIn), sizeof(this->byteIn));
    }, reinterpret_cast<uintptr_t>(this));

    UART0_Read(&byteIn, sizeof(byteIn));
}


void TCHandlingTask::resetInput() {
    new(&(TCHandlingTask::savedMessage)) etl::string<MaxUsartTCSize>;
}

void TCHandlingTask::ingress() {
    BaseType_t higherPriorityTaskWoken = pdFALSE;

    if (savedMessage.full()) {
        resetInput();
    }

    if (byteIn == MessageEndDelimiter) {
        xQueueSendToBackFromISR(messageQueueHandle, static_cast<void *>(&savedMessage), &higherPriorityTaskWoken);
        resetInput();
    } else {
        savedMessage.push_back(byteIn);
    }

    if (higherPriorityTaskWoken) {
        portYIELD_FROM_ISR(higherPriorityTaskWoken);
    }
}

void TCHandlingTask::execute() {
    while (true) {
        xQueueReceive(messageQueueHandle, static_cast<void *>(&messageOut), portMAX_DELAY);

        LOG_DEBUG << "TURNING OFF OBC MAIN MCU";

        PIO_PinOutputEnable(ADM_DEPLOY_1_PIN);
        PIO_PinWrite(ADM_DEPLOY_1_PIN, true);

        vTaskSuspend(ambientTemperatureTask->taskHandle);
        vTaskSuspend(mcuTemperatureTask->taskHandle);
        vTaskSuspend(nandTask->nandTaskHandle);
        vTaskSuspend(mramTask->mramTaskHandle);
        vTaskSuspend(uartGatekeeperTask->taskHandle);
        vTaskSuspend(housekeepingTask->taskHandle);
        vTaskSuspend(timeBasedSchedulingTask->taskHandle);
        vTaskSuspend(timeKeepingTask->taskHandle);
        vTaskSuspend(canTestTask->taskHandle);
        vTaskSuspend(canGatekeeperTask->taskHandle);


        for (LCL lcl : LCLDefinitions::lclArray) {
            lcl.disableLCL();
        }

        vTaskSuspend(NULL);
    }
}
