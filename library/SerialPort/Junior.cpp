// junior.cpp
#include "SerialPort.h"
#include <iostream>
#include <string>
#include <thread>  // For simulating command execution

const uint8_t JUNIOR_ADDRESS = 1;  // Change this to the Junior's address
const uint32_t BAUDRATE = 9600;

enum JuniorState {
    STATE_NOT_INITIALIZED,
    STATE_IDLE,
    STATE_BUSY
};

// Define Junior device types
enum JuniorDeviceType {
    JUNIOR_MOTOR = 1,
    JUNIOR_PUMP = 2,
    // Add more device types as needed
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

    void handleConfigureCommand();
    void handleCommands();
    void executeMotorCommand();
    void executePumpCommand();
    void sendStateUpdate();
};

Junior::Junior(const char* rxPort, const char* txPort, uint8_t address, JuniorDeviceType deviceType)
    : state(STATE_NOT_INITIALIZED), address(address), deviceType(deviceType)
{
    serialPort.connect(rxPort, BAUDRATE);
    serialPort.registerReceiveCallback([&](const char* msg, uint16_t size) {
        // Handle received messages from Senior
        if (state == STATE_NOT_INITIALIZED && strcmp(msg, "ConfigureCommand") == 0) {
            handleConfigureCommand();
        } else if (state == STATE_IDLE) {
            // Implement message parsing and execute commands
            // For simplicity, assume the received message is a MotorCommand or PumpCommand
            if (deviceType == JUNIOR_MOTOR && strcmp(msg, "MotorCommand") == 0) {
                executeMotorCommand();
            } else if (deviceType == JUNIOR_PUMP && strcmp(msg, "PumpCommand") == 0) {
                executePumpCommand();
            }
        }
    }
}

void Junior::handleConfigureCommand() {
    // Handle the ConfigureCommand and initialize peripherals
    // Transition to STATE_IDLE
    std::cout << "Junior (Address: " << static_cast<int>(address) << ") received ConfigureCommand and is initializing." << std::endl;
    // Initialize peripherals and perform necessary setup
    // ...
    state = STATE_IDLE;
    std::cout << "Junior (Address: " << static_cast<int>(address) << ") is now in STATE_IDLE." << std::endl;
}

void Junior::handleCommands() {
    // Implement logic to handle various commands
    // For simplicity, we will simulate command execution

    if (state == STATE_IDLE) {
        if (deviceType == JUNIOR_MOTOR) {
            std::cout << "Junior (Address: " << static_cast<int>(address) << ") executing MotorCommand" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));  // Simulate command execution time
        } else if (deviceType == JUNIOR_PUMP) {
            std::cout << "Junior (Address: " << static_cast<int>(address) << ") executing PumpCommand" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));  // Simulate command execution time
        }
        state = STATE_IDLE;  // Command execution completed, return to IDLE state
    }
}


void Junior::executeMotorCommand() {
    if (state == STATE_IDLE) {
        // Implement logic to execute a MotorCommand
        // You can use the motor configuration parameters here
        std::cout << "Junior (Address: " << static_cast<int>(address) << ") executing MotorCommand" << std::endl;
        
        // Example motor control:
        MotorConfiguration motorConfig;
        if (deviceType == JUNIOR_MOTOR) {
            motorConfig.torqueRegulationIdle = device["motor"]["torqueRegulationIdle"].asUInt();
            motorConfig.torqueRegulation = device["motor"]["torqueRegulation"].asUInt();
            motorConfig.overcurrentThreshold = device["motor"]["overcurrentThreshold"].asUInt();
            motorConfig.stepSelection = device["motor"]["stepSelection"].asUInt();
            motorConfig.motorDriverType = device["motor"]["motorDriverType"].asUInt();
            motorConfig.isClockwiseWired = device["motor"]["isClockwiseWired"].asBool();
        }
 
        
        // Simulate motor operation
        std::this_thread::sleep_for(std::chrono::seconds(2));  // Simulate command execution time
        
        // After executing the command, set the state back to IDLE
        state = STATE_IDLE;
        
        // Optionally, send a state update to the Senior
        sendStateUpdate();
    }
    state = STATE_BUSY;
    handleCommands();  // Simulate execution for simplicity	
}


void Junior::executePumpCommand() {
    if (state == STATE_IDLE) {
        // Implement logic to execute a PumpCommand
        // You can use the pump configuration parameters here
        std::cout << "Junior (Address: " << static_cast<int>(address) << ") executing PumpCommand" << std::endl;
        
        // Example pump control:
        // You can choose the pump to control based on the pump's index
        int pumpIndex = /* retrieve the pump index from the PumpCommand */;
        int pumpType = /* retrieve the pump type from the PumpCommand */;
        
        // Depending on the pump type and index, control the corresponding pump
        // Simulate pump operation
        std::this_thread::sleep_for(std::chrono::seconds(1));  // Simulate command execution time
        
        // After executing the command, set the state back to IDLE
        state = STATE_IDLE;
        
        // Optionally, send a state update to the Senior
        sendStateUpdate();
    }
	state = STATE_BUSY;
    handleCommands();  // Simulate execution for simplicity

}
void Junior::executePumpCommand(int pumpIndex, int pumpType) {
    if (state == STATE_IDLE) {
        // Implement logic to execute a PumpCommand based on pumpIndex and pumpType
        std::cout << "Junior (Address: " << static_cast<int>(address) << ") executing PumpCommand" << std::endl;

        // Check the pumpType and index to determine which pump to control
        switch (pumpType) {
            case 1:
                // Control the pump with pumpIndex based on pumpType 1 logic
                std::cout << "Controlling pump Type 1, Index: " << pumpIndex << std::endl;
                break;
            case 2:
                // Control the pump with pumpIndex based on pumpType 2 logic
                std::cout << "Controlling pump Type 2, Index: " << pumpIndex << std::endl;
                break;
            case 3:
                // Control the pump with pumpIndex based on pumpType 3 logic
                std::cout << "Controlling pump Type 3, Index: " << pumpIndex << std::endl;
                break;
            // Add more cases for other pumpTypes as needed
            default:
                std::cerr << "Unsupported pumpType: " << pumpType << std::endl;
                break;
        }

        // Simulate pump operation
        std::this_thread::sleep_for(std::chrono::seconds(1));  // Simulate command execution time

        // After executing the command, set the state back to IDLE
        state = STATE_IDLE;

        // Optionally, send a state update to the Senior
        sendStateUpdate();
    }
}



void Junior::sendStateUpdate() {
    // Send a status update to the Senior
    // Include Junior's address, device type, and state
    std::string message = "Address: " + std::to_string(address) + ", Device Type: " +
        std::to_string(deviceType) + ", State: " + (state == STATE_IDLE ? "IDLE" : "BUSY");
    serialPort.sendMessage(const_cast<char*>(message.c_str()), message.length());
}

void Junior::run() {
    while (true) {
        handleCommands();
        sendStateUpdate();
        // Implement other logic as needed
    }
}

int main(int argc, char** argv) {
    if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " <RX port> <TX port> <address> <device type>" << std::
