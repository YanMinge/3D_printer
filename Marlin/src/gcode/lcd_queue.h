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

/*************Register and Variable addr*****************/
#define	RegAddr_W	0x80
#define	RegAddr_R	0x81
#define	VarAddr_W	0x82		
#define	VarAddr_R	0x83

#define SizeofDatabuf		20
#define MaxSendBUF		256

#define HeadOne   (0x5A)
#define HeadTwo   (0xA5)

#define PageBase	(unsigned long)0x5A010000

//var addr
#define	PageAddr	0x0084
#define MakeIconAddr 0x1000
#define StartIconAddr 0x1002


typedef struct LcdDataBuffer
{
    unsigned char head[2];
    unsigned char len;
    unsigned char command;
    unsigned long addr;
    unsigned long bytelen;
    unsigned short data[32];
    unsigned char reserv[4];
} LDB;

class LCDQUEUE {
  public:
    LCDQUEUE();
    void clear_lcd_data_buf(void);
    void clear_lcd_data_buf1(void);
    void clear_recevie_buf(void);
    void clear_send_data_buf(void);
    void lcd_receive_data(void);
    void lcd_send_data(void);
    void lcd_send_data(const String &, unsigned long, unsigned char = VarAddr_W);
    void lcd_send_data(const char[], unsigned long, unsigned char = VarAddr_W);
    void lcd_send_data(char, unsigned long, unsigned char = VarAddr_W);
    void lcd_send_data(unsigned char*, unsigned long, unsigned char = VarAddr_W);
    void lcd_send_data(int, unsigned long, unsigned char = VarAddr_W);
    void lcd_send_data(float, unsigned long, unsigned char = VarAddr_W);
    void lcd_send_data(unsigned int,unsigned long, unsigned char = VarAddr_W);
    void lcd_send_data(long,unsigned long, unsigned char = VarAddr_W);
    void lcd_send_data(unsigned long,unsigned long, unsigned char = VarAddr_W);
    void icon_update(void);
    void process_lcd_command(void);
    
    LDB recdat;
    LDB snddat;
  private:
    bool HaveLcdCommand;
    CmdType type;
    unsigned char RecNum;
    unsigned char recevie_data_buf[SizeofDatabuf];
    unsigned char send_data_buf[MaxSendBUF];
    unsigned long StartIconCount;
    millis_t NextUpdateTime;
    unsigned char UpdateStatus;
  };

extern LCDQUEUE lcdqueue;
extern void lcd_update(void);

