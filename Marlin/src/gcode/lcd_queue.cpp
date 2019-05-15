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

LCDQUEUE lcdqueue;
const unsigned long ButtonAddr[] = {0x1200, 0};

LCDQUEUE::LCDQUEUE()
{
  UpdateStatus = 1;
  recdat.head[0] = snddat.head[0] = HeadOne;
  recdat.head[1] = snddat.head[1] = HeadTwo;
  memset(recevie_data_buf,0, sizeof(recevie_data_buf));
  memset(send_data_buf,0, sizeof(send_data_buf));
}

void LCDQUEUE::clear_lcd_data_buf(void) {
  memset(recevie_data_buf,0,sizeof(recevie_data_buf));
  HaveLcdCommand = 0;
  RecNum = 0;
}

void LCDQUEUE::clear_lcd_data_buf1(void) {
  memset(recevie_data_buf,0,sizeof(recevie_data_buf));
  HaveLcdCommand = 1;
  RecNum = 0;
}

void LCDQUEUE::clear_recevie_buf(void)
{
  memset(&recdat, 0, sizeof(recdat));
  snddat.head[0] = HeadOne;
  snddat.head[1] = HeadTwo;
}

void LCDQUEUE::clear_send_data_buf(void) {
  memset(send_data_buf,0,sizeof(send_data_buf));
  memset(&snddat, 0, sizeof(snddat));
  snddat.head[0] = HeadOne;
  snddat.head[1] = HeadTwo;
}

void LCDQUEUE::lcd_receive_data(void)
{
  while(MYSERIAL2.available() > 0 && (RecNum < SizeofDatabuf))
  {
    recevie_data_buf[RecNum++] = MYSERIAL2.read();
    if(recevie_data_buf[0] != HeadOne)    //recevie data is wrong
    {
    	 if(RecNum > 0) // reset the databuf
    	 {
    	    clear_lcd_data_buf();
    	    continue;
    	 }
    }
    //  recevie a full command
    if(RecNum == (recevie_data_buf[2]+3))
    {
      if((recdat.head[0] == recevie_data_buf[0]) && (recdat.head[1] == recevie_data_buf[1]) && RecNum > 2)
      {
        recdat.len = recevie_data_buf[2];
        recdat.command = recevie_data_buf[3];
        if(recdat.len == 0x03 && recdat.command == 0x80 && (recevie_data_buf[4] == 0x4F) && (recevie_data_buf[5] == 0x4B))
        {   
          clear_lcd_data_buf1();
          type = CMD_WRITE_REG_OK;
          return;
        }
        else if(recdat.len == 0x03 && recdat.command == 0x82 && (recevie_data_buf[4] == 0x4F) && (recevie_data_buf[5] == 0x4B))
        {   
          clear_lcd_data_buf1();
          type = CMD_WRITE_VAR_OK;
          return;
        }
        //response for reading the data from the variate
        else if(recdat.command == 0x83)
        {
          recdat.addr = recevie_data_buf[4];
          recdat.addr = (recdat.addr << 8 ) | recevie_data_buf[5];
          recdat.bytelen = recevie_data_buf[6];
          for(int i = 0;i < recdat.bytelen;i+=2)
          {
            recdat.data[i/2]= recevie_data_buf[7+i];
            recdat.data[i/2]= (recdat.data[i/2] << 8 )| recevie_data_buf[8+i];
          }
          clear_lcd_data_buf1();
          type = CMD_READ_VAR;
          return;
        }
        //response for reading the page from the register
        else if(recdat.command == 0x81)
        {
          recdat.addr = recevie_data_buf[4];
          recdat.bytelen = recevie_data_buf[5];
          for(int i = 0;i < recdat.bytelen;i++)
          {
            recdat.data[i]= recevie_data_buf[6+i];
          }
          clear_lcd_data_buf1();
          type = CMD_READ_REG;
          return;
        }
      }
      else
      {
        clear_lcd_data_buf();
        type = CMD_ERROR;  //receive the wrong data
        return;
      }
    }
  }
}


