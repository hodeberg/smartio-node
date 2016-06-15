/* Contains data and functions for converting between smartio binary values and 
   the strings used by sysfs 

How do we convert?

Consider voltage encoded in a raw 12-bit value [0..4095], 
where 0V is at 2048, -10V at 0 and +10V at 4095. To convert this to a floating-point
value:

float = (((float)raw) - 2048) * (20.0/4096.0)

Which results in:
offset = 2048
scale = 4096 / 20 = 1024

and the formula

float = (((float)raw) - offset) / scale


And the reverse operation will be:
raw = (unsigned int)(float*scale + offset)


Note that many type definitions have been borrowed from here:
http://www.lonmark.org/technical_resources/resource_files/snvt.pdf

*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "convert.h"


struct scaled_int {
  uint8_t no_of_bytes; /* 1, 2 or 4 */
  int8_t scale_power;
  uint32_t offset;
  char *unit;
};


const struct scaled_int variables[] = {
  {0, 0, 0, NULL},  /* null */
  {2, 1, 32768, "A"},  /* 1 Ampere */
  {2, 1, 32768, "mA"},  /* 2 milli-Ampere */
  {2, 3, 0, "radians"},      /* 3 angle */
  {2, 1, 32768, "radians/s"},  /* 4 angular velocity */
  {2, 0, 0, "kBTU"},  /* 5 thermal energy */
  {2, 0, 0, "MBTU"},  /* 6 thermal energy */
  {1, 0, 0, NULL},  /* 7 ASCII char */
  {2, -2, 0, NULL},  /* 8 absolute count */
  {2, 0, 32768, NULL},  /* 9 incremental count */
  {0, 0, 0, NULL},  /* 10 obsolete! */
  {1, 0, 1, NULL},  /* 11 day of week (-1 = invalid) */
  {0, 0, 0, NULL},  /* 12 obsolete! */
  {2, 0, 0, "kWh"},  /* 13 kilowatt hours */
  {2, 1, 0, "Wh"},  /* 14 Watt hours */
  {2, 0, 0, "l/s"},  /* 15 flow volume */
  {2, 0, 0, "ml/s"},  /* 16 flow volume */
  {2, 1, 0, "m"},  /* 17 length */
  {2, 1, 0, "km"},  /* 18 length */
  {2, 1, 0, "um"},  /* 19 length */
  {2, 1, 0, "mm"},  /* 20 length */
  {1, 0, 0, "%"},  /* 21 continuous level */
  {0, 0, 0, NULL},  /* 22 obsolete! */
  {2, 1, 0, "g"},  /* 23 mass */
  {2, 1, 0, "kg"},  /* 24 mass */
  {2, 1, 0, "tons"},  /* 25 mass */
  {2, 1, 0, "mg"},  /* 26 mass */
  {2, 1, 0, "W"},  /* 27 power */
  {2, 1, 0, "kW"},  /* 28 power */
  {2, 0, 0, "ppm"},  /* 29 concentration in ppm */
  {2, 1, 32768, "kPa"},  /* 30 pressure */
  {2, 1, 0, "Ohm"},  /* 31 resistance */
  {2, 1, 0, "kOhm"},  /* 32 resistance */
  {31, 0, 0, NULL},  /* 33 ASCII string */
};

int smartio_attr_is_int(int type)
{
	switch (type) {
		case IO_AMPERE:
		case IO_MILLIAMPERE:
		case IO_ANGLE_RADIAN:
		case IO_ANGULAR_VELOCITY:
		case IO_THERMAL_KBTU:
		case IO_THERMAL_MBTU:
		case IO_ABSOLUTE_COUNT:
		case IO_INCREMENTAL_COUNT:
		case IO_DAY_OF_WEEK:
		case IO_ENERGY_KWH:
		case IO_ENERGY_WH:
		case IO_FLOW_LPS:
		case IO_FLOW_MLPS:
		case IO_LENGTH_METER:
		case IO_LENGTH_KILOMETER:
		case IO_LENGTH_MICROMETER:
		case IO_LENGTH_MILLIMETER:
		case IO_LEVEL_PERCENT:
		case IO_MASS_GRAM:
		case IO_MASS_KILOGRAM:
		case IO_MASS_TON:
		case IO_MASS_MILLIGRAM:
		case IO_POWER_WATT:
		case IO_POWER_KILOWATT:
		case IO_CONCENTRATION_PPM:
		case IO_PRESSURE_KPA:
		case IO_RESISTANCE_OHM:
		case IO_RESISTANCE_KOHM:
			return 1;
		default:
			return 0;
	}
}

int smartio_attr_is_str(int type)
{
	switch (type) {
		case IO_ASCII_CHAR:
		case IO_ASCII_STRING:
			return 1;
		default:
			return 0;
	}
}


