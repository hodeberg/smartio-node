/*
 * UartTxBuffer.cpp
 *
 *  Created on: 27 apr 2016
 *      Author: hodeberg
 */

#include <Uart/UartTxBuffer.h>
#include "TransactionBuffer.h"

UartTxBuffer::UartTxBuffer(): escapedChar(0), mBuf(nullptr) {}

int UartTxBuffer::get() {
	if (escapedChar != 0) {
		const int c = escapedChar;
		escapedChar = 0;
		return c;
	}
	else {
		const int c = mBuf->getBuffer().get();

		if ((c == ESC) || (c == STX) || (c == ETX)) {
			escapedChar = c + 0x80;
			return ESC;
		}
		return c;
	}
}

bool UartTxBuffer::isBusy() {
	return mBuf != nullptr;
}

void UartTxBuffer::transmissionIsComplete() {
	if (mBuf->bytes_stored() == 0) {
		TxMsgBase msg{mBuf, true};

		mBuf = nullptr;
		escapedChar = 0;
		TransportLayer::sendMsgFromIrq(msg);
	}
	else {
		// False interrupt. Probably the interrupt handler was too slow in getting the next byte,
		// so the tx buffer was not filled before the previous byte was shifted out.
	}
}

void UartTxBuffer::postMsg(TransactionBuffer& buf) {
	const int CRC_MSB = 0x11;
	const int CRC_LSB = 0x22;

	mBuf = &buf;
	mBuf->getBuffer().put(CRC_MSB);
	mBuf->getBuffer().put(CRC_LSB);
	mBuf->getBuffer().setSize();
	startTx(STX);
}
