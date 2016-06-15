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
 *  MSP432 I2C - EUSCI_B0_MODULE I2C Master TX  bytes to MSP432 Slave - Repeated Start
 *
 *  Description: This demo connects two MSP432 's via the I2C bus. The master
 *  transmits to the slave. This is the SLAVE CODE. It continuously
 *  transmits an array of data and demonstrates how to implement an I2C
 *  master transmitter sending multiple bytes followed by a repeated start,
 *  followed by a read of multiple bytes.  This is a common operation for
 *  reading register values from I2C slave devices such as sensors. The
 *  transaction for the I2C that is written looks as follows:
 *
 *  ________________________________________________________________
 *  |  Start   |      |      |  Start   |                   |       |
 *  | 0x48Addr | 0x04 | 0x00 | 0x48Addr |  <10 Byte Read>   | Stop  |
 *  |    W     |      |      |    R     |                   |       |
 *  |__________|______|______|__________|___________________|_______|
 *
 *  ACLK = n/a, MCLK = HSMCLK = SMCLK = BRCLK = default DCO = ~3.0MHz
 *
 *                                /|\  /|\
 *                MSP432P401      10k  10k      MSP432P401
 *                   slave         |    |         master
 *             -----------------   |    |   -----------------
 *            |     P1.6/UCB0SDA|<-|----+->|P1.6/UCB0SDA     |
 *            |                 |  |       |                 |
 *            |                 |  |       |                 |
 *            |     P1.7/UCB0SCL|<-+------>|P1.7/UCB0SCL     |
 *            |                 |          |                 |
 *
 * Author: Timothy Logan
 *****************************************************************************/
/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "I2CBuffer.h"
#include "FreeRTOSConfig.h"

/* Application Defines */
#define SLAVE_ADDRESS       0x48
#define NUM_OF_RX_BYTES        40
#define NUM_OF_TX_BYTES       10
#define EVDEBUG

extern "C" {
void euscib1_isr(void);
void PORT1_IRQHandler(void);
};

/* Application Variables */
volatile uint8_t RXData[NUM_OF_RX_BYTES];
volatile uint32_t xferIndex;
const uint32_t TXData[NUM_OF_TX_BYTES + 1] =
{ 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0x10 };



void I2C_AssertAlert()
{
#if 0
	I2C_initSlave(EUSCI_B0_BASE, 0xC, EUSCI_B_I2C_OWN_ADDRESS_OFFSET1,
	            EUSCI_B_I2C_OWN_ADDRESS_ENABLE);
#endif
	MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN0);
}

void I2C_AlertDone()
{
#if 0
	I2C_initSlave(EUSCI_B0_BASE, 0xC, EUSCI_B_I2C_OWN_ADDRESS_OFFSET1,
	            EUSCI_B_I2C_OWN_ADDRESS_DISABLE);
#endif
	MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN0);

}

void initI2CModule(void)
{
	unsigned int flags = EUSCI_B_I2C_TRANSMIT_INTERRUPT0 |
						 EUSCI_B_I2C_RECEIVE_INTERRUPT0 |
						 EUSCI_B_I2C_TRANSMIT_INTERRUPT1 |
						 EUSCI_B_I2C_RECEIVE_INTERRUPT1 |
						 EUSCI_B_I2C_STOP_INTERRUPT |
						 EUSCI_B_I2C_NAK_INTERRUPT;
#if 0
    /* Select Port B1 for I2C - Set Pin 4, 5 to input Primary Module Function,
     *   (UCB1SIMO/UCB1SDA, UCB1SOMI/UCB1SCL).
     */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
            GPIO_PIN4 + GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);


    /* eUSCI I2C Slave Configuration */
    I2C_initSlave(EUSCI_B1_BASE, SLAVE_ADDRESS, EUSCI_B_I2C_OWN_ADDRESS_OFFSET0,
            EUSCI_B_I2C_OWN_ADDRESS_ENABLE);

    I2C_initSlave(EUSCI_B1_BASE, 0xC, EUSCI_B_I2C_OWN_ADDRESS_OFFSET1,
    	            EUSCI_B_I2C_OWN_ADDRESS_ENABLE);

    I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_RECEIVE_MODE);
    /* Enable the module and enable interrupts */
    I2C_enableModule(EUSCI_B1_BASE);
    MAP_I2C_clearInterruptFlag(EUSCI_B1_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0);
    MAP_I2C_enableInterrupt(EUSCI_B1_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0);
    MAP_Interrupt_enableInterrupt(INT_EUSCIB1);
