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

#if ENABLED(NEWPANEL)
  #include "lcd_parser.h"
#endif

inline void G38_single_probe(const uint8_t move_value) {
  endstops.enable(true);
  G38_move = move_value;
  prepare_move_to_destination();
  planner.synchronize();
  G38_move = 0;
  endstops.hit_on_purpose();
  set_current_from_steppers_for_axis(ALL_AXES);
  sync_plan_position();
}

inline bool G38_run_probe() {

  bool G38_pass_fail = false;

  #if MULTIPLE_PROBING > 1
    // Get direction of move and retract
    float retract_mm[XYZ];
    LOOP_XYZ(i) {
      const float dist = destination[i] - current_position[i];
      retract_mm[i] = ABS(dist) < G38_MINIMUM_MOVE ? 0 : home_bump_mm((AxisEnum)i) * (dist > 0 ? -1 : 1);
    }
  #endif

  planner.synchronize();  // wait until the machine is idle

  #if ENABLED(USE_DWIN_LCD)
    if(dwin_parser.lcd_stop_status)return false;
  #endif

  // Move flag value
  #if ENABLED(G38_PROBE_AWAY)
    const uint8_t move_value = parser.subcode;
  #else
    constexpr uint8_t move_value = 1;
  #endif

  G38_did_trigger = false;

  // Move until destination reached or target hit
  G38_single_probe(move_value);

  if (G38_did_trigger) {
    G38_pass_fail = true;
    if(parser.seen('X')){
      current_position[X_AXIS] = X_MIN_POS;
    }
    if(parser.seen('Y')){
      current_position[Y_AXIS] = Y_MIN_POS;
    }
    if(parser.seen('Z') && (destination[Z_AXIS] >= 2 * Z_MAX_POS)){
      current_position[Z_AXIS] = Z_MAX_POS;
    }

    set_destination_from_current();
    sync_plan_position();

    #if MULTIPLE_PROBING > 1
      // Move away by the retract distance
      set_destination_from_current();
      LOOP_XYZ(i) destination[i] += retract_mm[i];
      endstops.enable(false);
      prepare_move_to_destination();
      planner.synchronize();

      REMEMBER(fr, feedrate_mm_s, feedrate_mm_s * 0.25);

      // Bump the target more slowly
      LOOP_XYZ(i) destination[i] -= retract_mm[i] * 2;

      G38_single_probe(move_value);
    #endif
  }

  endstops.not_homing();
  return G38_pass_fail;
}

/**
 * G38 Probe Target
 *
 *  G38.2 - Probe toward workpiece, stop on contact, signal error if failure
 *  G38.3 - Probe toward workpiece, stop on contact
 *
 * With G38_PROBE_AWAY:
 *
 *  G38.4 - Probe away from workpiece, stop on contact break, signal error if failure
 *  G38.5 - Probe away from workpiece, stop on contact break
 */
void GcodeSuite::G38(const int8_t subcode) {
  // Get X Y Z E F
  get_destination_from_command();

  setup_for_endstop_or_probe_move();

  const bool error_on_fail =
    #if ENABLED(G38_PROBE_AWAY)
      !TEST(subcode, 0)
    #else
      (subcode == 2)
    #endif
  ;

  // If any axis has enough movement, do the move
  LOOP_XYZ(i)
    if (ABS(destination[i] - current_position[i]) >= G38_MINIMUM_MOVE) {
      if (!parser.seenval('F')) feedrate_mm_s = homing_feedrate((AxisEnum)i);
      // If G38.2 fails throw an error
      if (!G38_run_probe() && error_on_fail) SERIAL_ERROR_MSG("Failed to reach target");
      break;
    }

  clean_up_after_endstop_or_probe_move();
}

#endif // G38_PROBE_TARGET
