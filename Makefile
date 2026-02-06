CUR_DIR=$(shell pwd)

# Remember that some tests depend on flags which are set inside the testInfo.json.
# That means that using Makefile directly might cause some tests to fail.
# Example: Doing sim-run for the test EXAMPLE_DEBUG without setting the DO_PROFILE flag
#          is going to cause an error even though it is normal, since the test requires the flag to work.

CORE := iob_soc_versat

SIMULATOR ?= verilator
BOARD ?= AES-KU040-DB-G

DISABLE_LINT:=1

FAST_COMPILE_VERSAT := $(MAKE) -C ./submodules/VERSAT -j 8 versat

LIB_DIR:=submodules/IOBSOC/submodules/LIB
include $(LIB_DIR)/setup.mk

VERSAT_SPEC:=versatSpec.txt

VCD ?= 0
INIT_MEM ?= 1
USE_EXTMEM ?= 1

ifeq ($(INIT_MEM),1)
SETUP_ARGS += INIT_MEM
endif

ifeq ($(USE_EXTMEM),1)
SETUP_ARGS += USE_EXTMEM
endif

TEST?=EXAMPLE_Simple
SETUP_ARGS+=TEST="$(TEST)"

VERSAT_CALL := ./versat
ifeq ($(DEBUG),1)
VERSAT_CALL := gdb -ex run --args ./versat
endif

AXI_DATA_W?=32
SETUP_ARGS+=AXI_DATA_W=$(AXI_DATA_W)

#
# TODO: This approach to testing stuff is becoming really clubersome. The Python -> Makefile -> Python flow is not very good and 
#

TEST_FOLDER_TEMP :=../$(CORE)_V0.70_$(TEST)
ifeq ($(AXI_DATA_W),32)
TEST_FOLDER_TEMP := $(TEST_FOLDER_TEMP)_b32
endif
ifeq ($(AXI_DATA_W),64)
TEST_FOLDER_TEMP := $(TEST_FOLDER_TEMP)_b64
endif
ifeq ($(AXI_DATA_W),128)
TEST_FOLDER_TEMP := $(TEST_FOLDER_TEMP)_b128
endif
ifeq ($(AXI_DATA_W),256)
TEST_FOLDER_TEMP := $(TEST_FOLDER_TEMP)_b256
endif

EXTRA_VERSAT_ARGS:=--debug

ifneq ($(DO_PROFILE),)
EXTRA_VERSAT_ARGS+=--profile
SETUP_ARGS+=PROFILE
endif

TEST_FOLDER := $(abspath $(TEST_FOLDER_TEMP))

VERSAT_ARGUMENTS:=$(CUR_DIR)/$(VERSAT_SPEC) $(EXTRA_VERSAT_ARGS) -b$(AXI_DATA_W) -d -t $(TEST)
VERSAT_ARGUMENTS+=-I $(CUR_DIR)/submodules/VERSAT/hardware/src -O $(TEST_FOLDER)/software
VERSAT_ARGUMENTS+=-o $(TEST_FOLDER)/hardware/src -g $(CUR_DIR)/../debug -u $(CUR_DIR)/hardware/src/units

get-versat-arguments:
	@echo $(VERSAT_ARGUMENTS)
	@echo $(TEST_FOLDER_TEMP)

# Single test rules
setup:
	+nix-shell --run 'make build-setup SETUP_ARGS="$(SETUP_ARGS) TEST=$(TEST)"'

setup_pc:
	+nix-shell --run 'make build-setup SETUP_ARGS="$(SETUP_ARGS) TEST=$(TEST)"'

pc-emul-run:
	+nix-shell --run 'make setup_pc && make -C $(TEST_FOLDER)/ pc-emul-run'

fpga-run:
	nix-shell --run 'make clean setup INIT_MEM=$(INIT_MEM) USE_EXTMEM=$(USE_EXTMEM) && make -C $(TEST_FOLDER)/ fpga-fw-build BOARD=$(BOARD)'
	make -C $(TEST_FOLDER)/ fpga-run BOARD=$(BOARD)

sim-build:
	+nix-shell --run 'make setup INIT_MEM=$(INIT_MEM) USE_EXTMEM=$(USE_EXTMEM) TEST=$(TEST) && make -C $(TEST_FOLDER)/ sim-build SIMULATOR=$(SIMULATOR) VCD=$(VCD)'

sim-run:
	+nix-shell --run 'make setup INIT_MEM=$(INIT_MEM) USE_EXTMEM=$(USE_EXTMEM) TEST=$(TEST) && make -C $(TEST_FOLDER)/ sim-run SIMULATOR=$(SIMULATOR) VCD=$(VCD)'

fpga-run-only:
	cp ./software/src/Tests/$(TEST).cpp $(TEST_FOLDER)/software/src/test.cpp
	cp ./software/src/Tests/testbench.hpp $(TEST_FOLDER)/software/src/
	cp ./software/src/Tests/unitConfiguration.hpp $(TEST_FOLDER)/software/src/
	+nix-shell --run "make -C $(TEST_FOLDER)/ fpga-fw-build fpga-run BOARD=$(BOARD)"

# Fast rules need to run inside nix-shell otherwise we have linker problems from different compiler versions
fast-compile-versat:
	cd ./submodules/VERSAT ; $(MAKE) -j 8 versat

fast-setup: fast-compile-versat
	cp ./software/src/Tests/testbench.hpp $(TEST_FOLDER)/software/src/testbench.hpp
	cp ./software/src/Tests/$(TEST).cpp $(TEST_FOLDER)/software/src/test.cpp
	./submodules/VERSAT/versat ./versatSpec.txt -b32 -d -t $(TEST) -o $(TEST_FOLDER)/hardware/src -O $(TEST_FOLDER)/software -g ../debug -u ./hardware/src/units 

fast-pc-emul: fast-setup
	make -C $(TEST_FOLDER) pc-emul-run

fast-sim-run: fast-setup
	make -C $(TEST_FOLDER) sim-run SIMULATOR=$(SIMULATOR) VCD=$(VCD)

test-folder:
	@echo $(TEST_FOLDER)

versat-only:
	mkdir -p $(TEST_FOLDER)/
	cd ./submodules/VERSAT ; $(MAKE) -j 8 versat
	cd ./submodules/VERSAT ; $(VERSAT_CALL) $(VERSAT_ARGUMENTS)

test-clean:
	rm -r $(TEST_FOLDER)/

.PHONY: setup test-setup

test-make:
	echo "test"
