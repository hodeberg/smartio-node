/*
 * LinearBuffer.cpp
 *
 *  Created on: 9 mar 2016
 *      Author: hodeberg
 */

#include <LinearBuffer.h>

LinearBuffer::LinearBuffer(uint8_t *buf, unsigned int size) : mBuf(buf), mSize(size), wrIx(0), rdIx(0)
{
	int x = 3, y=5;
	int z;

	z = x + y;
	x = 7 - z;
}

bool LinearBuffer::put(unsigned int c)
{
	if (isFull()) return false;
	mBuf[wrIx++] = c;
	return true;
}

int LinearBuffer::get()
{
	int c = -1;

	if (!isEmpty())
		c = mBuf[rdIx++];
	return c;
}

bool LinearBuffer::isFull() const
{
	return wrIx == mSize;
}

unsigned int LinearBuffer::bytes_stored() const
{
	return (wrIx - rdIx);
}

unsigned int LinearBuffer::bytes_free() const
{
	return mSize - bytes_stored();
}

