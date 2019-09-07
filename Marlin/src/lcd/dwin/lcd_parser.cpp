/**
 * \par Copyright (C), 2018-2019, MakeBlock
 * \class   lcd_file
 * \brief   lcd_file_list.
 * @file    lcd_file.h
 * @author  Akingpan
 * @version V1.0.0
 * @date    2019/05/15
 * @brief   Header file for lcd_file list.
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
 * This file is the fundamental function of lcd_file list.
 *
 * \par Method List:
 *
 *    1.  void    udisk_reader::init(void);
 *
 * \par History:
 * <pre>
 * `<Author>`         `<Time>`        `<Version>`        `<Descr>`
 * Akingpan           2019/05/15       1.0.0              Initial function design.
 * </pre>
 *
 */

#ifdef TARGET_LPC1768
#include "../Marlin.h"
#include "../../module/planner.h"
#include "../../gcode/gcode.h"
#include "../../module/configuration_store.h"

#if ENABLED(USE_DWIN_LCD)
#include "dwin.h"
#include "lcd_parser.h"
#include "lcd_file.h"
#include "lcd_process.h"
#include "filament_ui.h"
#include "../../gcode/queue.h"

#if ENABLED(USB_DISK_SUPPORT)
#include "udisk_reader.h"
#include "user_execution.h"

#if PIN_EXISTS(BEEPER)
  #include "../../libs/buzzer.h"

#if ENABLED(USE_MATERIAL_MOTION_CHECK)
  #include "material_check.h"

#if ENABLED(FACTORY_MACHINE_INFO)
  #include "machine_info.h"
#endif

#if ENABLED(POWER_LOSS_RECOVERY)
#include "../../feature/power_loss_recovery.h"
#endif

#if ENABLED(SPINDLE_LASER_ENABLE)
#include "laser.h"
#endif

#if ENABLED(ADVANCED_PAUSE_FEATURE)
#include "../../feature/pause.h"
#endif

lcd_parser dwin_parser;
const unsigned long button_addr[] =
{0x1200,0x1202,0x1204,0x120e,0x1210,0x1211,0x1212,0x1213,0x1214,0x1215,0x1216,0x1217,0x1218,0x121A,0x121B,0X121C,0};

lcd_parser::lcd_parser(void)
{
  type = CMD_NULL;
  receive_data = 0;
  receive_addr = 0;

  current_path = new char[2];
  current_path[0] = '/';
  current_path[1] = '\0';

  file_list_open_status = false;
  lcd_stop_status = false;
}

void lcd_parser::lcd_update(void)
{
  static bool read_status = true;
  static uint32_t time = millis();

  if(read_status)
  {
    if((millis() - time) > 3000)
    {
      dwin_process.show_start_up_page();
      read_status = false;
#if ENABLED(POWER_LOSS_RECOVERY)
      recovery.check();
#endif
    }
  }

  if((file_list_open_status == true) && (file_read_status == false))
  {
    dwin_process.show_usb_pull_out_page();
  }

  dwin_parser.parser_lcd_command();
  dwin_process.lcd_loop();
}

void lcd_parser::get_command_type(void)
{
  receive_addr = dwin_process.get_receive_addr();
  receive_data = dwin_process.get_receive_data();

  type = dwin_process.get_command_type();
  dwin_process.reset_command_type();

  if(CMD_READ_VAR == type)
  {
    for(int i = 0;button_addr[i] != 0; i++)
    {
      if(receive_addr == button_addr[i])
      {
        if(PRINT_FILE_LIST_BUTTONS == button_addr[i])
        {
          type = CMD_MENU_FILE;
        }
        else if(PRINT_FILE_SELECT_BUTTONS == button_addr[i])
        {
          type = CMD_SELECT_FILE;
        }
        else if(PRINT_FILE_PRINT_BUTTONS == button_addr[i])
        {
          type = CMD_PRINT_FILE;
        }
        else if(((button_addr[i] >= PRINT_SET_PAGE_XYZ_AXIS_BTN_RELEASE) && \
			     (button_addr[i] <= LASER_SET_XY_AXIS_ZERO_BTN)) || \
			    (button_addr[i] == HOME_OFFSET_BTN))
        {
          type = CMD_PRINT_AXIS_MOVE;
        }
        else if(PRINT_SET_PAGE_SET_BTN == button_addr[i])
        {
          type = CMD_PRINT_SETTING;
        }
        else if(PRINT_HOME_PAGE_FILEMENT_BTN == button_addr[i])
        {
          type = CMD_FILAMENT;
        }
        else if(PRINT_MACHINE_STATUS_BTN == button_addr[i])
        {
          type = CMD_PRINT_MACHINE_STATUS;
        }
        else
        {
          type = CMD_NULL;
        }
        return;
      }
    }
  }
}