#else
    /* eUSCI I2C Slave Configuration */
    I2C_initSlave(EUSCI_B0_BASE, SLAVE_ADDRESS, EUSCI_B_I2C_OWN_ADDRESS_OFFSET0,
            EUSCI_B_I2C_OWN_ADDRESS_ENABLE);
#if 1
    I2C_initSlave(EUSCI_B0_BASE, 0xC, EUSCI_B_I2C_OWN_ADDRESS_OFFSET1,
	            EUSCI_B_I2C_OWN_ADDRESS_ENABLE);
#endif

    /* Select Port B2 for I2C - Set Pin 6, 7 to input Primary Module Function
     */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
            GPIO_PIN6 | GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Define the SMbus alert pin */
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN0);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN0);


    I2C_setMode(EUSCI_B0_BASE, EUSCI_B_I2C_RECEIVE_MODE);
    /* Enable the module and enable interrupts */
    I2C_enableModule(EUSCI_B0_BASE);
    MAP_I2C_clearInterruptFlag(EUSCI_B0_BASE, flags);
    MAP_Interrupt_setPriority(INT_EUSCIB0,  configMAX_SYSCALL_INTERRUPT_PRIORITY);
    MAP_I2C_enableInterrupt(EUSCI_B0_BASE, flags);
    MAP_Interrupt_enableInterrupt(INT_EUSCIB0);
#endif
}


#define POLY    (0x1070U << 3)
static uint8_t crc8(uint16_t data)
{
	int i;

	for (i = 0; i < 8; i++) {
		if (data & 0x8000)
			data ^= POLY;
		data <<= 1;
	}
	return (uint8_t)(data >> 8);
}

/* Incremental CRC8 over count bytes in the array pointed to by p */
static uint8_t i2c_smbus_pec(uint8_t crc, uint8_t *p, size_t count)
{
	int i;

	for (i = 0; i < count; i++)
		crc = crc8((crc ^ p[i]) << 8);
	return crc;
}

#ifdef EVDEBUG
enum Event {
		NONE,
		RECEIVE,
		TRANSMIT,
		STOP,
		NAK,
		TRANSMIT_SMBUS_ADDR,
		UNKNOWN
};

enum Event eventArr[50];
int eventIx;
#endif


/* GPIO ISR */
void PORT1_IRQHandler(void)
{
    uint32_t status;

    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    /* Toggling the output on the LED */
    if(status & GPIO_PIN4)
    {
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN1);
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P5, GPIO_PIN0);
    }

}


/*******************************************************************************
 * eUSCIB1 ISR. The repeated start and transmit/receive operations happen
 * within this ISR.
 ******************************************************************************/
void euscib1_isr(void)
{
    uint_fast16_t status;
    status = MAP_I2C_getEnabledInterruptStatus(EUSCI_B0_BASE);
    MAP_I2C_clearInterruptFlag(EUSCI_B0_BASE, status);
    static bool inAlert = false;


    if (status & EUSCI_B_I2C_STOP_INTERRUPT) {
#ifdef EVDEBUG
    	eventArr[eventIx++] = STOP;
#endif
    	if (inAlert) {
    		inAlert = false;
    		I2C_AlertDone();
    	}
    	else
    		i2c_l2_buf.stop();
    	return;
    }
    if (status & EUSCI_B_I2C_NAK_INTERRUPT) {
#ifdef EVDEBUG
        eventArr[eventIx++] = NAK;
#endif
        i2c_l2_buf.nak();
        return;
    }


    /* RXIFG */
    if (status & EUSCI_B_I2C_RECEIVE_INTERRUPT0)
    {
#ifdef EVDEBUG
    	eventArr[eventIx++] = RECEIVE;
#endif
    	i2c_l2_buf.put(MAP_I2C_slaveGetData(EUSCI_B0_BASE));
    }

    /* TXIFG */
    if (status & EUSCI_B_I2C_TRANSMIT_INTERRUPT0)
    {
#ifdef EVDEBUG
    	eventArr[eventIx++] = TRANSMIT;
#endif
        MAP_I2C_slavePutData(EUSCI_B0_BASE, i2c_l2_buf.get());
    }

    if (status & EUSCI_B_I2C_TRANSMIT_INTERRUPT1)
    {
    	// SMbus alert response. Put my address as a reply
    #ifdef EVDEBUG
       	eventArr[eventIx++] = TRANSMIT_SMBUS_ADDR;
    #endif
       	inAlert = true;
    	MAP_I2C_slavePutData(EUSCI_B0_BASE, SLAVE_ADDRESS << 1);
    }
#ifdef EVDEBUG
    if (eventIx == 50) eventIx = 0;
#endif
}
