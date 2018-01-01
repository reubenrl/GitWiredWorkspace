/*! \file vt100.c \brief VT100 terminal function library. */
//*****************************************************************************
//
// File Name	: 'vt100.c'
// Title		: VT100 terminal function library
// Author		: Pascal Stang - Copyright (C) 2002
// Created		: 2002.08.27
// Revised		: 2002.08.27
// Version		: 0.1
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>			// include standard C string functions
#include "cmdlineconf.h"		// include cmdline function library

#include "global.h"
#include "vt100.h"

// function pointer to single character output routine
static void (*vt100OutputFunc)(unsigned char nUart, char *p);

// Program ROM constants

// Global variables

// Functions
void vt100Init(void(*output_func)(unsigned char nUart, char *p))
{
	char tmpBuf[16];

	vt100OutputFunc = output_func;
	// ESC c
	sprintf_P(tmpBuf,PSTR("\x1B\x63"));
	vt100OutputFunc(UART_PRINT, tmpBuf);
}

void vt100ClearScreen(void)
{
	char tmpBuf[16];
	// ESC [ 2 J
	sprintf_P(tmpBuf,PSTR("\x1B[2J"));
	vt100OutputFunc(UART_PRINT, tmpBuf);
}

void vt100SetAttr(u08 attr)
{
	char tmpBuf[16];

	sprintf_P(tmpBuf, PSTR("\x1B[%dm"), attr);
	// ESC [ Ps m
	vt100OutputFunc(UART_PRINT, tmpBuf);
}

void vt100SetCursorMode(u08 visible)
{
	char tmpBuf[16];

	if(visible)
		// ESC [ ? 25 h
		sprintf_P(tmpBuf,PSTR("\x1B[?25h"));
	else
		// ESC [ ? 25 l
		sprintf_P(tmpBuf,PSTR("\x1B[?25l"));

	vt100OutputFunc(UART_PRINT, tmpBuf);
}

void vt100SetCursorPos(u08 line, u08 col)
{
	char tmpBuf[16];

	sprintf_P(tmpBuf,PSTR("\x1B[%d;%dH") ,line, col);
	// ESC [ Pl ; Pc H
	vt100OutputFunc(UART_PRINT, tmpBuf);
}

