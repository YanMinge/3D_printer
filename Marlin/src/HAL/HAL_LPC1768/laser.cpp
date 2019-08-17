/**
 * \par Copyright (C), 2018-2019, MakeBlock
 * \class   laser
 * \brief   the driver for laser head.
 * @file    laser.cpp
 * @author  Mark Yan
 * @version V1.0.0
 * @date    2019/08/14
 * @brief   source code for laser head driver.
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
 * This file is used for laser head driver.
 *
 * \par Method List:
 *
 *    1.  uint16_t laser_class::get_laser_power(void);
 *    2.  void     laser_class::set_laser_power(uint16_t power);
 *    3.  void     laser_class::delay_for_power_up(void);
 *    4.  void     laser_class::delay_for_power_down(void);
 *    5.  void     laser_class::set_laser_frequency(uint16_t laser_frequency);
 *    6.  void     laser_class::set_laser_ocr(uint16_t power);
 *    7.  void     laser_class::reset(void);
 *    8.  void     laser_class::update_laser_power(void);
 *
 * \par History:
 * <pre>
 * `<Author>`         `<Time>`        `<Version>`        `<Descr>`
 * Mark Yan         2019/08/14     1.0.0            Initial function design.
 * </pre>
 *
 */

#ifdef TARGET_LPC1768

#include "../../Marlin.h"
#include "../../module/planner.h"
#include "../../gcode/gcode.h"


#if ENABLED(USB_DISK_SUPPORT)
#include "udisk_reader.h"
#endif

#if ENABLED(FACTORY_MACHINE_INFO)
#include "machine_info.h"
#endif

#if ENABLED(USE_DWIN_LCD)
#include "dwin.h"
#include "lcd_file.h"
#include "lcd_process.h"
#endif

#include "user_execution.h"

#if ENABLED(SPINDLE_LASER_ENABLE)
#include "laser.h"

laser_class Laser;

laser_class::laser_class(void)
{
  laser_power = 0;
  laser_focus = 30;
  synchronize_status = true;
  laser_border_xy_position.buttom_right_x_position = 0;
  laser_border_xy_position.buttom_right_y_position = 0;
  laser_border_xy_position.upper_left_x_position = 0;
  laser_border_xy_position.upper_left_y_position = 0;
}

uint16_t laser_class::get_laser_power(void)
{
  return laser_power;
}

void laser_class::set_laser_power(uint16_t power)
{
  laser_power = power;
}

void laser_class::delay_for_power_up(void)
{
  if(SPINDLE_LASER_POWERUP_DELAY)
  {
    safe_delay(SPINDLE_LASER_POWERUP_DELAY);
  }
}

void laser_class::delay_for_power_down(void)
{
  if(SPINDLE_LASER_POWERDOWN_DELAY)
  {
    safe_delay(SPINDLE_LASER_POWERDOWN_DELAY);
  }
}

void laser_class::set_laser_frequency(uint16_t laser_frequency)
{
  pwm_set_frequency(SPINDLE_LASER_PWM_PIN, laser_frequency);
}

void laser_class::set_laser_ocr(uint16_t power)
{
  uint16_t ocr_val = constrain(power, 0, 1000);
  pwm_write_ratio(SPINDLE_LASER_PWM_PIN, float(ocr_val/1000.0));
}

void laser_class::reset(void)
{
  set_laser_frequency(1000);
  set_laser_power(0);
  set_laser_ocr(0);
}

void laser_class::update_laser_power(void)
{
  if (laser_power == 0)
  {
    set_laser_ocr(0);
    //delay_for_power_down();
  }
  else 
  {
    set_laser_ocr(laser_power);
    //delay_for_power_up();
  }
}

void laser_class::set_current_position_zero(void)
{
  UserExecution.cmd_g92(0,0,current_position[Z_AXIS],0);
}

void laser_class::process_synchronize_subcommands_now(PGM_P pgcode)
{
  planner.synchronize();
  if(synchronize_status)
  {
    gcode.process_subcommands_now_P(pgcode);
  }
}

void laser_class::laser_walking_border(void)
{
  char cmd[42];
  synchronize_status = true;
  laser_before_print();
  //if (!TEST(axis_known_position, X_AXIS) || !TEST(axis_known_position, Y_AXIS))
  //{
  //  process_synchronize_subcommands_now(PSTR("G28 X Y"));
  //}
  process_synchronize_subcommands_now("M3 S4");
  sprintf_P(cmd, PSTR("G1 X%f Y%f F3000"), laser_border_xy_position.upper_left_x_position,laser_border_xy_position.buttom_right_y_position);
  process_synchronize_subcommands_now(cmd);
  sprintf_P(cmd, PSTR("G1 X%f Y%f F3000"), laser_border_xy_position.buttom_right_x_position,laser_border_xy_position.buttom_right_y_position);
  process_synchronize_subcommands_now(cmd);
  sprintf_P(cmd, PSTR("G1 X%f Y%f F3000"), laser_border_xy_position.buttom_right_x_position,laser_border_xy_position.upper_left_y_position);
  process_synchronize_subcommands_now(cmd);
  sprintf_P(cmd, PSTR("G1 X%f Y%f F3000"), laser_border_xy_position.upper_left_x_position,laser_border_xy_position.upper_left_y_position);
  process_synchronize_subcommands_now(cmd);
  process_synchronize_subcommands_now("M3 S0");
  planner.synchronize();
}

