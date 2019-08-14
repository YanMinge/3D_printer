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

#if ENABLED(SPINDLE_LASER_ENABLE)

#include "../gcode.h"
#include "../../module/stepper.h"
#include "laser.h"

#if ENABLED(FACTORY_MACHINE_INFO)
#include "machine_info.h"
#endif

void GcodeSuite::M3_M4(const bool is_M4) {

  planner.synchronize();   // wait until previous movement commands (G0/G0/G2/G3) have completed before playing with the spindle
  if(IS_HEAD_PRINT() || IS_HEAD_NULL()){
    SERIAL_ECHOPGM("Can not support this command, head type is: ");
    head_t head_type = MachineInfo.get_head_type();
    if(head_type == HEAD_NULL){
      SERIAL_ECHOLNPGM("no head");
    }
    else{
      SERIAL_ECHOLNPGM("print head");
    }
	return;
  }
  #if SPINDLE_DIR_CHANGE
    set_spindle_direction(is_M4);
  #endif

  /**
   * Our final value for ocr_val is an unsigned 8 bit value between 0 and 255 which usually means uint8_t.
   * Went to uint16_t because some of the uint8_t calculations would sometimes give 1000 0000 rather than 1111 1111.
   * Then needed to AND the uint16_t result with 0x00FF to make sure we only wrote the byte of interest.
   */
  #if ENABLED(SPINDLE_LASER_PWM)
    if(parser.seen('F')){
		uint16_t laser_frequency = parser.value_ushort();
		Laser.set_laser_frequency(laser_frequency);
	}
	else{
      Laser.set_laser_frequency(1000);
	}

    if (parser.seen('S')) {
      uint16_t spindle_laser_power = parser.value_ushort();
      Laser.set_laser_power(spindle_laser_power);
      Laser.update_laser_power();
    }
  #endif
}

/**
 * M5 turn off spindle
 */
void GcodeSuite::M5() {
  planner.synchronize();
  if(IS_HEAD_PRINT() || IS_HEAD_NULL()){
    SERIAL_ECHOPGM("Can not support this command, head type is: ");
    head_t head_type = MachineInfo.get_head_type();
    if(head_type == HEAD_NULL){
      SERIAL_ECHOLNPGM("no head");
    }
    else{
      SERIAL_ECHOLNPGM("print head");
    }
	return;
  }
  Laser.set_laser_power(0);
}

#endif // SPINDLE_LASER_ENABLE
