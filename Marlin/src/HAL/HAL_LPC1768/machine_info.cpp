/**
 * \par Copyright (C), 2018-2019, MakeBlock
 * \class   machine_info
 * \brief   management for machine information.
 * @file    machine_info.cpp
 * @author  Mark Yan
 * @version V1.0.0
 * @date    2019/06/15
 * @brief   source code for management machine information.
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
 *    14.  void     machine_info::usb_cable_status_update(void);
 *    15.  void     machine_info::machine_head_type_update(void);
 *    16.  void     machine_info::lcd_print_information_update(void);
 *    17.  void     machine_info::lcd_usb_status_update(void);
 *    18.  void     machine_info::lcd_material_info_update(void);
 *
 * \par History:
 * <pre>
 * `<Author>`         `<Time>`        `<Version>`        `<Descr>`
 * Mark Yan         2019/06/15     1.0.0            Initial function design.
 * </pre>
 *
 */

#ifdef TARGET_LPC1768
#include "../../Marlin.h"

#if ENABLED(FACTORY_MACHINE_INFO)
#include "../../module/printcounter.h"
#include "../../module/temperature.h"
#include "machine_info.h"

#if ENABLED(USE_DWIN_LCD)
#include "lcd_process.h"
#include "filament_ui.h"
#include "lcd_parser.h"
#endif //USE_DWIN_LCD

#if ENABLED(USB_DISK_SUPPORT)
#include "udisk_reader.h"
#include "user_execution.h"
#endif

#ifdef USE_MATERIAL_MOTION_CHECK
#include "material_check.h"
#endif


machine_info MachineInfo;

machine_info::machine_info(void)
{
  usb_cable_connect = false;
  exception_status = true;
}

void machine_info::init(void)
{
  SET_INPUT(USB_CABLE_DETECTION_PIN);
  bool status = READ(USB_CABLE_DETECTION_PIN);
  usb_cable_connect = !status;
  if(usb_cable_connect == true)
  {
#if PIN_EXISTS(LED)
    OUT_WRITE(LED_PIN, true);
#endif
  }
  else
  {
#if PIN_EXISTS(LED)
    OUT_WRITE(LED_PIN, false);
#endif
  }
}

void machine_info::print_uuid_info(void)
{
  for(uint8_t i = 0; i < 6; i++)
  {
    SERIAL_PRINTF("%c", factory_uuid[i]);
  }
  SERIAL_ECHOPGM("-");
  SERIAL_PRINTF("%02x", factory_uuid[6]);
  SERIAL_ECHOPGM("-");
  for(uint8_t i = 7; i < 12; i++)
  {
    SERIAL_PRINTF("%02x", factory_uuid[i]);
  }
  SERIAL_EOL();
}

void machine_info::send_uuid_string(void)
{
  char str_product[7];
  char str_machine[15];
  char str_uuid[21];
  sprintf_P(str_product,"%c%c%c%c%c%c", factory_uuid[0], factory_uuid[1], factory_uuid[2], factory_uuid[3], factory_uuid[4], factory_uuid[5]);
  sprintf_P(str_machine,"-%02x-%02x%02x%02x%02x%02x", factory_uuid[6], factory_uuid[7], factory_uuid[8], factory_uuid[9], factory_uuid[10], factory_uuid[11]);
  strcpy(str_uuid, str_product);
  strcat(str_uuid, str_machine);
  dwin_process.lcd_send_data(str_uuid,PRINT_MACHINE_INFO_ID_ADDR);
}

uint32_t machine_info::get_total_printing_time(void)
{
  printStatistics data = print_job_timer.getStats();
  return data.printTime;
}

uint32_t machine_info::get_total_engraving_time(void)
{
  printStatistics data = print_job_timer.getStats();
  return data.laserTime;
}

void machine_info::print_working_time(void)
{
  char buffer[21];
  duration_t elapsed;
  elapsed.value = get_total_working_time() + millis()/1000;
  elapsed.toString(buffer);
  SERIAL_ECHOPGM(buffer);
  SERIAL_EOL();
}

uint32_t machine_info::get_total_working_time(void)
{
  return total_working_time;
}

void machine_info::reset_total_working_time(void)
{
  total_working_time = 0;
}

void machine_info::set_total_working_time(uint32_t time)
{
  total_working_time = time;
}

