#include "main.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "list.h"
#include "task.h"
#include "definitions.h"
#include "OBC_Definitions.hpp"
#include "TaskInitialization.hpp"
#include "HousekeepingTask.hpp"
#include "TimeBasedSchedulingTask.hpp"
#include "StatisticsReportingTask.hpp"
#include "CANGatekeeperTask.hpp"
#include "CANTestTask.hpp"
#include "TCHandlingTask.hpp"
#include "NANDTask.hpp"
#include "MRAMTask.hpp"
#include "PayloadTestTask.hpp"

#define IDLE_TASK_SIZE 1000

#if configSUPPORT_STATIC_ALLOCATION
/* static memory allocation for the IDLE task */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[IDLE_TASK_SIZE];

extern "C" void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer,
                                              uint32_t *pulIdleTaskStackSize) {
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
    *ppxIdleTaskStackBuffer = &xIdleStack[0];
    *pulIdleTaskStackSize = IDLE_TASK_SIZE;
}

#endif

extern "C" void main_cpp() {
    SYS_Initialize(NULL);

    // initializeTasks();
    initializeSemaphores();

    uartGatekeeperTask.emplace();

    nandTask.emplace();
    payloadTestTask.emplace();
    canGatekeeperTask.emplace();
    canTestTask.emplace();
    housekeepingTask.emplace();
    tcHandlingTask.emplace();
    mcuTemperatureTask.emplace();
    ambientTemperatureTask.emplace();
    mramTask.emplace();
    timeKeepingTask.emplace();
    
    watchdogTask.emplace();
    

    __disable_irq();

    uartGatekeeperTask->createTask();

    nandTask->createTask();
    payloadTestTask->createTask();
    canGatekeeperTask->createTask();
    canTestTask->createTask();
    housekeepingTask->createTask();
    tcHandlingTask->createTask();
    mcuTemperatureTask->createTask();
    ambientTemperatureTask->createTask();
    mramTask->createTask();
    timeKeepingTask->createTask();
    
    watchdogTask->createTask();
    __enable_irq();
    

//    resetChecks();

    vTaskStartScheduler();

    while (true) {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks();
    }

    return;
}

