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

#if ENABLED(USE_DWIN_LCD)
#include "dwin.h"
#include "lcd_parser.h"
#include "lcd_file.h"
#include "lcd_process.h"

#if ENABLED(USB_DISK_SUPPORT)
#include "udisk_reader.h"
#include "user_execution.h"

lcd_parser dwin_parser;
const unsigned long button_addr[] = {0x1200,0x1202,0x1204,0x120E,0x1210,0x1212,0x1214,0x1216,0x1218,0x121A,0};

lcd_parser::lcd_parser(void)
{
  type = CMD_NULL;
  receive_data = 0;
  receive_addr = 0;

  current_path = new char[2];
  current_path[0] = '/';
  current_path[1] = '\0';
}

void lcd_parser::lcd_update(void)
{
  dwin_process.icon_update();
  dwin_parser.parser_lcd_command();
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
        if(MENU_BUTTONS == button_addr[i])
        {
          type = CMD_MENU_FILE;
        }
        else if(SELECT_BUTTONS == button_addr[i])
        {
          type = CMD_SELECT_FILE;
        }
        else if(PRINT_BUTTONS == button_addr[i])
        {
          type = CMD_PRINT_FILE;
        }
        else if(button_addr[i] >= AXIS_MOVE_BTN && button_addr[i] <= HOME_MOVE_BTN)
        {
          type = CMD_AXIS_MOVE;
        }
        else if(LANGUAGE_SET_BTN == button_addr[i])
        {
          type = CMD_SET_LANGUAGE;
        }
        else if(FILEMENT_BTN == button_addr[i])
        {
          type = CMD_FILAMENT;
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
  bool is_command = 0;

  dwin_process.lcd_receive_data();
  is_command = dwin_process.is_have_command();
  dwin_process.reset_command();

  if(is_command)
  {
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

      case CMD_AXIS_MOVE:
        response_move_axis();
        break;

      case CMD_SET_LANGUAGE:
        response_set_language();
        break;

      case CMD_FILAMENT:
        response_filament();
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
    // usb have not insert
    dwin_process.move_usb_hint_page();
    return;
  }

  if(!file_read_status)
  {
    value = udisk.ls(LS_GET_FILE_NAME, current_path, ".gcode");
    if(USB_NOT_DETECTED == value)
    {
      set_file_show_status(false);
      return;
    }
    file_read_status = true;
  }

  set_file_show_status(true);
  LcdFile.set_file_page_info();
  LcdFile.set_current_page(0);
  dwin_process.send_first_page_data();
  dwin_process.lcd_receive_data_clear();
  dwin_process.simage_send_start();
}

void lcd_parser::response_return_button(void)
{
  bool usb_status = udisk.is_usb_detected();

  dwin_process.reset_image_parameters();
  dwin_process.simage_send_end();
  dwin_process.image_send_delay();

  if(!usb_status)
  {
    // usb have not insert
    dwin_process.move_usb_hint_page();
    return;
  }

  if(last_path_fresh())
  {
    uint16_t value;
    SERIAL_PRINTF("enter new path folder...\r\n");

    value = udisk.ls(LS_GET_FILE_NAME, current_path, ".gcode");
    if(USB_NOT_DETECTED == value)
    {
      set_file_show_status(false);
      return;
    }

    LcdFile.set_file_page_info();
    LcdFile.set_current_page(0);
    dwin_process.send_first_page_data();
    dwin_process.lcd_receive_data_clear();
    dwin_process.simage_send_start();
  }
  else
  {
    dwin_process.move_main_page();
    set_file_show_status(false);
  }
}

void lcd_parser::response_next_page_button(void)
{
  bool usb_status = udisk.is_usb_detected();

  if(!usb_status)
  {
    // usb have not insert
    dwin_process.move_usb_hint_page();
    set_file_show_status(false);
    return;
  }
  dwin_process.image_send_delay();
  dwin_process.send_next_page_data();
  dwin_process.reset_image_parameters();
  dwin_process.simage_send_start();
}

void lcd_parser::response_last_page_button(void)
{
  bool usb_status = udisk.is_usb_detected();

  if(!usb_status)
  {
    // usb have not insert
    dwin_process.move_usb_hint_page();
    set_file_show_status(false);
    return;
  }

  dwin_process.image_send_delay();
  dwin_process.send_last_page_data();
  dwin_process.reset_image_parameters();
  dwin_process.simage_send_start();
}




void lcd_parser::response_menu_file(void)
{
  //main page print button
  if(0x09 == receive_data)
  {
    response_print_button();
  }
  // return button
  else if(0x0A == receive_data)
  {
    response_return_button();
  }
  // next file page button
  else if(0x0B == receive_data)
  {
    response_next_page_button();
  }
  //last file page button
  else if(0x0C == receive_data)
  {
    response_last_page_button();
  }
}

void lcd_parser::response_select_file(void)
{
  int max_index = 0;
  pfile_list_t temp = NULL;

  dwin_process.reset_image_parameters();
  dwin_process.simage_send_end();

  max_index = LcdFile.get_file_list_len();
  LcdFile.set_current_status(out_printing);

  if((0x01 == receive_data) || \
     (0x02 == receive_data) || \
     (0x03 == receive_data) || \
     (0x04 == receive_data))
  {
    current_page_index = LcdFile.get_current_index();
    current_page_index += receive_data;
  }
  else if(0x05 == receive_data)
  {
    //return;
  }
  else if(0x06 == receive_data)
  {
    current_page_index = 0;
    response_print_button();
    return;
  }
  else
  {
    return;
  }

  if(current_page_index > max_index)
  {
    return;
  }
  temp = LcdFile.file_list_index((current_page_index));
  select_file(temp);
}

void lcd_parser::response_print_file(void)
{
  print_status status;
  status = LcdFile.get_current_status();

  if(0x01 == receive_data)
  {
  	// first press print button, should initlize
    if(status == out_printing)
    {
      LcdFile.set_current_status(on_printing);
      dwin_process.lcd_send_data(STOP_MESSAGE,START_STOP_ICON_ADDR);
      UserExecution.cmd_M2024();

      //change to prepare show picture
      //dwin_process.lcd_send_data(PAGE_BASE + 13, PAGE_ADDR);
    }
    else if(status == on_printing )
    {
      LcdFile.set_current_status(stop_printing);
      dwin_process.lcd_send_data(START_MESSAGE,START_STOP_ICON_ADDR);
      UserExecution.cmd_M2025();
    }
    else if(status == stop_printing)
    {
      LcdFile.set_current_status(on_printing);
      dwin_process.lcd_send_data(STOP_MESSAGE,START_STOP_ICON_ADDR);
      UserExecution.cmd_M2024();
    }
  }
  else
  {
    return;
  }
}

void lcd_parser::response_move_axis(void)
{
  if(AXIS_MOVE_BTN == receive_addr)
  {
    //move xaxis distance away;
    SERIAL_PRINTF("AXIS_MOVE_BTN...\r\n");
    dwin_process.lcd_send_data(0,X_AXIS_MOVE_BTN);
    dwin_process.lcd_send_data(0,Y_AXIS_MOVE_BTN);
    dwin_process.lcd_send_data(0,Z_AXIS_MOVE_BTN);
    dwin_process.lcd_send_data(PAGE_BASE + 9, PAGE_ADDR);
    UserExecution.cmd_g92(0, 0, 0, 0);
  }
  else if((X_AXIS_MOVE_BTN == receive_addr)|| \
         (Y_AXIS_MOVE_BTN == receive_addr) || \
         (Z_AXIS_MOVE_BTN == receive_addr))
  {
    UserExecution.cmd_g1((float)receive_data/10, (float)receive_data/10, (float)receive_data/10, 0);
  }
  else if(HOME_MOVE_BTN == receive_addr)
  {
    //go home;
    SERIAL_PRINTF("go hmoenow ...\r\n");
    UserExecution.cmd_g28();
  }

}

void lcd_parser::response_set_language(void)
{
  if(0x01 == receive_data)
  {
    dwin_process.lcd_send_data(PAGE_BASE + 1, PAGE_ADDR);
    dwin_process.set_language_type(0x01);
  }
  if(0x02 == receive_data)
  {
    dwin_process.lcd_send_data(PAGE_BASE + 11, PAGE_ADDR);
    dwin_process.set_language_type(0x00);
  }
}

void lcd_parser::response_filament(void)
{
  if(0x04 == receive_data)
  {
    dwin_process.move_main_page();
  }
  else if(0x03 == receive_data)
  {
    UserExecution.cmd_M109_M701();
    dwin_process.set_progress_start_status(true);
    dwin_process.set_progress_temp_page_status(true);
  }
  else if(0x02 == receive_data)
  {
    UserExecution.user_stop();
    dwin_process.set_progress_load_return_status(true);
  }
  else if(0x01 == receive_data)
  {
    UserExecution.user_stop();
  }
}

void lcd_parser::select_file(pfile_list_t temp)
{
  if(temp->file_type == TYPE_FOLDER)
  {
    LcdFile.file_list_clear();
    next_path_fresh(temp->file_name);
    udisk.ls(LS_GET_FILE_NAME, current_path, ".gcode");

    LcdFile.set_file_page_info();
    LcdFile.set_current_page(0);
    dwin_process.send_first_page_data();
    dwin_process.reset_image_parameters();
    dwin_process.simage_send_start();
  }
  else if(temp->file_type == TYPE_MAKEBLOCK_GM)
  {
    dwin_process.lcd_send_data(temp->file_name,(FILE_TEXT_ADDR_D));
    dwin_process.lcd_send_data(PAGE_BASE + 7, PAGE_ADDR);
    dwin_process.set_select_file_num(receive_data);
    dwin_process.limage_send_start();
    dwin_process.send_print_time(2020);
  }
  else if(temp->file_type == TYPE_DEFAULT_FILE)
  {
    dwin_process.lcd_send_data(temp->file_name,(FILE_TEXT_ADDR_D));
    dwin_process.lcd_send_data(PAGE_BASE + 7, PAGE_ADDR);
    UserExecution.cmd_M2023(temp->file_name);
  }
  else if(temp->file_type == TYPE_FIRMWARE)
  {

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

#endif // USB_DISK_SUPPORT
#endif // USE_DWIN_LCD
#endif // TARGET_LPC1768
