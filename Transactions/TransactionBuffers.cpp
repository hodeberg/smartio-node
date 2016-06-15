/*
 * TransactionBuffers.cpp
 *
 *  Created on: 16 mar 2016
 *      Author: hodeberg
 */

#include <TransactionBuffers.h>

TransactionBuffers::TransactionBuffers() : lbufs{{bufs[0], bufsize}, {bufs[1], bufsize}, {bufs[2], bufsize}, {bufs[3], bufsize}}, xmitInProgress(false) {
}


TransactionBuffer* TransactionBuffers::claimBuffer() {
	for (int i=0; i < noOfTransactionBuffers; i++)
		if (lbufs[i].isEmpty()) {
			lbufs[i].reset();
			return &lbufs[i];
		}
	return nullptr;
}

TransactionBuffer *TransactionBuffers::claimBuffer(RxMsgBase &rxMsg)
{
	TransactionBuffer *buf{claimBuffer()};

	if (buf != nullptr) {
		buf->initFromRxMsg(*rxMsg.buf);
	}
	return buf;
}

TransactionBuffer* TransactionBuffers::claimReplyBuffer(TransactionBuffer& reqBuffer) {
	TransactionBuffer *reply = claimBuffer();

	if (reply == nullptr) return reply;

	return nullptr;
}

#if 1
void TransactionBuffers::postMsg(TransactionBuffer &msg)
{
	if (xmitInProgress) {
		// Cannot send now. Mark it for transmission, though.
		msg.setReadyToTransmit();
	}
	else {
		xmitInProgress = true;
		msg.post();
	}
}
#endif


void TransactionBuffers::handleMsg(RxMsgBase& rxmsg) {
	if (rxmsg.buf->bytes_stored() < 2) {
		// Not even room for msg header - something is wrong!
	}
	else {
		rxmsg.buf->get(); // skip the size
		rxmsg.buf->get(); // skip the header
		processRxBuffer(rxmsg);
	}
	rxmsg.completion.messageHandled(*rxmsg.buf);
}

// Media layer reports that a buffer has been sent and now can be reclaimed.
void TransactionBuffers::msgWasSent(TxMsgBase& msg)
{
	TransactionBuffer &buf = *msg.buf;
	MsgHeader h{buf.getHeader()};

	switch (h.getType()) {
	case MsgHeader::MsgType::REQUEST:
		// Not implemented
		break;
	case MsgHeader::MsgType::RESPONSE:
		// Return buffer to pool.
		buf.reset();
		xmitInProgress = false;
		tryToTransmit();
		break;
	case MsgHeader::MsgType::ACKNOWLEDGED:
		// Not implemented
		break;
	case MsgHeader::MsgType::UNACKNOWLEDGED:
		// Not implemented
		break;
	case MsgHeader::MsgType::ACK:
	case MsgHeader::MsgType::NAK:
	case MsgHeader::MsgType::EMPTY_MSG:
		break;
	}
}


bool TransactionBuffers::searchForExistingTransactions(RxMsgBase& msg)
{
	MsgHeader h{msg.buf->getHeader()};

	for (int i=0; i < noOfTransactionBuffers; i++) {
		if (!lbufs[i].isEmpty()) {
			MsgHeader stored_h{lbufs[i].getHeader()};

			if (h.getID() == stored_h.getID()) {
				switch (h.getType()) {
				case MsgHeader::MsgType::REQUEST:
					if (stored_h.getType() == MsgHeader::MsgType::REQUEST) {
						// Duplicate. Ignore
						return true;
					}
					else {
						// Same transaction ID, but not matching type. Ignore.
					}
					break;
				case MsgHeader::MsgType::RESPONSE:
					if (stored_h.getType() == MsgHeader::MsgType::REQUEST) {
						// We just got a response to a previously sent request. Great! Handle it.
						handleResponse(msg, lbufs[i]);
						return true;
					}
					break;
				case MsgHeader::MsgType::ACKNOWLEDGED:
					// Not implemented
					return true;
					break;
				case MsgHeader::MsgType::UNACKNOWLEDGED:
					// Not implemented
					return true;
					break;
				case MsgHeader::MsgType::ACK:
				case MsgHeader::MsgType::NAK:
					if (stored_h.getType() == MsgHeader::MsgType::REQUEST) {
						// We just got an ack/nak to a message (which should be resent).
						handleAckNak(msg, lbufs[i]);
						return true;
					}
					break;
				case MsgHeader::MsgType::EMPTY_MSG:
					break;
				}
			}
		}
	}
	return false;
}

void TransactionBuffers::processRxBuffer(RxMsgBase& msg) {
	if (!searchForExistingTransactions(msg)) {
		MsgHeader h{msg.buf->getHeader()};
		// There is no match for this received message in the buffer store.
		// For some types of messages it is time to allocate a new buffer.
		switch (h.getType()) {
		case MsgHeader::MsgType::REQUEST:
		{
			TransactionBuffer *newBuf = claimBuffer(msg);

			if (newBuf == nullptr) return;
			newBuf->handleMsg();
			// There should now be a formatted response message in the buffer.
			postMsg(*newBuf);
			break;
		}
		case MsgHeader::MsgType::ACKNOWLEDGED:
		case MsgHeader::MsgType::UNACKNOWLEDGED:
			// Not implemented
			break;
		default:
			// Do nothing
			break;
		}
	}
}

void TransactionBuffers::handleResponse(RxMsgBase& rxmsg, TransactionBuffer& request) {
	// TBD
}

void TransactionBuffers::handleAckNak(RxMsgBase& rxmsg,	TransactionBuffer& request) {
	// TBD
}


