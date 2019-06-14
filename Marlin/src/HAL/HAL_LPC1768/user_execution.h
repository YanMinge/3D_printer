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
 *    3.  void    user_execution::cmd_g28(void);
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
  void cmd_g1(float x, float y, float z, float e);
  void cmd_g1_x(float x);
  void cmd_g1_y(float y);
  void cmd_g1_z(float z);
  void cmd_g28(void);
  void user_stop(void);
  void user_hardware_stop(void);
  void cmd_M109_M701(void);
  void cmd_M109_M702(void);
  void cmd_M2023(char *file_name);
  void cmd_M2024(void);
  void cmd_M2025(void);
  void cmd_M500(void);
};

extern user_execution UserExecution;
#endif // USE_DWIN_LCD
#endif // TARGET_LPC1768
#endif // _USER_EXECUTION_H_