bool machine_info::set_uuid_from_str(char* string)
{
  char *p = string;
  uint8_t str_len = strlen(string);
  if(str_len == 2 * (sizeof(factory_uuid) / sizeof(factory_uuid[0])) - 6)
  {
    for(uint8_t i = 0; i < 6; i++)
    {
       factory_uuid[i] = p[i];
    }

    uint8_t factory_uuid_temp[6];
    for(uint8_t i = 6; i < str_len; i++)
    {
      char c = p[i];
      uint8_t dbyte = 0x00;

      //Convert all to uppercase
      if((c >= 'a') && (c <= 'f'))
      {
        c = c - 32;
      }

      if((c >= '0') && (c <= '9'))
      {
        dbyte = c - '0';
      }
      else if((c >= 'A') && (c <= 'F'))
      {
        dbyte = c - 'A' + 10;
      }
      else
      {
        return false;
      }
      if(i%2 == 0)
      {
        factory_uuid_temp[(i-6)/2] = (dbyte << 4) & 0xf0;
      }
      else
      {
        factory_uuid_temp[(i-6)/2] += dbyte & 0x0f;
      }
    }
    for(uint8_t i = 0; i < (sizeof(factory_uuid_temp) / sizeof(factory_uuid_temp[0])); i++)
    {
      factory_uuid[i + 6] = factory_uuid_temp[i];
    }
  }
  return true;
}

void machine_info::set_uuid(uint8_t *uuid)
{
  for(uint8_t i = 0; i < 12; i++)
  {
    factory_uuid[i] = uuid[i];
  }
}

uint8_t* machine_info::get_uuid(void)
{
  return factory_uuid;
}

head_t machine_info::get_head_type(void)
{
  //head_type = HEAD_PRINT;
  return head_type;
}

bool machine_info::get_usb_cable_status(void)
{
  return usb_cable_connect;
}

void machine_info::usb_cable_status_update(void)
{
  static long previous_cable_status_update_time = 0;
  if(millis() - previous_cable_status_update_time > MACHINE_INFORMATION_UPDATE_PERIOD)
  {
    //usb cable access detection
    bool status = READ(USB_CABLE_DETECTION_PIN);
    usb_cable_connect = !status;
	previous_cable_status_update_time = millis();
  }
}

void machine_info::machine_head_type_update(void)
{
  static long previous_info_update_time = 0;
  if(millis() - previous_info_update_time > MACHINE_INFORMATION_UPDATE_PERIOD)
  {
    //head type detection
    int16_t temp = thermalManager.temp_hotend[HOTEND_INDEX].single;
    if(temp > 1000)
    {
      head_type = HEAD_NULL;
    }
    else if(temp < 20)
    {
      head_type = HEAD_LASER;
    }
    else
    {
      head_type = HEAD_PRINT;
    }

    if((get_head_type() == HEAD_NULL) && (exception_status))
    {
      if(LAN_NULL == dwin_process.get_language_type())
      {
        exception_status = false;
        dwin_process.lcd_send_data(PAGE_BASE + START_UP_LANGUAGE_SET_PAGE, PAGE_ADDR);
        dwin_process.set_machine_status(PRINT_MACHINE_STATUS_NO_SET_LAN_NO_HEAD);
      }
      else
      {
        dwin_process.change_lcd_page(EXCEPTION_NO_HEAD_PAGE_EN, EXCEPTION_NO_HEAD_PAGE_CH);
      }
    }
    previous_info_update_time = millis();
  }
}

#if ENABLED(USE_DWIN_LCD)
//This function cannot be run in an interrupt, the LCD may display an exception.
void machine_info::lcd_print_information_update(void)
{
  static long previous_time = 0;
  //Report the temperature every 1 seconds
  if(millis() - previous_time > LCD_PRINT_TIME_UPDATE_PERIOD)
  {
    dwin_process.send_current_temperature(int(thermalManager.degBed()), int(thermalManager.degHotend(HOTEND_INDEX)));
    if(IS_UDISK_PRINTING())
    {
      dwin_process.send_print_time(udisk.get_print_time_dynamic());
    }
    previous_time = millis();
  }
}

void machine_info::lcd_usb_status_update(void)
{
  static bool pre_usb_status;
  bool usb_status = udisk.is_usb_detected();
  if(usb_status != pre_usb_status)
  {
    if(usb_status == true)
    {
      dwin_process.lcd_send_data(USB_INSERT, PRINT_STATUS_BAR_USB_ICON_ADDR);
      udisk.ls(LS_COUNT, "/", ".gcode");
    }
    else
    {
      dwin_process.lcd_send_data(NULL_INSERT, PRINT_STATUS_BAR_USB_ICON_ADDR);
    }
    pre_usb_status = usb_status;
  }

  static bool pre_usb_cable_status;
  bool usb_cable_status = get_usb_cable_status();
  if(usb_cable_status != pre_usb_cable_status)
  {
    if(usb_cable_status == true)
    {
      dwin_process.lcd_send_data(PC_CABLE_INSERT, PRINT_STATUS_BAR_PC_ICON_ADDR);
    }
    else
    {
      dwin_process.lcd_send_data(NULL_INSERT, PRINT_STATUS_BAR_PC_ICON_ADDR);
      if(dwin_process.is_computer_print())
      {
        dwin_process.show_start_up_page();
        dwin_process.set_computer_print_status(false);
      }
    }
    //Lcd status bar update
    pre_usb_cable_status = usb_cable_status;
  }
}

