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
	enum class MsgType {REQUEST, RESPONSE, ACKNOWLEDGED, UNACKNOWLEDGED, ACK, NAK, EMPTY_MSG };
	MsgHeader(MsgType t, unsigned int id): type(t), transId(id), direction(0) {}
	MsgType getType() const;
	unsigned int getID() const;
	unsigned int mkHeader() const;
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

class MsgCompletion {
public:
	virtual void messageHandled(LinearBuffer& buf) = 0;
};

class NullCompletion : public MsgCompletion {
public:
	virtual void messageHandled(LinearBuffer& buf) {}
};


struct RxMsgBase : public MsgBaseType {
public:
	RxMsgBase(LinearBuffer* b);
	RxMsgBase(LinearBuffer* b, MsgCompletion &c);
	LinearBuffer* buf;
	MsgCompletion &completion;
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
	TxMsgBase txMsg;
} TransportMessages;
#endif /* TRANSPORTMESSAGES_H_ */

namespace TransportLayer {
void sendMsg(MsgBaseType& msg);
bool sendMsgFromIrq(MsgBaseType& msg);
};
