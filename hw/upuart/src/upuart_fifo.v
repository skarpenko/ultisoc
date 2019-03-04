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
 * UART FIFO
 */


/* FIFO */
module upuart_fifo #(
	parameter DATA_WIDTH = 32,	/* FIFO data width */
	parameter DEPTH_POW2 = 2	/* FIFO depth = 2^DEPTH_POW2 */
)
(
	clk,
	nrst,
	/* Data read/write */
	data_in,
	data_out,
	rd,
	wr,
	/* FIFO state */
	count,
	full,
	empty
);
input wire			clk;
input wire			nrst;
/* Data read/write */
input wire [DATA_WIDTH-1:0]	data_in;
output wire [DATA_WIDTH-1:0]	data_out;
input wire			rd;
input wire			wr;
/* FIFO state */
output reg [DEPTH_POW2:0]	count;
output wire			full;
output wire			empty;


reg [DATA_WIDTH-1:0] fifo_buf[0:2**DEPTH_POW2-1];	/* FIFO buffer */
reg [DEPTH_POW2-1:0] rd_p;				/* Read pointer */
reg [DEPTH_POW2-1:0] wr_p;				/* Write pointer */

reg wr_nrd;	/* Last operation (read or write) */

assign empty = (wr_nrd == 1'b0) && (rd_p == wr_p);	/* FIFO is empty */
assign full = (wr_nrd == 1'b1) && (rd_p == wr_p);	/* FIFO is full */
assign data_out = fifo_buf[rd_p];			/* Current read data */


/* FIFO read logic */
always @(posedge clk or negedge nrst)
begin
	if(!nrst)
		rd_p <= {(DEPTH_POW2){1'b0}};
	else if(!empty && rd)
		rd_p <= rd_p + 1'b1;
end


/* FIFO write logic */
always @(posedge clk or negedge nrst)
begin
	if(!nrst)
		wr_p <= {(DEPTH_POW2){1'b0}};
	else if(!full && wr)
	begin
		fifo_buf[wr_p] <= data_in;
		wr_p <= wr_p + 1'b1;
	end
end


/* Last operation logic */
always @(posedge clk or negedge nrst)
begin
	if(!nrst)
		wr_nrd <= 1'b0;
	else if(!empty && rd && !wr)
		wr_nrd <= 1'b0;
	else if(!full && !rd && wr)
		wr_nrd <= 1'b1;
end


/* Words count logic */
always @(posedge clk or negedge nrst)
begin
	if(!nrst)
		count <= {(DEPTH_POW2+1){1'b0}};
	else if(!empty && rd && !wr)
		count <= count - 1'b1;
	else if(!full && !rd && wr)
		count <= count + 1'b1;
end


endmodule /* upuart_fifo */
