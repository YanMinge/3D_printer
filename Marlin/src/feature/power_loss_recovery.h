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
#pragma once

/**
 * power_loss_recovery.h - Resume an SD print after power-loss
 */

#include "MassStorageLib.h"
#include "udisk_reader.h"
#include "../inc/MarlinConfigPre.h"

#if ENABLED(MIXING_EXTRUDER)
  #include "../feature/mixing.h"
#endif

#define SAVE_INFO_INTERVAL_MS 0
//#define SAVE_EACH_CMD_MODE
#define DEBUG_POWER_LOSS_RECOVERY

typedef struct {
  uint8_t valid_head;

  // Machine state
  float current_position[NUM_AXIS];
  float position_shift[XYZ];

  uint16_t feedrate;

  #if ENABLED(SPINDLE_LASER_PWM)
    uint16_t laser_power;
  #endif

  #if HOTENDS > 1
    uint8_t active_hotend;
  #endif

  int16_t target_temperature[HOTENDS];

  #if HAS_HEATED_BED
    int16_t target_temperature_bed;
  #endif

  #if FAN_COUNT
    uint8_t fan_speed[FAN_COUNT];
  #endif

  #if HAS_LEVELING
    bool leveling;
    float fade;
  #endif

  #if ENABLED(FWRETRACT)
    float retract[EXTRUDERS], retract_hop;
  #endif

  // Mixing extruder and gradient
  #if ENABLED(MIXING_EXTRUDER)
    //uint_fast8_t selected_vtool;
    //mixer_comp_t color[NR_MIXING_VIRTUAL_TOOLS][MIXING_STEPPERS];
    #if ENABLED(GRADIENT_MIX)
      gradient_t gradient;
    #endif
  #endif

  // Command queue
  uint8_t commands_in_queue, cmd_queue_index_r;
  char command_queue[BUFSIZE][MAX_CMD_SIZE];

  // udisk Filename and position
  char udisk_filename[FILE_NAME_LEN * 4];
  char file_path[(FILE_DIRECTORY_DEPTH + 1) * FILE_NAME_LEN];
  uint32_t udisk_pos;

  // Job elapsed time
  millis_t print_job_elapsed;

  uint8_t valid_foot;

} job_recovery_info_t;

class PrintJobRecovery {
  public:
    static FIL file_obj;
    bool is_file_opened;
    static job_recovery_info_t info;

    static void init();

    static bool enabled;
    static void enable(const bool onoff);
    static void changed();

    static void check();
    static void resume();

    static inline bool exists() { return udisk.job_recover_file_exists(); }
    static inline void open(const bool read) { udisk.open_job_recovery_file(read); }
    static inline void close() {f_close(&file_obj);}

    static void purge();
    static void load();
    static void save(const bool force=
      #if ENABLED(SAVE_EACH_CMD_MODE)
        true
      #else
        false
      #endif
      , const bool save_queue=true
    );

  static inline bool valid() { return info.valid_head && info.valid_head == info.valid_foot; }

    #if ENABLED(DEBUG_POWER_LOSS_RECOVERY)
      static void debug(PGM_P const prefix);
    #endif

  private:
    static void write();
};

extern PrintJobRecovery recovery;
