# This file is based on "Makefile Cookbook" from the https://makefiletutorial.com/.

# TARGET name must contain only [a-zA-Z0-9_]+
TARGET := bwapi_bot.exe
CXX := i686-w64-mingw32-g++

SRC_DIR := ./src
BIN_DIR := ./bin
SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(SRCS:%=$(BIN_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

# Every folder in ./src will need to be passed to GCC so that it can find header files
INC_DIRS := $(shell find $(SRC_DIR) -type d)
# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA. GCC understands this -I flag
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# The -MMD and -MP flags together generate Makefiles for us!
# These files will have .d instead of .o as the output.
CPPFLAGS := $(INC_FLAGS) -MMD -MP
CXXFLAGS := -Wall -Wextra -Wall -O2 --std=c++20

# The final build step.
$(BIN_DIR)/$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# Build step for C++ source
$(BIN_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	rm -r $(BIN_DIR)/src $(BIN_DIR)/$(TARGET)
