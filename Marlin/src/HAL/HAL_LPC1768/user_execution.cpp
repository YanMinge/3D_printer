/**
 * \par Copyright (C), 2018-2019, MakeBlock
 * \class   user_execution
 * \brief   Execution command API for LCD.
 * @file    user_execution.h
 * @author  Mark Yan
 * @version V1.0.0
 * @date    2019/05/15
 * @brief   source code for LCD execution command API.
 *
 * \par Copyright
 * This software is Copyright (C), 2018-2019, MakeBlock. Use is subject to license \n
 * conditions. The main licensing options available are GPL V2 or Commercial: \n
 *
 * \par Open Source Licensing GPL V2
 * This is the appropriate option if you want to share the source code of your \n
 * application with everyone you distribute it to, and you also want to give them \n
 * the right to share who uses it. If you wish to use this software under Open \n
 * Source Licensing, you must contribute all your source code to the open source \n
 * community in accordance with the GPL Version 2 when your application is \n
 * distributed. See http://www.gnu.org/copyleft/gpl.html
 *
 * \par Description
 * This file is used for LCD execution command.
 *
 * \par Method List:
 *
 *    1.  void    user_execution::cmd_g92(float x, float y, float z, float e);
 *    2.  void    user_execution::cmd_g1(float x, float y, float z, float e);
 *    3.  void    user_execution::cmd_g1_x(float x);
 *    4.  void    user_execution::cmd_g1_y(float y);
 *    5.  void    user_execution::cmd_g1_z(float feedrate, float z);
 *    6.  void    user_execution::cmd_g28(void);
 *    7.  void    user_execution::user_start(void);
 *    8.  void    user_execution::user_stop(void);
 *    9.  void    user_execution::user_hardware_stop(void);
 *    10.  void    user_execution::cmd_M109(uint16_t temperature);
 *    11.  void    user_execution::cmd_M109_M701(void);
 *    12.  void    user_execution::cmd_M109_M702(void);
 *    13.  void    user_execution::cmd_M2023(char *file_name);
 *    14.  void    user_execution::cmd_M2024(void);
 *    15.  void    user_execution::pause_udisk_print(void);
 *    16.  void    user_execution::cmd_M500(void);
 *    17.  void    user_execution::cmd_M300(uint16_t frequency, uint16_t duration);
 *    18.  void    user_execution::cmd_M2033(bool val);
 *
 * \par History:
 * <pre>
 * `<Author>`         `<Time>`        `<Version>`        `<Descr>`
 * Mark Yan         2019/05/15     1.0.0            Initial function design.
 * </pre>
 *
 */

#ifdef TARGET_LPC1768
#include "user_execution.h"
#include "../../gcode/gcode.h"

#if ENABLED(USE_DWIN_LCD)
#include HAL_PATH(.., HAL.h)
#include "../../gcode/queue.h"
#include "../../Marlin.h"
#include "../../module/motion.h"
#include "../../module/planner.h"
#include "../../module/temperature.h"
#include "machine_info.h"

#include "lcd_process.h"
#include "dwin.h"
#include "material_check.h"

#if ENABLED(USB_DISK_SUPPORT)
#include "udisk_reader.h"
#include "../../module/printcounter.h"
#endif

#if ENABLED(ADVANCED_PAUSE_FEATURE)
#include "../../feature/pause.h"
#endif

user_execution UserExecution;

user_execution::user_execution(void)
{
  lcd_immediate_execution = false;
}

void user_execution::cmd_g92(float x, float y, float z, float e)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("G92 X%4.1f Y%4.1f Z%4.1f E%4.1f"), x, y, z, e);
  enqueue_and_echo_command(cmd);
}

void user_execution::cmd_g92_e(float e)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("G92 E%4.1f"), e);
  enqueue_and_echo_command(cmd);
}

void user_execution::cmd_now_g1(float x[3])
{
  char cmd[32];
  sprintf_P(cmd, PSTR("G1 X%4.1f Y%4.1f Z%4.1f F3000"), x[0], x[1], x[2]);
  cmd_now_g_m(cmd);
}

void user_execution::cmd_g1(float x, float y, float z, float e)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("G1 X%4.1f Y%4.1f Z%4.1f E%4.1f"), x, y, z, e);
  enqueue_and_echo_command(cmd);
  cmd_now_g_m(cmd);
}

void user_execution::cmd_now_g1_xy(float x, float y, float f)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("G1 X%4.1f Y%4.1f F%4.1f"), x, y, f);
  cmd_now_g_m(cmd);
}

void user_execution::cmd_g1_x(float x, float feedrate)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("G1 F%5.1f X%4.1f"), feedrate, x);
  enqueue_and_echo_command(cmd);
}

void user_execution::cmd_g1_y(float y, float feedrate)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("G1 F%5.1f Y%4.1f"), feedrate, y);
  enqueue_and_echo_command(cmd);
}

