#include <cstddef>
#include "Function.h"
#include "Application.h"
#include "Attribute.h"
#include "LinearBuffer.h"
#include "smartio.h"

FunctionBase::FunctionBase(const char *name, Attribute **attrs) : mName(name), mAttributes(attrs)
{
}


int FunctionBase::getNoOfAttributes() const {
	int noOfAttrs = 0;

	if (mAttributes != nullptr)
		while (mAttributes[noOfAttrs] != nullptr)
			noOfAttrs++;

	return noOfAttrs;
}

Attribute *FunctionBase::getAttr(int attr) const
{
	if (getNoOfAttributes() <= attr) return nullptr; // Index too large
	
	return mAttributes[attr];
};


void FunctionBase::reqGetNoOfAttr(LinearBuffer &buf) const
{
	buf.prepareForResponse();
				
	buf.put(SMARTIO_SUCCESS);
	buf.putWord16(getNoOfAttributes());
	buf.putStr(mName);
}

void FunctionBase::reqGetAttrInfo(LinearBuffer &buf) const
{
	const int attr_ix = buf.getWord16();
	
	if (attr_ix < getNoOfAttributes())
		mAttributes[attr_ix]->reqGetAttrInfo(buf);
	else {
		buf.appErrorResp(SMARTIO_ILLEGAL_ATTRIBUTE_INDEX);
	}
}

void FunctionBase::reqGetAttrValue(LinearBuffer &buf) const
{
	const int attr_ix = buf.getWord16();
	
	if (attr_ix < getNoOfAttributes())
		mAttributes[attr_ix]->reqGetAttrValue(buf);
	else {
		buf.appErrorResp(SMARTIO_ILLEGAL_ATTRIBUTE_INDEX);
	}
}

void FunctionBase::reqSetAttrValue(LinearBuffer &buf)
{
	const int attr_ix = buf.getWord16();
	
	if (attr_ix < getNoOfAttributes())
		mAttributes[attr_ix]->reqSetAttrValue(buf);
	else {
		buf.appErrorResp(SMARTIO_ILLEGAL_ATTRIBUTE_INDEX);
	}
}


void FunctionBase::handleMsg(LinearBuffer &buf, int cmd)
{
	switch (cmd) {
	case SMARTIO_GET_NO_OF_ATTRIBUTES:
		reqGetNoOfAttr(buf);
		break;
	case SMARTIO_GET_ATTRIBUTE_DEFINITION:
		reqGetAttrInfo(buf);
		break;
	case SMARTIO_GET_ATTR_VALUE:
		reqGetAttrValue(buf);
		break;
	case SMARTIO_SET_ATTR_VALUE:
		reqSetAttrValue(buf);
		break;
	}
}

Function::Function(const char *name, Attribute **attrs) : FunctionBase(name, attrs)
{
	getApp().registerFcn(*this);
}
