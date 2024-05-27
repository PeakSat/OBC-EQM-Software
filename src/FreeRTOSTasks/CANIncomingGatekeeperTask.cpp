#include "CAN/Driver.hpp"
#include "CAN/Frame.hpp"
#include "CANIncomingHandlerTask.hpp"

CANIncomingHandlerTask::CANIncomingHandlerTask() : Task("CANGatekeeperTask") {

    incomingSFQueue = xQueueCreateStatic(CAN::FrameQueueSize, sizeof(CAN::Frame), incomingSFQueueStorageArea,
                                         &incomingSFQueueBuffer);
    vQueueAddToRegistry(incomingSFQueue, "CAN Incoming SF");
    configASSERT(incomingSFQueue);

    incomingMFQueue = xQueueCreateStatic(CAN::FrameQueueSize, sizeof(CAN::Frame), incomingSFQueueStorageArea,
                                         &incomingMFQueueBuffer);
    vQueueAddToRegistry(incomingSFQueue, "CAN Incoming MF");
    configASSERT(incomingMFQueue);
}

void CANIncomingHandlerTask::execute() {
    CAN::Frame out_message = {};
    CAN::Frame in_message = {};

    uint32_t ulNotifiedValue;

    while (true) {
        xTaskNotifyWait(0, 0, &ulNotifiedValue, portMAX_DELAY);
        if (ulNotifiedValue & singleFrameEvent) {
            xQueueReceive(incomingSFQueue, &in_message, portMAX_DELAY);
            CAN::TPProtocol::processSingleFrame(in_message);
        }

        if (ulNotifiedValue & multipleFramesEvent) {
            CAN::TPProtocol::processMultipleFrames();
        }
    }
}