void lcd_parser::parser_lcd_command(void)
{
  dwin_process.lcd_receive_data();

  if(dwin_process.is_have_command())
  {
    dwin_process.reset_command();
    get_command_type();

    switch (type)
    {
      case CMD_MENU_FILE:
        response_menu_file();
        break;

      case CMD_SELECT_FILE:
        response_select_file();
        break;

      case CMD_PRINT_FILE:
        response_print_file();
        break;

      case CMD_PRINT_AXIS_MOVE:
        response_print_move_axis();
        break;

      case CMD_PRINT_SETTING:
        response_print_set();
        break;

      case CMD_FILAMENT:
        response_filament();
        break;
      case CMD_PRINT_MACHINE_STATUS:
        response_print_machine_status();
        break;

      case CMD_WRITE_REG_OK:case CMD_WRITE_VAR_OK:case CMD_READ_REG:
        type = CMD_NULL;
        break;
      case CMD_NULL:
        break;
      default:
        type = CMD_NULL;
        break;
    }
    dwin_process.clear_recevie_buf();
  }
}

void lcd_parser::response_print_button(void)
{
  uint16_t value;
  bool usb_status = udisk.is_usb_detected();

  if(!usb_status)
  {
    dwin_process.show_usb_pull_out_page();
    return;
  }
  if(!file_read_status)
  {
    if(IS_HEAD_LASER())
    {
      value = udisk.ls(LS_GET_FILE_NAME, current_path, ".lbx");
    }
    else
    {
      value = udisk.ls(LS_GET_FILE_NAME, current_path, ".gcode");
    }
    if(USB_NOT_DETECTED == value || value)
    {
      dwin_process.show_usb_pull_out_page();
      return;
    }
    file_read_status = true;
  }
  file_list_open_status = true;
  LcdFile.set_file_page_info();
  LcdFile.set_current_page(0);
  dwin_process.send_first_page_data();
  dwin_process.lcd_receive_data_clear();
  dwin_process.simage_send_start();
}

void lcd_parser::response_return_button(void)
{
  uint16_t value;
  if(!udisk.is_usb_detected())
  {
    dwin_process.show_usb_pull_out_page();
    return;
  }

  dwin_process.reset_image_send_parameters(); //stop send image
  if(last_path_fresh())
  {
    if(IS_HEAD_LASER())
    {
      value = udisk.ls(LS_GET_FILE_NAME, current_path, ".lbx");
    }
    else
    {
      value = udisk.ls(LS_GET_FILE_NAME, current_path, ".gcode");
    }
    if(USB_NOT_DETECTED == value)
    {
      dwin_process.show_usb_pull_out_page();
      return;
    }
    LcdFile.directory_stack_pop();
    LcdFile.set_file_page_info();
    dwin_process.send_given_page_data();
    dwin_process.lcd_receive_data_clear();
    dwin_process.simage_send_start();
  }
  else
  {
    file_list_open_status = false;
    LcdFile.directory_stack_init();
    dwin_process.show_start_up_page();
  }
}

void lcd_parser::response_next_page_button(void)
{
  if(!udisk.is_usb_detected())
  {
    dwin_process.show_usb_pull_out_page();
    return;
  }
  dwin_process.reset_image_send_parameters();  //stop send simage
  dwin_process.send_next_page_data(); // send next page data
  dwin_process.simage_send_start();  //start send next page simage
}

void lcd_parser::response_last_page_button(void)
{
  if(!udisk.is_usb_detected())
  {
    dwin_process.show_usb_pull_out_page();
    return;
  }
  dwin_process.reset_image_send_parameters(); //stop send simage
  dwin_process.send_last_page_data();         // send last page data
  dwin_process.simage_send_start();           //start send last page simage
}

void lcd_parser::response_upate_firmware_button(void)
{
  void(*resetFunc)(void) = 0; // Declare resetFunc() at address 0
  if(!udisk.is_usb_detected())
  {
    dwin_process.show_usb_pull_out_page();
    return;
  }
  if(!udisk.firmware_upate_file_exists())
  {
    dwin_process.show_sure_block_page(PRINT_MACHINE_STATUS_NO_FIRMWARE_FILE_CH);
    return;
  }
  dwin_process.show_prepare_block_page(PRINT_MACHINE_STATUS_PREPARE_UPDATE_CH);
  if(udisk.is_firmware_same())
  {
    dwin_process.show_sure_block_page(PRINT_MACHINE_STATUS_SAME_FIRMWARE_FILE_CH);
    return;
  }
  resetFunc();                // Jump to address 0
}

