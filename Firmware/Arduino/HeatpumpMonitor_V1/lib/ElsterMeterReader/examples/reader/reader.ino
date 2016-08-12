/*
 Arduino library to read data from an Elster A100C electricity meter.

 Copyright (C) 2012 Dave Berkeley projects@rotwang.co.uk

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 USA
*/

#include "elster.h"

void meter_reading(unsigned long r)
{
  Serial.print(r);
  Serial.print("\r\n");
}

ElsterA100C meter(meter_reading);

void setup()
{
  Serial.begin(9600);
  Serial.println("IRDA Meter reader");
  meter.init(1);
}
void loop()
{
  // Decode the meter stream
  const int byte_data = meter.decode_bit_stream();
  if (byte_data != -1) {
    meter.on_data(byte_data);
  }
}
