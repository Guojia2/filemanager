# wx-config to use, may be overridden on make command line.
WX_CONFIG := wx-config

WX_CXXFLAGS := $(shell $(WX_CONFIG) --cxxflags)
WX_LIBS := $(shell $(WX_CONFIG) --libs)

CXX := clang++
CXXFLAGS := -std=c++17 -Wall -Wextra

SRC_DIR := src
OBJ_DIR := obj

OBJECTS := \
	$(OBJ_DIR)/FileManagerApp.o \
	$(OBJ_DIR)/MainFrame.o \
	$(OBJ_DIR)/FilePanel.o

TARGET := filemanager

$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(WX_LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) -c -o $@ $(WX_CXXFLAGS) $(CXXFLAGS) $<

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: clean
