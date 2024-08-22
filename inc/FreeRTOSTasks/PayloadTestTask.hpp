//
// Created by tats on 10/7/2024.
//

#pragma once

#include "RS422/RS422_Driver.hpp"
#include "Task.hpp"

class PayloadTestTask : public Task {
private:

public:
    const static inline uint16_t TaskStackDepth = 3000;

    StackType_t taskStack[TaskStackDepth];

    void execute();

    PayloadTestTask() : Task("Payload Test") {}

    /**
     * Create freeRTOS Task
     */
    void createTask() {
        xTaskCreateStatic(vClassTask < PayloadTestTask > , this->TaskName, PayloadTestTask::TaskStackDepth, this,
                          tskIDLE_PRIORITY + 1, this->taskStack, &(this->taskBuffer));
    }
};

inline std::optional<PayloadTestTask> payloadTestTask;

