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
 *    3.  void    user_execution::cmd_g28(void);
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

#if ENABLED(USE_DWIN_LCD)
#include HAL_PATH(.., HAL.h)
#include "../../gcode/queue.h"
#include "../../Marlin.h" 

user_execution UserExecution;

user_execution::user_execution(void)
{

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

void user_execution::cmd_g28(void)
{
  enqueue_and_echo_command("G28");
}

void user_execution::user_stop(void)
{
  wait_for_user = false;
  wait_for_heatup = false;
  clear_command_queue();
}

void user_execution::user_hardware_stop(void)
{
  enqueue_and_echo_command(PSTR("M105 S0"));
}

void user_execution::cmd_M109_M701(void)
{
  enqueue_and_echo_commands_P(PSTR("M109 S210\nM701"));
}

void user_execution::cmd_M109_M702(void)
{
  enqueue_and_echo_commands_P(PSTR("M109 S210\nM702"));
}

void user_execution::cmd_M2023(char *file_name)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("M2023 /%s"), file_name);
  enqueue_and_echo_command(cmd);
}

void user_execution::cmd_M2024(void)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("M2024"));
  enqueue_and_echo_command(cmd);
}

void user_execution::cmd_M2025(void)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("M2025"));
  enqueue_and_echo_command(cmd);
}

void user_execution::cmd_M500(void)
{
  char cmd[32];
  sprintf_P(cmd, PSTR("M500"));
  enqueue_and_echo_command(cmd);
}

#endif // USE_DWIN_LCD
#endif // TARGET_LPC1768
