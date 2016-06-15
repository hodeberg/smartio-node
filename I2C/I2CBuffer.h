/*
 * I2CBuffer.h
 *
 *  Created on: 25 maj 2016
 *      Author: hodeberg
 */

#ifndef I2C_I2CBUFFER_H_
#define I2C_I2CBUFFER_H_

#include "LinearBuffer.h"

class I2C_Buffer {
public:
	I2C_Buffer();
	// Interface from interrupt handler
	void put(unsigned int c);
	int get();
	void stop();
	void nak();
	// Interface from transaction layer
	bool postMsg(TransactionBuffer& buf); // Called by transaction layer to start a new message transmission
	void rxMsgIsHandled(LinearBuffer &buf); // We are now ready to receive more data
private:
	void initTx(TransactionBuffer *const txbuf);
	void handleReceivedMsg(); // Master has sent a message, forward it upwards
	void notifyTransaction();      // We have sent our message, inform transaction layer
	enum class State { IDLE, RCV, RCV_SKIP, TX };
	static const int bufsize = 32;
	uint8_t buf1[bufsize];
	uint8_t buf2[bufsize];
	LinearBuffer rBuf; // Receive from host
	LinearBuffer xBuf; // Write to host
	bool inbuf_locked_by_transaction_layer;
	State curState;
	TransactionBuffer *txBuf;
};


extern I2C_Buffer i2c_l2_buf;
#endif /* I2C_I2CBUFFER_H_ */
