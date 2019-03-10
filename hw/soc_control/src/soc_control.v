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
 * System control device
 */

`include "common.vh"
`include "ocp_const.vh"


/* Control device */
module soc_control(
	clk,
	nrst,
	/* OCP interface */
	i_MAddr,
	i_MCmd,
	i_MData,
	i_MByteEn,
	o_SCmdAccept,
	o_SData,
	o_SResp,
	/* Peripherals control */
	o_LED
);
`include "soc_info.vh"

/* Register offsets */
localparam [`ADDR_WIDTH-1:0] SOCVER_REG		= 32'h000;	/* SoC version (R/O) */
localparam [`ADDR_WIDTH-1:0] RAMBASE_REG	= 32'h004;	/* RAM base address (R/O) */
localparam [`ADDR_WIDTH-1:0] RAMSIZE_REG	= 32'h008;	/* RAM size (R/O) */
localparam [`ADDR_WIDTH-1:0] ROMSIZE_REG	= 32'h00C;	/* ROM size (R/O) */
localparam [`ADDR_WIDTH-1:0] SYSFREQ_REG	= 32'h010;	/* System frequency (R/O) */
localparam [`ADDR_WIDTH-1:0] LED_REG		= 32'h100;	/* LEDs register (R/W) */


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
output reg [7:0]		o_LED;


assign o_SCmdAccept = 1'b1;	/* Always ready to accept command */



/* Bus logic */
always @(*)
begin
	case(i_MCmd)
	`OCP_CMD_WRITE: begin
		o_SData = {(`DATA_WIDTH){1'b0}};
		o_SResp = `OCP_RESP_DVA;
	end
	`OCP_CMD_READ: begin
		case(i_MAddr)
		SOCVER_REG: o_SData = `SOC_INFO_SOC_VERSION;
		RAMBASE_REG: o_SData = `SOC_INFO_RAM_BASE;
		RAMSIZE_REG: o_SData = `SOC_INFO_RAM_SIZE;
		ROMSIZE_REG: o_SData = `SOC_INFO_ROM_SIZE;
		SYSFREQ_REG: o_SData = `SOC_INFO_SYS_FREQ;
		LED_REG: o_SData = { {(`DATA_WIDTH-8){1'b0}}, o_LED };
		default: o_SData = 32'hDEADDEAD;
		endcase
		o_SResp = `OCP_RESP_DVA;
	end
	default: begin
		o_SData = {(`DATA_WIDTH){1'b0}};
		o_SResp = `OCP_RESP_NULL;
	end
	endcase
end


/* State update */
always @(posedge clk or negedge nrst)
begin
	if(!nrst)
	begin
		o_LED <= 8'h00;
	end
	else if(i_MCmd == `OCP_CMD_WRITE)
	begin
		case(i_MAddr)
		LED_REG: o_LED <= i_MData[7:0];
		default: ;
		endcase
	end
end


endmodule /* soc_control */