void laser_class::show_laser_prepare_focus_page(void)
{
  char cmd[40];
  synchronize_status = true;
  dwin_process.show_machine_status_page(machine_status_type(LASER_MACHINE_STATUS_PREPARE_FOCUS_CH),LASER_PREPARE_PAGE_EN,LASER_PREPARE_PAGE_CH);
  dwin_process.set_machine_status(LASER_MACHINE_STATUS_PREPARE_FOCUS_CH);

  sprintf_P(cmd, PSTR("G38.2 Z%f F900"), current_position[Z_AXIS] + 20);
  process_synchronize_subcommands_now(cmd);
  if (!TEST(axis_known_position, X_AXIS) || !TEST(axis_known_position, Y_AXIS))
  {
    process_synchronize_subcommands_now(PSTR("G28 X Y"));
  }
  process_synchronize_subcommands_now(PSTR("G1 X120 Y100 F3000"));
  SERIAL_PRINTF("current_position = %f\r\n", current_position[Z_AXIS]);
  process_synchronize_subcommands_now(PSTR("G38.2 Z-20 F900"));
  SERIAL_PRINTF("current_position = %f\r\n", current_position[Z_AXIS]);
  process_synchronize_subcommands_now(PSTR("G92 Z0"));
  SERIAL_PRINTF("current_position = %f\r\n", current_position[Z_AXIS]);
  sprintf_P(cmd, PSTR("G1 Z%f F900"), laser_focus - 2);
  process_synchronize_subcommands_now(cmd);
  process_synchronize_subcommands_now(PSTR("M114"));

  //line1 and num1
  process_synchronize_subcommands_now(PSTR("G1 X80 Y150 F3000"));
  process_synchronize_subcommands_now(PSTR("M3 S1000"));
  process_synchronize_subcommands_now(PSTR("G1 X80 Y130"));
  process_synchronize_subcommands_now(PSTR("M3 S0"));

  process_synchronize_subcommands_now(PSTR("G1 X80 Y100"));
  process_synchronize_subcommands_now(PSTR("M3 S1000"));
  process_synchronize_subcommands_now(PSTR("G1 X80 Y80"));
  process_synchronize_subcommands_now(PSTR("M3 S0"));

  //line2 and num2
  sprintf_P(cmd, PSTR("G1 Z%f F900"), laser_focus - 1);
  process_synchronize_subcommands_now(cmd);
  process_synchronize_subcommands_now(PSTR("G1 X95 Y150 F3000"));
  process_synchronize_subcommands_now(PSTR("M3 S1000"));
  process_synchronize_subcommands_now(PSTR("G1 X95 Y130 F3000"));
  process_synchronize_subcommands_now(PSTR("M3 S0"));

  process_synchronize_subcommands_now(PSTR("G1 X90 Y100"));
  process_synchronize_subcommands_now(PSTR("M3 S1000"));
  process_synchronize_subcommands_now(PSTR("G1 X100 Y100"));
  process_synchronize_subcommands_now(PSTR("G1 X90 Y80"));
  process_synchronize_subcommands_now(PSTR("G1 X100 Y80"));
  process_synchronize_subcommands_now(PSTR("M3 S0"));

  //line3 and num3
  sprintf_P(cmd, PSTR("G1 Z%f F900"), laser_focus);
  process_synchronize_subcommands_now(cmd);
  process_synchronize_subcommands_now(PSTR("G1 X110 Y150 F3000"));
  process_synchronize_subcommands_now(PSTR("M3 S1000"));
  process_synchronize_subcommands_now(PSTR("G1 X110 Y130 F3000"));
  process_synchronize_subcommands_now(PSTR("M3 S0"));

  process_synchronize_subcommands_now(PSTR("G1 X105 Y100"));
  process_synchronize_subcommands_now(PSTR("M3 F100 S1000"));
  process_synchronize_subcommands_now(PSTR("G1 X115 Y100"));
  process_synchronize_subcommands_now(PSTR("G1 X115 Y80"));
  process_synchronize_subcommands_now(PSTR("G1 X105 Y80"));
  process_synchronize_subcommands_now(PSTR("M3 S0"));
  process_synchronize_subcommands_now(PSTR("G1 X105 Y90"));
  process_synchronize_subcommands_now(PSTR("M3 S1000"));
  process_synchronize_subcommands_now(PSTR("G1 X115 Y90"));
  process_synchronize_subcommands_now(PSTR("M3 S0"));

  //line4 and num4
  sprintf_P(cmd, PSTR("G1 Z%f F900"), laser_focus + 1);
  process_synchronize_subcommands_now(cmd);
  process_synchronize_subcommands_now(PSTR("G1 X125 Y150 F3000"));
  process_synchronize_subcommands_now(PSTR("M3 S1000"));
  process_synchronize_subcommands_now(PSTR("G1 X125 Y130 F3000"));
  process_synchronize_subcommands_now(PSTR("M3 S0"));

  process_synchronize_subcommands_now(PSTR("G1 X120 Y100"));
  process_synchronize_subcommands_now(PSTR("M3 F100 S1000"));
  process_synchronize_subcommands_now(PSTR("G1 X120 Y93"));
  process_synchronize_subcommands_now(PSTR("G1 X130 Y93"));
  process_synchronize_subcommands_now(PSTR("M3 S0"));
  process_synchronize_subcommands_now(PSTR("G1 X125 Y100"));
  process_synchronize_subcommands_now(PSTR("M3 S1000"));
  process_synchronize_subcommands_now(PSTR("G1 X125 Y80"));
  process_synchronize_subcommands_now(PSTR("M3 S0"));

  //line5 and num5
  sprintf_P(cmd, PSTR("G1 Z%f F900"), laser_focus + 2);
  process_synchronize_subcommands_now(cmd);
  process_synchronize_subcommands_now(PSTR("G1 X140 Y150 F3000"));
  process_synchronize_subcommands_now(PSTR("M3 S1000"));
  process_synchronize_subcommands_now(PSTR("G1 X140 Y130 F3000"));
  process_synchronize_subcommands_now(PSTR("M3 S0"));

  process_synchronize_subcommands_now(PSTR("G1 X150 Y100"));
  process_synchronize_subcommands_now(PSTR("M3 S1000"));
  process_synchronize_subcommands_now(PSTR("G1 X140 Y100"));
  process_synchronize_subcommands_now(PSTR("G1 X140 Y90"));
  process_synchronize_subcommands_now(PSTR("G1 X150 Y90"));
  process_synchronize_subcommands_now(PSTR("G1 X150 Y80"));
  process_synchronize_subcommands_now(PSTR("G1 X140 Y80"));
  process_synchronize_subcommands_now(PSTR("M3 S0"));

  if(synchronize_status)
  {
    dwin_process.change_lcd_page(LASER_LINE_CHOICE_PAGE_EN,LASER_LINE_CHOICE_PAGE_CH);
  }
  else
  {
    dwin_process.show_machine_status_page(LASER_MACHINE_STATUS_FOCUS_CONFIRM_CH,\
        LASER_EXCEPTION_SURE_RETURN_PAGE_EN,LASER_EXCEPTION_SURE_RETURN_PAGE_CH);
  }
}

