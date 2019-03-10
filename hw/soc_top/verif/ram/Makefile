# The UltiSoC Project
# RAM programs Makefile

# RAM programs list
TESTS := \
	simple_ram.elf


# Lists of object files
simple_ram.elf := entry.o simple_ram.o


# List of final targets
TARGETS := \
	$(TESTS) \
	$(TESTS:.elf=.bin) \
	$(TESTS:.elf=.hex) \
	$(TESTS:.elf=.dump)


# Include common options
include $(ULTISOC_HOME)/hw/soc_top/verif/common/common.mk


all: $(TARGETS)


simple_ram.elf: $(simple_ram.elf)
	$(GCC_PREFIX)ld -T $(ULTISOC_HOME)/hw/soc_top/verif/common/ram.ld -o $@ $(simple_ram.elf) $(LDFLAGS)


%.bin: %.elf
	$(GCC_PREFIX)objcopy -O binary $< $@


#%.hex: %.elf
#	$(GCC_PREFIX)objcopy -O verilog $< $@


%.hex: %.bin
	perl -e 'print "\@00000000\n"; while(read(STDIN, my $$b, 4)) { printf "%08x\n", unpack "I", $$b; }' < $< > $@


%.dump: %.elf
	$(GCC_PREFIX)objdump -D $< > $@


%.o: %.S
	$(GCC_PREFIX)gcc $(CFLAGS) $(ASFLAGS) -c $< -o $@


%.o: %.c
	$(GCC_PREFIX)gcc $(CFLAGS) -c $< -o $@


# Clean
.PHONY: clean
clean:
	-rm -f $(TARGETS) $(TESTS:.elf=.o)
	-rm -f $(foreach TARGET, $(TARGETS), $($(TARGET)))
