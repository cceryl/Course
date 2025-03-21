default: all

COMPILER        = g++
MIN_GCC_VERSION = 13.1.0
BUILDTYPE      ?= debug
GCC_VERSION     = $(shell $(COMPILER) -dumpfullversion)

ifeq ($(BUILDTYPE), debug)
CPPFLAGS = -fdiagnostics-color=always -g -Wall -Wextra -Werror -std=c++20
else ifeq ($(BUILDTYPE), release)
CPPFLAGS = -fdiagnostics-color=always -O3 -march=native -Wall -Wextra -Werror -std=c++20 -static -static-libgcc -static-libstdc++
else
	$(error Invalid BUILDTYPE: $(BUILDTYPE))
endif

ifeq ($(OS), Windows_NT)
	VERSION_FAIL = $(shell powershell (New-Object Version '$(GCC_VERSION)') -lt (New-Object Version '$(MIN_GCC_VERSION)'))
else
	VERSION_FAIL = $(shell expr $(GCC_VERSION) '<' $(MIN_GCC_VERSION))
endif

check_gcc_version:
ifeq ($(VERSION_FAIL), True)
	$(error GCC version $(MIN_GCC_VERSION) or higher is required)
else ifeq ($(VERSION_FAIL), 1)
	$(error GCC version $(MIN_GCC_VERSION) or higher is required)
endif



SRC_DIR  = source
OBJ_DIR  = build
SOURCES  = Core.cpp $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS  = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))
INCLUDES = -Iinclude
ifeq ($(OS), Windows_NT)
	LIBRARY += -lws2_32 
	TARGET   = Core.exe
else
	TARGET   = Core
endif



all: check_gcc_version $(TARGET)

$(TARGET): $(OBJECTS)
	$(COMPILER) $(CPPFLAGS) $(INCLUDES) $^ -o $@ $(LIBRARY)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
ifeq ($(OS), Windows_NT)
	powershell "New-Item -ItemType Directory -Force -Path '.\$(OBJ_DIR)' | Out-Null"
else
	mkdir -p $(OBJ_DIR)
endif
	$(COMPILER) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

clean:
ifeq ($(OS), Windows_NT)
	powershell Remove-Item -Path *.exe -Force -Recurse
	powershell if (Test-Path "./$(OBJ_DIR)") { Remove-Item -Path "./$(OBJ_DIR)" -Force -Recurse }
else
	rm -rf *.exe $(OBJ_DIR)
endif
