#pragma once

#include "FreeRTOS.h"
#include "SEGGER_RTT.h"
#include "TimeKeepingTask.hpp"
#include "MCUTemperatureTask.hpp"
#include "AmbientTemperatureTask.hpp"
#include "UARTGatekeeperTask.hpp"
#include "WatchdogTask.hpp"
#include "CANGatekeeperTask.hpp"
#include "CANTestTask.hpp"
#include "semphr.h"

enum smphr_groups{
    GROUP_A,
    GROUP_B,
    GROUP_C
};

void initializeTasks();

void initializeSemaphores();

bool takeSemaphoreGroup(smphr_groups group);

void releaseSemaphoreGroup(smphr_groups group);

void resetChecks();
