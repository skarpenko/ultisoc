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
 * XModem protocol implementation
 */

#include <crc16_ccitt.h>
#include <xmodem.h>


/* XModem defines */
#define SOH	0x01	/* Start of Header */
#define SOX	0x02	/* Start of Header (1K) */
#define EOT	0x04	/* End Of Transmission */
#define ACK	0x06	/* Acknowledge */
#define NAK	0x15	/* Not Acknowledge */
#define CAN	0x18	/* Cancel */
#define CRQ	0x43	/* Transmission Request (CRC) */


/* XModem protocol sync */
static int xm_sync(struct xm_recvr *xmr, char sync_ch, unsigned retries, unsigned timeout_sec)
{
	char syn = sync_ch;
	int eot = 0, can = 0;	/* CAN and EOT states. We wait for two CANs and do not acknowledge first EOT. */
	int r;
	char rc;	/* Received char */

	/* We need to send ACK or NAK only once */
	if(sync_ch != CRQ)
		sync_ch = 0;

	do {
		if(syn)
			xmr->outb(xmr, syn);

		syn = sync_ch;	/* Restore previous state */

		rc = r = xmr->inb(xmr, timeout_sec);

		if((r < 0 || rc == CAN) && can) {		/* Transmission canceled */
			return CAN;
		} else if((r < 0 || rc == EOT) && eot) {	/* End of transmission */
			xmr->outb(xmr, ACK);
			return EOT;
		} else if(r < 0) {				/* Receive timeout */
			--retries;
			continue;
		}

		if(rc == CAN) {		/* CAN received. Set state and wait for second CAN */
			syn = 0;
			can = 1;
			eot = 0;
			continue;
		} else if(rc == EOT) {	/* EOT received. Send NAK and wait for second EOT. */
			syn = NAK;
			can = 0;
			eot = 1;
			continue;
		}

		can = eot = 0;			/* Reset CAN and EOT states */

		if(rc == SOH || rc == SOX) {	/* Header start received */
			char blk, nblk;

			/* Receive sequence number */
			blk = r = xmr->inb(xmr, timeout_sec);
			if(r < 0) {
				syn = NAK;
				--retries;
				continue;
			}

			/* Receive sequence number ones' complement */
			nblk = r = xmr->inb(xmr, timeout_sec);
			if(r < 0) {
				syn = NAK;
				--retries;
				continue;
			}

			if(blk != ~nblk) {				/* Probably erroneous header start */
				syn = 0;
				continue;
			} else if(blk == xmr->blk_no) {			/* Re-transmission of already received block - skip */
				unsigned i;
				unsigned n = (rc == SOX ? 1026 : 130);
				for(i = 0; i < n; ++i) {
					if(xmr->inb(xmr, timeout_sec) < 0)
						break;
				}
				if(i != n) {
					syn = NAK;
					--retries;
					continue;
				}
			} else if(xmr->blk_no != (char)(blk-1)) {	/* Sequence error. Send cancellation. */
				xmr->outb(xmr, CAN);
				xmr->outb(xmr, CAN);
				return XM_ERR_OOSEQ;
			} else {					/* Valid header received. */
				xmr->cblk_no = blk;
				break;
			}
		}
	} while(retries);

	return retries ? rc : XM_ERR_RETR;
}


/* Receive data block */
static int xm_recv_block(struct xm_recvr *xmr, size_t blk_sz)
{
	size_t i;
	int r;
	crc16_t crc16;
	char *buf = xmr->buf;

	/* Receive data */
	for(i = 0; i < blk_sz; ++i) {
		r = xmr->inb(xmr, 10);
		if(r < 0)
			return NAK;
		*buf = r;
		++buf;
	}

	/* Receive first CRC byte */
	r = xmr->inb(xmr, 10);
	if(r < 0)
		return NAK;

	crc16 = (crc16_t)(r << 8);

	/* Receive second CRC byte */
	r = xmr->inb(xmr, 10);
	if(r < 0)
		return NAK;

	crc16 |= (crc16_t)r;

	/* Verify CRC */
	if(crc16_ccitt(xmr->buf, blk_sz) == crc16) {
		xmr->buf = buf;
		return ACK;
	}

	return NAK;
}


/* Start XModem receive */
int xm_recvr_start_rx(struct xm_recvr *xmr, void *buf)
{
	int r;

	xmr->buf = (char*)buf;
	xmr->blk_no = 0;
	xmr->cblk_no = 0;
	xmr->rx_size = 0;


	/* Start send request sequence */
	r = xm_sync(xmr, CRQ, 10, 3);


	/* Main protocol loop */
	while(1) {
		char *old_buf = xmr->buf;
		size_t blk_sz;

		if(r == EOT) {
			r = XM_ERR_EOT;
			break;
		} else if(r == CAN) {
			r = XM_ERR_CAN;
			break;
		} else if(r < 0)
			break;

		blk_sz = (r == SOX ? 1024 : 128);

		r = xm_recv_block(xmr, blk_sz);

		if(r == ACK && xmr->callback) {		/* User callback */
			int cbr = xmr->callback(xmr, old_buf, blk_sz);
			if(cbr) {
				xmr->outb(xmr, CAN);
				xmr->outb(xmr, CAN);
				r = cbr;
				break;
			}
		}

		/* Update state if acknowledged */
		if(r == ACK) {
			xmr->blk_no = xmr->cblk_no;
			xmr->rx_size += blk_sz;
		}

		/* Send ACK or NAK and synchronize */
		r = xm_sync(xmr, r, 10, 10);
	}

	/* Flush remaining data */
	while(xmr->inb(xmr, 1) >= 0)
		;

	return r;
}
