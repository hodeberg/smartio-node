/*
 * TransactionBuffer.cpp
 *
 *  Created on: 11 apr 2016
 *      Author: hodeberg
 */

#include "TransactionBuffer.h"
#include "Application.h"
#include "L2_Interface.h"

TransactionBuffer::TransactionBuffer(uint8_t *b, unsigned int size) : buf(b, size), timeout(-1) {

}

void TransactionBuffer::initFromRxMsg(LinearBuffer &rxMsg)
{
	buf = rxMsg;
}


void TransactionBuffer::handleMsg()
{
	getApp().handleMsg(buf);
}


void TransactionBuffer::post()
{
	L2_IF::postMessage(*this);
	timeout = TX_TIMEOUT;
}

