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

#if HAS_HEATED_BED

#include "../gcode.h"
#include "../../module/temperature.h"
#include "../../module/motion.h"
#include "../../lcd/ultralcd.h"

#if ENABLED(PRINTJOB_TIMER_AUTOSTART)
  #include "../../module/printcounter.h"
#endif

#if ENABLED(PRINTER_EVENT_LEDS)
  #include "../../feature/leds/leds.h"
#endif

#if ENABLED(NEWPANEL)
  #include "filament_ui.h"
  #include "lcd_process.h"
#endif

#include "../../Marlin.h" // for wait_for_heatup and idle()

/**
 * M140: Set bed temperature
 */
void GcodeSuite::M140() {
  if (DEBUGGING(DRYRUN)) return;
  if (parser.seenval('S')) thermalManager.setTargetBed(parser.value_celsius());

  #if ENABLED(PRINTJOB_TIMER_AUTOSTART)
  if (parser.value_celsius() < BED_MINTEMP){
    if(dwin_process.is_computer_print()){
      dwin_process.show_start_up_page();
      dwin_process.set_computer_print_status(false);
    }
  }
  else{
    if(dwin_process.is_computer_print()){
      dwin_process.change_lcd_page(HOST_COMPUTER_PRINT_PAGE_EN, HOST_COMPUTER_PRINT_PAGE_CH);
    }
  }
  #endif
}

/**
 * M190: Sxxx Wait for bed current temp to reach target temp. Waits only when heating
 *       Rxxx Wait for bed current temp to reach target temp. Waits when heating and cooling
 */
void GcodeSuite::M190() {
  if (DEBUGGING(DRYRUN)) return;

  const bool no_wait_for_cooling = parser.seenval('S');
  if (no_wait_for_cooling || parser.seenval('R')) {
    thermalManager.setTargetBed(parser.value_celsius());
    #if ENABLED(PRINTJOB_TIMER_AUTOSTART)
      if (parser.value_celsius() < BED_MINTEMP){
        if(dwin_process.is_computer_print()){
          dwin_process.show_start_up_page();
          dwin_process.set_computer_print_status(false);
        }
      }
      else{
        print_job_timer.start();
        if(dwin_process.is_computer_print()){
          dwin_process.change_lcd_page(HOST_COMPUTER_PRINT_PAGE_EN, HOST_COMPUTER_PRINT_PAGE_CH);
        }
      }
    #endif
  }
  else return;

  ui.set_status_P(thermalManager.isHeatingBed() ? PSTR(MSG_BED_HEATING) : PSTR(MSG_BED_COOLING));
  thermalManager.wait_for_bed(no_wait_for_cooling);
}

#endif // HAS_HEATED_BED