void user_execution::cmd_g1_z(float z, float feedrate)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("G1 F%5.1f Z%4.2f"), feedrate, z);
  enqueue_and_echo_command(cmd);
}

void user_execution::cmd_g1_e(float e, float feedrate)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("G1 F%5.1f E%4.1f"), feedrate, e);
  enqueue_and_echo_command(cmd);
}

void user_execution::cmd_g28(void)
{
  enqueue_and_echo_command("G28");
}

void user_execution::cmd_now_g28(void)
{
  cmd_now_g_m("G28");
}

void user_execution::cmd_now_M420(bool onoff)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("M420 S%d"), onoff);
  cmd_now_g_m(cmd);
}

void user_execution::cmd_now_M206(float height)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("M206 Z%0.2f"), height);
  cmd_now_g_m(cmd);
}

void user_execution::cmd_g29(void)
{
  enqueue_and_echo_command("G29");
}

void user_execution::cmd_now_g29(void)
{
  cmd_now_g_m("G29");
}

void user_execution::user_start(void)
{
  wait_for_user = true;
  wait_for_heatup = true;
}

void user_execution::user_stop(void)
{
  wait_for_user = false;
  wait_for_heatup = false;
  clear_command_queue();
}

void user_execution::user_hardware_stop(void)
{
  enqueue_and_echo_command(PSTR("M109 S0"));
}

void user_execution::cmd_now_M104(uint16_t temperature)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("M104 S%d"),temperature);
  cmd_now_g_m(cmd);
}

void user_execution::cmd_M106(uint16_t speed)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("M106 P0 S%d"),speed);
  enqueue_and_echo_command(cmd);
}

void user_execution::cmd_now_M106(uint16_t speed, uint8_t funs)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("M106 P%d S%d"),funs, speed);
  //gcode.process_subcommands_now_P(cmd);
  cmd_now_g_m(cmd);
}

void user_execution::cmd_M109(uint16_t temperature)
{
  char cmd[32];
  dwin_process.pre_percentage = 0;
  sprintf_P(cmd, PSTR("M109 S%d"),temperature);
  enqueue_and_echo_command(cmd);
}

void user_execution::cmd_now_M109(uint16_t temperature)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("M109 S%d"),temperature);
  cmd_now_g_m(cmd);
}

void user_execution::cmd_now_M140(uint16_t temperature)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("M140 S%d"),temperature);
  cmd_now_g_m(cmd);
}

void user_execution::cmd_now_M190(uint16_t temperature)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("M190 S%d"),temperature);
  cmd_now_g_m(cmd);
}

void user_execution::cmd_M109_M701(void)
{
  //enqueue_and_echo_commands_P(PSTR("M106 S255\nM109 S210\nG38.2 F480 Z400\nM701"));
  dwin_process.pre_percentage = 0;
  enqueue_and_echo_commands_P(PSTR("M106 S150\nM109 S230\nM701"));
}

void user_execution::cmd_now_M701(void)
{
  //gcode.process_subcommands_now_P(PSTR("M701"));
  cmd_now_g_m(PSTR("M701"));
}

void user_execution::cmd_now_M702(void)
{
  //gcode.process_subcommands_now_P(PSTR("M702"));
  cmd_now_g_m(PSTR("M702"));
}

void user_execution::cmd_M109_M702(void)
{
  dwin_process.pre_percentage = 0;
  enqueue_and_echo_commands_P(PSTR("M106 S120\nM109 S230\nM702"));
}

void user_execution::cmd_M2023(char *file_name)
{
  char cmd[50];
  sprintf_P(cmd, PSTR("M2023 %s"), file_name);
  enqueue_and_echo_command(cmd);
}

void user_execution::cmd_M2024(void)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("M2024"));
  enqueue_and_echo_command(cmd);
}

void user_execution::cmd_M2026(int32_t pos)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("M2026 S%ld"), pos);
  enqueue_and_echo_command(cmd);
}

void user_execution::cmd_M2032(bool val)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("M2032 S%d"), val);
  enqueue_and_echo_command(cmd);
}

void user_execution::cmd_M2034(bool val)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("M2034 S%d"), val);
  enqueue_and_echo_command(cmd);
}

void user_execution::pause_udisk_print(void)
{
#if ENABLED(USB_DISK_SUPPORT)
  if(IS_UDISK_PRINTING())
  {
    udisk.pause_udisk_print();
    print_job_timer.pause();
    clear_command_queue();
    quickstop_stepper();
#if ENABLED(ADVANCED_PAUSE_FEATURE)
    pause_print_data.udisk_pos = udisk.udisk_block_pos;
    // Retract filament
    if(IS_HEAD_PRINT())
    {
      do_pause_e_move(-FILAMENT_UNLOAD_RETRACT_LENGTH, PAUSE_PARK_RETRACT_FEEDRATE);
    }
    else if(IS_HEAD_LASER())
    {
      //turn off laser
      cmd_now_g0_xy(NATIVE_TO_LOGICAL(current_position[X_AXIS], X_AXIS) + 0.1, NATIVE_TO_LOGICAL(current_position[Y_AXIS],Y_AXIS), 300);
      cmd_now_g0_xy(NATIVE_TO_LOGICAL(current_position[X_AXIS], X_AXIS) - 0.1, NATIVE_TO_LOGICAL(current_position[Y_AXIS],Y_AXIS), 300);
    }
#endif
    wait_for_heatup = false;
  }
#endif
}

