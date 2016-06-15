/*
 * TransactionBuffer.h
 *
 *  Created on: 11 apr 2016
 *      Author: hodeberg
 */

#ifndef TRANSACTIONS_TRANSACTIONBUFFER_H_
#define TRANSACTIONS_TRANSACTIONBUFFER_H_

#include "LinearBuffer.h"


class TransactionBuffer {
public:
	TransactionBuffer(uint8_t *b, unsigned int size);
	bool isEmpty() const { return buf.isEmpty(); }
	void reset() { buf.reset(); }
	class MsgHeader getHeader() const { return buf.getHeader(); }
	void initFromRxMsg(LinearBuffer &rxMsg);
	void handleMsg();
	void post();
	void setReadyToTransmit() { timeout = 0; }
	LinearBuffer& getBuffer() { return buf; }
	unsigned int bytes_stored() const { return buf.bytes_stored(); }
private:
	static const int TX_TIMEOUT = 100; // 100 ms timeout
	LinearBuffer buf;
	int timeout; // 0 => ready to send, -1 = do not send, >0 = waiting for resend.
};

#endif /* TRANSACTIONS_TRANSACTIONBUFFER_H_ */
