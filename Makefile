CXX := clang
CXXFLAGS := -std=c23 -O2 -Isrc -Iinclude -Wall -Wextra
CXXMACRODEF :=
#-DTERMINALCHESS_INCLUDE_CONFIGREADER -DDEV_HELPERS_LATER_REMOVE


SRC := $(shell find src/ -name '*.c')
OBJ := $(patsubst src/%.c,build/main/%.o,$(SRC))

TESTCXXFLAGS := -std=c23 -O2 -Isrc -Iinclude -Wall -Wextra

TESTSRC := $(shell find test/ -name '*.c')
TESTOBJ := $(patsubst test/%.c,build/test/%.o,$(TESTSRC))

all: $(OBJ)
	@mkdir -p ./bin
	@$(CXX) $(CXXFLAGS) $(CXXMACRODEF) $(OBJ) -o ./bin/main

build/main/%.o: src/%.c
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $(CXXMACRODEF) -c $< -o $@

test: $(TESTOBJ)
	@mkdir -p ./bin
	@$(CXX) $(TESTCXXFLAGS) $(TESTOBJ) -o bin/test
	@printf '    compiled into bin/test'

build/test/%.o: test/%.c
	@mkdir -p $(dir $@)
	@$(CXX) $(TESTCXXFLAGS) $(CXXMACRODEF) -c $< -o $@

.PHONY: all clean run runtest
clean: 
	@printf '    cleaning build/ and bin/ directories...'
	@rm -r build/ bin/ 2>/dev/null || true

run: all
	@./bin/main

runtest: test
	@./bin/test
