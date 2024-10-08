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
    LOG_DEBUG << "Runtime init: " << this->TaskName;
   vTaskSuspend(NULL);

    mramLCL.enableLCL();

    areYouAliveAddress = 100;

    mram.mramWriteByte(areYouAliveAddress, areYouAliveValue);

    uint32_t randomAddressOffset = 0;
    uint8_t randomValueOffset = 0, readData = 0;

    while (true) {
//        LOG_DEBUG << "Runtime entered: " << this->TaskName;
        // if (isMRAMAlive()) {
        //     for (uint8_t address = 0; address < 150; address++) {
        //         mram.mramWriteByte(randomAddressOffset + address, randomValueOffset + address);
        //     }
        // }

        bool readWasCorrect = true;
        if (isMRAMAlive()) {
            for (uint32_t address = 0x200000 - 100000; address < 0x200000 - 1; address++) {
                mram.mramWriteByte(randomAddressOffset + address, randomValueOffset + (uint8_t)(address%250));
                // SYSTICK_DelayUs(10);
                readData = mram.mramReadByte(randomAddressOffset + address);
                // SYSTICK_DelayUs(10);
                if (readData != (randomValueOffset + (uint8_t)(address%250))) {
                    LOG_ERROR << "MRAM: offset = " << randomAddressOffset << ",value = " << address
                        << " had a wrong value = " << readData;
                    readWasCorrect = false;
                }
            }
        }

        if (readWasCorrect) {
            LOG_INFO << "MRAM read and write test succeeded";
        } else {
            LOG_INFO << "MRAM read and write test failed";
        }

        if (randomAddressOffset > (areYouAliveAddress - 200)) {
            randomAddressOffset = 0;
        }
        if (randomValueOffset > 100) {
            randomValueOffset = 0;
        }

       LOG_DEBUG << "Runtime is exiting: " << this->TaskName;
       vTaskResume(NANDTask::nandTaskHandle);
       vTaskSuspend(NULL);

        vTaskDelay(pdMS_TO_TICKS(DelayMs));
    }
}