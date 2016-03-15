/*
 * TransportMessages.h
 *
 *  Created on: 12 mar 2016
 *      Author: hodeberg
 */

#ifndef TRANSPORTMESSAGES_H_
#define TRANSPORTMESSAGES_H_

#include <cstdint>

class BufReadIf;

enum class TransportCodes {
	DUMMY = 1,
	RXMSG,
	BLINK = 100,
};

struct MsgBaseType {
public:
	MsgBaseType(uint8_t code) : msgCode(code) {}
	uint8_t msgCode;
};

struct RxMsgBase : public MsgBaseType {
public:
	RxMsgBase(BufReadIf* b) : MsgBaseType(static_cast<uint8_t>(TransportCodes::RXMSG)), buf(b) {}
	BufReadIf* buf;
};


// Used to determine size of message queue elements.
typedef union {
	MsgBaseType base;
	RxMsgBase rxMsg;
} TransportMessages;
#endif /* TRANSPORTMESSAGES_H_ */

namespace TransportLayer {
void sendMsg(MsgBaseType& msg);
bool sendMsgFromIrq(MsgBaseType& msg);
};
