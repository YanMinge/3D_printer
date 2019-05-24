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
#include "lcd_file.h"


/*
 * discribe the type of the command
 * language type can be update if you want have more language choices
 */
enum CmdType : unsigned char {
  CMD_NULL,
  CMD_ERROR,
  CMD_OK,
  CMD_WRITE_REG_OK,
  CMD_WRITE_VAR_OK,
  CMD_READ_REG,
  CMD_READ_VAR,
};

enum Cmd {
  MenuFile=0,SelectFile,PrintFile,AxisMove,SetLanguage,
};

enum print_status {
  out_printing,on_printing,stop_printing,
};


/**
 * lcd Command Queue
 * A simple buffer of command strings.
 *
 * Commands are copied into this buffer by the command injectors
 */

#define DEBUGPRINTF(...) SERIAL_OUT(printf, __VA_ARGS__)

/*************Register and Variable addr*****************/
#define	WRITE_REGISTER_ADDR	0x80
#define	READ_REGISTER_ADDR	0x81
#define	WRITE_VARIABLE_ADDR	0x82		
#define	READ_VARIABLE_ADDR	0x83

#define DATA_BUF_SIZE		  20
#define MAX_SEND_BUF		  256
#define FILE_NUM_MAX      40

#define HEAD_ONE   (0x5A)
#define HEAD_TWO   (0xA5)

#define PAGE_BASE	      (unsigned long)0x5A010000
#define STOP_MESSAGE    (unsigned long)0xCDA3D6B9
#define START_MESSAGE   (unsigned long)0xBFAACABC


//btn addr
#define MENU_BUTTONS    0x1200
#define SELECT_BUTTONS  0x1202
#define PRINT_BUTTONS   0x1204

#define AXIS_MOVE_BTN   0x120E
#define X_AXIS_MOVE_BTN 0x1210
#define Y_AXIS_MOVE_BTN 0x1212
#define Z_AXIS_MOVE_BTN 0x1214
#define HOME_MOVE_BTN   0x1216
#define LANGUAGE_SET_BTN   0x1218

//var addr
#define	PAGE_ADDR	            0x0084
#define MAKEBLOCK_ICON_ADDR   0x1000
#define START_ICON_ADDR       0x1002
#define FILE_ICON_ADDR        0x1004

#define VOICE_ICON_ADDR         0x1020
#define TEMP_HOTEND_ADDR        0X1300
#define TEMP_HOTEND_TARGET_ADDR 0X1302
#define TEMP_BED_ADDR           0X1304
#define TEMP_BED_TARGET_ADDR    0X1306

#define START_STOP_ICON_ADDR    0X1430

#define FILE_TEXT_ADDR_1 0x1500
#define FILE_TEXT_ADDR_5 0x1540
#define FILE_TEXT_ADDR_9 0x1580
#define FILE_TEXT_ADDR_D 0x15C0

/***************not used text button addr*************/
/*****************************************************
#define MainPagetextaddr   1400--1438  //进料退料、返回复位等文字的地址
#define EnglishPagetextaddr   1440-1460  //英文主几面地址
#define 12page ADDR   1470-1480
#define EnTextAddrD  0x15D0  0x1600//language set text addr
******************************************************/

typedef struct lcd_data_buffer
{
    unsigned char head[2];
    unsigned char len;
    unsigned char command;
    unsigned long addr;
    unsigned long bytelen;
    unsigned short data[32];
    unsigned char reserv[4];
} lcd_buffer;

class lcd_queue 
{
public:
  lcd_queue();
  void clear_lcd_data_buf(void);
  void clear_lcd_data_buf1(void);
  void clear_recevie_buf(void);
  void clear_send_data_buf(void);
  void lcd_receive_data(void);
  void lcd_send_data(void);
  void lcd_send_data(const String &, unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void lcd_send_data(const char[], unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void lcd_send_data_clear(unsigned long addr,int len, unsigned char cmd = WRITE_VARIABLE_ADDR);
  void lcd_send_data(char, unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void lcd_send_data(unsigned char*, unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void lcd_send_data(int, unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void lcd_send_data(float, unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void lcd_send_data(unsigned int,unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void lcd_send_data(long,unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void lcd_send_data(unsigned long,unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void icon_update(void);
  void process_lcd_command(void);

  void lcd_send_temperature(int tempbed, int tempbedt, int temphotend, int temphotendt);
  
  void main_button_response(void);
  void nextpage_button_response(void);
  void lastpage_button_response(void);
  void filereturn_button_response(void);

  void next_page_clear(void);
  void last_page_clear(void);
  void send_first_page_data(void);
  void send_next_page_data(void);
  void send_last_page_data(void);

  lcd_buffer recive_data;
  lcd_buffer send_data;
private:
  bool HaveLcdCommand;
  CmdType type;
  unsigned char RecNum;
  unsigned char recevie_data_buf[DATA_BUF_SIZE];
  unsigned char send_data_buf[MAX_SEND_BUF];
  unsigned long StartIconCount;
  millis_t NextUpdateTime;
  unsigned char UpdateStatus;
  int current_page;
  char current_path[FILE_NAME_LEN];
  char select_file_name[FILE_NAME_LEN];
  char current_dir[FILE_NAME_LEN];
  int current_file_index;
  print_status current_status;
  int max_file_index;
  };

extern lcd_queue lcdqueue;
extern void lcd_update(void);
extern void my_lcd_init(void);
