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

LcdFile DwinLcdFile;

LcdFile::LcdFile()
{
  PageCount = 0;
}

void LcdFile::file_list_init(void)
{
  pfile_list L;
  L =(pfile_list) new char[sizeof(file_list)];
  memset(L,0,sizeof(file_list));
  if(NULL == L)
  {
    return;
  }
  L->NextFile = NULL;
  SERIAL_PRINTF("file list init ok.\r\n");
  FileList = L;
  return;
}

//insert to head
void LcdFile::file_list_insert(pfile_list m)
{
  m->NextFile = FileList->NextFile;
  FileList->NextFile = m;
}

//insert to tail
void LcdFile::file_list_insert_tail(pfile_list m)
{
  pfile_list t;
  t = FileList;
  while( t->NextFile != NULL)
      t = t->NextFile;
  t->NextFile = m;
}

//insert to tail
void LcdFile::file_list_insert_tail(char isdir,char filename[])
{
  pfile_list t,m;
  t = FileList;
  m = (pfile_list) new char[sizeof(file_list)];
  m->NextFile = NULL;
  m->IsDir = isdir;
  strcpy(m->UsbFlieName,filename);
  while( t->NextFile != NULL)
      t = t->NextFile;
  t->NextFile = m;
}


void LcdFile::file_list_del(void)
{
  pfile_list t;
  t = FileList->NextFile;
  FileList->NextFile = t->NextFile;
  delete (t);
  t = NULL;
}

void LcdFile::file_list_clear(void)
{
  pfile_list t,temp;
  t = FileList->NextFile;
  int i=0;
  while( t != NULL)
  {
    temp = t;
    t = t->NextFile;
    delete (temp);
    SERIAL_PRINTF("link clear file %d.\r\n",i++);
    SERIAL_PRINTF("the t is  %d.\r\n",t);
  }
  FileList->NextFile = NULL;
}


pfile_list LcdFile::file_list_index(int index)
{
  int i = 0;
  pfile_list t = NULL;
  t = FileList;
  while( (t != NULL) && (i != index))
  {
    i++;
    t = t->NextFile;
  }
  if(i == index)
    return t;
  else
    return NULL;
}

bool LcdFile::file_list_isempty(void)
{
  if(FileList->NextFile == NULL)
    return true;
  else
    return false;
}

int LcdFile::file_list_len(void)
{
  int i = 0;
  pfile_list head = FileList->NextFile;
  for(; head; head = head->NextFile,i++);
  return i;
}

void LcdFile::get_file_page_count(void)
{
  if(file_list_isempty())
  {
    PageCount = 0;
    LastPageFlieCount = 0;
    return;
  }
  int file_len;
  file_len = file_list_len();
  PageCount = file_len / 4;
  if(file_len % 4)
  {
    PageCount += 1;
  }
  LastPageFlieCount = file_len % 4;
  SERIAL_PRINTF("page count = %d.\r\n",PageCount);
  SERIAL_PRINTF("last page file count = %d.\r\n",LastPageFlieCount);
}

void LcdFile::linklist_create(void)
{
  int i;
  pfile_list m;
  
  for(i = 0; i < 18; i++)
  {
    m = (pfile_list) new char[(sizeof(file_list))];	
    memset(m,0,sizeof(file_list));
    m->NextFile = NULL;
    if(NULL==m)
    {
      SERIAL_PRINTF("link create failed.\r\n");
      return;
    }
    if(m->NextFile == NULL)
    {
      SERIAL_PRINTF(" m_nextfile = NULL  %d.\r\n",(m->NextFile));
    }
    strcpy(m->UsbFlieName, "123456创客工厂");
    SERIAL_PRINTF("before insert m_nextfile  %d.\r\n",(m->NextFile));
    file_list_insert(m);
    SERIAL_PRINTF("link create file 1   %d.\r\n",i);
    SERIAL_PRINTF("after insert m_nextfile  %d.\r\n",(m->NextFile));
  }
}

void LcdFile::linklist_create2(void)
{
  int i;
  pfile_list m;
  for(i = 0; i < 34; i++)
  {
    m = (pfile_list) new char[(sizeof(file_list))];	
    memset(m,0,sizeof(file_list));
    m->NextFile = NULL;
    if(NULL==m)
    {
      return;
    }
    if(i == 0)
    {
      strcpy(m->UsbFlieName, "文件夹一");
      m->IsDir = 1;
    }
    if(i == 1)
    {
      strcpy(m->UsbFlieName, "文件夹二");
      m->IsDir = 1;
    }
    if((i == 2) || (i == 3))
    {
      strcpy(m->UsbFlieName, "123456789.gcode");
    }
    if((i >= 4)&& (i < 8))
    {
      strcpy(m->UsbFlieName, "你好世界.gcode");
    }
    if((i >= 8)&& (i < 12))
    {
      strcpy(m->UsbFlieName, "创客工厂.gcode");
      if(m->IsDir == 1)
        SERIAL_PRINTF("THIS IS NOT DIR");
    }
    if((i >= 12)&& (i < 16))
    {
      strcpy(m->UsbFlieName, "晨风.gcode");
    }
    if((i >= 16)&& (i < 20))
    {
      strcpy(m->UsbFlieName, "makeblock.gcode");
    }
    if((i >= 20)&& (i < 24))
    {
      strcpy(m->UsbFlieName, "maker.gcode");
    }
    if((i >= 24)&& (i < 28))
    {
      strcpy(m->UsbFlieName, "MakeBlock.gcode");
    }
    if((i >= 28)&& (i < 32))
    {
      strcpy(m->UsbFlieName, "Hello风.gcode");
    }
    if((i >= 32)&& (i < 34))
    {
      strcpy(m->UsbFlieName, "hello民革.gcode");
      if(m->IsDir == 1)
        SERIAL_PRINTF("THIS IS NOT DIR");
      SERIAL_PRINTF("THIS IS NOT DIR");
    }
    file_list_insert_tail(m);
  }
}


//TEST 文件夹1下面的子文件
void LcdFile::linklist_create3(void)
{
  int i;
  pfile_list m;
  for(i = 0; i < 3; i++)
  {
    m = (pfile_list) new char[(sizeof(file_list))];	
    memset(m,0,sizeof(file_list));
    m->NextFile = NULL;
    if(NULL==m)
    {
      return;
    }
    if(i == 0)
    {
      strcpy(m->UsbFlieName, "子文件1.gcode");
    }
    if(i == 1)
    {
      strcpy(m->UsbFlieName, "auriga.gcode");
      m->IsDir = 1;
    }
    if((i == 2))
    {
      strcpy(m->UsbFlieName, "orion.gcode");
    }
    file_list_insert_tail(m);
  }
}

//TEST 文件夹4下面的子文件
void LcdFile::linklist_create4(void)
{
  int i;
  pfile_list m;
  for(i = 0; i < 7; i++)
  {
    m = (pfile_list) new char[(sizeof(file_list))];	
    memset(m,0,sizeof(file_list));
    m->NextFile = NULL;
    if(NULL==m)
    {
      return;
    }
    if(i == 0)
    {
      strcpy(m->UsbFlieName, "子文件2");
      m->IsDir = 1;
    }
    if(i == 1)
    {
      strcpy(m->UsbFlieName, "hellogirl.gcode");
    }
    if((i == 2) || (i == 3))
    {
      strcpy(m->UsbFlieName, "helloworld.gcode");
    }
    if((i >= 4)&& (i < 7))
    {
      strcpy(m->UsbFlieName, "你好世界.gcode");
    }
    file_list_insert_tail(m);
  }
}
