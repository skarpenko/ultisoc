# The UltiSoC Project
# BootROM Makefile


# Final targets
targets = $(target-elf) $(target-bin) $(target-hex) $(target-mif) $(target-dump)


# Target names
target-elf  := usoc_rom.elf
target-bin  := usoc_rom.bin
target-hex  := usoc_rom.hex
target-mif  := usoc_rom.mif
target-dump := usoc_rom.dump


# C source files
c-files :=		\
	main.c		\
	except.c	\
	uart.c		\
	con.c		\
	str.c		\
	cmd.c		\
	disasm.c	\
	dis.c		\
	ctype.c		\
	exc.c		\
	reset.c		\
	mem.c		\
	movc0.c		\
	jump.c		\
	leds.c		\
	crc16_ccitt.c	\
	xmodem.c	\
	xm_load.c	\
	elf_stream.c


# Assembly source files
S-files :=		\
	entry.S

# Linker script
ldscript := ldscript/rom.ld

# Quiet
Q ?= @

# Include common options and macros
include mkconfig/config.mk
include mkconfig/macro.mk

# Prepare file lists
o-files := $(addprefix build/, $(c-files:.c=.o) $(S-files:.S=.o))
d-files := $(addprefix build/, $(c-files:.c=.d) $(S-files:.S=.d))
c-files := $(addprefix src/, $(c-files))
S-files := $(addprefix src/, $(S-files))

# Prepare targets
target-elf  := $(addprefix build/, $(target-elf))
target-bin  := $(addprefix build/, $(target-bin))
target-hex  := $(addprefix build/, $(target-hex))
target-mif  := $(addprefix build/, $(target-mif))
target-dump := $(addprefix build/, $(target-dump))


.PHONY: all
all: $(targets)
	$(Q)echo "All done."


.PHONY: help
help:
	@echo "The UltiSoC Project / BootROM"
	@echo "============================="
	@echo "Targets:"
	@echo "  help     - print this help;"
	@echo "  clean    - remove intermediate build results;"
	@echo "  cleanall - remove all build results."
	@echo "Arguments:"
	@echo "  Q=@      - quiet (default: on)."


$(target-elf): $(o-files) $(ldscript)
	$(Q)echo "Linking [$@]"
	$(Q)$(create-dir-rule)
	$(Q)$(GCC_PREFIX)ld -T $(ldscript) -o $@ $(o-files) $(LDFLAGS)


build/%.bin: build/%.elf
	$(Q)echo "Generating [$@]"
	$(Q)$(create-dir-rule)
	$(Q)$(GCC_PREFIX)objcopy -O binary $< $@


PERL_HEX_SCRIPT := 'print "\@00000000\n"; \
	my $$a =0; \
	while(read(STDIN, my $$b, 4)) \
	{ printf "%08x\n", unpack "I", $$b; $$a = $$a + 1;}; \
	while($$a < 8192) { printf "00000000\n", $$a = $$a + 1; };'

build/%.hex: build/%.bin
	$(Q)echo "Generating [$@]"
	$(Q)$(create-dir-rule)
	$(Q)perl -e $(PERL_HEX_SCRIPT) < $< > $@


PERL_MIF_SCRIPT := 'print "DEPTH = 8192;\n"; print "WIDTH = 32;\n"; \
	print "ADDRESS_RADIX = HEX;\n"; print "DATA_RADIX = HEX;\n"; \
	print "CONTENT\n"; print "BEGIN\n"; my $$a = 0; \
	while(read(STDIN, my $$b, 4)) \
	{ printf "%08x: %08x;\n", $$a, unpack "I", $$b; $$a = $$a + 1; }; \
	while($$a < 8192) { printf "%08x: 00000000;\n", $$a; $$a = $$a + 1; }; \
	print "END;\n"'

build/%.mif: build/%.bin
	$(Q)echo "Generating [$@]"
	$(Q)$(create-dir-rule)
	$(Q)perl -e $(PERL_MIF_SCRIPT) < $< > $@


build/%.dump: build/%.elf
	$(Q)echo "Generating [$@]"
	$(Q)$(create-dir-rule)
	$(Q)$(GCC_PREFIX)objdump -D $< > $@


build/%.o: src/%.S
	$(Q)echo "Compiling [$<]"
	$(Q)$(create-dir-rule)
	$(Q)$(call make-depend,$<,$@,$(subst .o,.d,$@),$(CFLAGS) $(ASFLAGS))
	$(Q)$(GCC_PREFIX)gcc $(CFLAGS) $(ASFLAGS) -c $< -o $@


build/%.o: src/%.c
	$(Q)echo "Compiling [$<]"
	$(Q)$(create-dir-rule)
	$(Q)$(call make-depend,$<,$@,$(subst .o,.d,$@),$(CFLAGS))
	$(Q)$(GCC_PREFIX)gcc $(CFLAGS) -c $< -o $@


# Clean
.PHONY: clean
clean:
	$(Q)-rm -f $(o-files) $(d-files)


# Clean all
.PHONY: cleanall
cleanall:
	$(Q)-rm -f $(targets) $(o-files) $(d-files)
	$(Q)-rmdir build


# Include dependency files if not a clean target
ifeq ($(call lists-intersect,clean cleanall,$(MAKECMDGOALS)),)
-include $(d-files)
endif
