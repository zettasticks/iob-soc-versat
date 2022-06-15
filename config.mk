######################################################################
#
# IOb-SoC Configuration File
#
######################################################################

IOBSOC_NAME:=IOBSOCSHA

#
# PRIMARY PARAMETERS: CAN BE CHANGED BY USERS OR OVERRIDEN BY ENV VARS
#

#TEST VECTOR
TEST_VECTOR_RSP ?=SHA256ShortMsg.rsp

#CPU ARCHITECTURE
DATA_W := 32
ADDR_W := 32

#FIRMWARE SIZE (LOG2)
FIRM_ADDR_W ?=17

#SRAM SIZE (LOG2)
SRAM_ADDR_W ?=17

#DDR
USE_DDR ?=1
RUN_EXTMEM ?=0

#DATA CACHE ADDRESS WIDTH (tag + index + offset)
DCACHE_ADDR_W:=24

#ROM SIZE (LOG2)
BOOTROM_ADDR_W:=12

#PRE-INIT MEMORY WITH PROGRAM AND DATA
INIT_MEM ?=1

#ETHERNET RMAC_ADDR
RMAC_ADDR=4437e6a6893b

#PERIPHERAL LIST
#must match respective submodule CORE_NAME in the core.mk file of the submodule
#PERIPHERALS:=UART
PERIPHERALS ?=UART TIMER ETHERNET

#RISC-V HARD MULTIPLIER AND DIVIDER INSTRUCTIONS
USE_MUL_DIV ?=1

#RISC-V COMPRESSED INSTRUCTIONS
USE_COMPRESSED ?=1

#ROOT DIRECTORY ON REMOTE MACHINES
REMOTE_ROOT_DIR ?=sandbox/iob-soc-sha

#SIMULATION
#default simulator running locally or remotely
#check the respective Makefile in hardware/simulation/$(SIMULATOR) for specific settings
SIMULATOR ?=icarus

#BOARD
#default board running locally or remotely
#check the respective Makefile in hardware/fpga/$(BOARD) for specific settings
BOARD ?=AES-KU040-DB-G

#IOB LIBRARY
UART_HW_DIR:=$(UART_DIR)/hardware

####################################################################
# DERIVED FROM PRIMARY PARAMETERS: DO NOT CHANGE BELOW THIS POINT
####################################################################

ifeq ($(RUN_EXTMEM),1)
DEFINE+=$(defmacro)RUN_EXTMEM
USE_DDR=1
endif

ifeq ($(USE_DDR),1)
DEFINE+=$(defmacro)USE_DDR
endif

ifeq ($(INIT_MEM),1)
DEFINE+=$(defmacro)INIT_MEM
endif

#submodule paths
VEXRISCV_DIR=$(ROOT_DIR)/submodules/VEXRISCV
CACHE_DIR=$(ROOT_DIR)/submodules/CACHE
UART_DIR=$(ROOT_DIR)/submodules/UART
TIMER_DIR=$(ROOT_DIR)/submodules/TIMER
ETHERNET_DIR=$(ROOT_DIR)/submodules/ETHERNET
LIB_DIR=$(ROOT_DIR)/submodules/LIB
MEM_DIR=$(ROOT_DIR)/submodules/MEM
AXI_DIR=$(ROOT_DIR)/submodules/AXI

#sw paths
SW_DIR:=$(ROOT_DIR)/software
PC_DIR:=$(SW_DIR)/pc-emul
FIRM_DIR:=$(SW_DIR)/firmware
BOOT_DIR:=$(SW_DIR)/bootloader
CONSOLE_DIR:=$(SW_DIR)/console
SW_TEST_DIR:=$(SW_DIR)/test

#hw paths
HW_DIR=$(ROOT_DIR)/hardware
SIM_DIR=$(HW_DIR)/simulation/$(SIMULATOR)
BOARD_DIR ?=$(shell find hardware -name $(BOARD))
DOC_DIR=$(ROOT_DIR)/document

#define macros
DEFINE+=$(defmacro)DATA_W=$(DATA_W)
DEFINE+=$(defmacro)ADDR_W=$(ADDR_W)
DEFINE+=$(defmacro)BOOTROM_ADDR_W=$(BOOTROM_ADDR_W)
DEFINE+=$(defmacro)SRAM_ADDR_W=$(SRAM_ADDR_W)
DEFINE+=$(defmacro)FIRM_ADDR_W=$(FIRM_ADDR_W)
DEFINE+=$(defmacro)DCACHE_ADDR_W=$(DCACHE_ADDR_W)
DEFINE+=$(defmacro)N_SLAVES=$(N_SLAVES) #peripherals

#address selection bits
E:=31 #extra memory bit
ifeq ($(USE_DDR),1)
P:=30 #periphs
B:=29 #boot controller
else
P:=31
B:=30
endif

DEFINE+=$(defmacro)E=$E
DEFINE+=$(defmacro)P=$P
DEFINE+=$(defmacro)B=$B

#PERIPHERAL IDs
#assign a sequential ID to each peripheral
#the ID is used as an instance name index in the hardware and as a base address in the software
N_SLAVES:=0
$(foreach p, $(PERIPHERALS), $(eval $p=$(N_SLAVES)) $(eval N_SLAVES:=$(shell expr $(N_SLAVES) \+ 1)))
$(foreach p, $(PERIPHERALS), $(eval DEFINE+=$(defmacro)$p=$($p)))

N_SLAVES_W = $(shell echo "import math; print(math.ceil(math.log($(N_SLAVES),2)))"|python3 )
DEFINE+=$(defmacro)N_SLAVES_W=$(N_SLAVES_W)


#default baud and system clock freq
BAUD ?=5000000 #simulation default
FREQ ?=100000000

SHELL = /bin/bash

#RULES
gen-clean:
	@rm -f *# *~

.PHONY: gen-clean