# ==========================================
# cdnwsh - COSC 560 Programming Assignment 1
# Clayton Davis, Nathan Wilder
# ==========================================

CLEANUP = rm -rf
MKDIR = mkdir -p

UNITY_ROOT=contrib/unity
C_COMPILER=gcc

CFLAGS=-std=c11
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

TEST_TARGET = test_cdnwsh.o
TEST_SRC_FILES=$(UNITY_ROOT)/src/unity.c src/*.c  test/test_cdnwsh.c  test/test_runners/test_cdnwsh_runner.c
TEST_INC_DIRS=-Isrc -Iinclude -I$(UNITY_ROOT)/src `pkg-config --cflags glib-2.0`
TEST_LDFLAGS = `pkg-config --libs glib-2.0`
TEST_SYMBOLS=-DTEST

.PHONY: clean test

all: test 

test:
	ruby $(UNITY_ROOT)/auto/generate_test_runner.rb test/test_cdnwsh.c  test/test_runners/test_cdnwsh_runner.c
	$(C_COMPILER) -g -O0 $(CFLAGS) $(TEST_INC_DIRS) $(TEST_SYMBOLS) $(TEST_LIBS) $(TEST_SRC_FILES) -o test/$(TEST_TARGET)
	./test/$(TEST_TARGET)

clean:
	$(CLEANUP) *.o

