/*
 * L2_Interface.cpp
 *
 *  Created on: 27 maj 2016
 *      Author: hodeberg
 */

#include "L2_Interface.h"


namespace L2_IF {

MsgSender *msgSender;

void registerMsgSender(MsgSender& sender)
{
	msgSender = &sender;
}

void postMessage(TransactionBuffer& outMsg)
{
	msgSender->postMessage(outMsg);
}
};
