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

#ifndef _BOOT_XMODEM_H_
#define _BOOT_XMODEM_H_

#include <stddef.h>


/* XModem return codes */
#define XM_ERR_EOT	(0)	/* End of transmission */
#define XM_ERR_CAN	(-1)	/* Transmission canceled */
#define XM_ERR_OOSEQ	(-2)	/* Out of sequence error */
#define XM_ERR_RETR	(-3)	/* Max number of retries reached */


/* XModem receiver state */
struct xm_recvr {
	char	*buf;		/* Current receive buffer  */
	char	blk_no;		/* Last received block number */
	char	cblk_no;	/* Current block number (not acknowledged yet) */
	size_t	rx_size;	/* Received data size */
	void	*udata;		/* Optional user data */

	/* Out byte function */
	void (*outb)(struct xm_recvr *xmr, char ch);
	/* In byte with timeout. Tries to receive a byte within specified time. */
	int (*inb)(struct xm_recvr *xmr, unsigned timeout_sec);
	/* User callback */
	int (*callback)(struct xm_recvr *xmr, void *buf, size_t size);
};


/* Init XModem receiver instance */
static inline void xm_recvr_init(struct xm_recvr *xmr, void (*outb)(struct xm_recvr*, char),
	int (*inb)(struct xm_recvr*, unsigned))
{
	xmr->buf = NULL;
	xmr->blk_no = 0;
	xmr->cblk_no = 0;
	xmr->rx_size = 0;
	xmr->udata = NULL;
	xmr->outb = outb;
	xmr->inb = inb;
	xmr->callback = NULL;
}


/* Set new receive buffer */
static inline void xm_recvr_setbuf(struct xm_recvr *xmr, void *buf)
{
	xmr->buf = (char*)buf;
}


/* Get current pointer to receive buffer */
static inline void *xm_recvr_getbuf(struct xm_recvr *xmr)
{
	return xmr->buf;
}


/* Reset received data size */
static inline void xm_recvr_rstrxsize(struct xm_recvr *xmr)
{
	xmr->rx_size = 0;
}


/* Get received data size */
static inline size_t xm_recvr_getrxsize(struct xm_recvr *xmr)
{
	return xmr->rx_size;
}


/* Set user data */
static inline void xm_recvr_setudata(struct xm_recvr *xmr, void *udata)
{
	xmr->udata = udata;
}


/* Get user data */
static inline void *xm_recvr_getudata(struct xm_recvr *xmr)
{
	return xmr->udata;
}


/* Set user callback */
static inline void xm_recvr_setucb(struct xm_recvr *xmr, int (*callback)(struct xm_recvr*, void*, size_t))
{
	xmr->callback = callback;
}


/* Start receive */
int xm_recvr_start_rx(struct xm_recvr *xmr, void *buf);


#endif /* _BOOT_XMODEM_H_ */
