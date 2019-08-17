/**
 * \par Copyright (C), 2018-2019, MakeBlock
 * \class   laser
 * \brief   The driver for laser head.
 * @file    laser.h
 * @author  Mark Yan
 * @version V1.0.0
 * @date    2019/08/14
 * @brief   Header file for laser head driver.
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

#ifndef _LASER_H_
#define _LASER_H_

#ifdef TARGET_LPC1768
#include <stdarg.h>
#include <stdio.h>
#include <Arduino.h>
#include "../../inc/MarlinConfigPre.h"
#include "../../inc/MarlinConfig.h"

#if ENABLED(SPINDLE_LASER_ENABLE)

typedef struct{
  float upper_left_x_position;
  float upper_left_y_position;
  float buttom_right_x_position;
  float buttom_right_y_position;
}laser_border_xy_position_t;

class laser_class
{
public:
  laser_class(void);
  uint16_t get_laser_power(void);
  void set_laser_power(uint16_t power);
  void delay_for_power_up(void);
  void delay_for_power_down(void);
  void set_laser_frequency(uint16_t laser_frequency);
  void set_laser_ocr(uint16_t power);
  void reset(void);
  void update_laser_power(void);
  void set_current_position_zero(void);
  void process_synchronize_subcommands_now(PGM_P pgcode);
  void laser_walking_border(void);
  void show_laser_prepare_focus_page(void);
  void laser_before_print(void);
  laser_border_xy_position_t laser_border_xy_position;
  float laser_focus;
  bool synchronize_status;
  uint16_t spindle_pwm;
private:
  uint16_t laser_power;
};

extern laser_class Laser;
#endif // SPINDLE_LASER_ENABLE
#endif // TARGET_LPC1768
#endif // _LASER_H_
