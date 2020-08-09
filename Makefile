CXX      := -c++
CXXFLAGS := -std=c++17 -pedantic-errors -Wall -Wextra #-Werror
LDFLAGS  := -L/usr/lib -lstdc++ -lm -lz
BUILD    := ./build
#do NOT blank this var
OBJ_DIR  := $(BUILD)/objects
#APP_DIR  := .
APP_DIR  := $(BUILD)
#APP_DIR  := $(BUILD)/apps
TARGET   := tileSAT
INCLUDE  := -Iinclude/
SRC      :=                      \
	$(wildcard src/exemple/*.cpp) \
	$(wildcard src/*.cpp)         \

OBJECTS  := $(SRC:%.cpp=$(OBJ_DIR)/%.o)

all: build $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@ $(LDFLAGS)

$(APP_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $(APP_DIR)/$(TARGET) $^ $(LDFLAGS)

.PHONY: all build clean debug release

build:
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR)

debug: CXXFLAGS += -DDEBUG -g
debug: all

release: CXXFLAGS += -O2 -s
release: all

clean:
	-@rm -rv $(OBJ_DIR)/*
	-@rm -v $(APP_DIR)/$(TARGET)