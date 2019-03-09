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
 * Memory top level
 */


`include "common.vh"
`include "ocp_const.vh"


/*
 * Memory top
 */
module memory_top #(
	parameter SEL_BIT = 24 /* ROM / RAM select bit number */
)
(
	clk,
	nrst,
	/* OCP interface */
	i_MAddr,
	i_MCmd,
	i_MData,
	i_MByteEn,
	o_SCmdAccept,
	o_SData,
	o_SResp
);
/* Inputs and outputs */
input wire			clk;
input wire			nrst;
input wire [`ADDR_WIDTH-1:0]	i_MAddr;
input wire [2:0]		i_MCmd;
input wire [`DATA_WIDTH-1:0]	i_MData;
input wire [`BEN_WIDTH-1:0]	i_MByteEn;
output reg			o_SCmdAccept;
output reg [`DATA_WIDTH-1:0]	o_SData;
output reg [1:0]		o_SResp;


/* ROM wires */
reg [`ADDR_WIDTH-1:0]	rom_maddr;
reg [2:0]		rom_mcmd;
reg [`DATA_WIDTH-1:0]	rom_mdata;
reg [`BEN_WIDTH-1:0]	rom_mbyteen;
wire			rom_scmdaccept;
wire [`DATA_WIDTH-1:0]	rom_sdata;
wire [1:0]		rom_sresp;

/* RAM wires */
reg [`ADDR_WIDTH-1:0]	sram_maddr;
reg [2:0]		sram_mcmd;
reg [`DATA_WIDTH-1:0]	sram_mdata;
reg [`BEN_WIDTH-1:0]	sram_mbyteen;
wire 			sram_scmdaccept;
wire [`DATA_WIDTH-1:0]	sram_sdata;
wire [1:0]		sram_sresp;


reg wait_state;
reg selr;
wire sel = i_MAddr[SEL_BIT] || selr;

/* ROM / RAM selection FSM */
always @(posedge clk or negedge nrst)
begin
	if(!nrst)
	begin
		wait_state <= 1'b0;
		selr <= 1'b0;
	end
	else if(!wait_state)
	begin
		if(i_MCmd != `OCP_CMD_IDLE && o_SResp == `OCP_RESP_NULL)
		begin
			wait_state <= 1'b1;
			selr <= i_MAddr[SEL_BIT];
		end
	end
	else
	begin
		if(o_SResp != `OCP_RESP_NULL)
		begin
			wait_state <= 1'b0;
			selr <= 1'b0;
		end
	end
end


/* MUX logic */
always @(*)
begin
	if(sel)
	begin
		rom_maddr = { (`ADDR_WIDTH){1'b0} };
		rom_mcmd = 3'b000;
		rom_mdata = { (`DATA_WIDTH){1'b0} };
		rom_mbyteen = { (`BEN_WIDTH){1'b0} };
		sram_maddr = { {(`ADDR_WIDTH-SEL_BIT){1'b0}}, i_MAddr[SEL_BIT-1:0] };
		sram_mcmd = i_MCmd;
		sram_mdata = i_MData;
		sram_mbyteen = i_MByteEn;
		o_SCmdAccept = sram_scmdaccept;
		o_SData = sram_sdata;
		o_SResp = sram_sresp;
	end
	else
	begin
		rom_maddr = { {(`ADDR_WIDTH-SEL_BIT){1'b0}}, i_MAddr[SEL_BIT-1:0] };
		rom_mcmd = i_MCmd;
		rom_mdata = i_MData;
		rom_mbyteen = i_MByteEn;
		sram_maddr = { (`ADDR_WIDTH){1'b0} };
		sram_mcmd = 3'b000;
		sram_mdata = { (`DATA_WIDTH){1'b0} };
		sram_mbyteen = { (`BEN_WIDTH){1'b0} };
		o_SCmdAccept = rom_scmdaccept;
		o_SData = rom_sdata;
		o_SResp = rom_sresp;
	end
end


/* ROM instance */
rom_top rom(
	.clk(clk),
	.nrst(nrst),
	.i_MAddr(rom_maddr),
	.i_MCmd(rom_mcmd),
	.i_MData(rom_mdata),
	.i_MByteEn(rom_mbyteen),
	.o_SCmdAccept(rom_scmdaccept),
	.o_SData(rom_sdata),
	.o_SResp(rom_sresp)
);


/* RAM instance */
ram_top ram(
	.clk(clk),
	.nrst(nrst),
	.i_MAddr(sram_maddr),
	.i_MCmd(sram_mcmd),
	.i_MData(sram_mdata),
	.i_MByteEn(sram_mbyteen),
	.o_SCmdAccept(sram_scmdaccept),
	.o_SData(sram_sdata),
	.o_SResp(sram_sresp)
);


endmodule /* memory_top */
