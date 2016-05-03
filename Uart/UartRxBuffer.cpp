/*
 * UartRxBuffer.cpp
 *
 *  Created on: 10 mar 2016
 *      Author: hodeberg
 */

#include "UartRxBuffer.h"
#include "TransportMessages.h"

namespace L2_IF {
bool verifyIntegrity(LinearBuffer &receivedMsg)
{
	// For now, message is always OK
	return true;
}
bool sendL2Reply(LinearBuffer& replyMsg, LinearBuffer& inMsg)
{
	replyMsg.put(UartRxBuffer::STX);
	// TBD
	return true;
}

}

UartRxBuffer::UartRxBuffer() : lBuf{{buf1, bufsize}, {buf2, bufsize}}, state(RxState::IDLE), curWriteBuf(&lBuf[0]),
                               receivedBuffers(0), droppedBuffers(0) {}


bool UartRxBuffer::l2ChecksOk()
{
	// TBD: check CRC
	return true;
}


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
			RxMsgBase msg{curWriteBuf};

			state = RxState::DONE;
			// If CRC is bad, we cannot trust anything in the packet and thus just discard it.
			if (l2ChecksOk()) {
				// Remove CRC
				curWriteBuf->unget();
				curWriteBuf->unget();
				if (TransportLayer::sendMsgFromIrq(msg)) {
					flipBuffer();
					receivedBuffers++;
					break;
				}
			}
			droppedBuffers++;
			curWriteBuf->reset();
			state = RxState::HUNTING;
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
			c -= 0x80;
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
	curWriteBuf->reset();
}
