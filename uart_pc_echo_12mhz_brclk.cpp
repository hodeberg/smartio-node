/*
 * -------------------------------------------
 *    MSP432 DriverLib - v2_20_00_08 
 * -------------------------------------------
 *
 * --COPYRIGHT--,BSD,BSD
 * Copyright (c) 2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/******************************************************************************
 * MSP432 UART - PC Echo with 8MHz BRCLK
 *
 * Description: This demo echoes back characters received via a PC serial port.
 * SMCLK/DCO is used as a clock source and the device is put in LPM0
 * The auto-clock enable feature is used by the eUSCI and SMCLK is turned off
 * when the UART is idle and turned on when a receive edge is detected.
 * Note that level shifter hardware is needed to shift between RS232 and MSP
 * voltage levels.
 *
 *               MSP432P401
 *             -----------------
 *            |                 |
 *            |                 |
 *            |                 |
 *       RST -|     P1.3/UCA0TXD|----> PC (echo)
 *            |                 |
 *            |                 |
 *            |     P1.2/UCA0RXD|<---- PC
 *            |                 |
 *
 * Author: Timothy Logan
 *
 *
 * The serial protocol is:
 * <STX><SIZE><SEQ_ID><TYPE><DATA><CRC><ETX>
 * Any X in [<STX>, <ESC>, <ETX>] between <STX>/<ETX> is escaped by <ESC><X>
 * Size is one byte.
 * SEQ_ID is a message identifier. ACK/NAK messages echo this.
 * TYPE is ACK, NAK or MSG
 * DATA is the payload.
 *
 * When an entire STX/ETX stream has been received, it is briefly analyzed.
 * If the CRC does not match:
 *   If ACK/NAK: just discard
 *   If MSG: send NAK
 * If upper layers have no room for message: Send NAK
 * Otherwise, send ACK.
 * CRC is a CRC-16, MSB first. x^15 + x^12 + x^5 + 1
*******************************************************************************/
/* DriverLib Includes */
#include "driverlib.h"
#include "LinearBuffer.h"

/* Standard Includes */
#include <cstdint>
//#include <stdio.h>
#include <string.h>

//#include <stdbool.h>

/* UART Configuration Parameter. These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 9600 baud rate. These
 * values were calculated using the online calculator that TI provides
 * at:
 *http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */
const eUSCI_UART_Config uartConfig =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        78,                                     // BRDIV = 78
        2,                                       // UCxBRF = 2
        0,                                       // UCxBRS = 0
        EUSCI_A_UART_NO_PARITY,                  // No Parity
		EUSCI_A_UART_LSB_FIRST,                  // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
        EUSCI_A_UART_MODE,                       // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // Oversampling
};

extern "C" {
void initUART(void);
};

void initUART(void)
{
    /* Selecting P1.2 and P1.3 in UART mode */
    // I/O direction is N/A, handled by peripheral.
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3,
                GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Setting DCO to 12MHz */
#if 0
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);
#endif
    /* Configuring UART Module */
    MAP_UART_initModule(EUSCI_A2_BASE, &uartConfig);

    /* Enable UART module */
    MAP_UART_enableModule(EUSCI_A2_BASE);

    /* Enabling interrupts */
    MAP_UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT | EUSCI_A_UART_TRANSMIT_INTERRUPT);
    MAP_UART_clearInterruptFlag(EUSCI_A2_BASE, MAP_UART_getEnabledInterruptStatus(EUSCI_A2_BASE));
    MAP_Interrupt_enableInterrupt(INT_EUSCIA2);
}


#define UART_BUF_SIZE 34


uint8_t inbuf[UART_BUF_SIZE];
uint8_t in_wr_ix;
uint8_t outbuf[UART_BUF_SIZE];
uint8_t out_rd_ix;


void handleRxMsg()
{
	out_rd_ix = 0;
	in_wr_ix = 0;
	memcpy(outbuf, inbuf, sizeof outbuf);
	MAP_UART_transmitData(EUSCI_A2_BASE, outbuf[out_rd_ix++]);
}

void putRxData(uint8_t c)
{
	if (in_wr_ix < UART_BUF_SIZE)
		inbuf[in_wr_ix++] = c;
	if (c == '\r')
		handleRxMsg();
}

int getRxData(uint8_t * const c)
{
	if (outbuf[out_rd_ix-1] == '\r')
		return 0;
	*c = outbuf[out_rd_ix++];
	return 1;
}

extern "C" {
void euscia2_isr(void);
};

const int UART_RX_BUF_SIZE = 32;
uint8_t rxbuf1[UART_RX_BUF_SIZE];
LinearBuffer rxLinearBuf1(rxbuf1, UART_RX_BUF_SIZE);


/* EUSCI A2 UART ISR - Echoes data back to PC host */
void euscia2_isr(void)
{
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A2_BASE);

    MAP_UART_clearInterruptFlag(EUSCI_A2_BASE, status);

    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT)
    {
    	const uint8_t c = MAP_UART_receiveData(EUSCI_A2_BASE);

    	putRxData(c);
    }
    else if (status & EUSCI_A_UART_TRANSMIT_INTERRUPT) {
    	uint8_t c;
    	const int more = getRxData(&c);

    	if (more) {
    		MAP_UART_transmitData(EUSCI_A2_BASE, c);
    	}
    }
}

