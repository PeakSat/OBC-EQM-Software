//
// Created by tats on 10/7/2024.
//

#include "RS422/RS422_Driver.hpp"

bool sendPayloadMessage(uint8_t* msg, size_t msg_size){
    if(UART2_TransmitComplete()){
        return UART2_Write(msg, msg_size);
    }
    return false;
}

bool sendPayloadStatus(){
    return UART2_TransmitComplete();
}