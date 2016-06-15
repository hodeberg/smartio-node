/*
 * Application.h
 *
 *  Created on: 13 apr 2016
 *      Author: hodeberg
 */

#ifndef APP_APPLICATION_H_
#define APP_APPLICATION_H_

#define MAX_NO_OF_FUNCTIONS 10

#include "Function.h"

class LinearBuffer;


class Application {
public:
	Application(const char* s);
	void registerFcn(FunctionBase &fcn);
	void handleMsg(LinearBuffer &msg);
private:
	const char *name;
	int noOfFunctions() const;
	FunctionBase *functions[MAX_NO_OF_FUNCTIONS];
	FunctionBase nodeFcn;
};

Application &getApp();

#endif /* APP_APPLICATION_H_ */
