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
const unsigned long button_addr[] = {0x1200,0x1202,0x1204,0x120E,0x1210,0x1212,0x1214,0x1216,0x1218, 0};

lcd_parser::lcd_parser(void)
{
  type = CMD_NULL;
  receive_data = 0;
  receive_addr = 0;
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

void lcd_parser::response_menu_file(void)
{
  //main page print button
  if(0x09 == receive_data)
  {
    dwin_process.lcd_send_temperature(172,256,93,48);
    udisk.ls(LS_GET_FILE_NAME, "", ".gcode");
    LcdFile.set_file_page_info();
    LcdFile.set_current_page(0);
    dwin_process.send_first_page_data();
    dwin_process.lcd_receive_data_clear();
    dwin_process.set_loop_status(true);
    dwin_process.set_file_status(true);
    //dwin_process.image_read_test();
  }
  // return button
  else if(0x0A == receive_data)
  {
    dwin_process.lcd_send_data(PAGE_BASE +1, PAGE_ADDR);
    dwin_process.set_loop_status(false);
    dwin_process.reset_image_parameters();
    dwin_process.set_file_status(false);
  }
  // next file page button
  else if(0x0B == receive_data)
  {
    dwin_process.send_next_page_data();
    dwin_process.reset_image_parameters();
    dwin_process.set_loop_status(true);
    dwin_process.set_file_status(true);
  }
  //last file page button
  else if(0x0C == receive_data)
  {
    dwin_process.send_last_page_data();
    dwin_process.reset_image_parameters();
    dwin_process.set_loop_status(true);
    dwin_process.set_file_status(true);
  }
}

void lcd_parser::response_select_file(void)
{
  int index = 0;
  int max_index = 0;
  char file_name[FILE_NAME_LEN];

  pfile_list_t temp = NULL;
  max_index = LcdFile.get_file_list_len();
  memset(file_name,0,FILE_NAME_LEN);
  LcdFile.set_current_status(out_printing);
  index = LcdFile.get_current_index();
  if((0x01 == receive_data) || \
     (0x02 == receive_data) || \
     (0x03 == receive_data) || \
     (0x04 == receive_data))
  {
    index += receive_data;
  }
  else
  {
    return;
  }

  if(index > max_index)
  {
    return;
  }
  temp = LcdFile.file_list_index((index));
  strcpy(file_name,temp->file_name);
  if(temp->IsDir)
  {
    LcdFile.file_list_clear();
    LcdFile.list_test();
    LcdFile.set_file_page_info();
    LcdFile.set_current_page(0);
    dwin_process.send_first_page_data();
  }
  else
  {
    dwin_process.lcd_send_data(file_name,(FILE_TEXT_ADDR_D));
    dwin_process.lcd_send_data(PAGE_BASE + 7, PAGE_ADDR);
    UserExecution.cmd_M2023(file_name);
  }
}

void lcd_parser::response_print_file(void)
{
  print_status status;
  status = LcdFile.get_current_status();

  if(0x01 == receive_data)
  {
    if(status == out_printing)
    {
      LcdFile.set_current_status(on_printing);
      dwin_process.lcd_send_data(STOP_MESSAGE,START_STOP_ICON_ADDR);
      UserExecution.cmd_M2024();
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

#endif // USB_DISK_SUPPORT
#endif // USE_DWIN_LCD
#endif // TARGET_LPC1768
