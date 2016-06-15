/*
 * Adc.cpp
 *
 *  Created on: 22 apr 2016
 *      Author: hodeberg
 */

#include <User/Adc.h>
#include "convert.h"

MyIntAttr::MyIntAttr(const char *name, bool isInput, int type) : IntAttribute(name, AttrInfo{isInput, !isInput, false, false, 0xFF, type}), val(5) {}

int MyIntAttr::getValue() const {
	return val;
}

void MyIntAttr::setValue(int newValue) {
	val = newValue;
}


Adc::Adc(const char *name) : Function(name, mAttrs),
					   myRWStr("myRWStr", "Hello"),
					   myROStr("myROStr", "World"),
					   gain("gain", true, IO_LEVEL_PERCENT),
					   offset("offset", false, IO_LEVEL_PERCENT),
					   mAttrs{&myRWStr, &myROStr, &gain, &offset, nullptr } {
								  	// TODO Auto-generated constructor stub
}

Adc adc1("ADC_A");
Adc adc2("ADC_B");
