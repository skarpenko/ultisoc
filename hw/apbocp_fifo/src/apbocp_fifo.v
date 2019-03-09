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
 * OCP to APB FIFO
 */

module apbocp_fifo
#(
	parameter APB_ADDR_WIDTH = 32,
	parameter OCP_ADDR_WIDTH = 32,
	parameter DATA_WIDTH = 32,
	parameter FIFO_DEPTH = 256,
	parameter FIFO_DATA_WIDTH = 8
)
(
	clk,
	nrst,
	/* APB interface */
	apb_paddr,
	apb_psel,
	apb_penable,
	apb_pwrite,
	apb_pwdata,
	apb_prdata,
	apb_pready,
	/* OCP interface */
	ocp_maddr,
	ocp_mcmd,
	ocp_mdata,
	ocp_mbyteen,
	ocp_scmdaccept,
	ocp_sdata,
	ocp_sresp
);
/* OCP commands */
localparam [2:0] OCP_CMD_IDLE	= 3'h0;	/* Idle */
localparam [2:0] OCP_CMD_WRITE	= 3'h1;	/* Write command */
localparam [2:0] OCP_CMD_READ	= 3'h2;	/* Read command */
/* OCP responses */
localparam [1:0] OCP_RESP_NULL	= 2'h0;	/* NULL response */
localparam [1:0] OCP_RESP_DVA	= 2'h1;	/* Data valid */
localparam [1:0] OCP_RESP_FAIL	= 2'h2;	/* Fail response */
localparam [1:0] OCP_RESP_ERR	= 2'h3;	/* Error response */
/****/
localparam [15:0] FIFO_MASK	= FIFO_DEPTH - 1;
/****/
localparam FIFO_PTR_WIDTH	= 16;
/****/
localparam LAST_READ		= 1'b0;
localparam LAST_WRITE		= 1'b1;
/****/
localparam APB_SETUP		= 1'b0;	/* APB FSM Setup phase */
localparam APB_ENABLE		= 1'b1;	/* APB FSM Enable phase */
/****/
input wire			clk;
input wire			nrst;
/* APB interface */
input wire [APB_ADDR_WIDTH-1:0]	apb_paddr;
input wire			apb_psel;
input wire			apb_penable;
input wire			apb_pwrite;
input wire [DATA_WIDTH-1:0]	apb_pwdata;
output reg [DATA_WIDTH-1:0]	apb_prdata;
output reg			apb_pready;
/* OCP interface */
input wire [OCP_ADDR_WIDTH-1:0]	ocp_maddr;
input wire [2:0]		ocp_mcmd;
input wire [DATA_WIDTH-1:0]	ocp_mdata;
input wire [DATA_WIDTH/8-1:0]	ocp_mbyteen;
output wire			ocp_scmdaccept;
output reg [DATA_WIDTH-1:0]	ocp_sdata;
output reg [1:0]		ocp_sresp;

assign ocp_scmdaccept = 1'b1;	/* OCP FSM is always ready to accept command */


/* FIFO memory */
reg [FIFO_DATA_WIDTH-1:0] fifo[0:FIFO_DEPTH-1];

/* Read / write pointers */
reg [FIFO_PTR_WIDTH-1:0] rd_ptr;
reg [FIFO_PTR_WIDTH-1:0] wr_ptr;
/* Last operation (RD/WR) */
reg last_op;

/* FIFO status */
wire fifo_empty = (rd_ptr == wr_ptr) && (last_op == LAST_READ);
wire fifo_full = (rd_ptr == wr_ptr) && (last_op == LAST_WRITE);

/* APB FSM state */
reg apb_state;


/* APB FSM */
always @(posedge clk or negedge nrst)
begin : apb_fsm
	if(!nrst)
	begin
		rd_ptr <= {(FIFO_PTR_WIDTH){1'b0}};
		apb_prdata <= {(DATA_WIDTH){1'b0}};
		apb_pready <= 1'b0;
		apb_state <= APB_SETUP;
	end
	else if(apb_state == APB_SETUP)
	begin
		apb_pready <= 1'b0;
		if(apb_psel && apb_penable)
			apb_state <= APB_ENABLE;
	end
	else if(apb_psel && apb_penable && apb_state == APB_ENABLE)
	begin
			apb_pready <= 1'b1;
			apb_state <= APB_SETUP;
			if(!apb_pwrite && !(|apb_paddr) && !fifo_empty)
			begin
				apb_prdata <= { 1'b0, fifo_full,
					{(DATA_WIDTH-FIFO_DATA_WIDTH-2){1'b0}},
						fifo[rd_ptr] };
				rd_ptr <= (rd_ptr + 1'b1) & FIFO_MASK;
			end
			else if(!apb_pwrite && !(|apb_paddr))
			begin
				apb_prdata <= { 1'b1, fifo_full, {(DATA_WIDTH-2){1'b0}} };
			end
			else if(!apb_pwrite)
			begin
				apb_prdata <= {(DATA_WIDTH){1'b1}};
			end
	end
	else
		apb_state <= APB_SETUP;
end


/* OCP FSM */
always @(posedge clk or negedge nrst)
begin : ocp_fsm
	if(!nrst)
	begin
		wr_ptr <= {(FIFO_PTR_WIDTH){1'b0}};
		ocp_sdata <= {(DATA_WIDTH){1'b0}};
		ocp_sresp <= OCP_RESP_NULL;
	end
	else
	begin
		ocp_sresp <= OCP_RESP_NULL;
		ocp_sdata <= {(DATA_WIDTH){1'b0}};
		if(ocp_mcmd != OCP_CMD_IDLE)
		begin
			ocp_sresp <= OCP_RESP_DVA;
			if(ocp_mcmd == OCP_CMD_WRITE && !(|ocp_maddr) && !fifo_full)
			begin
				fifo[wr_ptr] <= ocp_mdata[FIFO_DATA_WIDTH-1:0];
				wr_ptr <= (wr_ptr + 1'b1) & FIFO_MASK;
			end
			else if(ocp_mcmd == OCP_CMD_READ)
			begin
				ocp_sdata <= { fifo_empty, fifo_full, {(DATA_WIDTH-2){1'b0}} };
			end
		end
	end
end


/* Last operation */
always @(posedge clk or negedge nrst)
begin : last_operation
	reg read;
	reg write;

	if(!nrst)
	begin
		last_op <= LAST_READ;
	end
	else
	begin
		read = apb_psel && apb_penable && !apb_pwrite && !(|apb_paddr) && !fifo_empty;
		write = (ocp_mcmd == OCP_CMD_WRITE) && !(|ocp_maddr) && !fifo_full;

		if(read)
			last_op <= LAST_READ;
		else if(write)
			last_op <= LAST_WRITE;
	end
end


endmodule /* apbocp_fifo */
