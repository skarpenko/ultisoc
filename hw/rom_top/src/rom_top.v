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
 * FPGA ROM (supposed to be synthesizable for most FPGAs)
 */

`include "common.vh"
`include "ocp_const.vh"


/*
 * ROM
 */
module rom_top #(
	parameter N2 = 13 /* ROM words number in powers of 2 */
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
output wire			o_SCmdAccept;
output reg [`DATA_WIDTH-1:0]	o_SData;
output reg [1:0]		o_SResp;

assign o_SCmdAccept = 1'b1;	/* Always accept command */


/* ROM */
reg [`DATA_WIDTH-1:0] rom[0:2**N2-1];

/* Init ROM */
initial
	$readmemh(`ULTISOC_ROM_IMAGE, rom);


/* Internal wires */
wire [N2-1:0]		idx = i_MAddr[N2+1:2];
wire			valid_addr = ~(|i_MAddr[`ADDR_WIDTH-1:N2+2]);


/* ROM read */
always @(posedge clk)
	o_SData <= (valid_addr ? rom[idx] : {(`DATA_WIDTH){1'b1}});


/* Bus logic */
always @(posedge clk or negedge nrst)
begin
	if(!nrst)
		o_SResp <= `OCP_RESP_NULL;
	else if(i_MCmd != `OCP_CMD_IDLE)
		o_SResp <= `OCP_RESP_DVA;
	else
		o_SResp <= `OCP_RESP_NULL;
end


endmodule /* rom_top */
