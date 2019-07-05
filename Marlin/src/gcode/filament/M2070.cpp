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

#if ENABLED(G38_PROBE_TARGET)
#include "../gcode.h"
#include "../../module/endstops.h"
#include "../../module/motion.h"
#include "../../module/stepper.h"
#include "../../module/probe.h"
#include "../../module/temperature.h"
#include "../../Marlin.h"

#if ENABLED(USE_DWIN_LCD)
#include "dwin.h"
#include "lcd_process.h"
#include "filament_ui.h"

void synchronize_temperature_and_motion(void)
{
  millis_t now = 0, next_temp_ms = 0;
  const int8_t target_extruder = gcode.get_target_extruder_from_command();
  do{

    idle();
#if HAS_TEMP_HOTEND
    now = millis();
    if (ELAPSED(now, next_temp_ms)) { // Print temp & remaining time every 1s while waiting
      next_temp_ms = now + 1000UL;
      thermalManager.print_heater_states(target_extruder);
    }
    if(!thermalManager.still_heating(HOTEND_INDEX))
    {
      wait_for_heatup = false;
    }
#endif
    if(filament_show.get_progress_load_return_status())
    {
      dwin_process.change_lcd_page(PRINT_HOME_PAGE_EN,PRINT_HOME_PAGE_CH);
      filament_show.reset_progress_status();
      break;
    }
  }while(planner.has_blocks_queued() || planner.cleaning_buffer_counter || wait_for_heatup);

  if(filament_show.get_progress_start_status())
  {
    if(filament_show.get_progress_heat_cool_status()) //load the filament
    {
      dwin_process.change_lcd_page(EXCEPTION_COMPLETE_HINT_PAGE_EN,EXCEPTION_COMPLETE_HINT_PAGE_CH);
      dwin_process.show_machine_status(PRINT_MACHINE_STATUS_LOAD_FILAMENT_CH);
      dwin_process.set_machine_status(PRINT_MACHINE_STATUS_LOAD_FILAMENT_CH);
    }
  }
}

inline void G38_run_probe_temperature() {
  planner.synchronize();  // wait until the machine is idle

  // Move until destination reached or target hit
  endstops.enable(true);
  G38_move = 1;
  prepare_move_to_destination();
  synchronize_temperature_and_motion();
}

/**
 * M2070 Z400
 */
void GcodeSuite::M2070(void) {
  // Get X Y Z E F
  get_destination_from_command();
  setup_for_endstop_or_probe_move();

  // If any axis has enough movement, do the move
  LOOP_XYZ(i)
    if (ABS(destination[i] - current_position[i]) >= G38_MINIMUM_MOVE) {
      if (!parser.seenval('F')) feedrate_mm_s = homing_feedrate((AxisEnum)i);

      G38_run_probe_temperature();
    }

  G38_move = 0;
  endstops.hit_on_purpose();
  set_current_from_steppers_for_axis(ALL_AXES);
  sync_plan_position();

  endstops.not_homing();
  clean_up_after_endstop_or_probe_move();
}

#endif //USE_DWIN_LCD
#endif // G38_PROBE_TARGET

