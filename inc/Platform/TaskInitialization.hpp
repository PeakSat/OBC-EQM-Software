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

void initializeTasks();

void initializeSemaphores();

bool takeSemaphoreGroupA();

void releaseSemaphoreGroupA();

void resetChecks();
