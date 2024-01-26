#include "CANTestTask.hpp"
#include "CANGatekeeperTask.hpp"

void CANTestTask::execute() {
    CAN::Frame frame = {CAN::NodeID};
    for (auto i = 0; i < CAN::Frame::MaxDataLength; i++) {
        frame.data.at(i) = i;
    }

    String<ECSSMaxMessageSize> testPayload("WHO LIVES IN A PINEAPPLE UNDER THE SEA?");
    while (true) {
        CAN::Application::createLogMessage(CAN::NodeIDs::COMMS, false, testPayload.data(), false);
        xTaskNotify(canGatekeeperTask->taskHandle, 0, eNoAction);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}