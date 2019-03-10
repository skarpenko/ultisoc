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
 * SoC top for verilated model
 */

`include "common.vh"


module vl_soc_top(
	clk,
	nrst
);
input wire	clk;
input wire	nrst;


wire [7:0]	LED;
reg		CTS;
wire		RTS;
wire		TxD;
reg		RxD;


always @(negedge nrst)
begin
	CTS <= 1'b0;
	RxD <= 1'b0;
end


/* Instantiate system top */
ultisoc_soc_top sys(
	.clk(clk),
	.nrst(nrst),
	.LED(LED),
	.CTS(CTS),
	.RTS(RTS),
	.TxD(TxD),
	.RxD(RxD)
);


/* UART receiver */

wire rx_brreset;
wire rx_baud_rate;
wire [7:0] rx_data;
wire rx_data_rdy;

/* RX baud rate generator */
upuart_brgen rx_brgen (
	.clk(clk),
	.nrst(nrst),
	.count_val(32'd27),	/* Clock divider for 115200bps */
	.ovrsamp(1'b1),
	.enable(1'b1),
	.reset(rx_brreset),
	.baud_rate(rx_baud_rate)
);

/* Receiver */
upuart_rx rx(
	.clk(clk),
	.nrst(nrst),
	.data_out(rx_data),
	.data_wr(rx_data_rdy),
	.uclk(rx_baud_rate),
	.brreset(rx_brreset),
	.rxd(TxD)
);

always @(posedge clk)
begin
	if(rx_data_rdy)
	begin
		if(rx_data != 8'hFF)
			$write("%c", rx_data);
		else
			$finish; /* Stop simulation if 0xFF received */
	end
end

endmodule /* vl_soc_top */
