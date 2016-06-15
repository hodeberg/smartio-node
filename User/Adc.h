/*
 * Adc.h
 *
 *  Created on: 22 apr 2016
 *      Author: hodeberg
 */

#ifndef USER_ADC_H_
#define USER_ADC_H_

#include "Function.h"
#include "Attribute.h"

class MyIntAttr : public IntAttribute {
public:
	MyIntAttr(const char *name, bool isInput, int type);
private:
	virtual int getValue() const;
    virtual void setValue(int newValue);
	int val;
};

class Adc : public Function {
public:
	Adc(const char *name);
private:
	RW_StrAttribute myRWStr;
	RO_StrAttribute myROStr;
	MyIntAttr gain;
	MyIntAttr offset;
	Attribute *mAttrs[5];
};

#endif /* USER_ADC_H_ */
