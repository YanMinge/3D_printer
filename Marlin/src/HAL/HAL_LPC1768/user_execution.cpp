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
 *    5.  void    user_execution::cmd_g1_z(float z);
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
#include "lcd_process.h"

#if ENABLED(USB_DISK_SUPPORT)
#include "udisk_reader.h"
#include "../../module/printcounter.h"
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

void user_execution::cmd_g1(float x, float y, float z, float e)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("G1 X%4.1f Y%4.1f Z%4.1f E%4.1f"), x, y, z, e);
  enqueue_and_echo_command(cmd);
}

void user_execution::cmd_now_g1_xy(float x, float y, float f)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("G1 X%4.1f Y%4.1f F%4.1f"), x, y, f);
  gcode.process_subcommands_now_P(cmd);
}


void user_execution::cmd_g1_x(float x)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("G1 F3000 X%4.1f"), 2*x);
  enqueue_and_echo_command(cmd);
}

void user_execution::cmd_g1_y(float y)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("G1 F3000 Y%4.1f"), 2*y);
  enqueue_and_echo_command(cmd);
}

void user_execution::cmd_g1_z(float z)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("G1 F600 Z%4.1f"), 2*z);
  enqueue_and_echo_command(cmd);
}

void user_execution::cmd_g1_single_z(float z)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("G1 F600 Z%4.1f"), z);
  enqueue_and_echo_command(cmd);
}


void user_execution::cmd_g28(void)
{
  enqueue_and_echo_command("G28");
}

void user_execution::cmd_now_g28(void)
{
  gcode.process_subcommands_now_P("G28");
}

void user_execution::cmd_now_M420(bool onoff)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("M420 S%d"), onoff);
  gcode.process_subcommands_now_P("cmd");
}

void user_execution::cmd_now_M206(float height)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("M206 Z%0.1f"), height);
  SERIAL_PRINTF("show_save_calibration_data_page Z%0.1f\r\n",height);
  gcode.process_subcommands_now_P(cmd);
}

void user_execution::cmd_g29(void)
{
  enqueue_and_echo_command("G29");
}

void user_execution::cmd_now_g29(void)
{
  gcode.process_subcommands_now_P("G29");
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
  gcode.process_subcommands_now_P(cmd);
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
  gcode.process_subcommands_now_P(cmd);
}

void user_execution::cmd_M109_M701(void)
{
  //enqueue_and_echo_commands_P(PSTR("M106 S255\nM109 S210\nG38.2 F480 Z400\nM701"));
  dwin_process.pre_percentage = 0;
  enqueue_and_echo_commands_P(PSTR("M106 S150\nM109 S230\nM701"));
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

void user_execution::cmd_M107(void)
{
  enqueue_and_echo_command(PSTR("M107"));
}

void user_execution::cmd_M104_M2070(void)
{
  enqueue_and_echo_commands_P(PSTR("M106 S255\n M104 S230 \nM2070 Z500\nM701"));
}

void user_execution::cmd_g38_x(float x)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("G38.2 F3000 X%4.1f"), 2*x);
  enqueue_and_echo_command(cmd);
}

void user_execution::cmd_g38_y(float y)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("G38.2 F3000 Y%4.1f"), 2*y);
  enqueue_and_echo_command(cmd);
}

void user_execution::cmd_g38_z(float z)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("G38.2 F600 Z%4.1f"), 2*z);
  enqueue_and_echo_command(cmd);
}

void user_execution::cmd_now_g38_z(float z)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("G38.2 F600 Z%4.1f"), 2*z);
  gcode.process_subcommands_now_P(cmd);
}


void user_execution::cmd_user_synchronize(void)
{
  planner.synchronize();
}

void user_execution::cmd_now_M500(void)
{
  gcode.process_subcommands_now_P(PSTR("M500"));
}

void user_execution::cmd_now_M502(void)
{
  gcode.process_subcommands_now_P(PSTR("M502"));
}

#endif // USE_DWIN_LCD
#endif // TARGET_LPC1768
