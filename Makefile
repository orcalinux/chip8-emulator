##############################################################################
# Compiler and Flags
##############################################################################
CC        = gcc
CFLAGS    = -std=c17 -Wall -Wextra -Werror -Iinclude
SDL_FLAGS = `sdl2-config --cflags --libs` -lSDL2_ttf -lSDL2_mixer

# Optional debug mode: make DEBUG=1
ifdef DEBUG
  CFLAGS += -g -O0
else
  CFLAGS += -O2
endif

##############################################################################
# Project Directories
##############################################################################
SRC_DIR   = src
OBJ_DIR   = obj
BIN_DIR   = bin

##############################################################################
# Output Binary
##############################################################################
TARGET    = chip8

##############################################################################
# Source Files and Corresponding Object Files
##############################################################################
# Find all .c files in SRC_DIR
SRCS      = $(wildcard $(SRC_DIR)/*.c)

# Convert each .c file in SRCS into a .o file in OBJ_DIR
OBJS      = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Create .d files to track header dependencies
DEPS      = $(OBJS:.o=.d)

##############################################################################
# Phony Targets
##############################################################################
.PHONY: all clean clean-all help

##############################################################################
# Default Target
##############################################################################
all: $(BIN_DIR)/$(TARGET)

##############################################################################
# Link Final Executable
##############################################################################
$(BIN_DIR)/$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	@$(CC) $(CFLAGS) -o $@ $^ $(SDL_FLAGS)
	@echo "[LINK]  $@"

##############################################################################
# Compile Each .c -> .o (+ Generate .d for Dependencies)
##############################################################################
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@ $(SDL_FLAGS)
	@echo "[CC]    $< -> $@"

##############################################################################
# Auto-Include Dependency Files
##############################################################################
-include $(DEPS)

##############################################################################
# Utility Targets
##############################################################################
help:
	@echo "-------------------------------------------------------"
	@echo " Available targets:"
	@echo "   make [all]     - Build the project (default)"
	@echo "   make DEBUG=1   - Build in debug mode (-g -O0)"
	@echo "   make clean     - Remove object files and binary"
	@echo "   make clean-all - Remove all build artifacts (.o, .d, binary)"
	@echo "   make help      - Print this help message"
	@echo "-------------------------------------------------------"

clean:
	@rm -f $(OBJ_DIR)/*.o $(BIN_DIR)/$(TARGET)
	@echo "[CLEAN] Removed object files and binary."

clean-all: clean
	@rm -f $(OBJ_DIR)/*.o $(BIN_DIR)/$(TARGET) $(OBJ_DIR)/*.d
	@echo "[CLEAN-ALL] Removed object, binary and dependency files."
