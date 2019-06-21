/**   
 * \par Copyright (C), 2018-2019, MakeBlock
 * \class   machine_info
 * \brief   Header for machine_info.cpp.
 * @file    machine_info.h
 * @author  Mark Yan
 * @version V1.0.0
 * @date    2019/06/15
 * @brief   Header file for management machine information.
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
 * This file is used for management machine information.
 *
 * \par Method List:
 *
 *    1.  void     machine_info::init(void);
 *    2.  void     machine_info::print_uuid_info(void);
 *    3.  void     machine_info::send_uuid_string(void);
 *    4.  uint32_t machine_info::get_total_printing_time(void);
 *    5.  void     machine_info::print_working_time(void);
 *    6.  uint32_t machine_info::get_total_working_time(void);
 *    7.  void     machine_info::reset_total_working_time(void);
 *    8.  void     machine_info::set_total_working_time(uint32_t time);
 *    9.  bool     machine_info::set_uuid_from_str(char* string);
 *    10.  void     machine_info::set_uuid(uint8_t *uuid);
 *    11.  uint8_t* machine_info::get_uuid(void);
 *    12.  head_t   machine_info::get_head_type(void);
 *    13.  bool     machine_info::get_usb_cable_report_status(void);
 *    14.  void     machine_info::machine_information_update(void);
 *    15.  void     machine_info::lcd_print_information_update(void);
 *    16.  void     machine_info::lcd_usb_status_update(void);
 *
 * \par History:
 * <pre>
 * `<Author>`         `<Time>`        `<Version>`        `<Descr>`
 * Mark Yan         2019/06/15     1.0.0            Initial function design.
 * </pre>
 *
 */

#ifndef _MACHINE_INFO_H_
#define _MACHINE_INFO_H_

#ifdef TARGET_LPC1768
#include <stdarg.h>
#include <stdio.h>
#include <Arduino.h>
#include "../../inc/MarlinConfigPre.h"
#include "../../inc/MarlinConfig.h"

#if ENABLED(FACTORY_MACHINE_INFO)

#define MACHINE_INFORMATION_UPDATE_PERIOD 50
#define LCD_PRINT_TIME_UPDATE_PERIOD      1000

enum head_t : uint8_t { HEAD_NULL, HEAD_PRINT, HEAD_LASER };

class machine_info
{
public:

  machine_info(void);
  void init(void);
  void print_uuid_info(void);
  void send_uuid_string(void);
  uint32_t get_total_printing_time(void);
  void print_working_time(void);
  uint32_t get_total_working_time(void);
  void reset_total_working_time(void);
  void set_total_working_time(uint32_t time);
  bool set_uuid_from_str(char* string);
  void set_uuid(uint8_t *uuid);
  uint8_t* get_uuid(void);
  head_t get_head_type(void);
  bool get_usb_cable_status(void);
  void machine_information_update(void);
#if ENABLED(USE_DWIN_LCD)
  void lcd_print_information_update(void);
  void lcd_usb_status_update(void);
#endif

private:
  uint8_t factory_uuid[8];
  head_t head_type;
  uint32_t total_working_time;
  bool usb_cable_connect;
};

extern machine_info MachineInfo;
#endif // FACTORY_MACHINE_INFO
#endif // TARGET_LPC1768
#endif // _MACHINE_UUID_H_
