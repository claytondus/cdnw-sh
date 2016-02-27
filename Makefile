# ==========================================
# cdnwsh - COSC 560 Programming Assignment 1
# Clayton Davis, Nathan Wilder
# ==========================================

CLEANUP = rm -rf
MKDIR = mkdir -p

UNITY_ROOT=contrib/unity
C_COMPILER=gcc

CFLAGS=
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Werror 
CFLAGS += -Wpointer-arith
CFLAGS += -Wcast-align
CFLAGS += -Wwrite-strings
CFLAGS += -Wswitch-default
CFLAGS += -Wunreachable-code
CFLAGS += -Winit-self
CFLAGS += -Wmissing-field-initializers
CFLAGS += -Wno-unknown-pragmas
CFLAGS += -Wstrict-prototypes
CFLAGS += -Wundef
CFLAGS += -Wold-style-definition

TEST_TARGET = all_tests.o
DEBUG_TARGET = debug_shell.o
TEST_SRC_FILES=\
$(UNITY_ROOT)/src/unity.c \
$(UNITY_ROOT)/extras/fixture/src/unity_fixture.c \
src/*.c \
test/*.c \
test/test_runners/*.c
DEBUG_SRC_FILES=\
src/blockdev.c src/client.c src/server.c src/shell.c src/cdnwsh.c src/bitmap.c src/inode.c src/fs.c
TEST_INC_DIRS=-Isrc -Iinclude -I$(UNITY_ROOT)/src -I$(UNITY_ROOT)/extras/fixture/src
DEBUG_INC_DIRS=-Isrc -Iinclude 
TEST_LDFLAGS = 
TEST_SYMBOLS=-DUNITY_FIXTURES

.PHONY: clean test

all: debug test

debug: 
	$(C_COMPILER) -g -O0 $(CFLAGS) $(DEBUG_INC_DIRS) $(DEBUG_SRC_FILES) -o db/$(DEBUG_TARGET)
	
test:
	$(C_COMPILER) -g -O0 $(CFLAGS) $(TEST_INC_DIRS) $(TEST_LDFLAGS) $(TEST_SYMBOLS) $(TEST_SRC_FILES)  -o test/$(TEST_TARGET)
	./test/$(TEST_TARGET)

clean:
	$(CLEANUP) *.o