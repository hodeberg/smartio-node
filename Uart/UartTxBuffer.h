/*
 * UartTxBuffer.h
 *
 *  Created on: 27 apr 2016
 *      Author: hodeberg
 */

#ifndef UART_UARTTXBUFFER_H_
#define UART_UARTTXBUFFER_H_

#include <cstdint>

class TransactionBuffer;
void startTx(int c);

class UartTxBuffer {
public:
	UartTxBuffer();
	int get(); // Called by interrupt to send next char
	bool isBusy(); // Called by transaction layer to check if it can post
	void transmissionIsComplete(); // Called by interrupt when it thinks transmission is done.
	void postMsg(TransactionBuffer& buf); // Called by transaction layer to start a new message transmission
	static const int STX = 2;
	static const int ETX = 3;
	static const int ESC = 27;
private:
	uint8_t escapedChar;
	TransactionBuffer *mBuf;
};

#endif /* UART_UARTTXBUFFER_H_ */
