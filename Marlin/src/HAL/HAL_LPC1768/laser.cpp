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
#include "../../module/motion.h"
#include "../../module/temperature.h"
#include "../../libs/buzzer.h"

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
#include "lcd_parser.h"
#include "filament_ui.h"
#endif

#include "user_execution.h"

#if ENABLED(SPINDLE_LASER_ENABLE)
#include "laser.h"

#if ENABLED(ADVANCED_PAUSE_FEATURE)
#include "../../feature/pause.h"
#endif

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
  spindle_pwm = 0;
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
  UserExecution.cmd_g92_xy(0,0);
  UserExecution.get_remain_command();
  UserExecution.cmd_now_M3(2);
  safe_delay(500);
  UserExecution.cmd_now_M3(0);
  dwin_process.lcd_receive_data_clear();
}

void laser_class::laser_walking_border(void)
{
  dwin_process.show_prepare_block_page(LASER_MACHINE_STATUS_PREPARE_WALKING_BORDER_CH);
  UserExecution.cmd_now_g0_xy(laser_border_xy_position.upper_left_x_position,laser_border_xy_position.upper_left_y_position, 3000);
  UserExecution.cmd_now_M3(2);
  UserExecution.cmd_now_g1_xy(laser_border_xy_position.upper_left_x_position,laser_border_xy_position.buttom_right_y_position, 3000);
  UserExecution.cmd_now_g1_xy(laser_border_xy_position.buttom_right_x_position,laser_border_xy_position.buttom_right_y_position, 3000);
  UserExecution.cmd_now_g1_xy(laser_border_xy_position.buttom_right_x_position,laser_border_xy_position.upper_left_y_position, 3000);
  UserExecution.cmd_now_g1_xy(laser_border_xy_position.upper_left_x_position,laser_border_xy_position.upper_left_y_position, 3000);
  UserExecution.cmd_now_M3(0);
  UserExecution.cmd_now_g0_xy(laser_border_xy_position.upper_left_x_position + 0.01,laser_border_xy_position.upper_left_y_position, 3000);
  planner.synchronize();
  UserExecution.cmd_M300(VOICE_H1, VOICE_T/2);
}

void laser_class::show_laser_prepare_focus_page(void)
{
  uint16_t i = 0;
  dwin_process.pre_percentage = 0;
  dwin_process.send_temperature_percentage(0);
  dwin_process.show_prepare_no_block_page(LASER_MACHINE_STATUS_PREPARE_FOCUS_CH);
  dwin_parser.lcd_stop_status = false;
  dwin_process.send_temperature_percentage(5);

  if(dwin_parser.lcd_stop_status)return;
  UserExecution.cmd_now_g28();
  if(dwin_parser.lcd_stop_status)return;
  dwin_process.send_temperature_percentage(20);

  UserExecution.cmd_now_g1_xy(X_BED_SIZE/2, Y_BED_SIZE/2, 3000);
  UserExecution.cmd_g38_z(-20);
  UserExecution.get_remain_command();
  UserExecution.cmd_user_synchronize();
  if(dwin_parser.lcd_stop_status)return;
  dwin_process.send_temperature_percentage(40);

  UserExecution.cmd_g92_z(0);
  UserExecution.get_remain_command();
  UserExecution.cmd_user_synchronize();
  if(dwin_parser.lcd_stop_status)return;

  UserExecution.cmd_g92_xy(60,60);
  if(dwin_parser.lcd_stop_status)return;

  //add_laser_path hear
  int t_count = -2;
  for(i = 0; (i < sizeof(laser_path)/sizeof(laser_path[0])) && !dwin_parser.lcd_stop_status; i++)
  {
    if(NULL != strstr_P((const char *)laser_path[i], PSTR("G0 X")))
    {
      if(dwin_parser.lcd_stop_status)return;
      while((commands_in_queue >= BUFSIZE) && !dwin_parser.lcd_stop_status)idle();
      if(dwin_parser.lcd_stop_status)return;
      UserExecution.cmd_now_g0_z(laser_focus + LASER_FOCUS_STEP*t_count, 600);
      dwin_process.send_temperature_percentage(70 + 10*t_count);
      t_count ++;
    }
    if(dwin_parser.lcd_stop_status)return;
    while((commands_in_queue >= BUFSIZE) && !dwin_parser.lcd_stop_status)idle();
    if(dwin_parser.lcd_stop_status)return;
    UserExecution.cmd_now_cmd((char*)laser_path[i]);
  }
  if(dwin_parser.lcd_stop_status)return;

  //change the laser_focus_confirm_page
  dwin_process.change_lcd_page(LASER_LINE_CHOICE_PAGE_EN,LASER_LINE_CHOICE_PAGE_CH);
}

