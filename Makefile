# Compiler and flags
CXX = g++
CXXFLAGS = -Iinclude -std=c++17
SRC_DIR = src
BUILD_DIR = build

# Source files
COMMON_SOURCES = $(SRC_DIR)/Packet.cpp $(SRC_DIR)/UDPSocket.cpp $(SRC_DIR)/Utils.cpp
SENDER_SOURCES = $(SRC_DIR)/Sender.cpp
RECEIVER_SOURCES = $(SRC_DIR)/Receiver.cpp

# Targets
all: $(BUILD_DIR)/sender $(BUILD_DIR)/receiver

# Build sender
$(BUILD_DIR)/sender: main_sender.cpp $(COMMON_SOURCES) $(SENDER_SOURCES)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ main_sender.cpp $(COMMON_SOURCES) $(SENDER_SOURCES)

# Build receiver
$(BUILD_DIR)/receiver: main_receiver.cpp $(COMMON_SOURCES) $(RECEIVER_SOURCES)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ main_receiver.cpp $(COMMON_SOURCES) $(RECEIVER_SOURCES)

# Clean build files
clean:
	rm -rf $(BUILD_DIR)
