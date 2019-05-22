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
#include "lcd_queue.h"

#define FileNameLen   32

typedef struct usbfile
{
  char IsDir;
  unsigned long Time;
  struct usbfile* NextFile;
  char UsbFlieName[FileNameLen];
}file_list, *pfile_list;

class LcdFile{
  public:
    LcdFile();
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

    void linklist_create(void);
    void linklist_create2(void);
    void linklist_create3(void);
    void linklist_create4(void);


    pfile_list FileList;
    int PageCount;
    int LastPageFlieCount;
  private:
    char Sort;
};

extern LcdFile DwinLcdFile;
