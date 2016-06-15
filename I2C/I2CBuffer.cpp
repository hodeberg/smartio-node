/*
 * I2CBuffer.cpp
 *
 *  Created on: 25 maj 2016
 *      Author: hodeberg
 */

#include <I2C/I2CBuffer.h>
#include "TransactionBuffer.h"
#include "TransportMessages.h"
#include "L2_Interface.h"

extern void I2C_AssertAlert();


I2C_Buffer i2c_l2_buf;

class RxCompletion: public MsgCompletion {
public:
	virtual void messageHandled(LinearBuffer& buf);
};

void RxCompletion ::messageHandled(LinearBuffer& buf)
{
	i2c_l2_buf.rxMsgIsHandled(buf);
}

RxCompletion rxCompletion;


class I2C_Sender : public L2_IF::MsgSender {
	virtual void postMessage(TransactionBuffer& outMsg);
};

void I2C_Sender::postMessage(TransactionBuffer& outMsg)
{
	outMsg.getBuffer().setSizeUnescaped();
	i2c_l2_buf.postMsg(outMsg);
}

I2C_Sender i2cSender;

I2C_Buffer::I2C_Buffer(): rBuf{buf1, bufsize}, xBuf{buf2, bufsize},
		 	 	 	 	  inbuf_locked_by_transaction_layer(false),
						  curState(State::IDLE), txBuf(nullptr) {}

void I2C_Buffer::put(unsigned int c) {
	switch (curState) {
	case State::IDLE:
		if (inbuf_locked_by_transaction_layer) {
			curState = State::RCV_SKIP;
		}
		else {
			curState = State::RCV;
			rBuf.reset();
			rBuf.put(c);
		}
		break;
	case State::RCV:
		if (!rBuf.put(c)) {
			// No room in buffer
			curState = State::RCV_SKIP;
		}
		break;
	case State::RCV_SKIP:
		// Do nothing
		break;
	case State::TX:
		// This is not expected. Darn!
		notifyTransaction();
		curState = State::IDLE;
		break;
	}
}



int I2C_Buffer::get() {
	switch (curState) {
	case State::RCV:
		// Write/Read switchover in a process call
		handleReceivedMsg();
		// Intentional pass-through.
	case State::IDLE:
	case State::RCV_SKIP:
		curState = State::TX;
		initTx(txBuf);
		break;
	case State::TX:
		break;
	}
	return xBuf.get();
}

void I2C_Buffer::stop() {
	switch (curState) {
	case State::RCV_SKIP:
		curState = State::IDLE;
	case State::IDLE:
		// Do nothing
		break;
	case State::RCV:
		handleReceivedMsg();
		curState = State::IDLE;
		break;
	case State::TX:
		notifyTransaction();
		curState = State::IDLE;
		break;
	}
}

void I2C_Buffer::nak() {
	switch (curState) {
	case State::IDLE:
		break;
	case State::RCV:
	case State::RCV_SKIP:
	case State::TX:
		curState = State::IDLE;
		break;
	}
}


bool I2C_Buffer::postMsg(TransactionBuffer& buf) {
	if (txBuf) return false; // No free space
	txBuf = &buf;
	I2C_AssertAlert();
	return true;
}

void I2C_Buffer::initTx(TransactionBuffer* const buf) {
	if (!buf) {
		// Fill in an empty null message
		MsgHeader h(MsgHeader::MsgType::EMPTY_MSG, 0);

		xBuf.reset();
		xBuf.put(2);
		xBuf.put(h.mkHeader());
	}
	else {
		xBuf = buf->getBuffer();
	}
}

void I2C_Buffer::handleReceivedMsg() {
	RxMsgBase msg{&rBuf, rxCompletion};

	inbuf_locked_by_transaction_layer = true;
	L2_IF::registerMsgSender(i2cSender);
	if (!TransportLayer::sendMsgFromIrq(msg)) {
		inbuf_locked_by_transaction_layer = false;
	}
}

void I2C_Buffer::rxMsgIsHandled(LinearBuffer& buf) {
	if (&buf == &rBuf) {
		// Yes, transaction layer is acking our buffer
		inbuf_locked_by_transaction_layer = false;
	}
}

void I2C_Buffer::notifyTransaction() {
	TxMsgBase msg{txBuf, true};

	txBuf = nullptr;
	TransportLayer::sendMsgFromIrq(msg);
}
