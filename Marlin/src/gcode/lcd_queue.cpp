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
#include "msd_reader.h"

#if ENABLED(PRINTER_EVENT_LEDS)
  #include "../feature/leds/printer_event_leds.h"
#endif

#if ENABLED(POWER_LOSS_RECOVERY)
  #include "../feature/power_loss_recovery.h"
#endif

LCDFILINFO lcd_file[20];
LCDQUEUE lcdqueue;
const unsigned long ButtonAddr[] = {0x1200,0x1202,0x120E,0x1210,0x1212,0x1214,0x1216,0x1218, 0};

bool UsbStatus;//check the 

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
  recdat.head[0] = HeadOne;
  recdat.head[1] = HeadTwo;
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
    MYSERIAL2.write(recevie_data_buf[RecNum-1]);
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
          for(int i = 0;i < (signed long)recdat.bytelen;i+=2)
          {
            recdat.data[i/2]= recevie_data_buf[7+i];
            recdat.data[i/2]= (recdat.data[i/2] << 8 ) | recevie_data_buf[8+i];
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
          for(int i = 0;i < (signed long)recdat.bytelen;i++)
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
        for(int i = 0;ButtonAddr[i] != 0; i++)
        {
          if(recdat.addr == ButtonAddr[i])
          {
            if(ButtonAddr[i] == MenuButtons)
              key = MenuFile;
            else if(ButtonAddr[i] == SelectButtons)
              key = SelectFile;
            else if(ButtonAddr[i] == PrintButtons)
              key = PrintFile;
            else if(ButtonAddr[i] >= AxisMoveBtn && ButtonAddr[i] <= HomeMoveBtn)
            {
              key = AxisMove;
              SERIAL_PRINTF("\r\n this axis\r\n");
            }
            else if(ButtonAddr[i] == LangSetBtn)
              key = SetLanguage;
            else
              key = i;
            break;
          }
        }
        switch (key)
        {
          case MenuFile:
            //main page print button
            if(recdat.data[0] == 0x09)
            { 
              lcd_send_temperature(102,200,50,80);
			        MsdReader.ls(LS_GetFilename, "");
              DwinLcdFile.get_file_page_count();
              CurrentPage = 0;
              send_first_page_data();
            }
            // return button
            else if(recdat.data[0] == 0x0A)
            {
              lcd_send_data(PageBase +1, PageAddr);
            }
            // next file page button
            else if(recdat.data[0] == 0x0B)
            {
              send_next_page_data();
            }
            //last file page button 
            else if(recdat.data[0] == 0x0C)
            {
              send_last_page_data();
            }
            break;
            
          case SelectFile:
            //first file select button
            if(recdat.data[0] == 0x01)
            {
              int index = ((CurrentPage-1)*4);
              pfile_list temp = NULL;
              temp = DwinLcdFile.file_list_index((index+1));
              if(temp->IsDir)
              {
                DwinLcdFile.file_list_clear();
                DwinLcdFile.linklist_create2();
                DwinLcdFile.get_file_page_count();
                CurrentPage = 0;
                send_first_page_data();
              }
              else
              {
                lcd_send_data(temp->UsbFlieName,(FileTextAddrD));
                lcd_send_data(PageBase + 7, PageAddr);
              }
            }
            //second file select button
            else if(recdat.data[0] == 0x02)
            {
            }
            //third file select button
            if(recdat.data[0] == 0x03)
            {
            }
            //forth file select button
            if(recdat.data[0] == 0x04)
            {
            }
            break;
          case PrintFile:
            if(recdat.data[0] == 0x01)
            {
            }
            break;
          case AxisMove:
            if(recdat.addr == AxisMoveBtn)
            {
              //move xaxis distance away;
              SERIAL_PRINTF("AxisMoveBtn...\r\n");
              lcd_send_data(0,XaxisMoveBtn);
              lcd_send_data(0,YaxisMoveBtn);
              lcd_send_data(0,ZaxisMoveBtn);
              lcd_send_data(PageBase + 9, PageAddr);
            }
            else if(recdat.addr == XaxisMoveBtn)
            {
              //move xaxis distance away;
              SERIAL_PRINTF("X move distance =  %f.\r\n",((float)recdat.data[0])/10);
            }
            else if(recdat.addr == YaxisMoveBtn)
            {
              //move yaxis distance away;
              SERIAL_PRINTF("Y move distance =  %f.\r\n",((float)recdat.data[0])/10);
            }
            else if(recdat.addr == ZaxisMoveBtn)
            {
              //move xaxis distance away;
              SERIAL_PRINTF("Z move distance =  %f.\r\n",((float)recdat.data[0])/10);
            }
            else if(recdat.addr == HomeMoveBtn)
            {
              //go home;
              SERIAL_PRINTF("go hmoenow ...\r\n");
            }
            break;
          case SetLanguage:
            if(recdat.data[0] == 0x01)
            {
              lcd_send_data(PageBase + 1, PageAddr);
            }
            if(recdat.data[0] == 0x02)
            {
              lcd_send_data(PageBase + 11, PageAddr);
            }
            break;
          default:
            break;
        }
        clear_recevie_buf();
        type = CMD_NULL;
        break;
        
      case CMD_WRITE_REG_OK:case CMD_WRITE_VAR_OK:case CMD_READ_REG:
        type = CMD_NULL;
        clear_recevie_buf();
        break;
      case CMD_NULL:
        break;
        clear_recevie_buf();
      default:
        clear_recevie_buf();
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
		send_data_buf[1] = HeadTwo;
		send_data_buf[2] = 3+len;
		send_data_buf[3] = cmd;
		send_data_buf[4] = addr >> 8;
		send_data_buf[5] = addr & 0x00FF;
		for(int i = 0;i <len ;i++)
			send_data_buf[6 + i] = str[i];
    for(int i = 0; i < (len + 6); i++)
    {
      MYSERIAL2.write(send_data_buf[i]);
      //MYSERIAL1.write(send_data_buf[i]);
    }
    clear_send_data_buf();
	}
}

