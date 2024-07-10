#include "MCUTemperatureTask.hpp"
#include <definitions.h>

void MCUTemperatureTask::execute() {
    LOG_DEBUG << "Runtime init: " << this->TaskName;

    uint8_t message[] = {
            'M', 'C', 'U', ' ', 'T', 'e', 'm', 'p', 'e', 'r', 'a', 't', 'u', 'r', 'e', ' ', '7', '0', 'C', '\0'
    };

    while (true) {
//        LOG_DEBUG << "Runtime entered: " << this->TaskName;
        AFEC0_ConversionStart();
        vTaskDelay(pdMS_TO_TICKS(1));
        uint16_t ADCconversion = AFEC0_ChannelResultGet(AFEC_CH11);
        float voltageConversion = static_cast<float>(ADCconversion) * PositiveVoltageReference / MaxADCValue;
        const float MCUtemperature =
                (voltageConversion - TypicalVoltageAt25) / TemperatureSensitivity + ReferenceTemperature;
        LOG_DEBUG << "The temperature of the MCU is: " << MCUtemperature << " degrees Celsius";
        CommonParameters::mcuTemperature.setValue(MCUtemperature);

        LOG_DEBUG << "MCU Temperature sending to UART2";
        UART2_Write(message, 20);
        while(!UART2_TransmitComplete()){

            vTaskDelay(pdMS_TO_TICKS(1));
        };
        LOG_DEBUG << "MCU Temperature sent to UART2";



//        LOG_DEBUG << "Runtime exiting: " << this->TaskName;
        vTaskDelay(pdMS_TO_TICKS(delayMs));
    }
}
