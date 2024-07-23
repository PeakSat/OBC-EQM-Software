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