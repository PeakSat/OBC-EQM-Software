#include "CAN/Driver.hpp"
#include "CAN/Frame.hpp"
#include "CANIncomingGatekeeperTask.hpp"

CANIncomingGatekeeperTask::CANIncomingGatekeeperTask() : Task("CANGatekeeperTask") {

    incomingSFQueue = xQueueCreateStatic(CAN::FrameQueueSize, sizeof(CAN::Frame), incomingSFQueueStorageArea,
                                         &incomingSFQueueBuffer);
    vQueueAddToRegistry(incomingSFQueue, "CAN Incoming SF");
    configASSERT(incomingSFQueue);

    incomingMFQueue = xQueueCreateStatic(CAN::FrameQueueSize, sizeof(CAN::Frame), incomingSFQueueStorageArea,
                                         &incomingMFQueueBuffer);
    vQueueAddToRegistry(incomingSFQueue, "CAN Incoming MF");
    configASSERT(incomingMFQueue);
}

void CANIncomingGatekeeperTask::execute() {
#ifdef OBC_EQM_LCL
    LCLDefinitions::lclArray[LCLDefinitions::CAN1].enableLCL();
    LCLDefinitions::lclArray[LCLDefinitions::CAN2].enableLCL();
#endif

    PIO_PinWrite(CAN::CAN_SILENT_1, false);
    PIO_PinWrite(CAN::CAN_SILENT_2, false);

    CAN::Frame out_message = {};
    CAN::Frame in_message = {};

    uint32_t ulNotifiedValue;

    while (true) {
        xTaskNotifyWait(0, 0, &ulNotifiedValue, portMAX_DELAY);

        if (getIncomingSFMessagesCount()) {
            xQueueReceive(incomingSFQueue, &in_message, portMAX_DELAY);
            CAN::TPProtocol::processSingleFrame(in_message);
        }

        if (getIncomingMFMessagesCount()) {
            CAN::TPProtocol::processMultipleFrames();
        }
    }
}
