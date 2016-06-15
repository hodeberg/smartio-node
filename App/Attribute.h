#ifndef __SMARTIO_ATTRIBUTE_H__
#define __SMARTIO_ATTRIBUTE_H__

#include <stdint.h>
#include "smartio.h"

struct AttrInfo {
	AttrInfo(bool in, bool out, bool dev, bool dir, int size, int t) : input(in), output(out), device(dev), isDir(dir), arrSize(size), type(t) {}
	uint8_t input:1;
	uint8_t output:1;
	uint8_t device:1;
	uint8_t isDir:1;
	uint8_t arrSize;
	uint8_t type;
};

class LinearBuffer;

class Attribute {
	public:
		Attribute(const char *name, AttrInfo &info): mInfo(info), mName(name) {}
		void reqGetAttrInfo(LinearBuffer &buf) const;
		void reqGetAttrValue(LinearBuffer &buf) const;
		void reqSetAttrValue(LinearBuffer &buf);
		virtual SmartioErr getAttrValue(void *buf, int& len) const = 0;
		virtual SmartioErr setAttrValue(void *buf) = 0;
	protected:
		int getType() const { return mInfo.type; }
	private:
  	const char* getName() const {return mName; }
		const struct AttrInfo *getAttrInfo(int attr) const { return &mInfo; }
		virtual int getAttrSize() const = 0;

		struct AttrInfo mInfo;
		const char *mName;
};



class IntAttribute: public Attribute {
	public:
		IntAttribute(const char *name, AttrInfo info) : Attribute(name, info) {}
		virtual SmartioErr getAttrValue(void *buf, int &len) const;
	private:
	  virtual int getAttrSize() const;
	  virtual SmartioErr setAttrValue(void *buf);
		virtual int getValue() const = 0;
	  virtual void setValue(int newValue) = 0;
};

class StrAttribute: public Attribute {
	public:
		StrAttribute(const char *name, AttrInfo info) : Attribute(name, info) {}
		virtual SmartioErr getAttrValue(void *buf, int &len) const;
	private:
	  virtual SmartioErr setAttrValue(void *buf);
 	  virtual char *getValue() const = 0;
	  virtual void setValue() {};
};




class RO_StrAttribute: public StrAttribute {
	public:
		RO_StrAttribute(const char *name, const char* initValue);
	private:
		virtual int getAttrSize() const;
		virtual char *getValue() const {return const_cast<char *>(s);}
		const char *s;
};

class RW_StrAttribute: public StrAttribute {
	public:
		RW_StrAttribute(const char *name, const char* initValue);
	private:
		virtual int getAttrSize() const;
		virtual char *getValue() const {return const_cast<char*>(&s[0]);}
		char s[SMARTIO_DATA_SIZE];
};



#endif
