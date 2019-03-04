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
 * UART baud rate generator
 */


/* Baud rate generator */
module upuart_brgen #(
	parameter COUNT_WIDTH = 32
)
(
	clk,
	nrst,
	count_val,
	ovrsamp,
	enable,
	reset,
	baud_rate
);
input wire			clk;
input wire			nrst;
input wire [COUNT_WIDTH-1:0]	count_val;
input wire			ovrsamp;
input wire			enable;
input wire			reset;
output wire			baud_rate;


assign baud_rate = uclk && uclk_n;

reg [COUNT_WIDTH-1:0] count;		/* Counter */
reg uclk;
reg uclk_n;

wire en = enable && |count_val;		/* Enabled state */


/* Counter logic */
always @(posedge clk or negedge nrst)
begin
	if(!nrst)
	begin
		count <= {(COUNT_WIDTH){1'b0}};
		uclk <= 1'b1;
		uclk_n <= 1'b0;
	end
	else if(en && !reset)
	begin
		/* Count and generate baud rate */
		uclk_n <= ~uclk;
		if(!count)
		begin
			count <= !ovrsamp ? { count_val[COUNT_WIDTH-4:0], 3'h0 } :
				{ 1'b0, count_val[COUNT_WIDTH-1:1] };
			uclk <= ~uclk;
		end
		else
			count <= count - 1'b1;
	end
	else
	begin
		/* Rearm counter */
		count <= !ovrsamp ? { count_val[COUNT_WIDTH-4:0], 3'h0 } :
			{ 1'b0, count_val[COUNT_WIDTH-1:1] };
		uclk <= 1'b1;
		uclk_n <= 1'b0;
	end
end


endmodule /* upuart_brgen */
