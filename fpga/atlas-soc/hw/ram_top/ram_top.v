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
 * Gasket for Altera 1-port RAM
 */

`include "common.vh"
`include "ocp_const.vh"


/*
 * RAM top
 */
module ram_top
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
output wire [`DATA_WIDTH-1:0]	o_SData;
output reg [1:0]		o_SResp;


assign o_SCmdAccept = 1'b1;	/* Always accept command */


wire [15:0] addr = i_MAddr[17:2];
wire addr_valid = ~(|i_MAddr[`ADDR_WIDTH-1:18]);
wire wren = addr_valid && (i_MCmd == `OCP_CMD_WRITE);


always @(posedge clk or negedge nrst)
begin
	if(!nrst)
	begin
		o_SResp <= `OCP_RESP_NULL;
	end
	else if(i_MCmd != `OCP_CMD_IDLE)
	begin
		o_SResp <= `OCP_RESP_DVA;
	end
	else
		o_SResp <= `OCP_RESP_NULL;
end


/* Altera RAM instance */
altera_ram_64Kx32b ram (
	.clock(clk),
	.address(addr),
	.data(i_MData),
	.byteena(i_MByteEn),
	.wren(wren),
	.q(o_SData)
);


endmodule /* ram_top */
