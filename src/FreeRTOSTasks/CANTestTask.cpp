#include "CANTestTask.hpp"
#include "CANGatekeeperTask.hpp"

void CANTestTask::execute() {
    CAN::Frame frame = {CAN::NodeID};
    for (auto i = 0; i < CAN::Frame::MaxDataLength; i++) {
        frame.data.at(i) = i;
    }

    String<ECSSMaxMessageSize> testPayload1("WHO LIVES IN A PINEAPPLE UNDER THE SEA?");

    String<ECSSMaxMessageSize> testPayload2("Giati?");

    while (true) {
        if(AcubeSATParameters::obcCANBUSActive.getValue() == CAN::Driver::ActiveBus::Redundant) {
            AcubeSATParameters::obcCANBUSActive.setValue(CAN::Driver::ActiveBus::Main);
            CAN::Application::createLogMessage(CAN::NodeIDs::COMMS, false, testPayload1.data(), false);
        } else {
            AcubeSATParameters::obcCANBUSActive.setValue(CAN::Driver::ActiveBus::Redundant);
            CAN::Application::createLogMessage(CAN::NodeIDs::COMMS, false, testPayload2.data(), false);
        }


//        xTaskNotify(canGatekeeperTask->taskHandle, 0, eNoAction);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}