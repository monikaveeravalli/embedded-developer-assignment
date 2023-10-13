# Assessment Embedded Software Developer

This is a repository that contains information for the Embedded Software Developer Assessment. 
Create your own repository on GitHub/Gitlab with the project files and share the link to your repository with us.
 
## 1. Context

Below is a short explanation of how our machine works: a machine consists of multiple devices connected through a shared RS485 bus. 

### Junior
Each Junior has a unique address ranging from 1..253. This is configured with 8 DIP-switches on the PCB.
Upon boot, the Junior's state will be `STATE_NOT_INITIALIZED`. Only when the Junior receives a `ConfigureCommand`, it's state will become `STATE_IDLE` after this command finishes. The Junior can also receive other commands (e.g: `MotorRunToPositionCommand`, or `PumpCommand`). When a Junior receives such a command, it will immediately start executing it and the Junior's state will be `STATE_BUSY`. When finished, it's state will be `STATE_IDLE` again.


There are different Junior deviceTypes, for example we have a `JUNIOR_MOTOR` (1) that can run MotorCommands, but also a `JUNIOR_PUMP` (2) that accepts PumpCommands to send pulses to a Pump to dose a certain volume of an perfume ingredient.

### Senior
There is 1 master device (_Senior_) that keeps track of every Junior's properties (e.g. state, deviceType, version, currentCommandId) by regularly polling each known Junior. 
The Senior can also send a command to the Junior, for example the ConfigureCommand, to configure the Junior to do some stuff
This Senior is a Linux C++ application that sends and receives bus messages through a serial UART port.


### RS485 Bus 
All devices are listening to the serial bus, so this means that all devices receive all sent messages, but only the Junior with the address of payload.receiverAddress should respond accordingly.

## 2. Assignment

### Pre-requisites
First, install `socat` to be able to create byte streams:
```
$ apt update
$ apt install socat
```

Make `start.sh` executable (only needed once of course) and run the script to create the ports:
```
$ sudo chmod +x start.sh
$ ./start.sh
```
This process should be kept open to keep the bytestreams open during the running of the applications.

In both applications, you may use the SerialPort class in `library/SerialPort` to open the byte stream as a serial port.




### Assignment
As explained above the machine works using a physical RS485 bus. We want you to create a simplified virtual simulation of the machine that can be used to run tests to test RS485 Bus communication. To simulate the serial port we are going to use `socat`. 

Your job is to write two applications: the Senior and the Junior.
Using the socat tunnels as virtual bus, write the communication logic for the Senior and the Junior.
There should be running one instance of the Senior (address: 254) and 5 instances of the Junior app running (see `devices` folder)
Use a bash script (or extend `start.sh`) to start all instances. Each executable should take command-line parameters for RX port, TX port, address, deviceType.

 The Junior application should have initially state `STATE_NOT_INITIALIZED`. Only after it receives a `ConfigureCommand` for this particular junior with this address, this junior will initialize "it's peripherals"  and then this Junior can change it's status to `STATUS_IDLE`. Show this also by logging this to the console.
 
 Senior should begin in a state `WAITING_FOR_CONFIG`. In this state it will read the configuration from the `devices` folder and will then move to `CONFIGURING` state. In this state it will send a `ConfigureCommand` to each Junior containing the configuration from the JSON. The senior should also regularly poll attributes (such as address, deviceType, status) from each Junior to keep track of all juniors. It will try to configure each Junior until all Juniors have `STATE_IDLE`. If so, the Senior's state will move to `IN_PRODUCTION`.

The goal of this assignment is to have a Senior application in state `IN_PRODUCTION`, e.g. regularly polling all Junior Applications through the virtual bus and responding with `STATE_IDLE`.



## 3. Tips

- Neatly written and properly formatted production-level code
- Demonstrate Linux Bash scripting
- Demonstration of knowledge of Object Oriented Programming in C/C++
- Demonstrate shared usage of classes between applications (parsing etc)
- Demonstrate logging skills
- Implementing additional commands such as `MotorRunToPositionCommand`, is appreciated.


Good luck!

