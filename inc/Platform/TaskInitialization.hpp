#pragma once

#include "FreeRTOS.h"
#include "SEGGER_RTT.h"
#include "TimeKeepingTask.hpp"
#include "MCUTemperatureTask.hpp"
#include "AmbientTemperatureTask.hpp"
#include "UARTGatekeeperTask.hpp"
#include "WatchdogTask.hpp"
#include "CANOutgoingGatekeeperTask.hpp"
#include "CANIncomingGatekeeperTask.hpp"
#include "CANTestTask.hpp"

void initializeTasks();
