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
 * OCP to APB FIFO testbench
 */


`ifndef TRACE_FILE
`define TRACE_FILE "trace.vcd"
`endif


module tb_apbocp_fifo();
	localparam HCLK = 5;
	localparam PCLK = 2*HCLK;	/* Clock period */
	localparam ADDR_WIDTH = 32;
	localparam DATA_WIDTH = 32;
	/* OCP commands */
	localparam [2:0] OCP_CMD_IDLE	= 3'h0;	/* Idle */
	localparam [2:0] OCP_CMD_WRITE	= 3'h1;	/* Write command */
	localparam [2:0] OCP_CMD_READ	= 3'h2;	/* Read command */
	/* OCP responses */
	localparam [1:0] OCP_RESP_NULL	= 2'h0;	/* NULL response */
	localparam [1:0] OCP_RESP_DVA	= 2'h1;	/* Data valid */
	localparam [1:0] OCP_RESP_FAIL	= 2'h2;	/* Fail response */
	localparam [1:0] OCP_RESP_ERR	= 2'h3;	/* Error response */

	reg clk;
	reg nrst;

	/* APB interface */
	reg [ADDR_WIDTH-1:0]	apb_paddr;
	reg			apb_psel;
	reg			apb_penable;
	reg			apb_pwrite;
	reg [DATA_WIDTH-1:0]	apb_pwdata;
	wire [DATA_WIDTH-1:0]	apb_prdata;
	wire			apb_pready;
	/* OCP interface */
	reg [ADDR_WIDTH-1:0]	ocp_maddr;
	reg [2:0]		ocp_mcmd;
	reg [DATA_WIDTH-1:0]	ocp_mdata;
	reg [DATA_WIDTH/8-1:0]	ocp_mbyteen;
	wire			ocp_scmdaccept;
	wire [DATA_WIDTH-1:0]	ocp_sdata;
	wire [1:0]		ocp_sresp;

	always
		#HCLK clk = !clk;


	/**************** OCP *********************************/

	/* Issue bus read transaction */
	task ocp_bus_read;
	input [ADDR_WIDTH-1:0] addr;
	begin
		@(posedge clk)
		begin
			ocp_maddr <= addr;
			ocp_mbyteen <= 4'hf;
			ocp_mcmd <= OCP_CMD_READ;
		end

		@(posedge clk)
		begin
			ocp_maddr <= 0;
			ocp_mbyteen <= 4'h0;
			ocp_mcmd <= OCP_CMD_IDLE;
		end
	end
	endtask

	/* Issue bus write transaction */
	task ocp_bus_write;
	input [ADDR_WIDTH-1:0] addr;
	input [DATA_WIDTH-1:0] data;
	begin
		@(posedge clk)
		begin
			ocp_maddr <= addr;
			ocp_mdata <= data;
			ocp_mbyteen <= 4'hf;
			ocp_mcmd <= OCP_CMD_WRITE;
		end

		@(posedge clk)
		begin
			ocp_maddr <= 0;
			ocp_mdata <= 0;
			ocp_mbyteen <= 4'h0;
			ocp_mcmd <= OCP_CMD_IDLE;
		end
	end
	endtask

	/***************** APB *****************************/

	/* Issue bus read transaction */
	task apb_bus_read;
	input [ADDR_WIDTH-1:0] addr;
	begin
		@(posedge clk)
		begin
			apb_paddr <= addr;
			apb_psel <= 1'b1;
			apb_penable <= 1'b1;
			apb_pwrite <= 1'b0;
		end

		@(posedge clk)
		begin
			apb_paddr <= 0;
			apb_psel <= 1'b0;
			apb_penable <= 1'b0;
			apb_pwrite <= 1'b0;
		end
	end
	endtask

	/* Issue bus write transaction */
	task apb_bus_write;
	input [ADDR_WIDTH-1:0] addr;
	input [DATA_WIDTH-1:0] data;
	begin
		@(posedge clk)
		begin
			apb_paddr <= addr;
			apb_psel <= 1'b1;
			apb_penable <= 1'b1;
			apb_pwrite <= 1'b1;
			apb_pwdata <= data;
		end

		@(posedge clk)
		begin
			apb_paddr <= 0;
			apb_psel <= 1'b0;
			apb_penable <= 1'b0;
			apb_pwrite <= 1'b0;
			apb_pwdata <= 0;
		end
	end
	endtask


	initial
	begin
		/* Set tracing */
		$dumpfile(`TRACE_FILE);
		$dumpvars(0, tb_apbocp_fifo);

		clk = 1;
		nrst = 0;
		apb_paddr = 0;
		apb_psel = 0;
		apb_penable = 0;
		apb_pwrite = 0;
		apb_pwdata = 0;
		ocp_maddr = 0;
		ocp_mcmd = 0;
		ocp_mdata = 0;
		ocp_mbyteen = 0;

		#(10*PCLK) nrst = 1;

		#(2*PCLK)

		ocp_bus_read(0);
		@(posedge clk);
		apb_bus_read(0);
		@(posedge clk);

		ocp_bus_write(0, 1);
		@(posedge clk);
		ocp_bus_write(0, 2);
		@(posedge clk);
		ocp_bus_write(0, 3);
		@(posedge clk);
		ocp_bus_write(0, 4);
		@(posedge clk);
		ocp_bus_write(0, 5);
		@(posedge clk);

		apb_bus_read(0);
		@(posedge clk);
		apb_bus_read(0);
		@(posedge clk);
		apb_bus_read(0);
		@(posedge clk);
		apb_bus_read(0);
		@(posedge clk);
		apb_bus_read(0);
		@(posedge clk);
		apb_bus_read(0);
		@(posedge clk);


		#500 $finish;
	end


	apbocp_fifo #(.FIFO_DEPTH(4)) fifo (
		.clk(clk),
		.nrst(nrst),
		/* APB interface */
		.apb_paddr(apb_paddr),
		.apb_psel(apb_psel),
		.apb_penable(apb_penable),
		.apb_pwrite(apb_pwrite),
		.apb_pwdata(apb_pwdata),
		.apb_prdata(apb_prdata),
		.apb_pready(apb_pready),
		/* OCP interface */
		.ocp_maddr(ocp_maddr),
		.ocp_mcmd(ocp_mcmd),
		.ocp_mdata(ocp_mdata),
		.ocp_mbyteen(ocp_mbyteen),
		.ocp_scmdaccept(ocp_scmdaccept),
		.ocp_sdata(ocp_sdata),
		.ocp_sresp(ocp_sresp)
	);

endmodule /* tb_apbocp_fifo */