void lcd_parser::response_menu_file(void)
{
  //main page print button
  if(0x01 == receive_data)
  {
    response_print_button();
  }
  // return button
  else if(0x02 == receive_data)
  {
    response_return_button();
  }
  // next file page button
  else if(0x03 == receive_data)
  {
    response_next_page_button();
  }
  //last file page button
  else if(0x04 == receive_data)
  {
    response_last_page_button();
  }

  //check the firmware upadate file
  else if(0x05 == receive_data)
  {
    response_upate_firmware_button();
  }
}

void lcd_parser::response_select_file(void)
{
  int max_index = 0;
  uint16_t value;
  pfile_list_t temp = NULL;

  dwin_process.reset_image_send_parameters(); //stop send image
  max_index = LcdFile.get_file_list_len();
  LcdFile.set_current_status(out_printing);

  //if((0x01 == receive_data) || (0x02 == receive_data) || (0x03 == receive_data) || (0x04 == receive_data))
  if((0x01 == receive_data) || (0x02 == receive_data) || (0x03 == receive_data))
  {
    current_page_index = LcdFile.get_current_index();
    current_page_index += receive_data;
    if(current_page_index > max_index) return;
    dwin_process.set_select_file_num(receive_data);
  }
  temp = LcdFile.file_list_index((current_page_index));
  if(temp->file_type == TYPE_FOLDER)
  {
    LcdFile.file_list_clear();
    next_path_fresh(temp->file_name);
    if(IS_HEAD_LASER())
    {
      value = udisk.ls(LS_GET_FILE_NAME, current_path, ".lbx");
    }
    else
    {
      value = udisk.ls(LS_GET_FILE_NAME, current_path, ".gcode");
    }
    if(value)
    {
      dwin_process.show_usb_pull_out_page();
      return;
    }
    LcdFile.directory_stack_push();
    LcdFile.set_file_page_info();
    LcdFile.set_current_page(0);
    dwin_process.send_first_page_data();
    dwin_process.simage_send_start();
  }
  else if(temp->file_type == TYPE_MAKEBLOCK_GM)
  {
    dwin_process.lcd_text_clear(PRINT_FILE_PRINT_TEXT_ADDR, FILE_NAME_LCD_LEN);
    dwin_process.lcd_send_data(temp->file_name,PRINT_FILE_PRINT_TEXT_ADDR);
    udisk.check_gm_file(temp->file_name);
    dwin_process.send_print_time(udisk.get_print_time(temp->file_name));
    CHANGE_PAGE(PRINT, LASER, _FILE_PRINT_STANDARD_START_PAGE_, EN, CH);
    dwin_process.limage_send_start();
  }
  else if(temp->file_type == TYPE_DEFAULT_FILE)
  {
    dwin_process.lcd_send_data(TYPE_DEFAULT_FILE,PRINT_FILE_LIMAGE_ICON_ADDR);
    dwin_process.lcd_text_clear(PRINT_FILE_PRINT_TEXT_ADDR, FILE_NAME_LCD_LEN);
    dwin_process.lcd_send_data(temp->file_name,PRINT_FILE_PRINT_TEXT_ADDR);
    udisk.check_gm_file(temp->file_name);
    dwin_process.send_print_time(udisk.get_print_time(temp->file_name));
    CHANGE_PAGE(PRINT, LASER, _FILE_PRINT_NOSTANDARD_START_PAGE_, EN, CH);
    UserExecution.cmd_M2023(temp->file_name);
  }
}

