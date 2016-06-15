/*
 * TransactionBuffers.h
 *
 *  Created on: 16 mar 2016
 *      Author: hodeberg
 */

#ifndef TRANSACTIONS_TRANSACTIONBUFFERS_H_
#define TRANSACTIONS_TRANSACTIONBUFFERS_H_

#include <cstdint>
#include "TransactionBuffer.h"
class RxMsgBase;
class TxMsgBase;



class TransactionBuffers {
public:
	TransactionBuffers();
	TransactionBuffer *claimBuffer();
	TransactionBuffer *claimReplyBuffer(TransactionBuffer &reqBuffer);
	void handleMsg(RxMsgBase& msg);
	void msgWasSent(TxMsgBase& msg);
private:
	bool searchForExistingTransactions(RxMsgBase& msg);
	void processRxBuffer(RxMsgBase &msg);
	TransactionBuffer *claimBuffer(RxMsgBase &rxMsg);
	void handleResponse(RxMsgBase &rxmsg, TransactionBuffer &request);
	void processMsg(TransactionBuffer &buf);
	void handleAckNak(RxMsgBase &rxmsg, TransactionBuffer &request);
	void postMsg(TransactionBuffer &msg);
	void tryToTransmit() { ;/* Not implemented */ }
	static const uint8_t noOfTransactionBuffers = 4;
	static const uint8_t bufsize = 32;
	uint8_t bufs[noOfTransactionBuffers][bufsize];
	TransactionBuffer lbufs[noOfTransactionBuffers];
	bool xmitInProgress;
};

#endif /* TRANSACTIONS_TRANSACTIONBUFFERS_H_ */
