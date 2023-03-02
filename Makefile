# Compiler options.
CC = gcc
CXX = g++
NASM = nasm
WINDRES=windres
CFLAGS=-m32 -g -Wall -std=gnu++11
CFLAGS_TESTLIB=-m32 -g -fPIC -Wall -std=gnu++11 -DTESTING_LIBRARY
CFLAGS_ZLIB=-m32
CFLAGS_ZLIB_TEST=-m32 -fPIC
LFLAGS=-m32
LFLAGS_TESTLIB=-m32 -shared

ifeq ($(OS),Windows_NT)
NASMFLAGS=-f win -dWin32 --prefix _
LLIBS=-static -mwindows -lws2_32 -lwinmm
else
NASMFLAGS=-f elf
LLIBS=-lpthread -lstdc++
endif

# Setup binary names.
ifeq ($(OS),Windows_NT)
BIN_NAME=cod2rev
LIB_NAME=libcod2rev
BIN_EXT=.exe
LIB_EXT=.dll
else
BIN_NAME=cod2rev_lnxded
LIB_NAME=libcod2rev
BIN_EXT=
LIB_EXT=.so
endif

# Setup directory names.
BIN_DIR=bin
OBJ_DIR=obj
TEST_OBJ_DIR=obj_test
SRC_DIR=src
TEST_DIR=test
LINUX_DIR=$(SRC_DIR)/unix
WIN32_DIR=$(SRC_DIR)/win32
ZLIB_DIR=$(SRC_DIR)/zlib
ifeq ($(OS),Windows_NT)
TESTLIB_DIR=$(TEST_DIR)/windows
else
TESTLIB_DIR=$(TEST_DIR)/linux
endif

# Source dirs
BOTLIB_DIR=$(SRC_DIR)/botlib
CLIENTSCR_DIR=$(SRC_DIR)/clientscript
GAME_DIR=$(SRC_DIR)/game
QCOMMON_DIR=$(SRC_DIR)/qcommon
SERVER_DIR=$(SRC_DIR)/server
STRINGED_DIR=$(SRC_DIR)/stringed
UNIVERSAL_DIR=$(SRC_DIR)/universal
XANIM_DIR=$(SRC_DIR)/xanim

# Target files
TARGET=$(addprefix $(BIN_DIR)/,$(BIN_NAME)$(BIN_EXT))
TARGET_TESTLIB=$(addprefix $(BIN_DIR)/,$(LIB_NAME)$(LIB_EXT))