void lcd_parser::response_print_file(void)
{
  print_status status;
  pfile_list_t temp = NULL;
  temp = LcdFile.file_list_index(dwin_parser.get_current_page_index());
  status = LcdFile.get_current_status();

  if(0x01 == receive_data)
  {
    //first press print button, should show print prepare page
    switch (status)
    {
      case out_printing:
        if(dwin_process.get_limage_status()) return;
        if(IS_HEAD_PRINT())
        {
          dwin_process.show_prepare_print_page(temp);
        }
        else if(IS_HEAD_LASER())
        {
          dwin_process.change_lcd_page(LASER_AXIS_MOVE_AJUST_PAGE_EN,LASER_AXIS_MOVE_AJUST_PAGE_CH);
        }
        break;

      case on_printing:
        if(IS_HEAD_LASER())
        {
          enqueue_and_echo_commands_P("M4 S0");
          dwin_process.show_pause_print_page(temp);
        }
        else if(IS_HEAD_PRINT())
        {
          dwin_process.show_pause_print_page(temp);
        }
        break;

      case stop_printing:
        if(IS_HEAD_LASER())
        {
          enqueue_and_echo_commands_P("M4 S800");
        }
        else if(IS_HEAD_PRINT())
        {
          dwin_process.show_prepare_from_pause_page(temp);
        }
        break;

      default:
        break;
    }
  }
  // return print file.
  else if(0x02 == receive_data)
  {
    if(on_printing == status || stop_printing == status)
    {
      #if ENABLED(ADVANCED_PAUSE_FEATURE)
        immediately_pause_flag = false;
      #endif
      dwin_process.reset_image_send_parameters();
      dwin_process.show_confirm_cancel_page(PRINT_MACHINE_STATUS_CANCEL_PRINT_CH);
      return;
    }
    else if(out_printing == status)
    {
      if(dwin_process.get_limage_status())return;
      lcd_stop_status = false;
      dwin_process.reset_image_send_parameters();
      dwin_process.send_given_page_data();
      dwin_process.simage_send_start();
      if(udisk.job_recover_file_exists())
      {
        udisk.stop_udisk_print();  //stop print from print_status
        lcd_exception_stop();      //stop steppre,fan,heaters, clear command queue, print_job_time,
        udisk.remove_job_recovery_file();
        dwin_process.delete_current_file();
      }
      return;
    }
  }
  // return laser print file
  else if(0x03 == receive_data)
  {
    dwin_process.show_start_print_file_page(temp);
    LcdFile.set_current_status(out_printing);
  }
  // laser print start from walking frame
  else if(0x04 == receive_data)
  {
    dwin_process.show_stop_print_file_page(temp);
    LcdFile.set_current_status(on_printing);
    UserExecution.cmd_M2023(temp->file_name);
    UserExecution.cmd_M2024();
  }
  else return;
}

void lcd_parser::response_print_move_axis(void)
{
  static uint8_t pressed_flag = 0x00;
  static unsigned long pressed_time;
  if((pressed_flag == 0) && (PRINT_SET_PAGE_X_AXIS_MOVE_MIN_BTN == receive_addr))
  {
    pressed_flag |= 0x01;
	pressed_time = millis();
    UserExecution.cmd_g38_x(-2 * X_MAX_POS);
  }
  else if((pressed_flag == 0) && (PRINT_SET_PAGE_X_AXIS_MOVE_ADD_BTN == receive_addr))
  {
    pressed_flag |= 0x02;
	pressed_time = millis();
    UserExecution.cmd_g1_x(X_MAX_POS);
  }
  else if((pressed_flag == 0) && (PRINT_SET_PAGE_Y_AXIS_MOVE_MIN_BTN == receive_addr))
  {
    pressed_flag |= 0x04;
	pressed_time = millis();
    UserExecution.cmd_g38_y(-2 * Y_MAX_POS);
  }
  else if((pressed_flag == 0) && (PRINT_SET_PAGE_Y_AXIS_MOVE_ADD_BTN == receive_addr))
  {
    pressed_flag |= 0x08;
	pressed_time = millis();
    UserExecution.cmd_g1_y(Y_MAX_POS);
  }
  else if((pressed_flag == 0) && (PRINT_SET_PAGE_Z_AXIS_MOVE_MIN_BTN == receive_addr))
  {
    pressed_flag |= 0x10;
	pressed_time = millis();
    UserExecution.cmd_g1_z(Z_MIN_POS);
  }
  else if((pressed_flag == 0) && (PRINT_SET_PAGE_Z_AXIS_MOVE_ADD_BTN == receive_addr))
  {
    pressed_flag |= 0x20;
	pressed_time = millis();
    UserExecution.cmd_g38_z(2 * Z_MAX_POS);
  }
  else if(!(pressed_flag & 0x40) && (PRINT_SET_PAGE_XYZ_AXIS_BTN_RELEASE == receive_addr))
  {
    pressed_flag |= 0x40;
    while(millis() - pressed_time < 250)
    {
      UserExecution.get_next_command();
    }
    clear_command_queue();
    UserExecution.cmd_M410();
    pressed_flag = 0x00;
  }
  else if(PRINT_SET_PAGE_HOME_MOVE_BTN == receive_addr)
  {
    dwin_process.show_xyz_prepare_home_page();
  }
  else if(LASER_SET_XY_AXIS_ZERO_BTN == receive_addr)
  {
    if(0x00 == receive_data)
    {
      Laser.set_current_position_zero();
    }
    else if(0x01 == receive_data)
    {
      Laser.laser_walking_border();
    }
  }
  else if(HOME_OFFSET_BTN == receive_addr)
  {
    if(0x01 == receive_data) //add home offset
    {
      UserExecution.cmd_g1_z(current_position[Z_AXIS] + 0.1);
    }
    else if(0x02 == receive_data) // mins home offset
    {
      UserExecution.cmd_g1_z(current_position[Z_AXIS] - 0.1);
    }
  }
}

