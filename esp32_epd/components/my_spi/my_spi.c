#include <stdio.h>
#include "my_spi.h"

void init_my_spi(void)
{

}
void _writeCommand(uint8_t c)
{
///  _beginTransaction(_spi_settings);
 //// if (_dc >= 0) digitalWrite(_dc, LOW);
///  if (_cs >= 0) digitalWrite(_cs, LOW);
////  _spi_write(c);
///  if (_cs >= 0) digitalWrite(_cs, HIGH);
////  if (_dc >= 0) digitalWrite(_dc, HIGH);
 //// _endTransaction();
}

void _writeData(uint8_t d)
{
 //// _beginTransaction(_spi_settings);
 //// if (_cs >= 0) digitalWrite(_cs, LOW);
 //// _spi_write(d);
 //// if (_cs >= 0) digitalWrite(_cs, HIGH);
 //// _endTransaction();
}
void _writeData_n(const uint8_t* data, uint16_t n)
{
 //// _beginTransaction(_spi_settings);
 //// if (_cs >= 0) digitalWrite(_cs, LOW);
 //// for (uint8_t i = 0; i < n; i++)
 //// {
 ////   _spi_write(*data++);
 //// }
 //// if (_cs >= 0) digitalWrite(_cs, HIGH);
 //// _endTransaction();
}

#if 0
void _Update_Part(void)
{
 //// _writeCommand(0x22);
 //// _writeData(0x04);
 //// _writeCommand(0x20);
///  _waitWhileBusy("_Update_Part", partial_refresh_time);
////  _writeCommand(0xff);
}
#endif