#ifdef USE_MATERIAL_MOTION_CHECK
void machine_info::lcd_material_info_update(void)
{
  bool run_status = print_job_timer.isRunning();

  static bool pre_filamen_runout_status;
  if(MaterialCheck.get_filamen_runout_report_status())
  {
    bool filamen_runout_status = MaterialCheck.is_filamen_runout();
    if(filamen_runout_status != pre_filamen_runout_status)
    {
      static uint8_t count = 0;
      static millis_t check_times = 0;
      if(!count)
      {
        ++count;
        check_times = millis();
      }
      if(count &&(millis() - check_times) > MATERIAL_CHECK_TIME_PERIOD)
      {
        SERIAL_PRINTF("M2034 E%d\r\n", filamen_runout_status);
        pre_filamen_runout_status = filamen_runout_status;
        count = 0;
        if(filamen_runout_status)
        {
          dwin_process.lcd_send_data(FILAMENT_INSERT, PRINT_STATUS_BAR_FILAMENT_ICON_ADDR);
          dwin_parser.machine_error_status = ERROR_FILAMENT_INSERT;
        }
        else
        {
          dwin_process.lcd_send_data(NULL_INSERT, PRINT_STATUS_BAR_FILAMENT_ICON_ADDR);
          dwin_parser.machine_error_status = ERROR_FILAMENT_NO_INSERT;
        }
      }
    }

    if(((run_status == true) && (filamen_runout_status == false)))
    {
      //LCD Pop-ups
      //udisk.pause_udisk_print();
      //dwin_process.show_machine_status(PRINT_MACHINE_STATUS_NO_FILAMENT_CH);
      //dwin_process.change_lcd_page(PRINT_EXCEPTION_SURE_PAGE_EN, PRINT_EXCEPTION_SURE_PAGE_CH);
      //dwin_process.set_machine_status(PRINT_MACHINE_STATUS_NO_FILAMENT_CH);
      //lcd_exception_stop();
    }
  }

  static long previous_material_info_update_time = 0;
  if(millis() - previous_material_info_update_time > MATERIAL_INFORMATION_UPDATE_PERIOD)
  {
    if(MaterialCheck.get_filamen_motion_report_status())
    {
      if((abs(MaterialCheck.get_material_extrusion_in_windows()) > 5) &&
      ((MaterialCheck.get_code_wheel_step_in_windows() * 2) < abs(MaterialCheck.get_material_extrusion_in_windows())))
      {
        SERIAL_PRINTF("M2032 E1\r\n");
        if(run_status == true)
        {
          //LCD Pop-ups
          //if (IS_UDISK_PRINTING())
          //{
          //  udisk.pause_udisk_print();
          //  dwin_process.show_machine_status(PRINT_MACHINE_STATUS_UNKNOW_ERROR_CH);
          //  dwin_process.change_lcd_page(PRINT_EXCEPTION_SURE_PAGE_EN, PRINT_EXCEPTION_SURE_PAGE_CH);
          //}
          //dwin_process.set_machine_status(PRINT_MACHINE_STATUS_UNKNOW_ERROR_CH);
          //lcd_exception_stop();
        }
      }
    }
    previous_material_info_update_time = millis();
  }
}
#endif

#if ENABLED(NEWPANEL)
void machine_info::send_version_string(void)
{
  dwin_process.lcd_send_data(SHORT_BUILD_VERSION,PRINT_MACHINE_INFO_FIRMWARE_ADDR);
  SERIAL_PRINTF("%s\r\n", SHORT_BUILD_VERSION);
}

void machine_info::send_work_time(void)
{
  char buffer[16];
  char hour_gbk[] = {0xD0,0xA1,0xCA,0xB1,0x00};
  uint32_t time;

  time = get_total_working_time();
  int lcd_time = time / 3600;
  sprintf_P(buffer, "%i %s", lcd_time, hour_gbk);
  dwin_process.lcd_send_data(buffer, PRINT_MACHINE_INFO_UPTIME_ADDR);
}

void machine_info::send_print_work_time(void)
{
  char print_time[16];
  char laser_time[16];
  char hour_gbk[] = {0xD0,0xA1,0xCA,0xB1,0x00};
  uint32_t time;
  int lcd_time;

  time = get_total_printing_time();
  lcd_time = time / 3600;
  sprintf_P(print_time, "%i %s", lcd_time, hour_gbk);
  dwin_process.lcd_send_data(print_time, PRINT_MACHINE_INFO_PRINT_TIME_ADDR);

  time = get_total_engraving_time();
  lcd_time = time / 3600;
  sprintf_P(laser_time, "%i %s", lcd_time, hour_gbk);
  dwin_process.lcd_send_data(laser_time, PRINT_MACHINE_INFO_LASER_TIME_ADDR);
}
#endif

#endif // USE_DWIN_LCD
#endif // FACTORY_MACHINE_INFO
#endif // TARGET_LPC1768