void lcd_parser::response_cancel_set_language(void)
{
  if(PRINT_MACHINE_STATUS_NO_SET_LANGUAGE == dwin_process.get_machine_status() || \
    PRINT_MACHINE_STATUS_NO_SET_LAN_NO_HEAD == dwin_process.get_machine_status())
  {
    dwin_process.lcd_send_data(PAGE_BASE + START_UP_LANGUAGE_SET_PAGE, PAGE_ADDR);
  }
  else
  {
    dwin_process.change_lcd_page(PRINT_LANGUAGE_SET_PAGE_EN, PRINT_LANGUAGE_SET_PAGE_CH);
  }
}

void lcd_parser::response_set_language_ch(void)
{
  UserExecution.cmd_M2050(LAN_CHINESE);
  UserExecution.get_remain_command();
  dwin_process.show_start_up_page();
  dwin_process.set_machine_status(PRINT_MACHINE_STATUS_NULL);
  UserExecution.cmd_M500();
}

void lcd_parser::response_set_language_en(void)
{
  dwin_process.set_language_type(LAN_ENGLISH);
  if(PRINT_MACHINE_STATUS_NO_SET_LAN_NO_HEAD == dwin_process.get_machine_status())
  {
    dwin_process.change_lcd_page(EXCEPTION_NO_HEAD_PAGE_EN, EXCEPTION_NO_HEAD_PAGE_CH);
    MachineInfo.set_exception_status(true);
  }
  else if(PRINT_MACHINE_STATUS_NO_SET_LANGUAGE == dwin_process.get_machine_status())
  {
    if(HEAD_PRINT == MachineInfo.get_head_type())
    {
      dwin_process.lcd_send_data(PAGE_BASE + PRINT_HOME_PAGE_EN, PAGE_ADDR);
    }
    else
    {
      dwin_process.lcd_send_data(PAGE_BASE + LASER_HOME_PAGE_EN, PAGE_ADDR);
    }
  }
  else
  {
    if(HEAD_PRINT == MachineInfo.get_head_type())
    {
      dwin_process.show_print_set_page();
    }
    else
    {
      dwin_process.show_laser_set_page();
    }
    return;
  }
  dwin_process.set_machine_status(PRINT_MACHINE_STATUS_NULL);
  UserExecution.cmd_M500();
}

void lcd_parser::response_set_languag(void)
{
  if(HEAD_NULL != MachineInfo.get_head_type())
  {
    dwin_process.change_lcd_page(PRINT_LANGUAGE_SET_PAGE_EN,PRINT_LANGUAGE_SET_PAGE_CH);
  }
}
void lcd_parser::response_set_buzzer(void)
{
  if(buzzer.get_buzzer_switch())
  {
    buzzer.set_buzzer_switch(false);
  }
  else
  {
    buzzer.set_buzzer_switch(true);
  }

  dwin_process.show_machine_set_page();
  UserExecution.cmd_M500();
  UserExecution.cmd_M300(200, 500);
  UserExecution.cmd_M300(300, 500);
  UserExecution.get_remain_command();
}