void user_execution::cmd_M500(void)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("M500"));
  enqueue_and_echo_command(cmd);
}

void user_execution::cmd_M300(uint16_t frequency, uint16_t duration)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("M300 S%d P%d"),frequency,duration);
  enqueue_and_echo_command(cmd);
}

void user_execution::cmd_M410(void)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("M410"));
  lcd_immediate_execution = true;
  enqueue_and_echo_command(cmd);
  get_next_command();
  lcd_immediate_execution =false;
}

void user_execution::cmd_M2033(bool val)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("M2033 S%d"), val);
  enqueue_and_echo_command(cmd);
}

void user_execution::get_next_command(void)
{
  if (commands_in_queue < BUFSIZE) get_available_commands();
  advance_command_queue();
  idle();
}

void user_execution::get_remain_command(void)
{
  while(commands_in_queue)
  {
    //get_available_commands();
    advance_command_queue();
  }
}

void user_execution::cmd_now_g_m(const char * cmd)
{
  enqueue_and_echo_command(cmd);
  get_remain_command();
}

void user_execution::cmd_M107(void)
{
  enqueue_and_echo_command(PSTR("M107"));
}

void user_execution::cmd_now_M107(void)
{
  //gcode.process_subcommands_now_P(PSTR("M107"));
  cmd_now_g_m(PSTR("M107"));
}

void user_execution::cmd_M104_M2070(void)
{
  enqueue_and_echo_commands_P(PSTR("M106 S255\n M104 S230 \nM2070 Z500\nM701"));
}

void user_execution::cmd_g38_x(float x)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("G38.2 F3000 X%4.1f"), x);
  enqueue_and_echo_command(cmd);
}

void user_execution::cmd_g38_y(float y)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("G38.2 F3000 Y%4.1f"), y);
  enqueue_and_echo_command(cmd);
}

void user_execution::cmd_g38_z(float z)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("G38.2 F600 Z%4.1f"), z);
  enqueue_and_echo_command(cmd);
}

void user_execution::cmd_now_g38_z(float z)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("G38.2 F600 Z%4.1f"), z);
  cmd_now_g_m(cmd);
}


void user_execution::cmd_user_synchronize(void)
{
  planner.synchronize();
}

void user_execution::cmd_now_M500(void)
{
  cmd_now_g_m(PSTR("M500"));
}

void user_execution::cmd_now_M502(void)
{
  cmd_now_g_m(PSTR("M502"));
}

void user_execution::cmd_now_M2524(void)
{
  cmd_now_g_m(PSTR("M2524"));
}

void user_execution::cmd_M1000(bool s)
{
  if(s)
  {
    enqueue_and_echo_command(PSTR("M1000 S"));
  }
  else
  {
    enqueue_and_echo_command(PSTR("M1000"));
  }
}

void user_execution::cmd_quick_stop(bool stop_status)
{
  if(stop_status)
  {
    lcd_immediate_execution = true;
    quickstop_stepper();
    lcd_immediate_execution = false;
  }
  else
  {
    quickstop_stepper();
  }
}

void user_execution::cmd_M2050(language_type lan)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("M2050 L%d"), lan);
  enqueue_and_echo_command(cmd);
}

void user_execution::cmd_g92_z(float z)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("G92 Z%4.1f"), z);
  enqueue_and_echo_command(cmd);
}

void user_execution::cmd_now_M3(uint16_t power)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("M3 S%d"), power);
  cmd_now_g_m(cmd);
}

void user_execution::cmd_now_g0_xy(float x, float y, float f)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("G0 X%4.2f Y%4.2f F%4.2f"), x, y, f);
  cmd_now_g_m(cmd);
}

void user_execution::cmd_now_g0_z(float z, float feedrate)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("G0 F%5.1f Z%4.2f"), feedrate, z);
  cmd_now_g_m(cmd);
}

void user_execution::stop_udisk_print(void)
{
#if ENABLED(USB_DISK_SUPPORT)
  if(IS_UDISK_PRINTING())
  {
    udisk.stop_udisk_print();
    print_job_timer.stop();
    clear_command_queue();
    quickstop_stepper();
  }
  thermalManager.disable_all_heaters();
  thermalManager.zero_fan_speeds();
#endif
}

void user_execution::cmd_g92_xy(float x, float y)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("G92 X%4.1f Y%4.1f"), x, y);
  enqueue_and_echo_command(cmd);
}

#endif // USE_DWIN_LCD
#endif // TARGET_LPC1768
