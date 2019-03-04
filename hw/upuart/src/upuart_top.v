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
 * UART top
 */


/* UART */
module upuart_top #(
	parameter ADDR_WIDTH = 32,
	parameter DATA_WIDTH = 32,
	parameter BEN_WIDTH = (DATA_WIDTH/8)
)
(
	clk,
	nrst,
	/* Interrupt output */
	o_intr,
	/* UART */
	rxd,	/* Receive data (RxD ) */
	txd,	/* Transmit data (TxD) */
	cts,	/* Clear to send (CTS) */
	rts,	/* Request to send (RTS) */
	/* OCP interface */
	i_MAddr,
	i_MCmd,
	i_MData,
	i_MByteEn,
	o_SCmdAccept,
	o_SData,
	o_SResp
);
/* Parameters */
localparam COUNT_WIDTH	= 16;
localparam FIFO_WIDTH	= 8;
localparam FIFO_DEPTH2	= 8;

/* Inputs and outputs */
input wire			clk;
input wire			nrst;
/* Interrupt output */
output wire			o_intr;
/* UART */
input wire			rxd;
output wire			txd;
input wire			cts;
output wire			rts;
/* OCP interface */
input wire [ADDR_WIDTH-1:0]	i_MAddr;
input wire [2:0]		i_MCmd;
input wire [DATA_WIDTH-1:0]	i_MData;
input wire [BEN_WIDTH-1:0]	i_MByteEn;
output wire			o_SCmdAccept;
output wire [DATA_WIDTH-1:0]	o_SData;
output wire [1:0]		o_SResp;


/* De-assert request to send if RX FIFO is nearly full */
assign rts = &rx_fifo_count[FIFO_DEPTH2:2];


/* Bus interface wires */
wire			bui_rd;
wire			bui_wr;
wire			bui_ctrlr;
wire			bui_divdr;
wire			bui_datar;
wire			bui_fifor;
wire [DATA_WIDTH-1:0]	bui_rdata;
wire [DATA_WIDTH-1:0]	bui_wdata;

/* Control unit wires */
wire [COUNT_WIDTH-1:0]	ctrl_count;

/* TX FIFO wires */
wire [FIFO_DEPTH2:0]	tx_fifo_count;
wire			tx_fifo_full;
wire			tx_fifo_empty;
wire [FIFO_WIDTH-1:0]	tx_fifo_data_i;
wire [FIFO_WIDTH-1:0]	tx_fifo_data_o;
wire			tx_fifo_rd;
wire			tx_fifo_wr;

/* TX baud rate */
wire			tx_brenable;
wire			tx_baud_rate;

/* RX FIFO wires */
wire [FIFO_DEPTH2:0]	rx_fifo_count;
wire			rx_fifo_full;
wire			rx_fifo_empty;
wire [FIFO_WIDTH-1:0]	rx_fifo_data_i;
wire [FIFO_WIDTH-1:0]	rx_fifo_data_o;
wire			rx_fifo_rd;
wire			rx_fifo_wr;

/* RX baud rate */
wire			rx_brreset;
wire			rx_baud_rate;


/* OCP bus interface adapter */
upuart_ocp_if #(
	.ADDR_WIDTH(ADDR_WIDTH),
	.DATA_WIDTH(DATA_WIDTH)
) bus_if (
	/* OCP interface */
	.i_MAddr(i_MAddr),
	.i_MCmd(i_MCmd),
	.i_MData(i_MData),
	.i_MByteEn(i_MByteEn),
	.o_SCmdAccept(o_SCmdAccept),
	.o_SData(o_SData),
	.o_SResp(o_SResp),
	/* Internal interface */
	.rd(bui_rd),
	.wr(bui_wr),
	.ctrlr(bui_ctrlr),
	.divdr(bui_divdr),
	.datar(bui_datar),
	.fifor(bui_fifor),
	.rdata(bui_rdata),
	.wdata(bui_wdata)
);