void lcd_parser::response_print_set(void)
{
  if(0x00== receive_data) //no set language(chinese), return
  {
    response_cancel_set_language();
  }
  else if(0x01 == receive_data) //set_chinese
  {
    response_set_language_ch();
  }
  else if(0x02 == receive_data) //set_english
  {
    response_set_language_en();
  }
  else if(0x03 == receive_data) //enter into language_set_page
  {
    response_set_languag();
  }
  else if(0x04 == receive_data) //set_buzzer
  {
    response_set_buzzer();
  }
  else if(0x05 == receive_data) //return from laser focus choice to  print_set_page
  {
    dwin_process.show_machine_set_page();
  }
  else if(0x06 == receive_data) //enter into laser_focus confirm set page
  {
    dwin_process.show_machine_status_page(LASER_MACHINE_STATUS_FOCUS_CONFIRM_CH,\
        LASER_EXCEPTION_SURE_RETURN_PAGE_EN,LASER_EXCEPTION_SURE_RETURN_PAGE_CH);
  }
  else if(0x07 <= receive_data && receive_data <= 0X0B) //enter into laser_focus confirm set page
  {
    Laser.laser_focus += (receive_data - 9);
    dwin_process.show_sure_block_page(LASER_MACHINE_STATUS_FOCUS_FINISHED_CH);
  }
  else if(0x0C == receive_data)  //enter into machine_info_page
  {
    CHANGE_PAGE(PRINT, LASER, _INFO_PAGE_, EN, CH);
  }
  else if(0x0D == receive_data)  //return to start_up page
  {
    dwin_process.show_start_up_page();
  }
  else if(0x0E == receive_data) //bed leveling
  {
    dwin_process.show_bed_leveling_page();
  }
  else if(0x10 == receive_data) //calibration
  {
    dwin_process.show_calibration_page();
  }
  else if(0x11 == receive_data) //restore the factory setting hint page
  {
    dwin_process.show_confirm_cancel_page(PRINT_MACHINE_STATUS_RESTORE_FACTORY_HINT_CH);
  }
}

void lcd_parser::response_filament(void)
{
  if(0x04 == receive_data)
  {
    dwin_process.show_start_up_page();
  }
  else if(0x03 == receive_data)
  {
    dwin_process.show_load_filament_page();
  }
  else if(0x05 == receive_data)
  {
    dwin_process.show_unload_filament_page();
  }
}

