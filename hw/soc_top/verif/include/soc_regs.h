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
 * UltiSoC registers
 */

#ifndef _VERIF_SOC_REGS_H_
#define _VERIF_SOC_REGS_H_


/* UART */
#define USOC_UART_IOBASE	0x80000000			/* UART I/O base */
#define USOC_UART_CTRL		(USOC_UART_IOBASE + 0x00)	/* Control register */
#define USOC_UART_DIVD		(USOC_UART_IOBASE + 0x04)	/* Divider register */
#define USOC_UART_DATA		(USOC_UART_IOBASE + 0x08)	/* Data register */
#define USOC_UART_FIFO		(USOC_UART_IOBASE + 0x0C)	/* FIFO state register */
/***/
#define USOC_UART_CTRL_TX_IM		(1<<0)			/* TX interrupt mask */
#define USOC_UART_CTRL_TX_FF		(1<<1)			/* TX FIFO full */
#define USOC_UART_CTRL_TX_FE		(1<<2)			/* TX FIFO empty */
#define USOC_UART_CTRL_RX_IM		(1<<3)			/* RX interrupt mask */
#define USOC_UART_CTRL_RX_FF		(1<<4)			/* RX FIFO full */
#define USOC_UART_CTRL_RX_FE		(1<<5)			/* RX FIFO empty */
#define USOC_UART_FIFO_TX_COUNT(a)	((a) & 0xFFFF)		/* TX FIFO bytes count */
#define USOC_UART_FIFO_RX_COUNT(a)	(((a) >> 16) & 0xFFFF)	/* RX FIFO bytes count */


/* Control device */
#define USOC_CTRL_IOBASE	0x80100000			/* Control device I/O base */
#define USOC_CTRL_SOCVER	(USOC_CTRL_IOBASE + 0x000)	/* SoC version (R/O) */
#define USOC_CTRL_RAMBASE	(USOC_CTRL_IOBASE + 0x004)	/* RAM base address (R/O) */
#define USOC_CTRL_RAMSIZE	(USOC_CTRL_IOBASE + 0x008)	/* RAM size (R/O) */
#define USOC_CTRL_ROMSIZE	(USOC_CTRL_IOBASE + 0x00C)	/* ROM size (R/O) */
#define USOC_CTRL_SYSFREQ	(USOC_CTRL_IOBASE + 0x010)	/* System frequency (R/O) */
#define USOC_CTRL_LED		(USOC_CTRL_IOBASE + 0x100)	/* LEDs control register (R/W) */


/* Interrupt controller */
#define USOC_INTCTL_IOBASE	0x80200000			/* Interrupt controller I/O base */
#define USOC_INTCTL_STATUS	(USOC_INTCTL_IOBASE + 0x00)	/* Status register */
#define USOC_INTCTL_MASK	(USOC_INTCTL_IOBASE + 0x04)	/* Mask register */
#define USOC_INTCTL_RAW		(USOC_INTCTL_IOBASE + 0x08)	/* Raw interrupts */
/***/
#define USOC_INTCTL_TMRINT	(1<<0)				/* Timer interrupt bit */
#define USOC_INTCTL_UARTINT	(1<<1)				/* UART interrupt bit */


/* Interval timer */
#define USOC_ITIMER_IOBASE	0x80300000			/* Interval timer I/O base */
#define USOC_ITIMER_CTRL	(USOC_ITIMER_IOBASE + 0x00)	/* Control register */
#define USOC_ITIMER_COUNT	(USOC_ITIMER_IOBASE + 0x04)	/* Counter register */
#define USOC_ITIMER_CURRENT	(USOC_ITIMER_IOBASE + 0x08)	/* Current count value */
/***/
#define USOC_ITIMER_CTRL_EN	(1<<0)				/* Enable time */
#define USOC_ITIMER_CTRL_IM	(1<<1)				/* Interrupt mask */
#define USOC_ITIMER_CTRL_RL	(1<<2)				/* Reload counter */


#endif /* _VERIF_SOC_REGS_H_ */
