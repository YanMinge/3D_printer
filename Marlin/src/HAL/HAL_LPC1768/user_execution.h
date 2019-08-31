/**
 * \par Copyright (C), 2018-2019, MakeBlock
 * \class   user_execution
 * \brief   Execution command API for LCD.
 * @file    user_execution.h
 * @author  Mark Yan
 * @version V1.0.0
 * @date    2019/05/15
 * @brief   Header file for LCD execution command API.
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
 *    5.  void    user_execution::cmd_g1_z(float z, float feedrate);
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

#ifndef _USER_EXECUTION_H_
#define _USER_EXECUTION_H_

#ifdef TARGET_LPC1768
#include <stdarg.h>
#include <stdio.h>
#include <Arduino.h>
#include "../../inc/MarlinConfigPre.h"
#include "../../inc/MarlinConfig.h"

#if ENABLED(USE_DWIN_LCD)
class user_execution
{
public:

  user_execution(void);
  virtual ~user_execution(void) { }
  void cmd_g92(float x, float y, float z, float e);
  void cmd_g92_e(float e);
  void cmd_g1(float x, float y, float z, float e);
  void cmd_now_g1(float x[3]);
  void cmd_now_g1_xy(float x, float y, float f);
  void cmd_g1_x(float x, float feedrate = 3000);
  void cmd_g1_y(float y, float feedrate = 3000);
  void cmd_g1_z(float z, float feedrate = 600);
  void cmd_g1_e(float e, float feedrate = 600);
  void cmd_g28(void);
  void cmd_now_g28(void);
  void cmd_now_M420(bool onoff);
  void cmd_now_M206(float height);
  void cmd_g29(void);
  void cmd_now_g29(void);
  void user_start(void);
  void user_stop(void);
  void user_hardware_stop(void);
  void cmd_now_M104(uint16_t temperature);
  void cmd_M106(uint16_t speed);
  void cmd_now_M106(uint16_t speed, uint8_t funs = 0);
  void cmd_M109(uint16_t temperature);
  void cmd_now_M109(uint16_t temperature);
  void cmd_now_M140(uint16_t temperature);
  void cmd_now_M190(uint16_t temperature);
  void cmd_M109_M701(void);
  void cmd_M109_M702(void);
  void cmd_now_M701(void);
  void cmd_now_M702(void);
  void cmd_M2023(char *file_name);
  void cmd_M2024(void);
  void cmd_M2026(int32_t pos);
  void cmd_M2032(bool val);
  void cmd_M2034(bool val);
  void pause_udisk_print(void);
  void cmd_M500(void);
  void cmd_M300(uint16_t frequency, uint16_t duration);
  void cmd_M410(void);
  void cmd_M2033(bool val);
  void get_next_command(void);
  void get_remain_command(void);
  void cmd_now_g_m(const char * cmd);
  void cmd_M107(void);
  void cmd_now_M107(void);
  void cmd_M104_M2070(void);
  void cmd_M2090_test(void);
  void cmd_g38_x(float x);
  void cmd_g38_y(float y);
  void cmd_g38_z(float z);
  void cmd_g38_z_single(float z);
  void cmd_now_g38_z(float z);
  void cmd_user_synchronize(void);
  void cmd_now_M500(void);
  void cmd_now_M502(void);
  void cmd_now_M2524(void);
  bool lcd_immediate_execution;
};

extern user_execution UserExecution;
#endif // USE_DWIN_LCD
#endif // TARGET_LPC1768
#endif // _USER_EXECUTION_H_
