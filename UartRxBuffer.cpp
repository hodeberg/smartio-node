/*
 * UartRxBuffer.cpp
 *
 *  Created on: 10 mar 2016
 *      Author: hodeberg
 */

#include "UartRxBuffer.h"

#if 0
UartRxBuffer::UartRxBuffer() : lBuf{{buf1, bufsize}, {buf2, bufsize}}, state(RxState::IDLE), curWriteBuf(&lBuf[0]) {}
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
			curWriteBuf->put(c);
		}
		break;
	case RxState::RCV:
		if (c == ESC)
			state = RxState::ESCAPING;
		else {
			if (curWriteBuf->put(c)) {
				if (c == ETX) {
					state = RxState::DONE;
					// TBD: switch to other buffer, notify upper layer.
				}
			}
			else {
				// Full buffer
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
					state = RxState::HUNTING;
				}
			}
			else {
				// Illegally escaped char.
				curWriteBuf->reset();
				state = RxState::HUNTING;
			}
		}
		break;
	}
}



