#include <cstring>

#include "Attribute.h"

extern "C" {
#include "convert.h"
};

// #include "smartio.h"
#include "LinearBuffer.h"
					
#define INPUT_BIT 0x80
#define OUTPUT_BIT 0x40
#define DEVICE_BIT 0x20
#define DIR_BIT 0x10


void Attribute::reqGetAttrInfo(LinearBuffer &buf) const
{
	uint8_t flags = 0;
		
	buf.prepareForResponse();
	if (mInfo.input) flags |= INPUT_BIT;
	if (mInfo.output) flags |= OUTPUT_BIT;
	if (mInfo.device) flags |= DEVICE_BIT;
	if (mInfo.isDir) flags |= DIR_BIT;
	buf.put(SMARTIO_SUCCESS);
	buf.put(flags);
	buf.put(mInfo.arrSize);
	buf.put(mInfo.type);
	buf.putStr(mName);
}

void Attribute::reqGetAttrValue(LinearBuffer &buf) const
{
	const unsigned int arrIx = buf.get();

	if (arrIx != 0xFF) {
		// Arrays not supported yet
		buf.appErrorResp(SMARTIO_ILLEGAL_ARRAY_INDEX);
	}
	else {
		buf.prepareForResponse();
		buf.put(SMARTIO_SUCCESS);
		buf.putAttr(*this);
	}
}

void Attribute::reqSetAttrValue(LinearBuffer &buf)
{
	const unsigned int arrIx = buf.get();
	

	if (arrIx != 0xFF) {
		// Arrays not supported yet
		buf.appErrorResp(SMARTIO_ILLEGAL_ARRAY_INDEX);
	}
	else {
		buf.prepareForResponse();
		buf.put(SMARTIO_SUCCESS);
		buf.getAttr(*this);
	}
}



int IntAttribute::getAttrSize() const
{
	return getIntSize(getType());
}

SmartioErr IntAttribute::getAttrValue(void *buf, int& len) const
{
	union val value;
	
	if (!smartio_attr_is_int(getType())) return SMARTIO_VALUE_TYPE_MISMATCH;
	
	value.intval = getValue();
	write_val_to_buffer(static_cast<uint8_t*>(buf), &len, getType(), value);
	return SMARTIO_SUCCESS;
}

SmartioErr IntAttribute::setAttrValue(void *buf)
{
	if (!smartio_attr_is_int(getType())) return SMARTIO_VALUE_TYPE_MISMATCH;
	
	setValue(smartio_buf2value(getType(), static_cast<uint8_t *>(buf)));
	return SMARTIO_SUCCESS;
}
	


SmartioErr StrAttribute::getAttrValue(void *buf, int& len) const
{
	union val value;
	
	if (!smartio_attr_is_str(getType())) return SMARTIO_VALUE_TYPE_MISMATCH;
	
	value.str = const_cast<char *>(getValue());
	write_val_to_buffer(static_cast<uint8_t*>(buf), &len, getType(), value);
	return SMARTIO_SUCCESS;
}
 
SmartioErr StrAttribute::setAttrValue(void *buf)
{
	char *str = getValue();
	
	if (!smartio_attr_is_str(getType())) return SMARTIO_VALUE_TYPE_MISMATCH;
	
	smartio_buf2str(str, getType(), static_cast<uint8_t *>(buf));
	setValue();
	return SMARTIO_SUCCESS;
}



RO_StrAttribute::RO_StrAttribute(const char *name, const char* value) :
  StrAttribute(name, AttrInfo(true, false, false, false, 0xFF, IO_ASCII_STRING)), s(value) {}

int RO_StrAttribute::getAttrSize() const
{
	return std::strlen(s) + 1;
}


	  
RW_StrAttribute::RW_StrAttribute(const char *name, const char* initValue) :
  StrAttribute(name, AttrInfo(true, true, false, false, 0xFF, IO_ASCII_STRING))
{
	if (initValue)
		std::strcpy(s, initValue);
}

int RW_StrAttribute::getAttrSize() const
{
	return std::strlen(s) + 1;
}
