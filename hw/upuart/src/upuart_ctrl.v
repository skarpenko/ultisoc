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
 * UART control unit
 */


/* Control */
module upuart_ctrl #(
	parameter DATA_WIDTH = 32,
	parameter FIFO_WIDTH = 8,
	parameter FIFO_DEPTH2 = 8,
	parameter DIVDR_WIDTH = 16
)
(
	clk,
	nrst,
	/* Read / Write */
	rd,
	wr,
	/* UART registers */
	ctrlr,
	divdr,
	datar,
	fifor,
	rdata,
	wdata,
	/* TX FIFO state */
	tx_fifo_count,
	tx_fifo_full,
	tx_fifo_empty,
	tx_fifo_data,
	tx_fifo_wr,
	/* RX FIFO state */
	rx_fifo_count,
	rx_fifo_full,
	rx_fifo_empty,
	rx_fifo_data,
	rx_fifo_rd,
	/* Baud rate counter */
	count,
	/* Interrupt */
	intr
);
input wire			clk;
input wire			nrst;
/* Read / Write */
input wire			rd;
input wire			wr;
/* UART registers */
input wire			ctrlr;
input wire			divdr;
input wire			datar;
input wire			fifor;
output reg [DATA_WIDTH-1:0]	rdata;
input wire [DATA_WIDTH-1:0]	wdata;
/* TX FIFO state */
input wire [FIFO_DEPTH2:0]	tx_fifo_count;
input wire			tx_fifo_full;
input wire			tx_fifo_empty;
output reg [FIFO_WIDTH-1:0]	tx_fifo_data;
output reg			tx_fifo_wr;
/* RX FIFO state */
input wire [FIFO_DEPTH2:0]	rx_fifo_count;
input wire			rx_fifo_full;
input wire			rx_fifo_empty;
input wire [FIFO_WIDTH-1:0]	rx_fifo_data;
output reg			rx_fifo_rd;
/* Baud rate counter */
output wire [DIVDR_WIDTH-1:0]	count;
/* Interrupt */
output wire			intr;


reg			tx_imask;	/* TX interrupt mask */
reg			rx_imask;	/* RX interrupt mask */
reg [DIVDR_WIDTH-1:0]	countr;		/* Baud rate counter register */

assign count = countr;

/* Interrupt state */
assign intr = (tx_fifo_empty & ~tx_imask) | (~rx_fifo_empty & ~rx_imask);


/* Read UART registers comb logic */
always @(*)
begin
	if(rd && ctrlr)
		rdata = { {(DATA_WIDTH-6){1'b0}},
				rx_fifo_empty, rx_fifo_full, rx_imask,
				tx_fifo_empty, tx_fifo_full, tx_imask };
	else if(rd && divdr)
		rdata = { {(DATA_WIDTH-DIVDR_WIDTH){1'b0}}, countr };
	else if(rd && datar)
		rdata = { {(DATA_WIDTH-FIFO_WIDTH){1'b0}}, rx_fifo_data };
	else if(rd && fifor)
		rdata = { {(DATA_WIDTH/2-FIFO_DEPTH2-1){1'b0}}, rx_fifo_count,
			 {(DATA_WIDTH/2-FIFO_DEPTH2-1){1'b0}}, tx_fifo_count };
	else
		rdata = {(DATA_WIDTH){1'b0}};
end


/* Read/write UART registers sequential logic */
always @(posedge clk or negedge nrst)
begin
	if(!nrst)
	begin
		tx_imask <= 1'b1;
		rx_imask <= 1'b1;
		countr <= {(DIVDR_WIDTH){1'b0}};
		rx_fifo_rd <= 1'b0;
		tx_fifo_data <= {(FIFO_WIDTH){1'b0}};
		tx_fifo_wr <= 1'b0;
	end
	else
	begin
		rx_fifo_rd <= 1'b0;
		tx_fifo_wr <= 1'b0;

		if(wr && ctrlr)
		begin
			tx_imask <= wdata[0];
			rx_imask <= wdata[3];
		end
		else if(wr && divdr)
		begin
			countr <= wdata[DIVDR_WIDTH-1:0];
		end
		else if(wr && datar)
		begin
			tx_fifo_wr <= 1'b1;
			tx_fifo_data <= wdata[FIFO_WIDTH-1:0];
		end
		else if(rd && datar)
			rx_fifo_rd <= 1'b1;
	end
end


endmodule /* upuart_ctrl */
