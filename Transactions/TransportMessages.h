/*
 * TransportMessages.h
 *
 *  Created on: 12 mar 2016
 *      Author: hodeberg
 */

#ifndef TRANSPORTMESSAGES_H_
#define TRANSPORTMESSAGES_H_

#include <cstdint>

class LinearBuffer;
class TransactionBuffer;

enum class TransportCodes {
	DUMMY = 1,
	RXMSG,
	TXMSG,
	BLINK = 100,
};


class MsgHeader {
public:
	MsgHeader(unsigned int header);
	enum class MsgType {REQUEST, RESPONSE, ACKNOWLEDGED, UNACKNOWLEDGED, ACK, NAK };
	MsgType getType() const;
	unsigned int getID() const;
private:
	MsgType type;
	unsigned int transId;
	unsigned int direction;
};

struct MsgBaseType {
public:
	MsgBaseType(uint8_t code) : msgCode(code) {}
	uint8_t msgCode;
};

struct RxMsgBase : public MsgBaseType {
public:
	RxMsgBase(LinearBuffer* b) : MsgBaseType(static_cast<uint8_t>(TransportCodes::RXMSG)), buf(b) {}
	LinearBuffer* buf;
};

struct TxMsgBase : public MsgBaseType {
public:
	TxMsgBase(TransactionBuffer* b, bool wasSent) : MsgBaseType(static_cast<uint8_t>(TransportCodes::TXMSG)), buf(b), mWasSent(wasSent) {}
	TransactionBuffer* buf;
	bool mWasSent;
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
