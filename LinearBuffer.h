/*
 * LinearBuffer.h
 *
 *  Created on: 9 mar 2016
 *      Author: hodeberg
 */

#ifndef LINEARBUFFER_H_
#define LINEARBUFFER_H_

#include <cstdint>
#include "TransportMessages.h"
#include "smartio.h"

class Attribute;

class LinearBuffer  {
public:
	LinearBuffer(unsigned char *buf, unsigned int size);
	bool put(unsigned int c); // Returns false if full.
	void putWord16(unsigned int v);
	void putStr(const char* buf);
	void putAttr(const Attribute &attr); // Write attribute value to buffer
	int get(); // Returns -1 if empty
	int getWord16();
	void getAttr(Attribute &attr); // Read attribute value from buffer
	bool isEmpty() const { return (wrIx == 0) && (rdIx == 0); }
	bool isFull() const;
	unsigned int bytes_stored() const; // Number of chars available for reading
	unsigned int bytes_free() const; // Number of bytes left in buffer
	void reset() { wrIx = rdIx = 0; }
	void setSize();
	unsigned char& operator[](unsigned int ix); // Potentially modifies class
	unsigned int operator[](unsigned int ix) const; // Never modifies class
	void unget(); // remove last byte
	MsgHeader getHeader() const;
	LinearBuffer& operator=(const LinearBuffer& that);
	void prepareForResponse();
	void appErrorResp(SmartioErr err);
private:
	static const int STX = 2;
	static const int ETX = 3;
	static const int ESC = 27;
	unsigned char *const mBuf;
	const uint8_t mSize;
	uint8_t wrIx, rdIx;
};

#endif /* LINEARBUFFER_H_ */
