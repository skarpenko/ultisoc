# The UltiSoC Project
# Verilated system model Makefile

TARGET     := vl_ultisoc.elf
VL_FLAGS   := -Wno-fatal -O3 --exe
TARGET_DEP := main.cxx vl_soc_top.lst


# Enable trace support
ifneq (,$(filter $(TRACE),1 y yes))
VL_FLAGS += --trace
endif


# Disable stdout flushing
ifneq (,$(filter $(NFLUSH),1 y yes))
VL_FLAGS += -CFLAGS "-D_NO_FFLUSH"
endif


# Main goal
.PHONY: all
all: $(TARGET)
	@echo "Done."


.PHONY: help
help:
	@echo "UltiSoC Verilated system model"
	@echo "==============================="
	@echo "Options:"
	@echo "  TRACE=1   - enable tracing support;"
	@echo "  NFLUSH=1  - disable stdout flushing on each cycle."


$(TARGET): $(TARGET_DEP)
	@verilator $(VL_FLAGS) -o ../$(TARGET) -f vl_soc_top.lst
	@$(MAKE) -C obj_dir -f Vvl_soc_top.mk


# Do clean
.PHONY: clean
clean:
	@echo "Clean"
	-@rm -f $(TARGET)
	-@rm -fR obj_dir
	-@rm -f vlt_dump.vcd
