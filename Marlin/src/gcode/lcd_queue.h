/**   
 * \par Copyright (C), 2018-2019, MakeBlock
 * \brief   cmd neede to seed to lcd
 * @file    lcd_queue.h
 * @author  MakeBlock
 * @version V1.0.0
 * @date    2019/04/29
 * @brief   the lcd commad queue
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

/**
 * queue.h - The G-code command queue, which holds commands before they
 *           go to the parser and dispatcher.
 */

#include "../inc/MarlinConfig.h"
#include "../lcd/dwin/lcd_data.h"

#define VIRTUAL_SIZE 100
#define LCD_SIZE 20

#define VIRTUAL_DEBUG
/**
 * discribe the type of the command
 * language type can be update if you want have more language choices
 */
enum CmdType : unsigned char {
  CMD_CHECK_ERR,
  CMD_CHECK_OK,
  CMD_ERROR,
  CMD_OK,

  CMD_BUTTON_SET_CN,
  CMD_BUTTON_SET_EN,
  CMD_BUTTON_SET_CHECK,
  UK_FONT_BT,
  FONT_CHECK_BT,
  
  MAINPAGE_PRINT_BT,
  FILE_LIST1_DOWN_BT,
  FILE_LIST2_UP_BT,
  FILE_LIST2_DOWN_BT,
  FILE_LIST3_UP_BT,
  FILE_LIST3_DOWN_BT,
  
  LIST1_FILE1_BT,
  LIST1_FILE2_BT,
  LIST1_FILE3_BT,
  LIST1_FILE4_BT,
  LIST2_FILE1_BT,
  LIST2_FILE2_BT,
  LIST2_FILE3_BT,
  LIST2_FILE4_BT,
  LIST3_FILE1_BT,
  LIST3_FILE2_BT,
  LIST3_FILE3_BT,
  LIST3_FILE4_BT,

  FILE_START_STOP_BT,
  LIGHT_BT,
  FILE_RETURN_BT,
  PRINT_RETURN_BT,

  LOAD_FILAMENT_BT,
  LOAD_HEAT_STOP_BT,
  UNLOAD_FILAMENT_BT,
  UNLOAD_HEAT_STOP_BT,

  BUZZER_BT,
  X_STEP_ADD_BT,
  X_STEP_MIN_BT,
  Y_STEP_ADD_BT,
  Y_STEP_MIN_BT,
  Z_STEP_ADD_BT,
  Z_STEP_MIN_BT,
  XYZ_HOME_BT,

};

/**
 * File status structure (FILINFO)
 */
typedef struct {
  uint8_t type;       /* File type */
	uint8_t	fsize;			/* File size */
	char	fname[20];	/* file name*/
} LCDFILINFO;
extern LCDFILINFO lcd_file[20];


/**
 * lcd Command Queue
 * A simple buffer of command strings.
 *
 * Commands are copied into this buffer by the command injectors
 */

extern uint8_t have_serial_cmd;

void lcd_font_init(void);

void clear_lcd_command_queue(void);
void clear_virtaul_command_queue(void);

void get_lcd_commands(void);
CmdType check_lcd_commands(uint8_t * str);

void send_lcd_commands(uint8_t * str);
void send_lcd_commands_CRC16(uint8_t * str,uint8_t times);
int add_lcd_commands_CRC16(uint8_t * str);

void get_virtual_serial_cmd(void);
void send_virtual_serial_cmd(uint8_t * str,uint8_t times);
uint8_t send_virtual_serial(char * str);
CmdType get_vserial_command_type(void);



CmdType get_command_type(void);
void parser_lcd_command(void);

extern void usb_read_test_lcd(void);

