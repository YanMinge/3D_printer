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

#include "../../inc/MarlinConfig.h"

#if ENABLED(USB_DISK_SUPPORT)

#include "../gcode.h"
#include "udisk_reader.h"

/**
 * M20: List SD card to serial output
 */
void GcodeSuite::M2020() {
  SERIAL_ECHOLNPGM(MSG_BEGIN_FILE_LIST);

  for (char *fn = parser.string_arg; *fn; ++fn)
  {
  	if (*fn == ' ') 
    { 
      *fn = '\0';
  	}
  }
  udisk.ls(LS_SERIAL_PRINT, parser.string_arg, NULL);
  SERIAL_ECHOLNPGM(MSG_END_FILE_LIST);
}

#endif // USB_DISK_SUPPORT