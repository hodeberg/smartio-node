/*
 * LinearBuffer.cpp
 *
 *  Created on: 9 mar 2016
 *      Author: hodeberg
 */

#include <cstring>
#include "LinearBuffer.h"
#include "Attribute.h"

LinearBuffer::LinearBuffer(uint8_t *buf, unsigned int size) : mBuf(buf), mSize(size), wrIx(0), rdIx(0)
{
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

	if (rdIx < wrIx)
		c = mBuf[rdIx++];
	return c;
}

int LinearBuffer::getWord16()
{
	int v = -1;

	if (bytes_stored() >= 2) {
		v = get() << 8;
		v |= 	get();
	}
	return v;
}


void LinearBuffer::getAttr(Attribute &attr)
{
	attr.setAttrValue(mBuf + rdIx);
}

void LinearBuffer::putWord16(unsigned int v){
	put(v >> 8);
	put(v);
}

void LinearBuffer::putStr(const char* buf)
{
	const int left = bytes_free() - 1;

	std::strncpy(reinterpret_cast<char*>(mBuf) + wrIx, buf, left);
	if (static_cast<int>(std::strlen(buf)) > left) {
		wrIx += left;
		mBuf[wrIx - 1] = '\0';
	}
	else
		wrIx += std::strlen(buf);
}

void LinearBuffer::putAttr(const Attribute &attr)
{
	int len;

	attr.getAttrValue(mBuf + wrIx, len);
	wrIx += len;
}

void LinearBuffer::setSize()
{
	int sz = wrIx - rdIx;

	for (int i=1; i < wrIx; i++)
		if ((mBuf[i] == STX) || (mBuf[i] == ETX) || (mBuf[i] == ESC))
			sz++;
	mBuf[0] = sz;
}

void LinearBuffer::unget() // remove last byte
{
	if (wrIx > rdIx)
		wrIx--;
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

unsigned char& LinearBuffer::operator [](unsigned int ix)
{
	return mBuf[ix];
}

unsigned int LinearBuffer::operator [](unsigned int ix) const
{
	return mBuf[ix];
}

MsgHeader LinearBuffer::getHeader() const {
	MsgHeader header(mBuf[1]);

	return header;
}

LinearBuffer& LinearBuffer::operator=(const LinearBuffer& that)
{
	if (this != &that) {
		rdIx = that.rdIx;
		wrIx = that.wrIx;
		memcpy(mBuf, that.mBuf, wrIx);
	}
	return *this;
}

void LinearBuffer::prepareForResponse() {
	rdIx = 0;
	wrIx = 2;
	mBuf[0] = 0; // Clear size for now
	// Switch from request to response, but retain same transaction ID
	mBuf[1] &= ~(7 << 4);
	mBuf[1] |= static_cast<int>(MsgHeader::MsgType::RESPONSE) << 4;
}

void LinearBuffer::appErrorResp(SmartioErr err) {
	prepareForResponse();
	put(err);
}
