################################################################################
# Wordle in a console
# @Author: Marek Poláček (Polda18)
# @version 1.0
# @date 2025-03-12
# @license GPL-3.0
# @see https://github.com/Polda18/CWordle
################################################################################

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c17

# Directories
SRC_DIR = ./src
TARGET_DIR = ./target
ZIP_DIR = ./zip

# Source and object files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(TARGET_DIR)/%.o, $(SRCS))

# Executable names
EXEC_LINUX = wordle
EXEC_WIN = wordle.exe

# Environment commands
ifeq ($(OS),Windows_NT)
  EXEC = $(EXEC_WIN)
  ZIP_FILE = wordle_win64.zip
  MKDIR = if not exist $(subst /,\,$(1)) mkdir $(subst /,\,$(1))
  RM_CMD = rmdir /S /Q $(subst /,\,$(TARGET_DIR)) 2>NUL || echo Target directory already clean
  CP_CMD = copy $(subst /,\,$(TARGET_DIR))\$(EXEC) $(subst /,\,$(ZIP_DIR))
  # Windows-specific zip command using PowerShell
  ZIP_CMD = powershell -Command "Compress-Archive -Path '$(subst /,\,$(ZIP_DIR))\*' -DestinationPath '$(subst /,\,$(TARGET_DIR))\$(ZIP_FILE)' -Force"
else
  EXEC = $(EXEC_LINUX)
  ZIP_FILE = wordle_linux_x64.zip
  MKDIR = mkdir -p $(1)
  RM_CMD = rm -rf $(TARGET_DIR)
  CP_CMD = cp $(TARGET_DIR)/$(EXEC) $(ZIP_DIR)
  # Linux zip command
  ZIP_CMD = cd $(ZIP_DIR) && zip -r ../$(TARGET_DIR)/$(ZIP_FILE) *
endif

# Default target
all: $(TARGET_DIR)/$(EXEC)

# Create target directory
$(TARGET_DIR):
	$(call MKDIR,$(TARGET_DIR))

# Create zip directory
$(ZIP_DIR):
	$(call MKDIR,$(ZIP_DIR))

# Compile object files
$(TARGET_DIR)/%.o: $(SRC_DIR)/%.c | $(TARGET_DIR)
ifeq ($(OS),Windows_NT)
	$(CC) $(CFLAGS) -c $< -o $@
else
	$(CC) $(CFLAGS) -c $^ -o $@
endif

# Link executable
$(TARGET_DIR)/$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

# Package into ZIP file
zip: all $(ZIP_DIR)
	$(CP_CMD)
	$(ZIP_CMD)

# Clean target
clean:
	$(RM_CMD)

.PHONY: all zip clean