void lcd_parser::response_print_machine_status()
{
  pfile_list_t temp = NULL;

  if(0x01 == receive_data)
  {
    switch(dwin_process.get_machine_status())
    {
      case PRINT_MACHINE_STATUS_NO_USB_DISK_CH:case PRINT_MACHINE_STATUS_LOAD_FILAMENT_SUCCESS_CH:
      case PRINT_MACHINE_STATUS_UNLOAD_SUCCESS_CH:
        CHANGE_PAGE(PRINT, LASER, _HOME_PAGE_, EN, CH);
        dwin_process.set_machine_status(PRINT_MACHINE_STATUS_NULL);
        lcd_exception_stop();
        break;

      case PRINT_MACHINE_STATUS_PRINT_SUCCESS_CH:
        file_list_open_status = false;
        dwin_process.change_lcd_page(PRINT_HOME_PAGE_EN,PRINT_HOME_PAGE_CH);
        dwin_process.set_machine_status(PRINT_MACHINE_STATUS_NULL);
        lcd_exception_stop();
        break;

      case PRINT_MACHINE_STATUS_LOAD_FILAMENT_CH:
        UserExecution.user_stop();
        UserExecution.user_hardware_stop();
        break;

      case PRINT_MACHINE_STATUS_CANCEL_PRINT_CH:   //cancel stop print file, engrave file
        temp = LcdFile.file_list_index(dwin_parser.get_current_page_index());
        dwin_process.show_cancel_stop_print_page(temp);
        break;

      case PRINT_MACHINE_STATUS_NO_FILAMENT_CH:
        if(HEAT_LOAD_STATUS == filament_show.get_heating_status_type())
        {
          dwin_process.change_lcd_page(PRINT_HOME_PAGE_EN,PRINT_HOME_PAGE_CH);
          dwin_process.set_machine_status(PRINT_MACHINE_STATUS_NULL);
          filament_show.set_heating_status_type(HEAT_NULL_STATUS);
        }
        else if(HEAT_PRINT_STATUS == filament_show.get_heating_status_type())
        {
#if ENABLED(ADVANCED_PAUSE_FEATURE)
          immediately_pause_flag = false;
#endif
          if(!MaterialCheck.is_filamen_runout()) //no filament
          {
            dwin_process.show_sure_block_page(PRINT_MACHINE_STATUS_MATERIAL_RUN_OUT_CH);
          }
          else
          {
            temp = LcdFile.file_list_index(dwin_parser.get_current_page_index());
            dwin_process.show_start_print_file_page(temp);
            LcdFile.set_current_status(out_printing);
          }
        }
        else if(HEAT_PRINT_NO_FILAMENT_STATUS == filament_show.get_heating_status_type())
        {

        }
        break;

      case PRINT_MACHINE_STATUS_UNKNOW_ERROR_CH:
        dwin_process.change_lcd_page(PRINT_HOME_PAGE_EN,PRINT_HOME_PAGE_CH);
        dwin_process.set_machine_status(PRINT_MACHINE_STATUS_NULL);
        break;

      //cancel print the recovery file,goto home page
      case PRINT_MACHINE_STATUS_PRINT_CONTINUE_CH:
        udisk.remove_job_recovery_file();
        dwin_process.delete_current_file();
        dwin_process.show_start_up_page();
        break;

      //cancel laser_focus set,goto to set page
      case LASER_MACHINE_STATUS_FOCUS_CONFIRM_CH:
        dwin_parser.lcd_stop_status = false;
      case LASER_MACHINE_STATUS_FOCUS_FINISHED_CH:
      case PRINT_MACHINE_STATUS_LEVELING_OK_CH:
      case PRINT_MACHINE_STATUS_CALIBRATION_OK_CH:
      case PRINT_MACHINE_STATUS_RESTORE_FACOTORY_CH:
      case PRINT_MACHINE_STATUS_XYZ_HOME_OK_CH:
        dwin_process.show_machine_set_page();
        break;

      case PRINT_MACHINE_STATUS_PREPARE_PRINT_CH:  //stop print file prepare heating
        if(dwin_process.get_limage_status())return;
        temp = LcdFile.file_list_index(dwin_parser.get_current_page_index());
        filament_show.set_heating_status_type(HEAT_NULL_STATUS);
        UserExecution.user_stop();
        lcd_exception_stop();
        if(stop_printing == LcdFile.get_current_status())
        {
          UserExecution.cmd_quick_stop(true);
          dwin_process.show_start_print_file_page(temp);
        }
        else if(out_printing == LcdFile.get_current_status())
        {
          if(!udisk.job_recover_file_exists())
          {
            lcd_stop_status = true;
            filament_show.set_print_after_heat_status(false);
            UserExecution.cmd_now_M2524();
            dwin_process.show_start_print_file_page(temp);
            UserExecution.cmd_quick_stop(true);
          }
          else
          {
            UserExecution.cmd_quick_stop(true);
            dwin_process.show_start_print_file_page(temp);
          }
        }
        break;

      case PRINT_MACHINE_STATUS_PREPARE_LOAD_CH: //stop load filament prepare heating
        if(HEAT_LOAD_STATUS == filament_show.get_heating_status_type() || \
          HEAT_UNLOAD_STATUS == filament_show.get_heating_status_type())
        {
          UserExecution.user_stop();
          dwin_process.set_lcd_temp_show_status(false);
          filament_show.set_heating_status_type(HEAT_NULL_STATUS);
          dwin_process.change_lcd_page(PRINT_HOME_PAGE_EN,PRINT_HOME_PAGE_CH);
          lcd_exception_stop();
        }
        break;

      case PRINT_MACHINE_STATUS_PREPARE_UNLOAD_CH: //stop unload filament prepare heating
        UserExecution.user_stop();
        filament_show.set_heating_status_type(HEAT_NULL_STATUS);
        dwin_process.change_lcd_page(PRINT_HOME_PAGE_EN,PRINT_HOME_PAGE_CH);
        lcd_exception_stop();
        break;

      case LASER_MACHINE_STATUS_PREPARE_FOCUS_CH:
        UserExecution.user_stop();
        UserExecution.cmd_quick_stop(true);
        lcd_exception_stop();
        UserExecution.cmd_now_M3(0);
        dwin_parser.lcd_stop_status = true;
        dwin_process.show_machine_status_page(LASER_MACHINE_STATUS_FOCUS_CONFIRM_CH,\
        LASER_EXCEPTION_SURE_RETURN_PAGE_EN,LASER_EXCEPTION_SURE_RETURN_PAGE_CH);
        break;

      case LASER_MACHINE_STATUS_ENGRAVE_FINISHED_EN:
        file_list_open_status = false;
        dwin_process.change_lcd_page(LASER_HOME_PAGE_EN,LASER_HOME_PAGE_CH);
        break;

      case PRINT_MACHINE_STATUS_SAME_FIRMWARE_FILE_CH:
      case PRINT_MACHINE_STATUS_NO_FIRMWARE_FILE_CH:
        CHANGE_PAGE(PRINT, LASER, _INFO_PAGE_, EN, CH);
        break;

      case PRINT_MACHINE_STATUS_NO_UPDATE_FILE_CH:
        CHANGE_PAGE(PRINT, LASER, _HOME_PAGE_, EN, CH);
        dwin_process.set_machine_status(PRINT_MACHINE_STATUS_NULL);
        break;

      case PRINT_MACHINE_STATUS_PREPARE_CALIBRATION_CH:
        dwin_process.show_save_calibration_data_page();
        break;

      case PRINT_MACHINE_STATUS_TASK_CANCEL_CH:
#if ENABLED(ADVANCED_PAUSE_FEATURE)
        immediately_pause_flag = false;
#endif
        temp = LcdFile.file_list_index(dwin_parser.get_current_page_index());
        filament_show.set_heating_status_type(HEAT_NULL_STATUS);
        LcdFile.set_current_status(out_printing);
        dwin_process.show_start_print_file_page(temp);
        break;

      case PRINT_MACHINE_STATUS_RESTORE_FACTORY_HINT_CH:  //cancel restore factory mode
        dwin_process.show_machine_set_page();
        break;

      default:
        break;
    }
  }
  else if(0x03 == receive_data)
  {
    switch(dwin_process.get_machine_status())
    {
      case PRINT_MACHINE_STATUS_LOAD_FILAMENT_CH:
        UserExecution.user_stop();
        UserExecution.user_hardware_stop();
        filament_show.set_heating_status_type(HEAT_NULL_STATUS);
        dwin_process.change_lcd_page(PRINT_HOME_PAGE_EN,PRINT_HOME_PAGE_CH);
        lcd_exception_stop();
        break;

      case PRINT_MACHINE_STATUS_CANCEL_PRINT_CH:  //confirm stop print file
        dwin_process.show_confirm_stop_print_page();
        break;

      //confirm print the recovery file, goto the machine print page
      case PRINT_MACHINE_STATUS_PRINT_CONTINUE_CH:
        dwin_process.show_prepare_print_page(temp);
        break;

      //confirm laser_focus set,goto to fucus_prepare page
      case LASER_MACHINE_STATUS_FOCUS_CONFIRM_CH:
        Laser.show_laser_prepare_focus_page();
        break;

      case PRINT_MACHINE_STATUS_PREPARE_QUIT_TASK_CH:
        dwin_process.send_given_page_data();
        dwin_process.simage_send_start();
        if(udisk.job_recover_file_exists())
        {
          udisk.remove_job_recovery_file();
          dwin_process.delete_current_file();
        }
        break;

      case PRINT_MACHINE_STATUS_RESTORE_FACTORY_HINT_CH:  //confirm restore factory mode
        dwin_process.show_restore_factory_page();
        break;

      default:
        break;
    }
  }
}

