# The UltiSoC Project
# ROM programs Makefile

# ROM programs list
TESTS := \
	simple_rom.elf


# Lists of object files
simple_rom.elf := entry.o simple_rom.o


# List of final targets
TARGETS := \
	$(TESTS) \
	$(TESTS:.elf=.bin) \
	$(TESTS:.elf=.hex) \
	$(TESTS:.elf=.dump)


# Include common options
include $(ULTISOC_HOME)/hw/soc_top/verif/common/common.mk


all: $(TARGETS)


simple_rom.elf: $(simple_rom.elf)
	$(GCC_PREFIX)ld -T $(ULTISOC_HOME)/hw/soc_top/verif/common/rom.ld -o $@ $(simple_rom.elf) $(LDFLAGS)


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
