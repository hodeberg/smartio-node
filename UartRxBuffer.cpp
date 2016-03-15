/*
 * UartRxBuffer.cpp
 *
 *  Created on: 10 mar 2016
 *      Author: hodeberg
 */

#include "UartRxBuffer.h"
#include "TransportMessages.h"

#if 1
UartRxBuffer::UartRxBuffer() : lBuf{{buf1, bufsize}, {buf2, bufsize}}, state(RxState::IDLE), curWriteBuf(&lBuf[0]),
                               receivedBuffers(0), droppedBuffers(0) {}
#else
UartRxBuffer::UartRxBuffer() : lBuf1{buf1, bufsize}, lBuf2{buf2, bufsize}, state(RxState::IDLE), curWriteBuf(&lBuf1) {}
#endif

void UartRxBuffer::put(unsigned int c)
{
	switch (state) {
	case RxState::IDLE:
	case RxState::HUNTING:
	case RxState::DONE:
		if (c == STX) {
			state = RxState::RCV;
			// We don't write the STX into the buffer.
		}
		break;
	case RxState::RCV:
		if (c == ESC)
			state = RxState::ESCAPING;
		else if (c == ETX) {
			RxMsgBase msg{static_cast<LinearBuffer*>(curWriteBuf)};
			state = RxState::DONE;
			if (TransportLayer::sendMsgFromIrq(msg)) {
				flipBuffer();
				receivedBuffers++;
			}
			else {
				droppedBuffers++;
				curWriteBuf->reset();
			}
		}
		else {
			if (!curWriteBuf->put(c)) {
				// Full buffer
				droppedBuffers++;
				curWriteBuf->reset();
				state = RxState::HUNTING;
			}
		}
		break;
	case RxState::ESCAPING:
		{
			if ((c == ESC) || (c == STX)  || (c == ETX)) {
				state = RxState::RCV;
				if (!curWriteBuf->put(c)) {
					curWriteBuf->reset();
					droppedBuffers++;
					state = RxState::HUNTING;
				}
			}
			else {
				// Illegally escaped char.
				curWriteBuf->reset();
				droppedBuffers++;
				state = RxState::HUNTING;
			}
		}
		break;
	}
}


void UartRxBuffer::flipBuffer()
{
	if (curWriteBuf == &lBuf[0])
		curWriteBuf = &lBuf[1];
	else
		curWriteBuf = &lBuf[0];
}