/* TX baud rate generator */
upuart_brgen #(
	.COUNT_WIDTH(COUNT_WIDTH)
) tx_brgen (
	.clk(clk),
	.nrst(nrst),
	.count_val(ctrl_count),
	.ovrsamp(1'b0),
	.enable(tx_brenable),
	.reset(1'b0),
	.baud_rate(tx_baud_rate)
);


/* RX baud rate generator */
upuart_brgen #(
	.COUNT_WIDTH(COUNT_WIDTH)
) rx_brgen (
	.clk(clk),
	.nrst(nrst),
	.count_val(ctrl_count),
	.ovrsamp(1'b1),
	.enable(1'b1),
	.reset(rx_brreset),
	.baud_rate(rx_baud_rate)
);


/* TX FIFO */
upuart_fifo #(
	.DATA_WIDTH(FIFO_WIDTH),
	.DEPTH_POW2(FIFO_DEPTH2)
) tx_fifo (
	.clk(clk),
	.nrst(nrst),
	.data_in(tx_fifo_data_i),
	.data_out(tx_fifo_data_o),
	.rd(tx_fifo_rd),
	.wr(tx_fifo_wr),
	.count(tx_fifo_count),
	.full(tx_fifo_full),
	.empty(tx_fifo_empty)
);


/* RX FIFO */
upuart_fifo #(
	.DATA_WIDTH(FIFO_WIDTH),
	.DEPTH_POW2(FIFO_DEPTH2)
) rx_fifo (
	.clk(clk),
	.nrst(nrst),
	.data_in(rx_fifo_data_i),
	.data_out(rx_fifo_data_o),
	.rd(rx_fifo_rd),
	.wr(rx_fifo_wr),
	.count(rx_fifo_count),
	.full(rx_fifo_full),
	.empty(rx_fifo_empty)
);


/* Receiver */
upuart_rx rx(
	.clk(clk),
	.nrst(nrst),
	.data_out(rx_fifo_data_i),
	.data_wr(rx_fifo_wr),
	.uclk(rx_baud_rate),
	.brreset(rx_brreset),
	.rxd(rxd)
);


/* Transmitter */
upuart_tx tx(
	.clk(clk),
	.nrst(nrst),
	.data_in(tx_fifo_data_o),
	.data_valid(~tx_fifo_empty),
	.data_rd(tx_fifo_rd),
	.uclk(tx_baud_rate),
	.uclk_rx(rx_baud_rate),
	.brenable(tx_brenable),
	.cts(cts),
	.txd(txd)
);


/* Control unit */
upuart_ctrl #(
	.DATA_WIDTH(DATA_WIDTH),
	.FIFO_WIDTH(FIFO_WIDTH),
	.FIFO_DEPTH2(FIFO_DEPTH2),
	.DIVDR_WIDTH(COUNT_WIDTH)
) ctrl (
	.clk(clk),
	.nrst(nrst),
	.rd(bui_rd),
	.wr(bui_wr),
	.ctrlr(bui_ctrlr),
	.divdr(bui_divdr),
	.datar(bui_datar),
	.fifor(bui_fifor),
	.rdata(bui_rdata),
	.wdata(bui_wdata),
	.tx_fifo_count(tx_fifo_count),
	.tx_fifo_full(tx_fifo_full),
	.tx_fifo_empty(tx_fifo_empty),
	.tx_fifo_data(tx_fifo_data_i),
	.tx_fifo_wr(tx_fifo_wr),
	.rx_fifo_count(rx_fifo_count),
	.rx_fifo_full(rx_fifo_full),
	.rx_fifo_empty(rx_fifo_empty),
	.rx_fifo_data(rx_fifo_data_o),
	.rx_fifo_rd(rx_fifo_rd),
	.count(ctrl_count),
	.intr(o_intr)
);


endmodule /* upuart_top */
