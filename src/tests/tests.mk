# When running test cases on the development computer, don't compile with
# embedded libraries

TEST_DIR = tests
TEST_OBJDIR = build/$(TEST_DIR)

TEST_SRC=$(wildcard $(TEST_DIR)/*_tests.cpp)
TESTS=$(patsubst %.cpp,$(TEST_OBJDIR)/%.bin,$(TEST_SRC))
TEST_LIBS = -lcheck
INCLUDE_PATHS += -I. -I./libs/cJSON

TESTABLE_OBJ_FILES = bitfield.o queue.o canutil.o canwrite.o canread.o \
				listener.o libs/cJSON/cJSON.o buffers.o strutil.o usbutil.o \
				serialutil.o ethernetutil.o
TESTABLE_LIB_SRCS = helpers.c usbutil_mock.c serialutil_mock.c \
				canwrite_mock.c log_mock.c ethernetutil_mock.c
TESTABLE_LIB_OBJ_FILES = $(addprefix $(TEST_OBJDIR)/$(TEST_DIR)/, $(TESTABLE_LIB_SRCS:.c=.o))
TESTABLE_OBJS = $(patsubst %,$(TEST_OBJDIR)/%,$(TESTABLE_OBJ_FILES)) \
				$(TESTABLE_LIB_OBJ_FILES)

.PRECIOUS: $(TESTABLE_OBJS) $(TESTS:.bin=.o)

test: unit_tests
	@make pic32_compile_test
	@make lpc17xx_compile_test
	@mv signals.cpp.bak signals.cpp
	@mv handlers.cpp.bak handlers.cpp
	@mv handlers.h.bak handlers.h

unit_tests: LD = g++
unit_tests: CC = gcc
unit_tests: CPP = g++
unit_tests: CC_FLAGS = -I. -c -m32 -w -Wall -Werror -g -ggdb -coverage
unit_tests: CC_SYMBOLS = -D__TESTS__
unit_tests: LDFLAGS = -m32 -lm -coverage
unit_tests: LDLIBS = $(TEST_LIBS)
unit_tests: $(TESTS)
	@sh tests/runtests.sh $(TEST_OBJDIR)/$(TEST_DIR)

pic32_compile_test: code_generation_test
	make -j4
	@make clean

lpc17xx_compile_test: code_generation_test
	PLATFORM=LPC17XX make -j4
	@make clean

code_generation_test:
	@make clean
	@mkdir -p $(TEST_OBJDIR)
	../generate_code.py --json signals.json.example > $(TEST_OBJDIR)/signals.cpp
	@if [[ -h signals.cpp ]]; then mv -f signals.cpp signals.cpp.bak; fi
	@if [[ -h handlers.cpp ]]; then mv -f handlers.cpp handlers.cpp.bak; fi
	@if [[ -h handlers.h ]]; then mv -f handlers.h handlers.h.bak; fi
	@ln -s $(TEST_OBJDIR)/signals.cpp
	@ln -s handlers.cpp.example handlers.cpp
	@ln -s handlers.h.example handlers.h

COVERAGE_INFO_FILENAME = coverage.info
COVERAGE_INFO_PATH = $(TEST_OBJDIR)/$(COVERAGE_INFO_FILENAME)
coverage:
	@lcov --base-directory . --directory . --zerocounters -q
	@make unit_tests
	@lcov --base-directory . --directory . -c -o $(TEST_OBJDIR)/coverage.info
	@lcov --remove $(COVERAGE_INFO_PATH) "libs/*" -o $(COVERAGE_INFO_PATH)
	@genhtml -o $(TEST_OBJDIR)/coverage -t "cantranslator test coverage" --num-spaces 4 $(COVERAGE_INFO_PATH)
	@google-chrome $(TEST_OBJDIR)/coverage/index.html

$(TEST_OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CPP) $(CC_FLAGS) $(CC_SYMBOLS) $(ONLY_CPP_FLAGS) $(INCLUDE_PATHS) -o $@ $<

$(TEST_OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CC_FLAGS) $(CC_SYMBOLS) $(ONLY_C_FLAGS) $(INCLUDE_PATHS) -o $@ $<

$(TEST_OBJDIR)/%.bin: $(TEST_OBJDIR)/%.o $(TESTABLE_OBJS)
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) $(LDLIBS) $(CC_SYMBOLS) $(ONLY_CPP_FLAGS) $(INCLUDE_PATHS) -o $@ $^
