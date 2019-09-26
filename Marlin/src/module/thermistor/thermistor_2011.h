/**
 * Marlin 3D Printer Firmware
 * Copyright (C) 2019 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// R25 = 100 kOhm, beta25 = 3950 K, 4.7 kOhm pull-up, QU-BD silicone bed QWG-104F-3950 thermistor
const short temptable_2011[][2] PROGMEM = {
  { OV(   1), 938 },
  { OV(  19), 314 },
  { OV(  26), 290 },
  { OV(  34), 272 },
  { OV(  42), 258 },
  { OV(  50), 247 },
  { OV(  59), 237 },
  { OV(  67), 229 },
  { OV(  72), 225 },
  { OV(  77), 221 },
  { OV(  86), 215 },
  { OV(  95), 209 },
  { OV( 104), 204 },
  { OV( 113), 199 },
  { OV( 122), 195 },
  { OV( 133), 190 },
  { OV( 141), 187 },
  { OV( 151), 183 },
  { OV( 162), 179 },
  { OV( 172), 176 },
  { OV( 191), 170 },
  { OV( 210), 165 },
  { OV( 229), 160 },
  { OV( 251), 155 },
  { OV( 274), 150 },
  { OV( 305), 144 },
  { OV( 332), 139 },
  { OV( 368), 133 },
  { OV( 399), 128 },
  { OV( 432), 123 },
  { OV( 473), 117 },
  { OV( 517), 111 },
  { OV( 561), 105 },
  { OV( 598), 100 },
  { OV( 635),  95 },
  { OV( 672),  90 },
  { OV( 707),  85 },
  { OV( 748),  79 },
  { OV( 793),  72 },
  { OV( 811),  69 },
  { OV( 833),  65 },
  { OV( 874),  57 },
  { OV( 883),  55 },
  { OV( 900),  51 },
  { OV( 923),  45 },
  { OV( 943),  39 },
  { OV( 971),  28 },
  { OV( 981),  23 },
  { OV( 991),  17 },
  { OV(1001),   9 },
  { OV(1020), -27 }
};
