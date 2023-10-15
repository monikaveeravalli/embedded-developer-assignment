#ifndef SENIOR_H
#define SENIOR_H

#include "SerialPort.h"
#include <vector>

const uint8_t SENIOR_ADDRESS = 254;
const uint32_t BAUDRATE = 9600;

// Define states for the Senior application
enum SeniorState {
    WAITING_FOR_CONFIG,
    CONFIGURING,
    IN_PRODUCTION
};
struct ConfigureCommand {
    int address;
    JuniorState targetState;
    MotorConfig motorConfig;
};
// Senior class to manage communication with Juniors
class Senior {
public:
    Senior(const char* rxPort, const char* txPort);

    void run();

private:
    SeniorState state;
    SerialPort serialPort;
    std::vector<uint8_t> juniorAddresses;
    // Add other necessary data structures

    void initialize();
    void configureJuniors();
    void pollJuniors();
};

#endif // SENIOR_H
