/*
 * RingBuffer.h
 *
 *  Created on: 8 mar 2016
 *      Author: hodeberg
 */

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#include <cstdint>

/* Notes:
 * There is no protection here. That must be added outside.
 * The buffer size must be a power of 2.
 */
class RingBuffer {
public:
	RingBuffer(uint8_t *buf, const int bufsize);
	bool put(uint8_t c); // Returns false if full.
	int get(); // Returns -1 if empty
	bool isEmpty() const;
	bool isFull() const;
	unsigned int bytes_stored() const; // Number of chars available for reading
	unsigned int bytes_free() const; // Number of bytes left in buffer
private:
	uint8_t * const mBuf;
	const uint8_t mSize;
	uint8_t wr_ix;
	uint8_t rd_ix;
};


#endif /* RINGBUFFER_H_ */
