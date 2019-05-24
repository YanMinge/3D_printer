/**   
 * \par Copyright (C), 2018-2019, MakeBlock
 * \class   lcd_queue
 * \brief   Virtual serial port driver.
 * @file    lcd_queue.cpp
 * @author  MakeBlock
 * @version V1.0.0
 * @date    2019/04/29
 * @brief   process the data lcd send to host

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

/**
 * queue.cpp - The G-code command queue
 */

#include "lcd_queue.h"
#include "lcd_file.h"
#include "../lcd/dwin/lcd_data.h"
#include "gcode.h"


#include "../lcd/ultralcd.h"
#include "../sd/cardreader.h"
#include "../module/planner.h"
#include "../module/temperature.h"
#include "../Marlin.h"

#if ENABLED(PRINTER_EVENT_LEDS)
  #include "../feature/leds/printer_event_leds.h"
#endif

#if ENABLED(POWER_LOSS_RECOVERY)
  #include "../feature/power_loss_recovery.h"
#endif

lcd_file LcdFile;

lcd_file::lcd_file(){}

void lcd_file::file_list_init(void)
{
  pfile_list m;
  m =(pfile_list) new char[sizeof(file_list)];
  memset(m,0,sizeof(file_list));
  if(NULL == m)
  {
    //add err code
    return;
  }
  m->next_file = NULL;
  FileList = m;
  return;
}

//insert to head
void lcd_file::file_list_insert(pfile_list m)
{
  m->next_file = FileList->next_file;
  FileList->next_file = m;
}

//insert to tail
void lcd_file::file_list_insert_tail(pfile_list m)
{
  pfile_list t;
  t = FileList;
  while( t->next_file != NULL)
  {
    t = t->next_file;
  }
  t->next_file = m;
}

//insert to tail
void lcd_file::file_list_insert_tail(char isdir,char filename[])
{
  pfile_list t,m;
  t = FileList;
  m = (pfile_list) new char[sizeof(file_list)];
  m->next_file = NULL;
  m->IsDir = isdir;
  strcpy(m->file_name,filename);
  while( t->next_file != NULL)
  {
    t = t->next_file;
  }
  t->next_file = m;
}


void lcd_file::file_list_del(void)
{
  pfile_list t;
  t = FileList->next_file;
  FileList->next_file = t->next_file;
  delete (t);
  t = NULL;
}

void lcd_file::file_list_clear(void)
{
  pfile_list t,temp;
  t = FileList->next_file;
  while( t != NULL)
  {
    temp = t;
    t = t->next_file;
    delete (temp);
  }
  FileList->next_file = NULL;
}


pfile_list lcd_file::file_list_index(int index)
{
  int i = 0;
  pfile_list t = NULL;
  t = FileList;
  while( (t != NULL) && (i != index))
  {
    i++;
    t = t->next_file;
  }
  if(i == index)
  {
    return t;
  }
  else
  {
    return NULL;
  }
}

bool lcd_file::file_list_isempty(void)
{
  if(FileList->next_file == NULL)
  {
    return true;
  }
  else
  {
    return false;
  }
}

int lcd_file::file_list_len(void)
{
  int i = 0;
  pfile_list head = FileList->next_file;
  for(; head; head = head->next_file,i++);
  return i;
}

void lcd_file::get_file_page_count(void)
{
  if(file_list_isempty())
  {
    page_count = 0;
    last_page_file_count = 0;
    return;
  }
  int file_len;
  file_len = file_list_len();
  page_count = file_len / 4;
  if(file_len % 4)
  {
    page_count += 1;
  }
  last_page_file_count = file_len % 4;
  SERIAL_PRINTF("page count = %d.\r\n",page_count);
  SERIAL_PRINTF("last page file count = %d.\r\n",last_page_file_count);
}

void lcd_file::linklist_create2(void)
{
  int i;
  pfile_list m;
  for(i = 0; i < 34; i++)
  {
    m = (pfile_list) new char[(sizeof(file_list))];	
    memset(m,0,sizeof(file_list));
    m->next_file = NULL;
    if(NULL==m)
    {
      return;
    }
    if(i == 0)
    {
      strcpy(m->file_name, "文件夹一");
      m->IsDir = 1;
    }
    if(i == 1)
    {
      strcpy(m->file_name, "文件夹二");
      m->IsDir = 1;
    }
    if((i == 2) || (i == 3))
    {
      strcpy(m->file_name, "123456789.gcode");
    }
    if((i >= 4)&& (i < 8))
    {
      strcpy(m->file_name, "你好世界.gcode");
    }
    if((i >= 8)&& (i < 12))
    {
      strcpy(m->file_name, "创客工厂.gcode");
      if(m->IsDir == 1)
        SERIAL_PRINTF("THIS IS NOT DIR");
    }
    if((i >= 12)&& (i < 16))
    {
      strcpy(m->file_name, "晨风.gcode");
    }
    if((i >= 16)&& (i < 20))
    {
      strcpy(m->file_name, "makeblock.gcode");
    }
    if((i >= 20)&& (i < 24))
    {
      strcpy(m->file_name, "maker.gcode");
    }
    if((i >= 24)&& (i < 28))
    {
      strcpy(m->file_name, "MakeBlock.gcode");
    }
    if((i >= 28)&& (i < 32))
    {
      strcpy(m->file_name, "Hello风.gcode");
    }
    if((i >= 32)&& (i < 34))
    {
      strcpy(m->file_name, "hello民革.gcode");
      if(m->IsDir == 1)
        SERIAL_PRINTF("THIS IS NOT DIR");
    }
    file_list_insert_tail(m);
  }
}
