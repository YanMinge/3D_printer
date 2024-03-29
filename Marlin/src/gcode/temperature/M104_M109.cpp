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

#include "../gcode.h"
#include "../../module/temperature.h"
#include "../../module/motion.h"
#include "../../module/planner.h"
#include "../../lcd/ultralcd.h"
#include "../../Marlin.h"

#if ENABLED(PRINTJOB_TIMER_AUTOSTART)
  #include "../../module/printcounter.h"
#endif

#if ENABLED(SINGLENOZZLE)
  #include "../../module/tool_change.h"
#endif

#if ENABLED(NEWPANEL)
  #include "filament_ui.h"
  #include "lcd_process.h"
#endif

/**
 * M104: Set hot end temperature
 */
void GcodeSuite::M104() {

  if (DEBUGGING(DRYRUN)) return;

  #if ENABLED(MIXING_EXTRUDER) && MIXING_VIRTUAL_TOOLS > 1
    constexpr int8_t target_extruder = 0;
  #else
    const int8_t target_extruder = get_target_extruder_from_command();
    if (target_extruder < 0) return;
  #endif

  #if PIN_EXISTS(HEATER_0_ENABLE)
    if(thermalManager.check_heater_status() == false){
      SERIAL_ECHOLNPGM("Hardware damage detected in the extruder");
      return;
    }
  #endif

  if (parser.seenval('S')) {
    const int16_t temp = parser.value_celsius();
    #if ENABLED(SINGLENOZZLE)
      singlenozzle_temp[target_extruder] = temp;
      if (target_extruder != active_extruder) return;
    #endif
    thermalManager.setTargetHotend(temp, target_extruder);

    #if ENABLED(DUAL_X_CARRIAGE)
      if (dxc_is_duplicating() && target_extruder == 0)
        thermalManager.setTargetHotend(temp ? temp + duplicate_extruder_temp_offset : 0, 1);
    #endif

    #if ENABLED(PRINTJOB_TIMER_AUTOSTART)
      /**
       * Stop the timer at the end of print. Start is managed by 'heat and wait' M109.
       * We use half EXTRUDE_MINTEMP here to allow nozzles to be put into hot
       * standby mode, for instance in a dual extruder setup, without affecting
       * the running print timer.
       */
      if (temp <= (EXTRUDE_MINTEMP) / 2) {
        print_job_timer.stop();
        ui.reset_status();
        if(dwin_process.is_computer_print())
        {
          dwin_process.show_start_up_page();
          dwin_process.set_computer_print_status(false);
        }
      }
      else
      {
        if(dwin_process.is_computer_print())
        {
          dwin_process.change_lcd_page(HOST_COMPUTER_PRINT_PAGE_EN, HOST_COMPUTER_PRINT_PAGE_CH);
        }
      }
    #endif
  }

  #if ENABLED(AUTOTEMP)
    planner.autotemp_M104_M109();
  #endif
}

/**
 * M109: Sxxx Wait for extruder(s) to reach temperature. Waits only when heating.
 *       Rxxx Wait for extruder(s) to reach temperature. Waits when heating and cooling.
 */
void GcodeSuite::M109() {

  if (DEBUGGING(DRYRUN)) return;

  #if ENABLED(MIXING_EXTRUDER) && MIXING_VIRTUAL_TOOLS > 1
    constexpr int8_t target_extruder = 0;
  #else
    const int8_t target_extruder = get_target_extruder_from_command();
    if (target_extruder < 0) return;
  #endif

  #if PIN_EXISTS(HEATER_0_ENABLE)
    if(thermalManager.check_heater_status() == false){
      SERIAL_ECHOLNPGM("Hardware damage detected in the extruder");
      return;
    }
  #endif

  const bool no_wait_for_cooling = parser.seenval('S'),
             set_temp = no_wait_for_cooling || parser.seenval('R');
  if (set_temp) {
    const int16_t temp = parser.value_celsius();
    #if ENABLED(SINGLENOZZLE)
      singlenozzle_temp[target_extruder] = temp;
      if (target_extruder != active_extruder) return;
    #endif
    thermalManager.setTargetHotend(temp, target_extruder);

    #if ENABLED(DUAL_X_CARRIAGE)
      if (dxc_is_duplicating() && target_extruder == 0)
        thermalManager.setTargetHotend(temp ? temp + duplicate_extruder_temp_offset : 0, 1);
    #endif

    #if ENABLED(PRINTJOB_TIMER_AUTOSTART)
      /**
       * Use half EXTRUDE_MINTEMP to allow nozzles to be put into hot
       * standby mode, (e.g., in a dual extruder setup) without affecting
       * the running print timer.
       */
      if (parser.value_celsius() <= (EXTRUDE_MINTEMP) / 2) {
        print_job_timer.stop();
        ui.reset_status();
        if(dwin_process.is_computer_print())
        {
          dwin_process.show_start_up_page();
          dwin_process.set_computer_print_status(false);
        }
      }
      else
      {
        if(dwin_process.is_computer_print())dwin_process.change_lcd_page(HOST_COMPUTER_PRINT_PAGE_EN, HOST_COMPUTER_PRINT_PAGE_CH);
        print_job_timer.start();
      }
    #endif

    #if EITHER(ULTRA_LCD, EXTENSIBLE_UI)
      if (thermalManager.isHeatingHotend(target_extruder) || !no_wait_for_cooling)
        thermalManager.set_heating_message(target_extruder);
    #endif
  }

  #if ENABLED(AUTOTEMP)
    planner.autotemp_M104_M109();
  #endif

  if (set_temp)
    (void)thermalManager.wait_for_hotend(target_extruder, no_wait_for_cooling);

}
