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
 * ROM startup code
 */

#include <arch.h>
#include <soc_regs.h>


.section .startup, "ax"
.set noreorder
.globl __reset
.globl __bus_error_exception
.globl __overflow_exception
.globl __address_error_exception
.globl __resvd_instr_exception
.globl __break_exception
.globl __syscall_exception
.globl __hardware_interrupt
/********************************* Reset Vector *******************************/
__reset:
	j __entry		/* Jump to program entry point */
	mtc0 $zero, $SR		/* Disable interrupts */
/*********************************** Bus Error ********************************/
__bus_error_exception:
	j __exception_entry
	addiu $k0, $zero, 1
/******************************* Integer Overflow *****************************/
__overflow_exception:
	j __exception_entry
	addiu $k0, $zero, 2
/********************************* Address Error ******************************/
__address_error_exception:
	j __exception_entry
	addiu $k0, $zero, 3
/***************************** Reserved Instruction ***************************/
__resvd_instr_exception:
	j __exception_entry
	addiu $k0, $zero, 4
/********************************** Breakpoint ********************************/
__break_exception:
	j __exception_entry
	addiu $k0, $zero, 5
/********************************** System Call *******************************/
__syscall_exception:
	j __exception_entry
	addiu $k0, $zero, 6
/****************************** Hardware Interrupt ****************************/
__hardware_interrupt:
	j __exception_entry
	addiu $k0, $zero, 7

/********************************** ENTRY POINT *******************************/
__entry:
	/* Restore Interrupt Vectors Table pointer */
	mtc0 $zero, $IVTB

	/* Set stack using SoC configuration registers */
	li $t0, USOC_CTRL_RAMBASE
	lw $t0, ($t0)
	li $t1, USOC_CTRL_RAMSIZE
	lw $t1, ($t1)
	nop
	addu $sp, $t0, $t1	/* Set stack top */

	/* Pass control to boot entry */
	/* void boot_entry() */
	.extern boot_entry
	jal boot_entry
	nop

	/* Reboot if returned */
	j __reset
	nop


/************************ Exception handler entry *****************************/

.set noat
__exception_entry:
	/* Registers $k0 and $k1 are ours! */

	move $k1, $sp	/* Save stack pointer */
	/* Adjust stack to store interrupt frame */
	addiu $sp, $sp, -INTR_FRAME_LEN*CPU_REG_SIZE

	/* Save vector number */
	sw $k0, 0*CPU_REG_SIZE($sp)

	/* Save Previous Status register (PSR) */
	mfc0 $k0, $PSR
	sw $k0, 1*CPU_REG_SIZE($sp)

	/* Save Status register (SR) */
	mfc0 $k0, $SR
	sw $k0, 2*CPU_REG_SIZE($sp)

	/* Save Cause register */
	mfc0 $k0, $CAUSE
	sw $k0, 3*CPU_REG_SIZE($sp)

	/* Save return address */
	mfc0 $k0, $EPC
	sw $k0, 4*CPU_REG_SIZE($sp)

	/* Save HI/LO pair */
	mflo $k0
	sw $k0, 5*CPU_REG_SIZE($sp)
	mfhi $k0
	sw $k0, 6*CPU_REG_SIZE($sp)

	/* Save general purpose registers */
	sw $ra, 7*CPU_REG_SIZE($sp)
	sw $fp, 8*CPU_REG_SIZE($sp)
	sw $k1, 9*CPU_REG_SIZE($sp)	/* Save original $sp */
	sw $gp, 10*CPU_REG_SIZE($sp)
	sw $t9, 11*CPU_REG_SIZE($sp)
	sw $t8, 12*CPU_REG_SIZE($sp)
	sw $s7, 13*CPU_REG_SIZE($sp)
	sw $s6, 14*CPU_REG_SIZE($sp)
	sw $s5, 15*CPU_REG_SIZE($sp)
	sw $s4, 16*CPU_REG_SIZE($sp)
	sw $s3, 17*CPU_REG_SIZE($sp)
	sw $s2, 18*CPU_REG_SIZE($sp)
	sw $s1, 19*CPU_REG_SIZE($sp)
	sw $s0, 20*CPU_REG_SIZE($sp)
	sw $t7, 21*CPU_REG_SIZE($sp)
	sw $t6, 22*CPU_REG_SIZE($sp)
	sw $t5, 23*CPU_REG_SIZE($sp)
	sw $t4, 24*CPU_REG_SIZE($sp)
	sw $t3, 25*CPU_REG_SIZE($sp)
	sw $t2, 26*CPU_REG_SIZE($sp)
	sw $t1, 27*CPU_REG_SIZE($sp)
	sw $t0, 28*CPU_REG_SIZE($sp)
	sw $a3, 29*CPU_REG_SIZE($sp)
	sw $a2, 30*CPU_REG_SIZE($sp)
	sw $a1, 31*CPU_REG_SIZE($sp)
	sw $a0, 32*CPU_REG_SIZE($sp)
	sw $v1, 33*CPU_REG_SIZE($sp)
	sw $v0, 34*CPU_REG_SIZE($sp)
	sw $at, 35*CPU_REG_SIZE($sp)

	/* Pass control to bootROM exception handler */
	move $a0, $sp
	/* void interrupt_entry(struct interrupt_frame*) */
	.extern interrupt_entry
	jal interrupt_entry
	nop

	/* No plans for recover - jump to reset */
	j __reset
	rfe
