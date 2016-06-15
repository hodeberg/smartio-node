/*
 * TransportMessages.cpp
 *
 *  Created on: 6 apr 2016
 *      Author: hodeberg
 */


#include "TransportMessages.h"

MsgHeader::MsgHeader(unsigned int header) : transId(header & 7), direction((header >> 3) & 1)
{
	switch ((header >> 4) & 7) {
	case 0:
		type = MsgType::REQUEST;
		break;
	case 1:
		type = MsgType::RESPONSE;
		break;
	case 2:
		type = MsgType::ACKNOWLEDGED;
		break;
	case 3:
		type = MsgType::UNACKNOWLEDGED;
		break;
	case 4:
		type = MsgType::ACK;
		break;
	case 6:
		type = MsgType::EMPTY_MSG;
	case 5:
	default:
		type = MsgType::NAK;
		break;
	}
}



MsgHeader::MsgType MsgHeader::getType() const { return type; }
unsigned int MsgHeader::getID() const {
	return transId;
}



unsigned int MsgHeader::mkHeader() const {
	return transId | (direction << 3) | (int(type) << 4);
}


NullCompletion nullCompletion;


RxMsgBase::RxMsgBase(LinearBuffer* b, MsgCompletion &c) : MsgBaseType(static_cast<uint8_t>(TransportCodes::RXMSG)), buf(b), completion(c) {}

RxMsgBase::RxMsgBase(LinearBuffer* b) : RxMsgBase(b, nullCompletion) {}

