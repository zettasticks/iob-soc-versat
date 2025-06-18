CUR_DIR=$(shell pwd)

CORE := iob_soc_versat

SIMULATOR ?= verilator
BOARD ?= AES-KU040-DB-G

DISABLE_LINT:=1

FAST_COMPILE_VERSAT := $(MAKE) -C ./submodules/VERSAT -j 8 versat

LIB_DIR:=submodules/IOBSOC/submodules/LIB
include $(LIB_DIR)/setup.mk

VERSAT_SPEC:=versatSpec.txt

VCD ?= 0
INIT_MEM ?= 0
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

TEST_FOLDER := $(abspath $(TEST_FOLDER_TEMP))

VERSAT_ARGUMENTS:=$(CUR_DIR)/$(VERSAT_SPEC) -s -b$(AXI_DATA_W) -d -t $(TEST) 
VERSAT_ARGUMENTS+=-I $(CUR_DIR)/submodules/VERSAT/hardware/src -O $(TEST_FOLDER)/software
VERSAT_ARGUMENTS+=-o $(TEST_FOLDER)/hardware/src -g $(CUR_DIR)/../debug -u $(CUR_DIR)/hardware/src/units -x64

get-versat-arguments:
	@echo $(VERSAT_ARGUMENTS)
	@echo $(TEST_FOLDER_TEMP)

# Single test rules
setup:
	+nix-shell --run 'make build-setup SETUP_ARGS="$(SETUP_ARGS) TEST=$(TEST)"'

setup_pc:
	+nix-shell --run 'make build-setup SETUP_ARGS="$(SETUP_ARGS) PC_EMUL TEST=$(TEST)"'

pc-emul-run:
	+nix-shell --run 'make setup_pc && make -C $(TEST_FOLDER)/ pc-emul-run'

fpga-run:
	nix-shell --run 'make clean setup INIT_MEM=$(INIT_MEM) USE_EXTMEM=$(USE_EXTMEM) && make -C $(TEST_FOLDER)/ fpga-fw-build BOARD=$(BOARD)'
	make -C $(TEST_FOLDER)/ fpga-run BOARD=$(BOARD)

sim-build:
	+nix-shell --run 'make setup INIT_MEM=1 USE_EXTMEM=$(USE_EXTMEM) TEST=$(TEST) && make -C $(TEST_FOLDER)/ sim-build SIMULATOR=$(SIMULATOR) VCD=$(VCD)'

sim-run:
	+nix-shell --run 'make setup INIT_MEM=1 USE_EXTMEM=$(USE_EXTMEM) TEST=$(TEST) && make -C $(TEST_FOLDER)/ sim-run SIMULATOR=$(SIMULATOR) VCD=$(VCD)'

fpga-run-only:
	cp ./software/src/Tests/$(TEST).cpp $(TEST_FOLDER)/software/src/test.cpp
	cp ./software/src/Tests/testbench.hpp $(TEST_FOLDER)/software/src/
	cp ./software/src/Tests/unitConfiguration.hpp $(TEST_FOLDER)/software/src/
	+nix-shell --run "make -C $(TEST_FOLDER)/ fpga-fw-build fpga-run BOARD=$(BOARD)"

fast-compile-versat:
	cd ./submodules/VERSAT ; $(MAKE) -j 8 versat

versat-only:
	mkdir -p $(TEST_FOLDER)/
	cd ./submodules/VERSAT ; $(MAKE) -j 8 versat
	cd ./submodules/VERSAT ; $(VERSAT_CALL) $(VERSAT_ARGUMENTS)

test-clean:
	rm -r $(TEST_FOLDER)/

.PHONY: setup test-setup

test-make:
	echo "test"
