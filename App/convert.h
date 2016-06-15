#include <stdint.h>

union val {
  int intval;
  char *str;
};


void smartio_raw_to_string(int ix, void* raw_value, char *result);
void smartio_string_to_raw(int ix, const char* str, uint8_t *raw, int *raw_len);


int getIntSize(int type);
void write_val_to_buffer(uint8_t *buf, int *len, int type, union val value);
int smartio_buf2value(int ix, const uint8_t* raw_value);
void smartio_buf2str(char *str, int type, const uint8_t *buf);
int smartio_attr_is_int(int type);
int smartio_attr_is_str(int type);

enum smartio_io_types {
  IO_ZERO,
  IO_AMPERE, /* 1 Ampere */
  IO_MILLIAMPERE,  /* 2 milli-Ampere */
  IO_ANGLE_RADIAN,  /* 3 angle */
  IO_ANGULAR_VELOCITY,  /* 4 angular velocity */
  IO_THERMAL_KBTU,  /* 5 thermal energy */
  IO_THERMAL_MBTU,  /* 6 thermal energy */
  IO_ASCII_CHAR,  /* 7 ASCII char */
  IO_ABSOLUTE_COUNT,  /* 8 absolute count */
  IO_INCREMENTAL_COUNT,  /* 9 incremental count */
  IO_10,  /* 10 obsolete! */
  IO_DAY_OF_WEEK,  /* 11 day of week (-1 = invalid) */
  IO_OBSOLETE,  /* 12 obsolete! */
  IO_ENERGY_KWH,  /* 13 kilowatt hours */
  IO_ENERGY_WH,  /* 14 Watt hours */
  IO_FLOW_LPS,  /* 15 flow volume */
  IO_FLOW_MLPS,  /* 16 flow volume */
  IO_LENGTH_METER,  /* 17 length */
  IO_LENGTH_KILOMETER,  /* 18 length */
  IO_LENGTH_MICROMETER,  /* 19 length */
  IO_LENGTH_MILLIMETER,  /* 20 length */
  IO_LEVEL_PERCENT,  /* 21 continuous level */
  IO_22,  /* 22 obsolete! */
  IO_MASS_GRAM,  /* 23 mass */
  IO_MASS_KILOGRAM,  /* 24 mass */
  IO_MASS_TON,  /* 25 mass */
  IO_MASS_MILLIGRAM,  /* 26 mass */
  IO_POWER_WATT,  /* 27 power */
  IO_POWER_KILOWATT,  /* 28 power */
  IO_CONCENTRATION_PPM,  /* 29 concentration in ppm */
  IO_PRESSURE_KPA,  /* 30 pressure */
  IO_RESISTANCE_OHM,  /* 31 resistance */
  IO_RESISTANCE_KOHM,  /* 32 resistance */
  IO_ASCII_STRING,     /* 33 ascii string */
  /*  IO_TEMP_C,
      IO_TEMP_K,*/
};
