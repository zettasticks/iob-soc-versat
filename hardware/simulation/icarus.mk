VFLAGS+=-W all -g2005-sv -I. -I../src -Isrc -Ihardware/src -DSIM

ifeq ($(VCD),1)
VFLAGS+=-DVCD
endif

ifneq ($(VTOP),)
VFLAGS+=-s $(VTOP)
endif

SIM_SERVER=$(IVSIM_SERVER)
SIM_USER=$(IVSIM_USER)

SIM_OBJ=a.out

comp: $(SIM_OBJ)

$(SIM_OBJ): $(VHDR) $(VSRC) iob_soc_versat_boot.hex test.hex #$(HEX)
	iverilog $(VFLAGS) $(VSRC)

exec: comp
	./$(SIM_OBJ)

clean: gen-clean
	@rm -f $(SIM_OBJ)

very-clean: clean

.PHONY: comp exec clean very-clean
