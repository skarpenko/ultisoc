/*
 * Copyright (c) 2018-2019 The UltiSoC Project. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Verilated cycle accurate system model.
 */

#include <stdio.h>		// For fflush(stdout)
#include <iostream>
#include <iomanip>
#include <string>
#include <verilated.h>		// Defines common routines
#if VM_TRACE
# include <verilated_vcd_c.h>	// Trace file format header
#endif
#include "Vvl_soc_top.h"	// From Verilating "vl_soc_top.v"


vluint64_t main_time = 0;	// Current simulation time
const int rst_cycles = 10;	// Reset cycles


// Called by $time in Verilog
double sc_time_stamp()
{
	return main_time;
}


// Top-level wrapper
template<class Top>
class system_top {
	Top *m_top;
public:
	system_top() : m_top(new Top) {}
	~system_top() { delete m_top; }
	Top* operator->() { return m_top; }
};


// MAIN
int main(int argc, char **argv)
{
#if VM_TRACE
	bool do_trace = false;
#endif
	bool do_stats = false;
	const char *rom_image = 0;
	const char *ram_image = 0;

	// Print welcome
	std::cout << "Verilated UltiSoC System Model" << std::endl
		<< "(verilated with "
		<< Verilated::productName() << " " << Verilated::productVersion() << ")"
		<< std::endl << std::endl;

	// Hint for help
	if(argc < 2)
		std::cout << "Use -h for help." << std::endl << std::endl;

	// Parse command-line arguments
	for(int i=1; i<argc; ++i) {
		if(!strcmp(argv[i], "-h")) {
			std::cout << "Command line arguments:" << std::endl
				<< "\t-h                    - this help screen;" << std::endl
				<< "\t-stats                - print statistics;" << std::endl
#if VM_TRACE
				<< "\t-trace                - dump trace;" << std::endl
#endif
				<< "\t-rom_image <file.hex> - ROM image." << std::endl
				<< "\t-ram_image <file.hex> - RAM image." << std::endl
				<< std::endl;
			return 0;
		} else if(!strcmp(argv[i], "-stats")) {
			do_stats = true;
#if VM_TRACE
		} else if(!strcmp(argv[i], "-trace")) {
			do_trace = true;
#endif
		} else if(!strcmp(argv[i], "-rom_image")) {
			++i;
			if(i<argc) {
				rom_image = argv[i];
			} else {
				std::cerr << "-rom_image: missing file name." << std::endl;
				return -1;
			}
		} else if(!strcmp(argv[i], "-ram_image")) {
			++i;
			if(i<argc) {
				ram_image = argv[i];
			} else {
				std::cerr << "-ram_image: missing file name." << std::endl;
				return -1;
			}
		} else {
			std::cerr << "Wrong argument: " << argv[i] << std::endl;
			return -1;
		}
	}


	// Prepare command line arguments for Verilator
	std::string v_rom_arg = (rom_image ? std::string("+ROM_FILE=") +
		rom_image : std::string(""));
	std::string v_ram_arg = (ram_image ? std::string("+RAM_FILE=") +
		ram_image : std::string(""));
	const char* v_argv[] = { "", v_rom_arg.c_str(), v_ram_arg.c_str() };
	Verilated::commandArgs(3, v_argv);	// Pass args to Verilator


	// Create top-level instance
	system_top<Vvl_soc_top> top;


	// Print simulation summary
	std::cout << std::setfill('=') << std::setw(80) << "=" << std::endl;
	std::cout << "Simulation parameters:" << std::endl;
#if VM_TRACE
	std::cout << "> Tracing: " << (do_trace ? "ON" : "OFF") << std::endl;
#endif
	std::cout << "> Statistics: " << (do_stats ? "ON" : "OFF") << std::endl;
	std::cout << "> ROM image: " << (rom_image ? rom_image : "N/A") << std::endl;
	std::cout << "> RAM image: " << (ram_image ? ram_image : "N/A") << std::endl;
	std::cout << std::setfill('=') << std::setw(80) << "=" << std::endl;
	std::cout << std::endl;


#if VM_TRACE
	VerilatedVcdC* tfp = 0;
	if(do_trace) {
		Verilated::traceEverOn(true);	// Enable traces
		tfp = new VerilatedVcdC;
		top->trace (tfp, 99);		// Trace 99 levels of hierarchy
		tfp->open ("vlt_dump.vcd");	// Open the dump file
	}
#endif


	// Set initial clock and reset
	top->nrst = 0;
	top->clk = 1;


	// Main simulation loop
	while (!Verilated::gotFinish()) {
		top->nrst = (main_time > 2*rst_cycles ? 1 : 0); // De-assert reset
		top->clk = !top->clk;				// Toggle clock
		top->eval();					// Evaluate model
#if VM_TRACE
		if (tfp) tfp->dump (main_time);			// Dump waveforms
#endif

#if !defined(_NO_FFLUSH)
		fflush(stdout);					// Flush stdout
#endif

		++main_time;					// Time passes...
	}


	top->final();	// Done simulating


#if VM_TRACE
	if (tfp) tfp->close();
#endif


	// Print statistics
	if(do_stats) {
		std::cout << std::endl;
		std::cout << std::setfill('=') << std::setw(80) << "=" << std::endl;
		std::cout << "Reset cycles:  " << rst_cycles << std::endl;
		std::cout << "Active cycles: " << main_time / 2 - rst_cycles << std::endl;
		std::cout << std::setfill('-') << std::setw(80) << "-" << std::endl;
		std::cout << "Total cycles:  " << main_time / 2 << std::endl;
		std::cout << std::setfill('=') << std::setw(80) << "=" << std::endl;
		std::cout << std::endl;
	}


	return 0;
}
