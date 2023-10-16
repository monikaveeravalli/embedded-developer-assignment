# Makefile for Senior and Junior applications

CXX := g++
CXXFLAGS := -std=c++17 -Wall
LDFLAGS := -lstdc++fs  # Link against filesystem library (for Senior app)

# Senior application
SRCS_SENIOR := Senior.cpp SerialPort.cpp  # Add more source files if needed
OBJS_SENIOR := $(SRCS_SENIOR:.cpp=.o)
EXEC_SENIOR := senior

# Junior application
SRCS_JUNIOR := Junior.cpp SerialPort.cpp  # Add more source files if needed
OBJS_JUNIOR := $(SRCS_JUNIOR:.cpp=.o)
EXEC_JUNIOR := junior

.PHONY: all senior junior clean

all: senior junior

senior: $(OBJS_SENIOR)
$(CXX) $(CXXFLAGS) -o $(EXEC_SENIOR) $(OBJS_SENIOR) $(LDFLAGS)

junior: $(OBJS_JUNIOR)
$(CXX) $(CXXFLAGS) -o $(EXEC_JUNIOR) $(OBJS_JUNIOR)

%.o: %.cpp
$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
rm -f $(OBJS_SENIOR) $(EXEC_SENIOR) $(OBJS_JUNIOR) $(EXEC_JUNIOR)
