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
 * UART transmitter
 */


/* TX */
module upuart_tx(
	clk,
	nrst,
	/* FIFO interface */
	data_in,
	data_valid,
	data_rd,
	/* Baud rate */
	uclk,
	uclk_rx,
	brenable,
	/* UART TX and CTS */
	cts,
	txd
);
input wire		clk;
input wire		nrst;
input wire [7:0]	data_in;
input wire		data_valid;
output reg		data_rd;
input wire		uclk;
input wire		uclk_rx;
output reg		brenable;
input wire		cts;
output reg		txd;

`include "upuart_aux.vh"


reg [8:0]	tx_frame;	/* TX frame: 8 data bits and 1 stop bit */
reg		tx_en;		/* TX enabled */
reg [2:0]	cts_samp;	/* CTS line sampled bits */

/* CTS active */
wire cts_active = upuart_vote3(cts_samp);


/* CTS line sampling (using RX clock uclk_rx) */
always @(posedge clk or negedge nrst)
begin
	if(!nrst)
		cts_samp <= 3'b111;
	else if(uclk_rx)
		cts_samp <= { cts_samp[1:0], cts };
end


/* TX FSM */
always @(posedge clk or negedge nrst)
begin
	if(!nrst)
	begin
		tx_frame <= 9'b0;
		tx_en <= 1'b0;
		txd <= 1'b1;
		brenable <= 1'b0;
		data_rd <= 1'b0;
	end
	else if(!tx_en && !cts_active && data_valid)
	begin
		tx_frame <= { 1'b1 /* stop bit */, data_in };
		data_rd <= 1'b1;
		brenable <= 1'b1;
		txd <= 1'b0;	/* Start bit */
		tx_en <= 1'b1;	/* Start TX */
	end
	else if(tx_en)
	begin
		data_rd <= 1'b0;
		if(uclk && tx_frame)
		begin
			txd <= tx_frame[0];	/* Send next data bit */
			tx_frame <= { 1'b0, tx_frame[8:1] };
		end
		else if(uclk && !tx_frame)
		begin
			brenable <= 1'b0;
			tx_en <= 1'b0;	/* TX complete */
		end
	end
end


endmodule /* upuart_tx */
