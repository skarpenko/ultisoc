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
 * OCP bus interface
 */


/* OCP interface */
module upuart_ocp_if #(
	parameter ADDR_WIDTH = 32,
	parameter DATA_WIDTH = 32,
	parameter BEN_WIDTH = (DATA_WIDTH/8)
)
(
	/* OCP interface */
	i_MAddr,
	i_MCmd,
	i_MData,
	i_MByteEn,
	o_SCmdAccept,
	o_SData,
	o_SResp,
	/* Internal interface */
	rd,
	wr,
	ctrlr,
	divdr,
	datar,
	fifor,
	rdata,
	wdata
);
/* OCP commands */
localparam [2:0] OCP_CMD_IDLE	= 3'h0;		/* Idle */
localparam [2:0] OCP_CMD_WRITE	= 3'h1;		/* Write command */
localparam [2:0] OCP_CMD_READ	= 3'h2;		/* Read command */

/* OCP responses */
localparam [1:0] OCP_RESP_NULL	= 2'h0;		/* NULL response */
localparam [1:0] OCP_RESP_DVA	= 2'h1;		/* Data valid */
localparam [1:0] OCP_RESP_FAIL	= 2'h2;		/* Fail response */
localparam [1:0] OCP_RESP_ERR	= 2'h3;		/* Error response */

/* Register offsets */
localparam [ADDR_WIDTH-1:0] CTRLREG = 4'h0;	/* Control register */
localparam [ADDR_WIDTH-1:0] DIVDREG = 4'h4;	/* Divider register */
localparam [ADDR_WIDTH-1:0] DATAREG = 4'h8;	/* Data register */
localparam [ADDR_WIDTH-1:0] FIFOREG = 4'hC;	/* FIFO register */

/* OCP interface */
input wire [ADDR_WIDTH-1:0]	i_MAddr;
input wire [2:0]		i_MCmd;
input wire [DATA_WIDTH-1:0]	i_MData;
input wire [BEN_WIDTH-1:0]	i_MByteEn;
output wire			o_SCmdAccept;
output reg [DATA_WIDTH-1:0]	o_SData;
output reg [1:0]		o_SResp;
/* Internal interface */
output reg			rd;
output reg			wr;
output reg			ctrlr;
output reg			divdr;
output reg			datar;
output reg			fifor;
input wire [DATA_WIDTH-1:0]	rdata;
output reg [DATA_WIDTH-1:0]	wdata;


assign o_SCmdAccept = 1'b1;	/* Always ready to accept command */


/* Bus logic */
always @(*)
begin
	o_SData = {(DATA_WIDTH){1'b0}};
	o_SResp = OCP_RESP_DVA;
	rd = 1'b0;
	wr = 1'b0;
	ctrlr = 1'b0;
	divdr = 1'b0;
	datar = 1'b0;
	fifor = 1'b0;
	wdata = {(DATA_WIDTH){1'b0}};

	if(i_MCmd == OCP_CMD_READ)
	begin
		o_SResp = OCP_RESP_DVA;
		rd = 1'b1;
		o_SData = rdata;

		if(i_MAddr == CTRLREG)
			ctrlr = 1'b1;
		else if(i_MAddr == DIVDREG)
			divdr = 1'b1;
		else if(i_MAddr == DATAREG)
			datar = 1'b1;
		else if(i_MAddr == FIFOREG)
			fifor = 1'b1;
		else
			o_SData = {(DATA_WIDTH){1'b1}};
	end
	else if(i_MCmd == OCP_CMD_WRITE)
	begin
		o_SResp = OCP_RESP_DVA;
		wr = 1'b1;
		wdata = i_MData;

		if(i_MAddr == CTRLREG)
			ctrlr = 1'b1;
		else if(i_MAddr == DIVDREG)
			divdr = 1'b1;
		else if(i_MAddr == DATAREG)
			datar = 1'b1;
		/* FIFO register is read only */
	end
end


endmodule /* upuart_ocp_if */
