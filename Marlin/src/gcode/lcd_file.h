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

#define FILE_NAME_LEN   32
#pragma once

/**
 * queue.h - The G-code command queue, which holds commands before they
 *           go to the parser and dispatcher.
 */

#include "stdio.h"
#include "../inc/MarlinConfig.h"
#include "lcd_queue.h"

//#define FILE_NAME_LEN   32
#define GET_MAX_INDEX(X) (4*X.page_count + X.last_page_file_count)

typedef struct usb_file
{
  bool IsDir;
  unsigned long Time;
  struct usb_file* next_file;
  char file_name[FILE_NAME_LEN];
}file_list, *pfile_list;

class lcd_file
{
public:

  lcd_file(void);
  virtual ~lcd_file(void) { }

  void file_list_init(void);
  void file_list_insert(pfile_list m);
  void file_list_insert_tail(pfile_list m);
  void file_list_insert_tail(char isdir,char filename[]);
  void file_list_del(void);
  void file_list_clear(void);
  void get_file_page_count(void);
  bool file_list_isempty(void);
  int file_list_len(void);
  pfile_list file_list_index(int index);
  void linklist_create2(void);

  int page_count;
  int last_page_file_count;

private:
  char sort;
  //char select_file_name[FILE_NAME_LEN];
  pfile_list FileList;
};

extern lcd_file LcdFile;
