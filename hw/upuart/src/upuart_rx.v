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
 * UART receiver
 */


/* RX */
module upuart_rx(
	clk,
	nrst,
	/* FIFO interface */
	data_out,
	data_wr,
	/* Baud rate */
	uclk,
	brreset,
	/* UART RX */
	rxd
);
/* FSM states */
localparam [1:0] RXWAIT	= 2'b00;	/* Wait for RX start */
localparam [1:0] COUNT	= 2'b01;	/* Count time */
localparam [1:0] SAMPLE	= 2'b10;	/* Sample bits */

input wire		clk;
input wire		nrst;
output wire [7:0]	data_out;
output reg		data_wr;
input wire		uclk;
output reg		brreset;
input wire		rxd;

`include "upuart_aux.vh"


reg [8:0] rx_frame;	/* RX frame */
reg [1:0] state;	/* Receiver state */
reg [4:0] count;	/* Timing counter */
reg [3:0] samp;		/* Sampled bits */


assign data_out = rx_frame[8:1];	/* Received byte */

wire rxstart = ~upuart_vote3(samp[2:0]);	/* RX start condition */


/* RX FSM */
always @(posedge clk or negedge nrst)
begin
	if(!nrst)
	begin
		rx_frame <= 9'b0;
		state <= RXWAIT;
		count <= 5'h0;
		samp <= 4'b1111;
		brreset <= 1'b0;
		data_wr <= 1'b0;
	end
	else if(uclk)
	begin
		case(state)
		RXWAIT: begin
			samp <= { samp[2:0], rxd };
			if(rxstart)
			begin
				brreset <= 1'b1;
				state <= COUNT;
				count <= 5'd18;	/* Time to middle of the first data bit */
				rx_frame <= { 1'b1 /* guard bit */, 8'b0 };
			end
		end
		COUNT: begin
			if(!count && !rx_frame[0])
			begin
				samp <= 4'b0001;	/* 1 - guard bit */
				state <= SAMPLE;	/* Ready to sample data */
			end
			else if(!count && rx_frame[0])
			begin
				samp <= 4'b1111;
				data_wr <= 1'b1;
				state <= RXWAIT;	/* Byte received */
			end
			else
				count <= count - 1'b1;
		end
		SAMPLE: begin
			if(samp[3])
			begin
				/* Ad hoc: compensate timing skew */
				count <= rx_frame[3:0] == 4'b1000 ? 5'd12 : 5'd10;
				/* Store sampled bit */
				rx_frame <= { upuart_vote3(samp[2:0]), rx_frame[8:1] };
				state <= COUNT;
			end
			else
				samp <= { samp[2:0], rxd };	/* Sample next */
		end
		default: ;
		endcase
	end
	else
	begin
		data_wr <= 1'b0;
		brreset <= 1'b0;
	end
end


endmodule /* upuart_rx */
