#include "TaskInitialization.hpp"

void initializeTasks(){
    uartGatekeeperTask.emplace();
    timeKeepingTask.emplace();
    ambientTemperatureTask.emplace();
    watchdogTask.emplace();
    mcuTemperatureTask.emplace();

    ambientTemperatureTask->createTask();
    mcuTemperatureTask->createTask();
    timeKeepingTask->createTask();
    uartGatekeeperTask->createTask();
    watchdogTask->createTask();
}

SemaphoreHandle_t Semaphore_Group_A = NULL;
SemaphoreHandle_t Semaphore_Group_B = NULL;
SemaphoreHandle_t Semaphore_Group_C = NULL;

void initializeSemaphores(){
    Semaphore_Group_A = xSemaphoreCreateMutex();
    Semaphore_Group_B = xSemaphoreCreateMutex();
    Semaphore_Group_C = xSemaphoreCreateMutex();

    if( Semaphore_Group_A == NULL )
    {
        while(1){

        }
    }
    if( Semaphore_Group_B == NULL )
    {
        while(1){

        }
    }
    if( Semaphore_Group_C == NULL )
    {
        while(1){

        }
    }
}

bool takeSemaphoreGroup(smphr_groups group){
    switch (group)
    {
    case GROUP_A:
        {
            if(xSemaphoreTake(Semaphore_Group_A, ( TickType_t ) 10) == pdTRUE){
                return true;
            }
        }
        break;
    case GROUP_B:
        {
            if(xSemaphoreTake(Semaphore_Group_B, ( TickType_t ) 10) == pdTRUE){
                return true;
            }
        }
        break;
    case GROUP_C:
        {
            if(xSemaphoreTake(Semaphore_Group_C, ( TickType_t ) 10) == pdTRUE){
                return true;
            }
        }
        break;
    default:
        break;
    }
    return false;
}

void releaseSemaphoreGroup(smphr_groups group){
    switch (group)
    {
    case GROUP_A:
        xSemaphoreGive(Semaphore_Group_A);
        break;
    case GROUP_B:
        xSemaphoreGive(Semaphore_Group_B);
        break;
    case GROUP_C:
        xSemaphoreGive(Semaphore_Group_C);
        break;
    default:
        break;
    }
}

void resetChecks(){
    RSTC_RESET_CAUSE resetCause = RSTC_ResetCauseGet();
    if(resetCause == RSTC_GENERAL_RESET){
        LOG_DEBUG<<"Last reset was: General reset.";
    }
    else if(resetCause == RSTC_BACKUP_RESET){
        LOG_DEBUG<<"Last reset was: Backup reset.";
    }
    else if(resetCause == RSTC_WATCHDOG_RESET){
        LOG_DEBUG<<"Last reset was: Watchdog reset.";
    }
    else if(resetCause == RSTC_USER_RESET){
        LOG_DEBUG<<"Last reset was: User reset.";
    }
    else if(resetCause == RSTC_SOFTWARE_RESET){
        LOG_DEBUG<<"Last reset was: Software reset.";
    }
    else {
        LOG_DEBUG<<"Last reset was: Undefined reset.";
    }
}