/*
 * LinearBuffer.h
 *
 *  Created on: 9 mar 2016
 *      Author: hodeberg
 */

#ifndef LINEARBUFFER_H_
#define LINEARBUFFER_H_

#include <cstdint>

class LinearBuffer {
public:
	LinearBuffer(unsigned char *buf, unsigned int size);
	bool put(unsigned int c); // Returns false if full.
	int get(); // Returns -1 if empty
	bool isEmpty() const { return wrIx == rdIx; }
	bool isFull() const;
	unsigned int bytes_stored() const; // Number of chars available for reading
	unsigned int bytes_free() const; // Number of bytes left in buffer
	void reset() { wrIx = rdIx = 0; }
private:
	unsigned char *const mBuf;
	const uint8_t mSize;
	uint8_t wrIx, rdIx;
};

#endif /* LINEARBUFFER_H_ */
