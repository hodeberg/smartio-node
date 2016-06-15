/*
 * Application.cpp
 *
 *  Created on: 13 apr 2016
 *      Author: hodeberg
 */

#include <App/Application.h>
#include "smartio.h"
#include "LinearBuffer.h"

Application &getApp()
{
	static Application app("node");
	return app;
}



Application::Application(const char* s):  name(s), nodeFcn("node", nullptr)
{
	for(unsigned int i=0; i < (sizeof functions / sizeof functions[0]); i++)
		functions[i] = nullptr;
	registerFcn(nodeFcn);
}


int Application::noOfFunctions() const
{
	for (unsigned int i=0; i < (sizeof functions / sizeof functions[0]); i++)
		if (functions[i] == nullptr) return i;

	return (sizeof functions / sizeof functions[0]);
}



void Application::registerFcn(FunctionBase &fcn)
{
	for (unsigned int i=0; i < (sizeof functions / sizeof functions[0]); i++)
		if (functions[i] == nullptr) {
			functions[i] = &fcn;
			break;
		}
}

void Application::handleMsg(LinearBuffer &buf)
{
	const int module = buf.get();
	const int cmd = buf.get();

	switch (cmd) {
	case SMARTIO_GET_NO_OF_MODULES:
		if (module == 0) {
			buf.prepareForResponse();
			buf.put(SMARTIO_SUCCESS);
			buf.putWord16(noOfFunctions());
			buf.putStr(name);
		}
		else {
			buf.appErrorResp(SMARTIO_ILLEGAL_COMMAND);
		}
		break;
	case SMARTIO_GET_NO_OF_ATTRIBUTES:
	case SMARTIO_GET_ATTRIBUTE_DEFINITION:
	case SMARTIO_GET_ATTR_VALUE:
	case SMARTIO_SET_ATTR_VALUE:
		if (module < noOfFunctions()) {
			functions[module]->handleMsg(buf, cmd);
		}
		else {
			buf.appErrorResp(SMARTIO_ILLEGAL_MODULE_INDEX);
		}
		break;
	case SMARTIO_GET_STRING:
	default:
		buf.appErrorResp(SMARTIO_ILLEGAL_COMMAND);
	}
}

