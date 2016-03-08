/*
 * RingBuffer.cpp
 *
 *  Created on: 8 mar 2016
 *      Author: hodeberg
 */

#include <cstdint>
#include "RingBuffer.h"



RingBuffer::RingBuffer(uint8_t *buf, const int bufsize) : mBuf(buf), mSize(bufsize), wr_ix(0), rd_ix(0) {};

bool RingBuffer::put(uint8_t c)
{
	if (isFull()) return false;
	mBuf[wr_ix] = c;
	wr_ix = (wr_ix + 1) % mSize;
	return true;
}

int RingBuffer::get()
{
	int c = -1;

	if (!isEmpty())  {
		c = mBuf[rd_ix];
		rd_ix = (rd_ix + 1) % mSize;
	}
	return c;
}

bool RingBuffer::isEmpty() const
{
	return wr_ix == rd_ix;
}

bool RingBuffer::isFull() const
{
	const int next_write = (wr_ix + 1 ) % mSize;

	return next_write == rd_ix;
}

unsigned int RingBuffer::bytes_stored() const
{
	return (wr_ix - rd_ix) & (mSize - 1);
}

unsigned int RingBuffer::bytes_free() const
{
	return mSize - 1 - bytes_stored();
}