# C files
BOTLIB_SOURCES=$(wildcard $(BOTLIB_DIR)/*.cpp)
CLIENTSCR_SOURCES=$(wildcard $(CLIENTSCR_DIR)/*.cpp)
GAME_SOURCES=$(wildcard $(GAME_DIR)/*.cpp)
QCOMMON_SOURCES=$(wildcard $(QCOMMON_DIR)/*.cpp)
SERVER_SOURCES=$(wildcard $(SERVER_DIR)/*.cpp)
STRINGED_SOURCES=$(wildcard $(STRINGED_DIR)/*.cpp)
UNIVERSAL_SOURCES=$(wildcard $(UNIVERSAL_DIR)/*.cpp)
XANIM_SOURCES=$(wildcard $(XANIM_DIR)/*.cpp)
TESTLIB_SOURCES=$(wildcard $(TESTLIB_DIR)/*.cpp)
LINUX_SOURCES=$(wildcard $(LINUX_DIR)/*.cpp)
WIN32_SOURCES=$(wildcard $(WIN32_DIR)/*.cpp)
WIN32_RESOURCES=$(wildcard $(WIN32_DIR)/*.rc)
ZLIB_SOURCES=$(wildcard $(ZLIB_DIR)/*.c)

# ASM files
ASM_BOTLIB_SOURCES=$(wildcard $(BOTLIB_DIR)/asm/*.asm)
ASM_CLIENTSCR_SOURCES=$(wildcard $(CLIENTSCR_DIR)/asm/*.asm)
ASM_GAME_SOURCES=$(wildcard $(GAME_DIR)/asm/*.asm)
ASM_QCOMMON_SOURCES=$(wildcard $(QCOMMON_DIR)/asm/*.asm)
ASM_SERVER_SOURCES=$(wildcard $(SERVER_DIR)/asm/*.asm)
ASM_STRINGED_SOURCES=$(wildcard $(STRINGED_DIR)/asm/*.asm)
ASM_UNIVERSAL_SOURCES=$(wildcard $(UNIVERSAL_DIR)/asm/*.asm)
ASM_XANIM_SOURCES=$(wildcard $(XANIM_DIR)/asm/*.asm)

# Object files lists.
BOTLIB_OBJ=$(patsubst $(BOTLIB_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(BOTLIB_SOURCES))
CLIENTSCR_OBJ=$(patsubst $(CLIENTSCR_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CLIENTSCR_SOURCES))
GAME_OBJ=$(patsubst $(GAME_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(GAME_SOURCES))
QCOMMON_OBJ=$(patsubst $(QCOMMON_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(QCOMMON_SOURCES))
SERVER_OBJ=$(patsubst $(SERVER_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SERVER_SOURCES))
STRINGED_OBJ=$(patsubst $(STRINGED_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(STRINGED_SOURCES))
UNIVERSAL_OBJ=$(patsubst $(UNIVERSAL_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(UNIVERSAL_SOURCES))
XANIM_OBJ=$(patsubst $(XANIM_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(XANIM_SOURCES))

# ASM Object files lists.
ASM_BOTLIB_OBJ=$(patsubst $(BOTLIB_DIR)/asm/%.asm,$(OBJ_DIR)/%.o,$(ASM_BOTLIB_SOURCES))
ASM_CLIENTSCR_OBJ=$(patsubst $(CLIENTSCR_DIR)/asm/%.asm,$(OBJ_DIR)/%.o,$(ASM_CLIENTSCR_SOURCES))
ASM_GAME_OBJ=$(patsubst $(GAME_DIR)/asm/%.asm,$(OBJ_DIR)/%.o,$(ASM_GAME_SOURCES))
ASM_QCOMMON_OBJ=$(patsubst $(QCOMMON_DIR)/asm/%.asm,$(OBJ_DIR)/%.o,$(ASM_QCOMMON_SOURCES))
ASM_SERVER_OBJ=$(patsubst $(SERVER_DIR)/asm/%.asm,$(OBJ_DIR)/%.o,$(ASM_SERVER_SOURCES))
ASM_STRINGED_OBJ=$(patsubst $(STRINGED_DIR)/asm/%.asm,$(OBJ_DIR)/%.o,$(ASM_STRINGED_SOURCES))
ASM_UNIVERSAL_OBJ=$(patsubst $(UNIVERSAL_DIR)/asm/%.asm,$(OBJ_DIR)/%.o,$(ASM_UNIVERSAL_SOURCES))
ASM_XANIM_OBJ=$(patsubst $(XANIM_DIR)/asm/%.asm,$(OBJ_DIR)/%.o,$(ASM_XANIM_SOURCES))

# Platform specific lists
ifeq ($(OS),Windows_NT)
WIN32_OBJ=$(patsubst $(WIN32_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(WIN32_SOURCES))
WIN32_RES_OBJ=$(patsubst $(WIN32_DIR)/%.rc,$(OBJ_DIR)/%.res,$(WIN32_RESOURCES))
else
LINUX_OBJ=$(patsubst $(LINUX_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(LINUX_SOURCES))
endif
ZLIB_OBJ=$(patsubst $(ZLIB_DIR)/%.c,$(OBJ_DIR)/%.o,$(ZLIB_SOURCES))

# Object files lists for testing library.
BOTLIB_TEST_OBJ=$(patsubst $(BOTLIB_DIR)/%.cpp,$(TEST_OBJ_DIR)/%.o,$(BOTLIB_SOURCES))
CLIENTSCR_TEST_OBJ=$(patsubst $(CLIENTSCR_DIR)/%.cpp,$(TEST_OBJ_DIR)/%.o,$(CLIENTSCR_SOURCES))
GAME_TEST_OBJ=$(patsubst $(GAME_DIR)/%.cpp,$(TEST_OBJ_DIR)/%.o,$(GAME_SOURCES))
QCOMMON_TEST_OBJ=$(patsubst $(QCOMMON_DIR)/%.cpp,$(TEST_OBJ_DIR)/%.o,$(QCOMMON_SOURCES))
SERVER_TEST_OBJ=$(patsubst $(SERVER_DIR)/%.cpp,$(TEST_OBJ_DIR)/%.o,$(SERVER_SOURCES))
STRINGED_TEST_OBJ=$(patsubst $(STRINGED_DIR)/%.cpp,$(TEST_OBJ_DIR)/%.o,$(STRINGED_SOURCES))
UNIVERSAL_TEST_OBJ=$(patsubst $(UNIVERSAL_DIR)/%.cpp,$(TEST_OBJ_DIR)/%.o,$(UNIVERSAL_SOURCES))
XANIM_TEST_OBJ=$(patsubst $(XANIM_DIR)/%.cpp,$(TEST_OBJ_DIR)/%.o,$(XANIM_SOURCES))

# Testing library obj
TESTLIB_OBJ=$(patsubst $(TESTLIB_DIR)/%.cpp,$(TEST_OBJ_DIR)/%.o,$(TESTLIB_SOURCES))

# Object files lists for testing library (asm).
ASM_BOTLIB_TEST_OBJ=$(patsubst $(BOTLIB_DIR)/asm/%.asm,$(TEST_OBJ_DIR)/%.o,$(ASM_BOTLIB_SOURCES))
ASM_CLIENTSCR_TEST_OBJ=$(patsubst $(CLIENTSCR_DIR)/asm/%.asm,$(TEST_OBJ_DIR)/%.o,$(ASM_CLIENTSCR_SOURCES))
ASM_GAME_TEST_OBJ=$(patsubst $(GAME_DIR)/asm/%.asm,$(TEST_OBJ_DIR)/%.o,$(ASM_GAME_SOURCES))
ASM_QCOMMON_TEST_OBJ=$(patsubst $(QCOMMON_DIR)/asm/%.asm,$(TEST_OBJ_DIR)/%.o,$(ASM_QCOMMON_SOURCES))
ASM_SERVER_TEST_OBJ=$(patsubst $(SERVER_DIR)/asm/%.asm,$(TEST_OBJ_DIR)/%.o,$(ASM_SERVER_SOURCES))
ASM_STRINGED_TEST_OBJ=$(patsubst $(STRINGED_DIR)/asm/%.asm,$(TEST_OBJ_DIR)/%.o,$(ASM_STRINGED_SOURCES))
ASM_UNIVERSAL_TEST_OBJ=$(patsubst $(UNIVERSAL_DIR)/asm/%.asm,$(TEST_OBJ_DIR)/%.o,$(ASM_UNIVERSAL_SOURCES))
ASM_XANIM_TEST_OBJ=$(patsubst $(XANIM_DIR)/asm/%.asm,$(TEST_OBJ_DIR)/%.o,$(ASM_XANIM_SOURCES))

# Platform specific lists for testing library.
ifeq ($(OS),Windows_NT)
WIN32_TEST_OBJ=$(patsubst $(WIN32_DIR)/%.cpp,$(TEST_OBJ_DIR)/%.o,$(WIN32_SOURCES))
else
LINUX_TEST_OBJ=$(patsubst $(LINUX_DIR)/%.cpp,$(TEST_OBJ_DIR)/%.o,$(LINUX_SOURCES))
endif
ZLIB_TEST_OBJ=$(patsubst $(ZLIB_DIR)/%.c,$(TEST_OBJ_DIR)/%.o,$(ZLIB_SOURCES))

# Default rule.
cod2rev: mkdir $(TARGET)
    $(TARGET): \
	$(ASM_BOTLIB_OBJ) $(ASM_CLIENTSCR_OBJ) $(ASM_GAME_OBJ) $(ASM_QCOMMON_OBJ) $(ASM_SERVER_OBJ) $(ASM_STRINGED_OBJ) \
	$(ASM_UNIVERSAL_OBJ) $(ASM_XANIM_OBJ) \
	$(BOTLIB_OBJ) $(CLIENTSCR_OBJ) $(GAME_OBJ) $(QCOMMON_OBJ) $(SERVER_OBJ) $(STRINGED_OBJ) \
	$(UNIVERSAL_OBJ) $(XANIM_OBJ) $(LINUX_OBJ) $(WIN32_OBJ) $(WIN32_RES_OBJ) $(ZLIB_OBJ)
	$(CXX) $(LFLAGS) -o $@ $^ $(LLIBS)

test: mkdir_test $(TARGET_TESTLIB)
    $(TARGET_TESTLIB): \
	$(ASM_BOTLIB_TEST_OBJ) $(ASM_CLIENTSCR_TEST_OBJ) $(ASM_GAME_TEST_OBJ) $(ASM_QCOMMON_TEST_OBJ) $(ASM_SERVER_TEST_OBJ) \
	$(ASM_STRINGED_TEST_OBJ) $(ASM_UNIVERSAL_TEST_OBJ) $(ASM_XANIM_TEST_OBJ) \
	$(BOTLIB_TEST_OBJ) $(CLIENTSCR_TEST_OBJ) $(GAME_TEST_OBJ) $(QCOMMON_TEST_OBJ) $(SERVER_TEST_OBJ) \
	$(STRINGED_TEST_OBJ) $(UNIVERSAL_TEST_OBJ) $(XANIM_TEST_OBJ) \
	$(LINUX_TEST_OBJ) $(WIN32_TEST_OBJ) $(TESTLIB_OBJ) $(ZLIB_TEST_OBJ)
	$(CXX) $(LFLAGS_TESTLIB) -o $@ $^ $(LLIBS)

ifeq ($(OS),Windows_NT)
mkdir:
	if not exist $(BIN_DIR) md $(BIN_DIR)
	if not exist $(OBJ_DIR) md $(OBJ_DIR)

mkdir_test:
	$(MKDIR) $(BIN_DIR)
	$(MKDIR) $(TEST_OBJ_DIR)
else
mkdir:
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)

mkdir_test:
	mkdir -p $(BIN_DIR)
	mkdir -p $(TEST_OBJ_DIR)
endif


# Build C sources


# A rule to build botlib source code.
$(OBJ_DIR)/%.o: $(BOTLIB_DIR)/%.cpp
	@echo $(CXX)  $@
	@$(CXX) -c $(CFLAGS) -o $@ $<

# A rule to build botlib source code (test).
$(TEST_OBJ_DIR)/%.o: $(BOTLIB_DIR)/%.cpp
	@echo $(CXX)  $@
	@$(CXX) -c $(CFLAGS_TESTLIB) -o $@ $<

# A rule to build clientscript source code.
$(OBJ_DIR)/%.o: $(CLIENTSCR_DIR)/%.cpp
	@echo $(CXX)  $@
	@$(CXX) -c $(CFLAGS) -o $@ $<

# A rule to build clientscript source code (test).
$(TEST_OBJ_DIR)/%.o: $(CLIENTSCR_DIR)/%.cpp
	@echo $(CXX)  $@
	@$(CXX) -c $(CFLAGS_TESTLIB) -o $@ $<

# A rule to build game source code.
$(OBJ_DIR)/%.o: $(GAME_DIR)/%.cpp
	@echo $(CXX)  $@
	@$(CXX) -c $(CFLAGS) -o $@ $<

# A rule to build game source code (test).
$(TEST_OBJ_DIR)/%.o: $(GAME_DIR)/%.cpp
	@echo $(CXX)  $@
	@$(CXX) -c $(CFLAGS_TESTLIB) -o $@ $<

# A rule to build qcommon source code.
$(OBJ_DIR)/%.o: $(QCOMMON_DIR)/%.cpp
	@echo $(CXX)  $@
	@$(CXX) -c $(CFLAGS) -o $@ $<

# A rule to build qcommon source code (test).
$(TEST_OBJ_DIR)/%.o: $(QCOMMON_DIR)/%.cpp
	@echo $(CXX)  $@
	@$(CXX) -c $(CFLAGS_TESTLIB) -o $@ $<

# A rule to build server source code.
$(OBJ_DIR)/%.o: $(SERVER_DIR)/%.cpp
	@echo $(CXX)  $@
	@$(CXX) -c $(CFLAGS) -o $@ $<

# A rule to build server source code (test).
$(TEST_OBJ_DIR)/%.o: $(SERVER_DIR)/%.cpp
	@echo $(CXX)  $@
	@$(CXX) -c $(CFLAGS_TESTLIB) -o $@ $<

# A rule to build stringed source code.
$(OBJ_DIR)/%.o: $(STRINGED_DIR)/%.cpp
	@echo $(CXX)  $@
	@$(CXX) -c $(CFLAGS) -o $@ $<

# A rule to build stringed source code (test).
$(TEST_OBJ_DIR)/%.o: $(STRINGED_DIR)/%.cpp
	@echo $(CXX)  $@
	@$(CXX) -c $(CFLAGS_TESTLIB) -o $@ $<

# A rule to build universal source code.
$(OBJ_DIR)/%.o: $(UNIVERSAL_DIR)/%.cpp
	@echo $(CXX)  $@
	@$(CXX) -c $(CFLAGS) -o $@ $<

# A rule to build universal source code (test).
$(TEST_OBJ_DIR)/%.o: $(UNIVERSAL_DIR)/%.cpp
	@echo $(CXX)  $@
	@$(CXX) -c $(CFLAGS_TESTLIB) -o $@ $<

# A rule to build xanim source code.
$(OBJ_DIR)/%.o: $(XANIM_DIR)/%.cpp
	@echo $(CXX)  $@
	@$(CXX) -c $(CFLAGS) -o $@ $<

# A rule to build xanim source code (test).
$(TEST_OBJ_DIR)/%.o: $(XANIM_DIR)/%.cpp
	@echo $(CXX)  $@
	@$(CXX) -c $(CFLAGS_TESTLIB) -o $@ $<

# A rule to build linux source code.
$(OBJ_DIR)/%.o: $(LINUX_DIR)/%.cpp
	@echo $(CXX)  $@
	@$(CXX) -c $(CFLAGS) -o $@ $<

# A rule to build linux source code (test).
$(TEST_OBJ_DIR)/%.o: $(LINUX_DIR)/%.cpp
	@echo $(CXX)  $@
	@$(CXX) -c $(CFLAGS_TESTLIB) -o $@ $<

# A rule to build win32 source code.
$(OBJ_DIR)/%.o: $(WIN32_DIR)/%.cpp
	@echo $(CXX)  $@
	@$(CXX) -c $(CFLAGS) -o $@ $<

# A rule to build win32 resource files.
$(OBJ_DIR)/%.res: $(WIN32_DIR)/%.rc
	@echo $(WINDRES)  $@
	@$(WINDRES) -i $< -O coff $@

# A rule to build win32 source code (test).
$(TEST_OBJ_DIR)/%.o: $(WIN32_DIR)/%.cpp
	@echo $(CXX)  $@
	@$(CXX) -c $(CFLAGS_TESTLIB) -o $@ $<

# A rule to build zlib source code.
$(OBJ_DIR)/%.o: $(ZLIB_DIR)/%.c
	@echo $(CC)  $@
	@$(CC) -c $(CFLAGS_ZLIB) -o $@ $<

# A rule to build zlib source code (test).
$(TEST_OBJ_DIR)/%.o: $(ZLIB_DIR)/%.c
	@echo $(CC)  $@
	@$(CC) -c $(CFLAGS_ZLIB_TEST) -o $@ $<

# A rule to build testlib source code.
$(TEST_OBJ_DIR)/%.o: $(TESTLIB_DIR)/%.cpp
	@echo $(CXX)  $@
	@$(CXX) -c $(CFLAGS_TESTLIB) -o $@ $<


# Build ASM sources


# A rule to build botlib source code.
$(OBJ_DIR)/%.o: $(BOTLIB_DIR)/asm/%.asm
	@echo $(NASM) $@
	@$(NASM) $(NASMFLAGS) $< -o $@

# A rule to build botlib source code (test).
$(TEST_OBJ_DIR)/%.o: $(BOTLIB_DIR)/asm/%.asm
	@echo $(NASM) $@
	@$(NASM) $(NASMFLAGS) $< -o $@

# A rule to build clientscript source code.
$(OBJ_DIR)/%.o: $(CLIENTSCR_DIR)/asm/%.asm
	@echo $(NASM) $@
	@$(NASM) $(NASMFLAGS) $< -o $@

# A rule to build clientscript source code (test).
$(TEST_OBJ_DIR)/%.o: $(CLIENTSCR_DIR)/asm/%.asm
	@echo $(NASM) $@
	@$(NASM) $(NASMFLAGS) $< -o $@

# A rule to build game source code.
$(OBJ_DIR)/%.o: $(GAME_DIR)/asm/%.asm
	@echo $(NASM) $@
	@$(NASM) $(NASMFLAGS) $< -o $@

# A rule to build game source code (test).
$(TEST_OBJ_DIR)/%.o: $(GAME_DIR)/asm/%.asm
	@echo $(NASM) $@
	@$(NASM) $(NASMFLAGS) $< -o $@

# A rule to build qcommon source code.
$(OBJ_DIR)/%.o: $(QCOMMON_DIR)/asm/%.asm
	@echo $(NASM) $@
	@$(NASM) $(NASMFLAGS) $< -o $@

# A rule to build qcommon source code (test).
$(TEST_OBJ_DIR)/%.o: $(QCOMMON_DIR)/asm/%.asm
	@echo $(NASM) $@
	@$(NASM) $(NASMFLAGS) $< -o $@

# A rule to build server source code.
$(OBJ_DIR)/%.o: $(SERVER_DIR)/asm/%.asm
	@echo $(NASM) $@
	@$(NASM) $(NASMFLAGS) $< -o $@

# A rule to build server source code (test).
$(TEST_OBJ_DIR)/%.o: $(SERVER_DIR)/asm/%.asm
	@echo $(NASM) $@
	@$(NASM) $(NASMFLAGS) $< -o $@

# A rule to build stringed source code.
$(OBJ_DIR)/%.o: $(STRINGED_DIR)/asm/%.asm
	@echo $(NASM) $@
	@$(NASM) $(NASMFLAGS) $< -o $@

# A rule to build stringed source code (test).
$(TEST_OBJ_DIR)/%.o: $(STRINGED_DIR)/asm/%.asm
	@echo $(NASM) $@
	@$(NASM) $(NASMFLAGS) $< -o $@

# A rule to build universal source code.
$(OBJ_DIR)/%.o: $(UNIVERSAL_DIR)/asm/%.asm
	@echo $(NASM) $@
	@$(NASM) $(NASMFLAGS) $< -o $@

# A rule to build universal source code (test).
$(TEST_OBJ_DIR)/%.o: $(UNIVERSAL_DIR)/asm/%.asm
	@echo $(NASM) $@
	@$(NASM) $(NASMFLAGS) $< -o $@

# A rule to build xanim source code.
$(OBJ_DIR)/%.o: $(XANIM_DIR)/asm/%.asm
	@echo $(NASM) $@
	@$(NASM) $(NASMFLAGS) $< -o $@

# A rule to build xanim source code (test).
$(TEST_OBJ_DIR)/%.o: $(XANIM_DIR)/asm/%.asm
	@echo $(NASM) $@
	@$(NASM) $(NASMFLAGS) $< -o $@


# Cleanup


ifeq ($(OS),Windows_NT)
clean:
	del /Q /S "$(BIN_DIR)\$(BIN_NAME)$(BIN_EXT)"
	del /Q /S "$(BIN_DIR)\$(LIB_NAME)$(LIB_EXT)"
	del /Q /S "$(OBJ_DIR)\*.o"
	del /Q /S "$(OBJ_DIR)\*.res"
	del /Q /S "$(TEST_OBJ_DIR)\*.o"
else
clean:
	rm -f $(BIN_DIR)/$(BIN_NAME)$(BIN_EXT)
	rm -f $(BIN_DIR)/$(LIB_NAME)$(LIB_EXT)
	rm -f $(OBJ_DIR)/*.o
	rm -f $(TEST_OBJ_DIR)/*.o
endif