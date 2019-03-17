# The UltiSoC Project
# Quartus compilation flow


# Targets
all: ultisoc.sof
rbf: ultisoc.rbf


# Main target
ultisoc.sof: synth/output_files/ultisoc.sof
	cp synth/output_files/ultisoc.sof .


# Optional target
ultisoc.rbf: config/ultisoc.cof ultisoc.sof
	quartus_cpf -c config/ultisoc.cof


# Compile flow
synth/output_files/ultisoc.sof: synth $(ULTISOC_HOME)/boot/build/usoc_rom.hex
	quartus_sh --flow compile synth/ultisoc


# Create Quartus project
synth: config/creat_proj.tcl
	mkdir synth
	quartus_sh -t config/creat_proj.tcl
	cp config/ultisoc.sdc synth/


# Build BootROM
$(ULTISOC_HOME)/boot/build/usoc_rom.hex:
	$(MAKE) -C $(ULTISOC_HOME)/boot


# Program FPGA target
.PHONY: prog
prog: config/ultisoc.cdf ultisoc.sof
	quartus_pgm config/ultisoc.cdf


# Clean
.PHONY: clean
clean:
	-rm -Rf synth/


# Clean all
.PHONY: cleanall
cleanall:
	-rm -Rf synth/
	-rm -f ultisoc.sof
	-rm -f ultisoc.rbf
