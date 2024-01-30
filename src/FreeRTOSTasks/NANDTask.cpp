#include "NANDTask.hpp"
#include <etl/to_string.h>
#include <etl/String.hpp>
#include "LCLDefinitions.hpp"
#include "MRAMTask.hpp"


void NANDTask::execute() {

    MT29F blabla(SMC::NCS3, MEM_NAND_BUSY_1_PIN, MEM_NAND_WR_ENABLE_PIN);

    LCL &nandLCL = LCLDefinitions::lclArray[LCLDefinitions::NANDFlash];

    nandLCL.enableLCL();

    uint8_t failedTries = 0;

    /* INITIALIZE & STATUS */
    for (failedTries = 0; failedTries < 3;) {
        uint8_t status = 1;
        status = blabla.resetNAND();
        if (status != 0) {
            LOG_DEBUG << "The status of the 1st LUN (die) is: " << status;
            break;
        } else {
            LOG_DEBUG << "Failed to reset NAND";
            nandLCL.enableLCL();
            failedTries++;
        }
    }
    while (true) {
        /* ID */
        for (failedTries = 0; failedTries < 3;) {
            if (blabla.isNANDAlive()) {
                uint8_t id[8];
                etl::array<unsigned char, 8> etlArray;
                for (size_t i = 0; i < 8; i++) {
                    etlArray[i] = static_cast<unsigned char>(id[i]);
                }
                blabla.readNANDID(etlArray);
                etl::string<32> stringID = "";
                for (uint8_t i = 0; i < 8; i++) {
                    etl::to_string(id[i], stringID, true);
                    stringID.append(" ");
                }
                LOG_DEBUG << "The ID of the NAND Flash is: " << stringID.c_str();
                break;
            } else {
                LOG_DEBUG << "Wrong NAND ID";
                blabla.resetNAND();
                failedTries++;
            }
        }

        /* WRITE */
        uint8_t dataWrite[20] = {};
        for (uint8_t i = 0; i < 20; i++) {
            dataWrite[i] = i + 1;
        }
        uint32_t writePosition = rand() % (1105920 * 10);
        LOG_DEBUG << "Write address is: " << writePosition;
        uint8_t LUN = 1;
        uint8_t BLOCK = writePosition / 1105920;
        uint32_t PAGE = BLOCK/8640;
        uint32_t COLUMN = PAGE/8;

        for (failedTries = 0; failedTries < 3;) {
            bool success = blabla.writeNAND(LUN, PAGE, COLUMN,dataWrite);
            if (!success) {
                LOG_DEBUG << "Failed to write NAND";
                if (blabla.errorHandler()) {
                    LOG_DEBUG << "NAND is alive";
                } else {
                    LOG_DEBUG << "NAND is not responding";
                    nandLCL.enableLCL();
                }
                failedTries++;
            } else {
                vTaskDelay(pdMS_TO_TICKS(10));
                break;
            }
        }

        /* READ */
        etl::array<uint8_t, 20> dataRead = {};

        for (failedTries = 0; failedTries < 3;) {
            bool success = blabla.readNAND(LUN, PAGE, COLUMN,dataRead);
            if (success) {
                etl::string<100> stringReadWrite = "";
                for (uint8_t i = 0; i < 20; i++) {
                    etl::to_string(dataRead[i], stringReadWrite, true);
                    stringReadWrite.append(" ");
                }

                // check if write-read was correct
                if (std::equal(std::begin(dataRead), std::end(dataRead), dataWrite)) {
                    LOG_INFO << "NAND read and write test succeeded";
                } else {
                    LOG_INFO << "NAND read and write test failed";
                    LOG_DEBUG << "NAND read: " << stringReadWrite.c_str();
                }

                break;
            } else {
                LOG_ERROR << "Failed to read NAND";
                if (blabla.errorHandler()) {
                    LOG_INFO << "NAND is alive";
                } else {
                    LOG_INFO << "NAND is not responding";
                    nandLCL.enableLCL();
                }
                failedTries++;
            }
        }
        /* ERASE */
        for (failedTries = 0; failedTries < 3;) {
            uint8_t success = blabla.eraseBlock(1, BLOCK);
            if (!success) {
                LOG_ERROR << "Failed to erase NAND block";
                failedTries++;
            } else {
                vTaskDelay(pdMS_TO_TICKS(10));
                break;
            }
        }

        uint8_t dataErase[20] = {};
        for (uint8_t i = 0; i < 20; i++) {
            dataErase[i] = 255;
        }
        /* READ */
        for (failedTries = 0; failedTries < 3;) {
            bool success = blabla.readNAND(LUN, PAGE, COLUMN, dataRead);
            if (success) {
                etl::string<100> stringReadErase = "";
                for (uint8_t i = 0; i < 20; i++) {
                    etl::to_string(dataRead[i], stringReadErase, true);
                    stringReadErase.append(" ");
                }
                if (std::equal(std::begin(dataRead), std::end(dataRead), dataWrite)) {
                    LOG_INFO << "NAND erase test failed";
                } else if (!std::equal(std::begin(dataRead), std::end(dataRead), dataErase)) {
                    LOG_INFO << "NAND erase test failed";
                    LOG_DEBUG << "Read NAND after erasing: " << stringReadErase.c_str();
                } else {
                    LOG_INFO << "NAND erase test succeeded";
                }
                break;
            } else {
                LOG_ERROR << "Failed to read NAND";
                failedTries++;
            }
        }

        vTaskResume(MRAMTask::mramTaskHandle);
        vTaskSuspend(NULL);

        vTaskDelay(DelayMs);
    }
}