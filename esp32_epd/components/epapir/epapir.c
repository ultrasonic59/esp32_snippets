#include <stdio.h>
#include "epapir.h"

bool _power_is_on = false;
void init_epapir(void)
{

}
void _Update_Part(void)
{
  _writeCommand(0x22);
  _writeData(0x04);
  _writeCommand(0x20);
///  _waitWhileBusy("_Update_Part", partial_refresh_time);
  _writeCommand(0xff);
}

void _setPartialRamArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  _writeCommand(0x11); // set ram entry mode
  _writeData(0x01);    // x increase, y decrease : as in demo code
  _writeCommand(0x44);
  _writeData(x / 8);
  _writeData((x + w - 1) / 8);
  _writeCommand(0x45);
  _writeData((y + h - 1) % 256);
  _writeData((y + h - 1) / 256);
  _writeData(y % 256);
  _writeData(y / 256);
  _writeCommand(0x4e);
  _writeData(x / 8);
  _writeCommand(0x4f);
  _writeData((y + h - 1) % 256);
  _writeData((y + h - 1) / 256);
}
void _PowerOn()
{
  if (!_power_is_on)
  {
    _writeCommand(0x22);
    _writeData(0xc0);
    _writeCommand(0x20);
    _waitWhileBusy("_PowerOn", power_on_time);
  }
  _power_is_on = true;
}

void GxEPD2_213::_PowerOff()
{
  _writeCommand(0x22);
  _writeData(0xc3);
  _writeCommand(0x20);
  _waitWhileBusy("_PowerOff", power_off_time);
  _power_is_on = false;
  _using_partial_mode = false;
}


void _InitDisplay()
{
 //// if (_hibernating) 
 ////   _reset();
  _writeCommand(0x01); // Panel configuration, Gate selection
  _writeData((HEIGHT - 1) % 256);
  _writeData((HEIGHT - 1) / 256);
  _writeData(0x00);
  _writeCommand(0x0c); // softstart
  _writeData(0xd7);
  _writeData(0xd6);
  _writeData(0x9d);
  _writeCommand(0x2c); // VCOM setting
  _writeData(0xa8);    // * different
  _writeCommand(0x3a); // DummyLine
  _writeData(0x1a);    // 4 dummy line per gate
  _writeCommand(0x3b); // Gatetime
  _writeData(0x08);    // 2us per line
  _setPartialRamArea(0, 0, WIDTH, HEIGHT);
}