void laser_class::show_laser_prepare_engrave_first_page(void)
{
  #define TRIGGERED(S) (READ(S##_PIN) != S##_ENDSTOP_INVERTING)
  #define IS_XYZ_HOME  (TRIGGERED(X_MIN) && TRIGGERED(Y_MIN) && TRIGGERED(Z_MAX))

  dwin_parser.lcd_stop_status = false;

  dwin_process.pre_percentage = 0;
  dwin_process.send_temperature_percentage(0);
  dwin_process.show_prepare_no_block_page(LASER_MACHINE_STATUS_PREPARE_ENGRAVE_CH);
  dwin_process.send_temperature_percentage(10);

  UserExecution.cmd_now_g28();
  if(dwin_parser.lcd_stop_status)return;
  dwin_process.send_temperature_percentage(30);

  UserExecution.cmd_now_g1_xy(X_BED_SIZE/2, Y_BED_SIZE/2,3000);
  UserExecution.cmd_user_synchronize();
  if(dwin_parser.lcd_stop_status)return;
  dwin_process.send_temperature_percentage(40);

  UserExecution.cmd_g38_z(-20);
  UserExecution.get_remain_command();
  UserExecution.cmd_user_synchronize();
  if(dwin_parser.lcd_stop_status)return;
  dwin_process.send_temperature_percentage(80);

  UserExecution.cmd_g92_z(0);
  UserExecution.cmd_g92_xy(0, 0);
  UserExecution.get_remain_command();
  UserExecution.cmd_user_synchronize();
  if(dwin_parser.lcd_stop_status)return;

  UserExecution.cmd_now_g0_z(laser_focus, 600);
  UserExecution.cmd_user_synchronize();
  if(dwin_parser.lcd_stop_status)return;
  dwin_process.send_temperature_percentage(90);

  if(dwin_parser.lcd_stop_status)return;
  dwin_process.change_lcd_page(LASER_AXIS_MOVE_AJUST_PAGE_EN,LASER_AXIS_MOVE_AJUST_PAGE_CH);
  LcdFile.set_current_status(prepare_printing);
}

bool laser_class::is_laser_size_out_range(void)
{
  float length_laser, width_laser;
  length_laser = laser_border_xy_position.buttom_right_x_position - laser_border_xy_position.upper_left_x_position;
  width_laser =  laser_border_xy_position.buttom_right_y_position - laser_border_xy_position.upper_left_y_position;
  SERIAL_PRINTF("LOGICAL_TO_NATIVE(length_laser(%f), LOGICAL_TO_NATIVE(width_laser(%f)",LOGICAL_TO_NATIVE(length_laser,
  X_AXIS), LOGICAL_TO_NATIVE(width_laser, Y_AXIS) );
  if(LOGICAL_TO_NATIVE(length_laser, X_AXIS) <= X_BED_SIZE && LOGICAL_TO_NATIVE(width_laser, Y_AXIS) <= Y_BED_SIZE)
  {
    return false;
  }
  else
  {
    return true;
  }
}

