#ifndef __SMARTIO_FUNCTION_H__
#define __SMARTIO_FUNCTION_H__

#include <stdint.h>

class Attribute;
class LinearBuffer;

class FunctionBase {
	public:
		FunctionBase(const char *name, Attribute **attrs);
		void handleMsg(LinearBuffer &msg, int cmd);
		const char* getName() const { return mName; }
		Attribute *getAttr(int attr) const;

	private:
		void reqGetNoOfAttr(LinearBuffer &buf) const;
		void reqGetAttrInfo(LinearBuffer &buf) const;
		void reqGetAttrValue(LinearBuffer &buf) const;
		void reqSetAttrValue(LinearBuffer &buf);
		int getNoOfAttributes() const;
		const char* mName;
		Attribute **mAttributes;
};

class Function: public FunctionBase {
	public:
		Function(const char *name, Attribute **attrs);
};

#endif