void laser_class::laser_before_print(void)
{
  char process_cmd[42];
  synchronize_status = true;
  float center_x;
  float center_y;
  center_x = (laser_border_xy_position.buttom_right_x_position + laser_border_xy_position.upper_left_x_position)/2;
  center_y = (laser_border_xy_position.upper_left_y_position + laser_border_xy_position.buttom_right_y_position)/2;
  sprintf_P(process_cmd, PSTR("G38.2 Z%f F900"), current_position[Z_AXIS] + 10);
  process_synchronize_subcommands_now(process_cmd);

  //if (!TEST(axis_known_position, X_AXIS) || !TEST(axis_known_position, Y_AXIS))
  //{
  //  process_synchronize_subcommands_now(PSTR("G28 X Y"));
  //}

  sprintf_P(process_cmd, PSTR("G1 X%f Y%f F3000"), center_x, center_y);
  process_synchronize_subcommands_now(process_cmd);
  process_synchronize_subcommands_now(PSTR("G38.2 Z-20 F900"));
  process_synchronize_subcommands_now("G92 Z0 F900");

  sprintf_P(process_cmd, PSTR("G1 Z%f F900"), laser_focus);
  process_synchronize_subcommands_now(process_cmd);
  sprintf_P(process_cmd, PSTR("G1 X%f Y%f F3000"), laser_border_xy_position.upper_left_x_position, laser_border_xy_position.upper_left_y_position);
  process_synchronize_subcommands_now(process_cmd);
  planner.synchronize();
}

#endif // SPINDLE_LASER_ENABLE
#endif // TARGET_LPC1768
