// junior.h

#ifndef JUNIOR_H
#define JUNIOR_H

#include "SerialPort.h"
#include <string>

enum JuniorState {
    STATE_NOT_INITIALIZED,
    STATE_IDLE,
    STATE_BUSY
};

enum JuniorDeviceType {
    JUNIOR_MOTOR = 1,
    JUNIOR_PUMP = 2,
};

struct MotorConfiguration {
    uint32_t torqueRegulationIdle;
    uint32_t torqueRegulation;
    uint32_t overcurrentThreshold;
    uint8_t stepSelection;
    uint8_t motorDriverType;
    bool isClockwiseWired;
};

class Junior {
public:
    Junior(const char* rxPort, const char* txPort, uint8_t address, JuniorDeviceType deviceType);

    void run();

private:
    JuniorState state;
    SerialPort serialPort;
    uint8_t address;
    JuniorDeviceType deviceType;

    void handleCommands();
    void executeMotorCommand();
    void executePumpCommand();
    void sendStateUpdate();
};

#endif // JUNIOR_H


