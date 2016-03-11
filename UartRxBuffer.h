/*
 * UartRxBuffer.h
 *
 *  Created on: 10 mar 2016
 *      Author: hodeberg
 */

#ifndef UARTRXBUFFER_H_
#define UARTRXBUFFER_H_

#include "LinearBuffer.h"

class UartRxBuffer  {
public:
	UartRxBuffer();
	void put(unsigned int c);
	enum class RxState { IDLE, HUNTING, RCV, ESCAPING, DONE };
private:
	static const int bufsize = 32;
	static const int STX = 2;
	static const int ETX = 3;
	static const int ESC = 27;
	uint8_t buf1[bufsize];
	uint8_t buf2[bufsize];
#if 0
	LinearBuffer lBuf[2];
#else
	LinearBuffer lBuf1;
	LinearBuffer lBuf2;
#endif
	RxState state;
	BufWriteIf *curWriteBuf;
};

#endif /* UARTRXBUFFER_H_ */
