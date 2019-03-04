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
 * UART testbench
 */


`ifndef TRACE_FILE
`define TRACE_FILE "trace.vcd"
`endif


module tb_upuart();
	localparam HCLK = 10;
	localparam PCLK = 2*HCLK;	/* Clock period */

	/* OCP commands */
	localparam [2:0] OCP_CMD_IDLE	= 3'h0;	/* Idle */
	localparam [2:0] OCP_CMD_WRITE	= 3'h1;	/* Write command */
	localparam [2:0] OCP_CMD_READ	= 3'h2;	/* Read command */

	/* TX data */
	localparam [31:0] UART1_CHAR	= 32'h002A;
	localparam [31:0] UART2_CHAR	= 32'h002B;


	reg clk;
	reg nrst;

	/* UART wires */
	wire rxd;
	wire txd;
	wire cts;
	wire rts;

	/* UART1 bus interface wires */
	wire intr1;
	reg [31:0] MAddr1;
	reg [2:0] MCmd1;
	reg [31:0] MData1;
	reg [3:0] MByteEn1;
	wire SCmdAccept1;
	wire [31:0] SData1;
	wire [1:0] SResp1;

	/* UART2 bus interface wires */
	wire intr2;
	reg [31:0] MAddr2;
	reg [2:0] MCmd2;
	reg [31:0] MData2;
	reg [3:0] MByteEn2;
	wire SCmdAccept2;
	wire [31:0] SData2;
	wire [1:0] SResp2;

	/* Received data */
	reg [7:0] data1;
	reg [7:0] data2;


	always
		#HCLK clk = !clk;


	always @(intr1)
		if(intr1)
			$write("UART1 interrupt line asserted\n");
		else
			$write("UART1 interrupt line de-asserted\n");


	always @(intr2)
		if(intr2)
			$write("UART2 interrupt line asserted\n");
		else
			$write("UART2 interrupt line de-asserted\n");


	task wait_pos_clk;
		@(posedge clk);
	endtask


	initial
	begin
		/* Set tracing */
		$dumpfile(`TRACE_FILE);
		$dumpvars(0, tb_upuart);

		clk = 1;
		nrst = 0;
		MAddr1 = 0;
		MCmd1 = 0;
		MData1 = 0;
		MByteEn1 = 0;
		MAddr2 = 0;
		MCmd2 = 0;
		MData2 = 0;
		MByteEn2 = 0;

		#(10*PCLK) nrst = 1;


		/* Set baud rate */
		wait_pos_clk();
		MAddr1 <= 32'h004;
		MData1 <= 32'd27;	/* UART1 baud rate */
		MCmd1 <= OCP_CMD_WRITE;
		MAddr2 <= 32'h004;
		MData2 <= 32'd27;	/* UART2 baud rate */
		MCmd2 <= OCP_CMD_WRITE;

		wait_pos_clk();
		MCmd1 <= OCP_CMD_IDLE;
		MCmd2 <= OCP_CMD_IDLE;

		/* Unmask interrupts */
		wait_pos_clk();
		MAddr1 <= 32'h000;
		MData1 <= 32'b000001;	/* unmask RX interrupt for UART1 */
		MCmd1 <= OCP_CMD_WRITE;
		MAddr2 <= 32'h000;
		MData2 <= 32'b000001;	/* unmask RX interrupt for UART2 */
		MCmd2 <= OCP_CMD_WRITE;

		wait_pos_clk();
		MCmd1 <= OCP_CMD_IDLE;
		MCmd2 <= OCP_CMD_IDLE;

		/* Send bytes */
		wait_pos_clk();
		$write("UART1 is sending '%c'\n", UART1_CHAR);
		$write("UART2 is sending '%c'\n", UART2_CHAR);
		MAddr1 <= 32'h008;
		MData1 <= UART1_CHAR;
		MCmd1 <= OCP_CMD_WRITE;
		MAddr2 <= 32'h008;
		MData2 <= UART2_CHAR;
		MCmd2 <= OCP_CMD_WRITE;

		wait_pos_clk();
		MCmd1 <= OCP_CMD_IDLE;
		MCmd2 <= OCP_CMD_IDLE;

		#150000 ;
		wait_pos_clk();

		/* Read received bytes */
		wait_pos_clk();
		MAddr1 <= 32'h008;
		MCmd1 <= OCP_CMD_READ;
		MAddr2 <= 32'h008;
		MCmd2 <= OCP_CMD_READ;

		wait_pos_clk();
		MCmd1 <= OCP_CMD_IDLE;
		MCmd2 <= OCP_CMD_IDLE;
		data1 <= SData1;
		data2 <= SData2;

		/* Print bytes */
		wait_pos_clk();
		$write("UART1 received '%c'\n", data1);
		$write("UART2 received '%c'\n", data2);

		/* Finish */
		wait_pos_clk();
		$write("\n");

		#10000 $finish;
	end


	/* UART1 instance */
	upuart_top uart1(
		.clk(clk),
		.nrst(nrst),
		/* Interrupt output */
		.o_intr(intr1),
		/* UART */
		.rxd(rxd),
		.txd(txd),
		.cts(cts),
		.rts(rts),
		/* OCP interface */
		.i_MAddr(MAddr1),
		.i_MCmd(MCmd1),
		.i_MData(MData1),
		.i_MByteEn(MByteEn1),
		.o_SCmdAccept(SCmdAccept1),
		.o_SData(SData1),
		.o_SResp(SResp1)
	);


	/* UART2 instance */
	upuart_top uart2(
		.clk(clk),
		.nrst(nrst),
		/* Interrupt output */
		.o_intr(intr2),
		/* UART */
		.rxd(txd),
		.txd(rxd),
		.cts(rts),
		.rts(cts),
		/* OCP interface */
		.i_MAddr(MAddr2),
		.i_MCmd(MCmd2),
		.i_MData(MData2),
		.i_MByteEn(MByteEn2),
		.o_SCmdAccept(SCmdAccept2),
		.o_SData(SData2),
		.o_SResp(SResp2)
	);


endmodule /* tb_upuart */
