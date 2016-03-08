/*
 * TestRingBuffer.cpp
 *
 *  Created on: 8 mar 2016
 *      Author: hodeberg
 */

#include "RingBuffer.h"
#include <stdio.h>

extern "C" {
void TestRingBuffer();
};

void TestRingBuffer()
{
	uint8_t buf[16];
	char msg1[] = "0123456789";
	char msg2[] = "hansodeberg";
	RingBuffer rb(buf, sizeof buf);
	int i;
	unsigned int stored, left;
	const int readsize = 7;

	printf("Starting RingBuffer test\n");
	for (i=0; i < (sizeof msg1 - 1); i++)
		if (!rb.put(msg1[i]))
			printf("Error: failed to write msg1 at index %d\n", i);
	printf("Written msg1, size %u\n", sizeof msg1);
	stored = rb.bytes_stored();
	left = rb.bytes_free();
	printf("Bytes stored/left: %u, %u", stored, left);
	for (i=0; i < readsize; i++) {
		const int c = rb.get();

		if (c != msg1[i])
			printf("Error: read wrong char %d at index %d, expected %d\n", c, i, (int) msg1[i]);
	}
	printf("Read %d bytes\n", readsize);

	for (i=0; i < (sizeof msg2 - 1); i++)
		if (!rb.put(msg2[i]))
			printf("Error: failed to write msg2 at index %d\n", i);
	printf("Written msg2, size %d\n", sizeof msg2);
	stored = rb.bytes_stored();
	left = rb.bytes_free();

	printf("Bytes stored/left: %d, %d", stored, left);
}
