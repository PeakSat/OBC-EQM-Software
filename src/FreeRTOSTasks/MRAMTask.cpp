#include "MRAMTask.hpp"
#include "LCLDefinitions.hpp"
#include "NANDTask.hpp"

bool MRAMTask::isMRAMAlive() {

    for (uint8_t numberOfAttempts = 0; numberOfAttempts < 2; numberOfAttempts++) {
        if (mram.mramReadByte(areYouAliveAddress) == areYouAliveValue) {
            return true;
        } else {
            if (numberOfAttempts == 0) {
                mramLCL.enableLCL();
                LOG_INFO << "MRAM was sleeping!";
            }
        }
    }

    LOG_ERROR << "MRAM is not responding, shutting down LCL!";
    mramLCL.disableLCL();

    return false;
}

void MRAMTask::execute() {

    vTaskSuspend(NULL);

    mramLCL.enableLCL();

    areYouAliveAddress = 0x200000 - 1;

    mram.mramWriteByte(areYouAliveAddress, areYouAliveValue);

    uint32_t randomAddressOffset;
    uint8_t randomValueOffset;

    while (true) {
        randomAddressOffset = rand() % (areYouAliveAddress - 200);
        randomValueOffset = rand() % 100;

        if (isMRAMAlive()) {
            for (uint8_t address = 0; address < 150; address++) {
                mram.mramWriteByte(randomAddressOffset + address, randomValueOffset + address);
            }
        }

        bool readWasCorrect = true;
        if (isMRAMAlive()) {
            for (uint8_t address = 0; address < 150; address++) {
                if (mram.mramReadByte(randomAddressOffset + address) != randomValueOffset + address) {
                    LOG_ERROR << "MRAM: address " << (randomAddressOffset + address) << " had a wrong value!";
                    readWasCorrect = false;
                }
            }
        }

        if (readWasCorrect) {
            LOG_INFO << "MRAM read and write test succeeded";
        } else {
            LOG_INFO << "MRAM read and write test failed";
        }

        vTaskResume(NANDTask::nandTaskHandle);
        vTaskSuspend(NULL);

        vTaskDelay(DelayMs);
    }
}