void LCDQUEUE::lcd_send_data_clear(unsigned long addr,int len, unsigned char cmd/*= VarAddr_W*/)
{
	if( len > 0)
	{
		send_data_buf[0] = HeadOne;
		send_data_buf[1] = HeadTwo;
		send_data_buf[2] = 3+len;
		send_data_buf[3] = cmd;
		send_data_buf[4] = addr >> 8;
		send_data_buf[5] = addr & 0x00FF;
		for(int i = 0;i <len ;i++)
			send_data_buf[6 + i] = 0xff;
    for(int i = 0; i < (len + 6); i++)
    {
      MYSERIAL2.write(send_data_buf[i]);
      //MYSERIAL1.write(send_data_buf[i]);
    }
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

void LCDQUEUE::lcd_send_temperature(int tempbed, int tempbedt, int temphotend, int temphotendt)
{
  lcd_send_data(tempbed, TempHotendAddr);
  lcd_send_data(tempbedt, TempHotendTargetAddr);
  lcd_send_data(temphotend, TempBedAddr);
  lcd_send_data(temphotendt, TempTargetAddr);
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

void LCDQUEUE::next_page_clear(void)
{
  if(DwinLcdFile.PageCount > 1 && CurrentPage != 0)
  {
    //最后一页
    if(DwinLcdFile.PageCount == CurrentPage + 1)
    {
      for(int i = 0; i < 4; i++)
      {
        lcd_send_data_clear((FileTextAddr9 + 16*i), 30, VarAddr_W);
        lcd_send_data(2,(FileIconAddr + i));
      }
    }
    else
    {
      if(CurrentPage % 2)
      {
        //奇数页
        for(int i = 0; i < 4; i++)
        {
          lcd_send_data_clear((FileTextAddr5 + 16*i), 30, VarAddr_W);
          lcd_send_data(2,(FileIconAddr + i));
        }
      }
      else
      {
        for(int i = 0; i < 4; i++)
        {
          lcd_send_data_clear((FileTextAddr5 + 16*i), 30, VarAddr_W);
          lcd_send_data(2,(FileIconAddr + i));
        }
      }
    }
  }
  if(CurrentPage == 0)
  {
    //只有一页
    for(int i = 0; i < 4; i++)
    {
      lcd_send_data_clear((FileTextAddr1 + 16*i), 30, VarAddr_W);
      lcd_send_data(2,(FileIconAddr + i));
    }
  }
  
}

void LCDQUEUE::last_page_clear(void)
{
  if(DwinLcdFile.PageCount > 2)
  {
    if(CurrentPage % 2)
    {
      //奇数页
      for(int i = 0; i < 4; i++)
      {
        lcd_send_data_clear((FileTextAddr5 + 16*i), 30, VarAddr_W);
        lcd_send_data(2,(FileIconAddr + i));
      }
    }
    else
    {
      for(int i = 0; i < 4; i++)
      {
        lcd_send_data_clear((FileTextAddr1 + 16*i), 30, VarAddr_W);
        lcd_send_data(2,(FileIconAddr + i));
      }
    }
  }
  //DwinLcdFile.PageCount == 2
  else
  {
    for(int i = 0; i < 4; i++)
    {
      lcd_send_data_clear((FileTextAddr1 + 16*i), 30, VarAddr_W);
      lcd_send_data(2,(FileIconAddr + i));
    }
  }
}


void LCDQUEUE::send_first_page_data(void)
{
  next_page_clear();
  pfile_list temp = NULL;
  if((DwinLcdFile.PageCount > 1) && CurrentPage == 0)
  {
    for(int i = 0; i < 4; i++)
    {
      temp = DwinLcdFile.file_list_index((i+1));
      if(temp->IsDir == 1)
        lcd_send_data(1,(FileIconAddr + i));
      else
        lcd_send_data(0,(FileIconAddr + i));
      lcd_send_data(temp->UsbFlieName,(FileTextAddr1 + 16*i));
    }
    lcd_send_data(PageBase +3, PageAddr);
  }
  if((DwinLcdFile.PageCount == 1) && CurrentPage == 0)
  {
    for(int i = 0; i < DwinLcdFile.LastPageFlieCount; i++)
    {
      temp = DwinLcdFile.file_list_index((i+1));
      if(temp->IsDir == 1)
        lcd_send_data(1,(FileIconAddr + i));
      else
        lcd_send_data(0,(FileIconAddr + i));
      lcd_send_data(temp->UsbFlieName,(FileTextAddr1 + 16*i));
    }
    lcd_send_data(PageBase + 2, PageAddr);
  }
  CurrentPage = 1;
}


void LCDQUEUE::send_next_page_data(void)
{
  pfile_list temp = NULL;
  next_page_clear();
  if(DwinLcdFile.PageCount == (CurrentPage + 1))
  {
    for(int i = CurrentPage*4; i < (CurrentPage*4 + DwinLcdFile.LastPageFlieCount); i++)
    {
      temp = DwinLcdFile.file_list_index((i+1));
      if(temp->IsDir == 1)
        lcd_send_data(1,(FileIconAddr + (i - CurrentPage*4)));
      else
        lcd_send_data(0,(FileIconAddr + (i - CurrentPage*4)));
      lcd_send_data(temp->UsbFlieName,(FileTextAddr9 + 16*(i - CurrentPage*4)));
    }
    lcd_send_data(PageBase +6, PageAddr);
  }
  else if(DwinLcdFile.PageCount > CurrentPage + 1)
  {
    //奇数页 
    if(CurrentPage % 2)
    {
      SERIAL_PRINTF(" 奇数页.\r\n");
      for(int i = CurrentPage*4; i < (CurrentPage*4 + 4); i++)
      {
        SERIAL_PRINTF(" i = %d .\r\n",i);
        temp = DwinLcdFile.file_list_index((i+1));
        if(temp->IsDir == 1)
          lcd_send_data(1,(FileIconAddr + (i - CurrentPage*4)));
        else
          lcd_send_data(0,(FileIconAddr + (i - CurrentPage*4)));
        lcd_send_data(temp->UsbFlieName,(FileTextAddr5 + 16*(i - CurrentPage*4)));
      }
      lcd_send_data(PageBase +5, PageAddr);
    }
    //偶数页
    else
    { 
      SERIAL_PRINTF(" 偶数页.\r\n");
      for(int i = CurrentPage*4; i < (CurrentPage*4 + 4); i++)
      {
        temp = DwinLcdFile.file_list_index((i+1));
        if(temp->IsDir == 1)
          lcd_send_data(1,(FileIconAddr + (i - CurrentPage*4)));
        else
          lcd_send_data(0,(FileIconAddr + (i - CurrentPage*4)));
        lcd_send_data(temp->UsbFlieName,(FileTextAddr1 + 16*(i - CurrentPage*4)));
      }
      lcd_send_data(PageBase +4, PageAddr);
    }
  }
  else
  {
    type = CMD_NULL;
  }
  CurrentPage += 1;
}


void LCDQUEUE::send_last_page_data(void)
{
  pfile_list temp = NULL;
  last_page_clear();
  if((DwinLcdFile.PageCount > 2))
  {
    //奇数页 
    if(CurrentPage % 2)
    {
      SERIAL_PRINTF(" 奇数页.\r\n");
      for(int i = (CurrentPage - 2)*4; i < (CurrentPage*4 -4); i++)
      {
        temp = DwinLcdFile.file_list_index((i+1));
        if(temp->IsDir == 1)
          lcd_send_data(1,(FileIconAddr + (i - CurrentPage*4 + 8)));
        else
          lcd_send_data(0,(FileIconAddr + (i - CurrentPage*4 + 8)));
        lcd_send_data(temp->UsbFlieName,(FileTextAddr5 + 16*(i - CurrentPage*4 + 8)));
      }
      CurrentPage -= 1;
      lcd_send_data(PageBase +5, PageAddr);
    }
    //偶数页
    else
    { 
      SERIAL_PRINTF(" 偶数页.\r\n");
      for(int i = (CurrentPage - 2)*4; i < (CurrentPage*4 -4); i++)
      {
        temp = DwinLcdFile.file_list_index((i+1));
        if(temp->IsDir == 1)
          lcd_send_data(1,(FileIconAddr + (i - CurrentPage*4 + 8)));
        else
          lcd_send_data(0,(FileIconAddr + (i - CurrentPage*4 + 8)));
        lcd_send_data(temp->UsbFlieName,(FileTextAddr1 + 16*(i - CurrentPage*4 + 8)));
      }
      if(CurrentPage == 2)
      {
        lcd_send_data(PageBase + 3, PageAddr);
      }
      else
      {
        lcd_send_data(PageBase + 4, PageAddr);
      }
      CurrentPage -= 1;
    }
  }
  else if((DwinLcdFile.PageCount == 2))
  {
    SERIAL_PRINTF(" 第二页，最后一页.\r\n");
    for(int i = (CurrentPage - 2)*4; i < (CurrentPage*4 -4); i++)
    {
      temp = DwinLcdFile.file_list_index((i+1));
      if(temp->IsDir == 1)
        lcd_send_data(1,(FileIconAddr + (i - CurrentPage*4 + 8)));
      else
        lcd_send_data(0,(FileIconAddr + (i - CurrentPage*4 + 8)));
      lcd_send_data(temp->UsbFlieName,(FileTextAddr1 + 16*(i - CurrentPage*4 + 8)));
    }
    CurrentPage -= 1;
    lcd_send_data(PageBase +3, PageAddr);
  }
  else
  {
    type = CMD_NULL;
  }
}

void my_lcd_init(void)
{
  DwinLcdFile.file_list_init();
  DwinLcdFile.linklist_create2();
}
void lcd_update(void)
{
  lcdqueue.icon_update();
  lcdqueue.process_lcd_command();
}

