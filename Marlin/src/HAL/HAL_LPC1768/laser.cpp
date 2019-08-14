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
#include "laser.h"

#include HAL_PATH(.., HAL.h)
#include "../../Marlin.h"

#if ENABLED(USB_DISK_SUPPORT)
#include "udisk_reader.h"
#endif

#if ENABLED(FACTORY_MACHINE_INFO)
#include "machine_info.h"
#endif

#if ENABLED(SPINDLE_LASER_ENABLE)

laser_class Laser;

laser_class::laser_class(void)
{
  laser_power = 0;
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

#endif // SPINDLE_LASER_ENABLE
#endif // TARGET_LPC1768