void lcd_parser::next_path_fresh(char* name)
{
  if(strcmp(current_path,"/") == 0)
  {
    char *temp_path = new char[strlen(name) + 2];
    temp_path[0]='/';
    strcpy(temp_path+1,name);
    delete [] current_path;
    current_path = temp_path;
  }
  else if(strcmp(current_path,"/") > 0)
  {
    char * temp_path = new char[strlen(current_path) + strlen(name) + 2];
    strcpy(temp_path,current_path);
    temp_path[strlen(current_path)] = '/';
    strcpy(temp_path+strlen(current_path)+1,name);
    delete [] current_path;
    current_path = temp_path;
  }
  DEBUGPRINTF("current_path = %s\r\n",current_path);
}

bool lcd_parser::last_path_fresh(void)
{
  if(strcmp(current_path,"/") == 0)
  {
    DEBUGPRINTF("current_path = %s\r\n",current_path);
    return false;
  }
  else if(strcmp(current_path,"/") > 0)
  {
    int len = 0;
    len = strlen(current_path);
    char *temp_path = new char[len + 1];
    for(int i = len; i >= 0; i--)
    {
      if(current_path[i] == '/')
      {
        if(i == 0)
        {
          temp_path[0] = '/';
          temp_path[1] = '\0';
        }
        else
        {
          current_path[i] = '\0';
          strcpy(temp_path,current_path);
        }
        delete [] current_path;
        current_path = temp_path;
        DEBUGPRINTF("current_path = %s\r\n",current_path);
        return true;
      }
    }
    return false;
  }
  else
  {
    DEBUGPRINTF("temp_path = %s wrong\r\n",current_path);
    return false;
  }
}

void lcd_parser::refresh_current_path(void)
{
  delete [] current_path;
  current_path = new char[2];
  current_path[0] = '/';
  current_path[1] = '\0';
}

void lcd_parser::malloc_current_path(uint16_t len)
{
  delete [] current_path;
  current_path = new char[len + 1];
  current_path[len] = '\0';
}

#endif // USE_MATERIAL_MOTION_CHECK
#endif // BEEPER
#endif // USB_DISK_SUPPORT
#endif // USE_DWIN_LCD
#endif // TARGET_LPC1768