int smartio_buf2value(int ix, const uint8_t* raw_value)
{
  unsigned int r = raw_value[0];
  int no_of_bytes = variables[ix].no_of_bytes;
  int i;

  if ((no_of_bytes != 1) && (no_of_bytes != 2) && (no_of_bytes != 4)) {
    return -1;
  }

  for (i = 1; i < no_of_bytes; i++) {
    r = r * 256 + raw_value[i];
  }
    
  return r;
}


void smartio_buf2str(char *str, int type, const uint8_t *buf)
{
  switch (type) {
  case IO_ASCII_CHAR:
		*str++ = *buf;
		*str = '\0';
    break;
  case IO_ASCII_STRING:
    strcpy(str, (const char*) buf);
    break;    
  default:
		strcpy(str, "ERR");
		break;
  }
}



static void int2buf(uint8_t* raw, int v, int bytes)
{
  switch (bytes) {
  case 1:
    *raw = v;
    break;
  case 2:
    *raw++ = v >> 8;
    *raw = v & 0xFF;
    break;
  case 4:
    *raw++ = v >> 24;
    *raw++ = v >> 16;
    *raw++ = v >> 8;
    *raw = v;
    break;
  }
}

void smartio_raw_to_string(int ix, void* raw_value, char *result)
{
  const struct scaled_int * const def = &variables[ix];
  int v;

    switch (ix) {
  case IO_LEVEL_PERCENT:
    /* Multiplier of 0.5 needs special handling */
    v = smartio_buf2value(ix, raw_value);
    sprintf(result, "%d %s\n", v / 2, def->unit);
    break;
  case IO_ASCII_STRING:
    /* Text string */
    strcpy(result, raw_value);
    strcat(result, "\n");
    break;    
  default:
    v = smartio_buf2value(ix, raw_value);
    v -= def->offset;
    if (def->scale_power > 0) {
      int i;
      int integral;
      int fraction;
      int divide_by = 1;

      for (i = 0; i < def->scale_power; i++) {
	divide_by  *= 10;
      }
      integral = v / divide_by;
      fraction = abs(v % divide_by);
      if (def->unit) {
	sprintf(result, "%d%s%d %s\n", integral, ".", fraction, def->unit);
      }
      else {
	sprintf(result, "%d%s%d\n", integral, ".", fraction);
      }
    }
    else if (def->scale_power < 0) {
      int i;

      for (i = 0; i < -def->scale_power; i++) {
	v *= 10;
      }
      if (def->unit) {
	sprintf(result, "%d %s\n", v, def->unit);
      }
      else {
	sprintf(result, "%d\n", v);
      }
    }
    else {
      if (def->unit) {
	sprintf(result, "%d %s\n", v, def->unit);
      }
      else {
	sprintf(result, "%d\n", v);
      }
    }
  }
}


void smartio_string_to_raw(int ix, const char* str, uint8_t *raw, int *raw_len)
{
  const struct scaled_int * const def = &variables[ix];
  char *dot_pos = strchr(str, '.');
  int result;
  char buf[30];
  int i;

  strcpy(buf, str);

  switch (ix) {
  case IO_LEVEL_PERCENT:
    /* Multiplier of 0.5 needs special handling */
    if (dot_pos) buf[dot_pos - str] = '\0';
    result = atoi(buf);
    *raw = result;
    *raw_len = 1;
    break;
  case IO_ASCII_STRING:
    /* Text string */
    strcpy((char *) raw, str);
    *raw_len = strlen((char *) raw);
    break;    
  default:
    if (dot_pos) {
      for (i=0; i < def->scale_power; i++) {
	buf[dot_pos-str+i] = dot_pos[i+1];
	if (dot_pos[i+1] == '\0') break;
      }
      for ( ; i < def->scale_power; i++)
	buf[dot_pos-str+i] = '0';
      buf[dot_pos-str+i] = '\0';
    }
    else {
      char *end = &buf[strlen(buf)-1];

      while (!isdigit(*end)) {
	*end-- = '\0';
      }
      for (i=0; i < def->scale_power; i++)
	strcat(buf, "0");
    }
    result = atoi(buf);
    int2buf(raw, result, def->no_of_bytes);
    *raw_len = def->no_of_bytes;
    break;
  }
}


void write_val_to_buffer(uint8_t *buf, int *len, int type, union val value)
{
  if (type == IO_ASCII_STRING) {
    *len = strlen(value.str) + 1;  /* Send null byte too */
    strcpy((char *) buf, value.str);
  }
  else {
    const struct scaled_int * const def = &variables[type];
    int bytes = def->no_of_bytes;

    int2buf(buf, value.intval, bytes);
    *len = bytes;
  }
}

int getIntSize(int type)
{
	return variables[type].no_of_bytes;
}
	