void LCDQUEUE::process_lcd_command(void)
{
  lcd_receive_data();
  if(HaveLcdCommand)
  {
    int key = -1;
    HaveLcdCommand = 0;
    switch (type)
    {
      case CMD_READ_VAR:
        
        type = CMD_NULL;
        break;



        
      case CMD_WRITE_REG_OK:case CMD_WRITE_VAR_OK:
        clear_recevie_buf();
        break;
      case CMD_NULL:
        break;
      default:
        break;
    }
  }
}
void LCDQUEUE::lcd_send_data(void)
{
  if((snddat.head[0] == HeadOne) && (snddat.head[1] == HeadTwo) && snddat.len >= 3)
  {
    send_data_buf[0] = snddat.head[0];
    send_data_buf[1] = snddat.head[1];
    send_data_buf[2] = snddat.len;
    send_data_buf[3] = snddat.command;
    //write data to the register
    if(snddat.command == RegAddr_W)
    {
      send_data_buf[4] = snddat.addr;
      for(int i = 0; i < (snddat.len - 2); i++)
        send_data_buf[5+i] = snddat.data[i];
    }
    // read data from register
    else if((snddat.command == RegAddr_R) && snddat.len == 3)
    {
      send_data_buf[4] = snddat.addr;
      send_data_buf[5] = snddat.bytelen;
    }
    // write data to var
    else if(snddat.command == VarAddr_W)
    {
      send_data_buf[4] = snddat.addr >> 8;
      send_data_buf[5] = snddat.addr & 0xFF;
      for(int i = 0; i < (snddat.len - 3); i += 2)
      {
        send_data_buf[6+i] = snddat.data[i/2] >> 8;
        send_data_buf[7+i] = snddat.data[i/2] & 0xFF;
      }
    }
    //read data from the var
    else if((snddat.command == VarAddr_R) && snddat.len == 4)
    {
      send_data_buf[4] = snddat.addr >> 8;
      send_data_buf[5] = snddat.addr & 0xFF;
      send_data_buf[6] = snddat.bytelen;
    }
    //send data to uart
    for(int i = 0; i < (snddat.len + 3); i++)
    {
      MYSERIAL2.write(send_data_buf[i]);
    }
    clear_send_data_buf();
  }
}

void LCDQUEUE::lcd_send_data(const String &s, unsigned long addr, unsigned char cmd /*= VarAddr_W*/)
{
	if(s.length() < 1)
		return;
	lcd_send_data(s.c_str(), addr, cmd);
}

void LCDQUEUE::lcd_send_data(const char *str, unsigned long addr, unsigned char cmd/*= VarAddr_W*/)
{
	int len = strlen(str);
	if( len > 0)
	{
		send_data_buf[0] = HeadOne;
		send_data_buf[0] = HeadOne;
		send_data_buf[2] = 3+len;
		send_data_buf[3] = cmd;
		send_data_buf[4] = addr >> 8;
		send_data_buf[5] = addr & 0x00FF;
		for(int i = 0;i <len ;i++)
			send_data_buf[6 + i] = str[i];
    for(int i = 0; i < (len + 6); i++)
      MYSERIAL2.write(send_data_buf[i]);
    clear_send_data_buf();
	}
}

void LCDQUEUE::lcd_send_data(char c, unsigned long addr, unsigned char cmd/*= VarAddr_W*/)
{
	snddat.command = cmd;
	snddat.addr = addr;
	snddat.data[0] = (unsigned long)c;
	snddat.data[0] = snddat.data[0] << 8;
	snddat.len = 5;
	lcd_send_data();
}

void LCDQUEUE::lcd_send_data(unsigned char* str, unsigned long addr, unsigned char cmd)
{
  lcd_send_data((char *)str, addr, cmd);
}

void LCDQUEUE::lcd_send_data(int n, unsigned long addr, unsigned char cmd/*= VarAddr_W*/)
{
	if(cmd == VarAddr_W )
	{
		if(n > 0xFFFF)
		{
			snddat.data[0] = n >> 16;
			snddat.data[1] = n & 0xFFFF;
			snddat.len = 7;
		}
		else
		{
			snddat.data[0] = n;
			snddat.len = 5;
		}
	}
	else if(cmd == RegAddr_W)
	{
		snddat.data[0] = n;
		snddat.len = 3;
	}
	else if(cmd == VarAddr_R)
	{
		snddat.bytelen = n;
		snddat.len = 4;
	}
	snddat.command = cmd;
	snddat.addr = addr;
	lcd_send_data();
}

void LCDQUEUE::lcd_send_data(unsigned int n, unsigned long addr, unsigned char cmd)
{
  lcd_send_data((int)n, addr, cmd);
}

void LCDQUEUE::lcd_send_data(float n, unsigned long addr, unsigned char cmd)
{
  lcd_send_data((int)n, addr, cmd);
}

void LCDQUEUE::lcd_send_data(long n, unsigned long addr, unsigned char cmd)
{
  lcd_send_data((unsigned long)n, addr, cmd);
}

void LCDQUEUE::lcd_send_data(unsigned long n, unsigned long addr, unsigned char cmd/*= VarAddr_W*/)
{
	if(cmd == VarAddr_W )
	{
		if(n > 0xFFFF)
		{
			snddat.data[0] = n >> 16;
			snddat.data[1] = n & 0xFFFF;
			snddat.len = 7;
		}
		else
		{
			snddat.data[0] = n;
			snddat.len = 5;
		}
	}
	else if(cmd == VarAddr_R)
	{
		snddat.bytelen = n;
		snddat.len = 4;
	}
	snddat.command = cmd;
	snddat.addr = addr;
	lcd_send_data();
}

void LCDQUEUE::icon_update(void)
{
  millis_t ms = millis();
  if(ms > NextUpdateTime && UpdateStatus)
  {
    if(StartIconCount < 100)
      lcd_send_data(StartIconCount, StartIconAddr);
    if((StartIconCount += 1) > 100)
    {
      lcd_send_data(PageBase +1, PageAddr);
      UpdateStatus = 0;
    }
    NextUpdateTime = ms +10;
  }
}



void lcd_update(void)
{
  lcdqueue.lcd_receive_data();
  lcdqueue.icon_update();
}



