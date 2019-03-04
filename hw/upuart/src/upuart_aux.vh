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
 * Auxiliary functions
 */


/* Returns most frequent bit out of 3 bits */
function upuart_vote3;
	input [2:0] i_bv;
	upuart_vote3 = (i_bv[0] && i_bv[1]) || (i_bv[0] && i_bv[2]) || (i_bv[1] && i_bv[2]);
endfunction


/* Returns most frequent bit out of 5 bits */
function upuart_vote5;
	input [4:0] i_bv;
	reg b0, b1, b2;
	reg c0, c1, c2;
	begin
		b0 = upuart_vote3( {i_bv[0], i_bv[1], i_bv[2]} );
		b1 = upuart_vote3( {i_bv[1], i_bv[2], i_bv[3]} );
		b2 = upuart_vote3( {i_bv[2], i_bv[3], i_bv[4]} );
		c0 = upuart_vote3( {b0, i_bv[3], i_bv[4]} );
		c1 = upuart_vote3( {b1, i_bv[0], i_bv[4]} );
		c2 = upuart_vote3( {b2, i_bv[0], i_bv[1]} );
		upuart_vote5 = upuart_vote3( {c0, c1, c2} );
	end
endfunction