void laser_class::show_laser_prepare_engrave_second_page(pfile_list_t temp)
{
  //if ckeck ok,show prepare_print page
  dwin_process.pre_percentage = 0;
  dwin_process.send_temperature_percentage(0);
  dwin_process.show_prepare_no_block_page(LASER_MACHINE_STATUS_PREPARE_ENGRAVE_CH);
  filament_show.set_heating_status_type(HEAT_PRINT_STATUS);
  dwin_parser.lcd_stop_status = false;

  //check if have recover_file
  if(udisk.job_recover_file_exists())
  {
    dwin_process.show_recovery_print_check_page();
    dwin_process.show_prepare_no_block_page(PRINT_MACHINE_STATUS_PREPARE_PRINT_CH);
    UserExecution.cmd_M1000(false);
    UserExecution.get_remain_command();
    UserExecution.cmd_user_synchronize();
    return;
  }

  //prepare_over, change to print_process page
  dwin_process.show_stop_print_file_page(temp);
  LcdFile.set_current_status(on_printing);

  //open the file,and start engrave
  UserExecution.cmd_M2023(temp->file_name);  //open file and start read file
  UserExecution.cmd_M2024();                 //start engrave
}

void laser_class::show_laser_prepare_from_pause_page(pfile_list_t temp)
{

  dwin_process.set_lcd_temp_show_status(false);
  dwin_process.pre_percentage = 0;
  dwin_process.send_temperature_percentage(dwin_process.pre_percentage);
  dwin_parser.lcd_stop_status = false;

  dwin_process.show_prepare_no_block_page(LASER_MACHINE_STATUS_PREPARE_ENGRAVE_CH);  //change to prepare_print_page
  filament_show.set_heating_status_type(HEAT_PRINT_STATUS);

#if ENABLED(ADVANCED_PAUSE_FEATURE)
  immediately_pause_flag = false;
#endif

  if(HEAT_PRINT_STATUS != filament_show.get_heating_status_type()) return;
  dwin_process.send_temperature_percentage(20);

  //turn off fan
  UserExecution.cmd_now_M106(pause_print_data.fan_speed[0], 0);
  if(HEAT_PRINT_STATUS != filament_show.get_heating_status_type()) return;
  dwin_process.send_temperature_percentage(30);

#if ENABLED(ADVANCED_PAUSE_FEATURE)
  //Set the printed position
  UserExecution.cmd_M2026(pause_print_data.udisk_pos);
  UserExecution.get_remain_command();
  UserExecution.cmd_user_synchronize();
  if(HEAT_PRINT_STATUS != filament_show.get_heating_status_type()) return;
  dwin_process.send_temperature_percentage(50);

  dwin_process.show_stop_print_file_page(temp);
  //change to start print page
  LcdFile.set_current_status(on_printing);
#endif
  UserExecution.cmd_M2024();
}

void laser_class::show_laser_pause_engrave_page(pfile_list_t temp)
{

#if ENABLED(ADVANCED_PAUSE_FEATURE)
  immediately_pause_flag = false;
#endif
  dwin_process.show_prepare_block_page(PRINT_MACHINE_STATUS_PREPARE_STOP_TASK_CH);

#if FAN_COUNT
  COPY(pause_print_data.fan_speed, thermalManager.fan_speed);
#endif

  //stop the print task
  UserExecution.lcd_immediate_execution = true;
  UserExecution.pause_udisk_print();
  UserExecution.lcd_immediate_execution = false;

  //turn off fan
  UserExecution.cmd_now_M106(0, 0);

  //if filament is not over ,go to start page, else go to no filament page
  dwin_process.show_start_print_file_page(temp);
  LcdFile.set_current_status(stop_printing);

  dwin_parser.lcd_stop_status = true;
#if ENABLED(ADVANCED_PAUSE_FEATURE)
  immediately_pause_flag = true;
#endif
}

#endif // SPINDLE_LASER_ENABLE
#endif // TARGET_LPC1768
