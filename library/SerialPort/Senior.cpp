#include <iostream>
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>
#include "SerialPort.h"

using json = nlohmann::json;

enum class SeniorState {
    WAITING_FOR_CONFIG,
    CONFIGURING,
    IN_PRODUCTION,
};

struct ConfigureCommand {
    int address;
    JuniorState targetState;
    MotorConfig motorConfig;
    // Add other configuration parameters as needed
};

class Junior {
public:
    Junior(const char* port, uint32_t baudrate, int address, int deviceType)
        : address(address), deviceType(deviceType), state(JuniorState::STATE_NOT_INITIALIZED) {
        serialPort.connect(port, baudrate);
    }

	void sendConfigureCommand(const ConfigureCommand& command) {
        // Serialize the ConfigureCommand to JSON
        json commandJson;
        commandJson["address"] = command.address;
        commandJson["targetState"] = static_cast<int>(command.targetState);
        commandJson["motorConfig"] = {
            {"torqueRegulationIdle", command.motorConfig.torqueRegulationIdle},
            {"torqueRegulation", command.motorConfig.torqueRegulation},
            {"overcurrentThreshold", command.motorConfig.overcurrentThreshold},
            {"stepSelection", command.motorConfig.stepSelection},
            {"motorDriverType", command.motorConfig.motorDriverType},
            {"isClockwiseWired", command.motorConfig.isClockwiseWired}
        };
		// Convert the JSON to a string
        std::string commandStr = commandJson.dump();

        // Log the sent ConfigureCommand
        std::cout << "Sending ConfigureCommand to Junior " << command.address << std::endl;
        std::cout << "Command: " << commandStr << std::endl;

        // Send the ConfigureCommand through the serial port
        serialPort.sendMessage(commandStr.c_str(), commandStr.length());

        // Log that the ConfigureCommand has been sent
        std::cout << "ConfigureCommand sent to Junior " << command.address << std::endl;
    }
	JuniorState getState() const {
        return state;
    }

	
};

class Senior {
public:
    Senior(const char* port, uint32_t baudrate)
        : state(SeniorState::WAITING_FOR_CONFIG) {
        serialPort.connect(port, baudrate);
    }

    bool isAllJuniorsConfigured() {
        for (const Junior& junior : juniors) {
            if (junior.getState() != JuniorState::STATE_IDLE) {
                return false;
            }
        }
        return true;
    }

    void configureAllJuniors() {
        // Load and parse the JSON configuration file
        std::ifstream configFile("junior_config.json");
        if (!configFile.is_open()) {
            std::cerr << "Error: Failed to open configuration file." << std::endl;
            return;
        }

        json configData;
        configFile >> configData;

        // Loop through all Juniors and send ConfigureCommand
        if (configData.contains("devices")) {
            for (const auto& device : configData["devices"]) {
                int address = device["address"];
                Junior junior("COM1", 9600, address, device["deviceType"]);

                // Extract configuration parameters for this Junior
                ConfigureCommand command;
                command.address = address;
                command.targetState = JuniorState::STATE_IDLE;
                command.motorConfig.torqueRegulationIdle = device["motor"]["torqueRegulationIdle"];
                command.motorConfig.torqueRegulation = device["motor"]["torqueRegulation"];
                command.motorConfig.overcurrentThreshold = device["motor"]["overcurrentThreshold"];
                command.motorConfig.stepSelection = device["motor"]["stepSelection"];
                command.motorConfig.motorDriverType = device["motor"]["motorDriverType"];
                command.motorConfig.isClockwiseWired = device["motor"]["isClockwiseWired"];

                // Send the ConfigureCommand to the Junior
                junior.sendConfigureCommand(command);

                // Add the Junior to the list of configured Juniors
                juniors.push_back(junior);
            }
        }
    }

    void pollJuniors() {
        while (state == SeniorState::IN_PRODUCTION) {
            for (Junior& junior : juniors) {
                // Poll attributes (e.g., address, deviceType, status) from each Junior
				    pollAttributesFromJunior(junior);
                // Handle responses and update Junior properties
                // You may set a timeout for responses and retry if needed
            }
        std::this_thread::sleep_for(std::chrono::seconds(POLL_INTERVAL_SECONDS));
    }
            if (isAllJuniorsConfigured()) {
                state = SeniorState::IN_PRODUCTION;
            }
        }
    }


void Senior::pollAttributesFromJunior(Junior& junior) {
    // Construct a poll message for the Junior
    char pollMsg[POLL_MESSAGE_SIZE];
    pollMsg[0] = POLL_COMMAND;          // Define a poll command code
    pollMsg[1] = junior.getAddress();    // Set the Junior's address
    // Add other parameters as needed

    // Send the poll message to the Junior and handle responses
    char response[RESPONSE_SIZE];
    if (sendPollMessageToJunior(junior, pollMsg, response)) {
        // Process the response and update Junior properties
        junior.updateAttributesFromResponse(response);
    }
}

bool Senior::sendPollMessageToJunior(Junior& junior, const char* pollMsg, char* response) {
    // Send the poll message to the Junior via the serial port
    serialPort.sendMessage(pollMsg, POLL_MESSAGE_SIZE);

    // Wait for and read the response from the Junior
    int bytesRead = serialPort.readResponse(response, RESPONSE_SIZE, RESPONSE_TIMEOUT);

    // Check if the response was received within the timeout
    if (bytesRead < 0) {
        // Handle the case where no response was received
        return false;
    }

    // Process the response as needed
    return true;
}


private:
    SerialPort serialPort;
    SeniorState state;
    std::vector<Junior> juniors;
    // Add other Senior-specific properties and methods here
};

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: senior_app <port> <baudrate>" << std::endl;
        return 1;
    }

    const char* port = argv[1];
    uint32_t baudrate = std::stoi(argv[2]);

    Senior senior(port, baudrate);

    // Main application loop
    while (true) {
        if (senior.state == SeniorState::WAITING_FOR_CONFIG) {
            // Read configuration from devices folder
            // Transition to CONFIGURING state
            senior.state = SeniorState::CONFIGURING;
        } else if (senior.state == SeniorState::CONFIGURING) {
            // Configure all Juniors
            senior.configureAllJuniors();

            // Transition to IN_PRODUCTION state if all Juniors are configured
            if (senior.isAllJuniorsConfigured()) {
                senior.state = SeniorState::IN_PRODUCTION;
            }
        } else if (senior.state == SeniorState::IN_PRODUCTION) {
            // Poll Juniors
            senior.pollJuniors();
        }

    }

    return 0;
}
