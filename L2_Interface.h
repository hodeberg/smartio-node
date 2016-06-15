/*
 * L2_Interface.h
 *
 *  Created on: 17 mar 2016
 *      Author: hodeberg
 */

#ifndef L2_INTERFACE_H_
#define L2_INTERFACE_H_

class LinearBuffer;
class TransactionBuffer;

namespace L2_IF {

class MsgSender {
public:
	virtual void postMessage(TransactionBuffer& outMsg) = 0;
};
void registerMsgSender(MsgSender& sender);
bool sendL2Reply(LinearBuffer& replyMsg, LinearBuffer& inMsg);
void postMessage(TransactionBuffer& outMsg);
}



#endif /* L2_INTERFACE_H_ */
