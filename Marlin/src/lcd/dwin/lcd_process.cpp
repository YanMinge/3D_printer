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
#include "lcd_file.h"
#include "lcd_process.h"

#if ENABLED(USB_DISK_SUPPORT)
#include "udisk_reader.h"
#include "user_execution.h"

lcd_process dwin_process;
const unsigned long ButtonAddr[] = {0x1200,0x1202,0x1204,0x120E,0x1210,0x1212,0x1214,0x1216,0x1218, 0};

lcd_process::lcd_process()
{
  start_icon_count = 0;
  icon_update_status = 1;

  recive_data.head[0] = send_data.head[0] = HEAD_ONE;
  recive_data.head[1] = send_data.head[1] = HEAD_TWO;

  memset(recevie_data_buf,0, sizeof(recevie_data_buf));
  memset(send_data_buf,0, sizeof(send_data_buf));
}

void lcd_process::clear_lcd_data_buf(void)
{
  RecNum = 0;
  HaveLcdCommand = 0;

  memset(recevie_data_buf,0,sizeof(recevie_data_buf));
}

void lcd_process::clear_lcd_data_buf1(void)
{
  RecNum = 0;
  HaveLcdCommand = 1;

  memset(recevie_data_buf,0,sizeof(recevie_data_buf));
}

void lcd_process::clear_recevie_buf(void)
{
  memset(&recive_data, 0, sizeof(recive_data));
  recive_data.head[0] = HEAD_ONE;
  recive_data.head[1] = HEAD_TWO;
}

void lcd_process::clear_send_data_buf(void)
{
  memset(send_data_buf,0,sizeof(send_data_buf));
  memset(&send_data, 0, sizeof(send_data));
  send_data.head[0] = HEAD_ONE;
  send_data.head[1] = HEAD_TWO;
}

void lcd_process::lcd_receive_data(void)
{
  while(MYSERIAL2.available() > 0 )
  {
    recevie_data_buf[RecNum++] = MYSERIAL2.read();
    //MYSERIAL1.write(recevie_data_buf[RecNum-1]);
    if(recevie_data_buf[0] != HEAD_ONE)    //recevie data is wrong
    {
      clear_lcd_data_buf();
      continue;
    	 if(RecNum > 0) // reset the databuf
    	 {
    	    clear_lcd_data_buf();
    	    continue;
    	 }
    }
    //  recevie a full command
    else if(RecNum == (recevie_data_buf[2]+3))
    {
      if((recive_data.head[0] == recevie_data_buf[0]) && (recive_data.head[1] == recevie_data_buf[1]) && RecNum > 2)
      {
        recive_data.len = recevie_data_buf[2];
        recive_data.command = recevie_data_buf[3];
        if(recive_data.len == 0x03 && recive_data.command == 0x80 && (recevie_data_buf[4] == 0x4F) && (recevie_data_buf[5] == 0x4B))
        {
          clear_lcd_data_buf1();
          type = CMD_WRITE_REG_OK;
          return;
        }
        else if(recive_data.len == 0x03 && recive_data.command == 0x82 && (recevie_data_buf[4] == 0x4F) && (recevie_data_buf[5] == 0x4B))
        {
          clear_lcd_data_buf1();
          type = CMD_WRITE_VAR_OK;
          return;
        }
        //response for reading the data from the variate
        else if(recive_data.command == 0x83)
        {
          recive_data.addr = recevie_data_buf[4];
          recive_data.addr = (recive_data.addr << 8 ) | recevie_data_buf[5];
          recive_data.bytelen = recevie_data_buf[6];
          for(int i = 0;i < (signed long)recive_data.bytelen;i+=2)
          {
            recive_data.data[i/2]= recevie_data_buf[7+i];
            recive_data.data[i/2]= (recive_data.data[i/2] << 8 ) | recevie_data_buf[8+i];
          }
          clear_lcd_data_buf1();
          type = CMD_READ_VAR;
          return;
        }
        //response for reading the page from the register
        else if(recive_data.command == 0x81)
        {
          recive_data.addr = recevie_data_buf[4];
          recive_data.bytelen = recevie_data_buf[5];
          for(int i = 0;i < (signed long)recive_data.bytelen;i++)
          {
            recive_data.data[i]= recevie_data_buf[6+i];
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
    else
    {
      continue;
    }
  }
}

void lcd_process::lcd_send_data(void)
{
  if((send_data.head[0] == HEAD_ONE) && (send_data.head[1] == HEAD_TWO) && send_data.len >= 3)
  {
    send_data_buf[0] = send_data.head[0];
    send_data_buf[1] = send_data.head[1];
    send_data_buf[2] = send_data.len;
    send_data_buf[3] = send_data.command;
    //write data to the register
    if(send_data.command == WRITE_REGISTER_ADDR)
    {
      send_data_buf[4] = send_data.addr;
      for(int i = 0; i < (send_data.len - 2); i++)
        send_data_buf[5+i] = send_data.data[i];
    }
    // read data from register
    else if((send_data.command == READ_REGISTER_ADDR) && send_data.len == 3)
    {
      send_data_buf[4] = send_data.addr;
      send_data_buf[5] = send_data.bytelen;
    }
    // write data to var
    else if(send_data.command == WRITE_VARIABLE_ADDR)
    {
      send_data_buf[4] = send_data.addr >> 8;
      send_data_buf[5] = send_data.addr & 0xFF;
      for(int i = 0; i < (send_data.len - 3); i += 2)
      {
        send_data_buf[6+i] = send_data.data[i/2] >> 8;
        send_data_buf[7+i] = send_data.data[i/2] & 0xFF;
      }
    }
    //read data from the var
    else if((send_data.command == READ_VARIABLE_ADDR) && send_data.len == 4)
    {
      send_data_buf[4] = send_data.addr >> 8;
      send_data_buf[5] = send_data.addr & 0xFF;
      send_data_buf[6] = send_data.bytelen;
    }
    //send data to uart
    for(int i = 0; i < (send_data.len + 3); i++)
    {
      MYSERIAL2.write(send_data_buf[i]);
    }
    clear_send_data_buf();
  }
}

void lcd_process::lcd_send_data(const String &s, unsigned long addr, unsigned char cmd /*= WRITE_VARIABLE_ADDR*/)
{
	if(s.length() < 1)
		return;
	lcd_send_data(s.c_str(), addr, cmd);
}

void lcd_process::lcd_send_data(const char *str, unsigned long addr, unsigned char cmd/*= WRITE_VARIABLE_ADDR*/)
{
	int len = strlen(str);
	if( len > 0)
	{
		send_data_buf[0] = HEAD_ONE;
		send_data_buf[1] = HEAD_TWO;
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

void lcd_process::lcd_send_data_clear(unsigned long addr,int len, unsigned char cmd/*= WRITE_VARIABLE_ADDR*/)
{
	if( len > 0)
	{
		send_data_buf[0] = HEAD_ONE;
		send_data_buf[1] = HEAD_TWO;
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

void lcd_process::lcd_send_data(char c, unsigned long addr, unsigned char cmd/*= WRITE_VARIABLE_ADDR*/)
{
	send_data.command = cmd;
	send_data.addr = addr;
	send_data.data[0] = (unsigned long)c;
	send_data.data[0] = send_data.data[0] << 8;
	send_data.len = 5;
	lcd_send_data();
}

void lcd_process::lcd_send_data(unsigned char* str, unsigned long addr, unsigned char cmd)
{
  lcd_send_data((char *)str, addr, cmd);
}

void lcd_process::lcd_send_data(int n, unsigned long addr, unsigned char cmd/*= WRITE_VARIABLE_ADDR*/)
{
	if(cmd == WRITE_VARIABLE_ADDR )
	{
		if(n > 0xFFFF)
		{
			send_data.data[0] = n >> 16;
			send_data.data[1] = n & 0xFFFF;
			send_data.len = 7;
		}
		else
		{
			send_data.data[0] = n;
			send_data.len = 5;
		}
	}
	else if(cmd == WRITE_REGISTER_ADDR)
	{
		send_data.data[0] = n;
		send_data.len = 3;
	}
	else if(cmd == READ_VARIABLE_ADDR)
	{
		send_data.bytelen = n;
		send_data.len = 4;
	}
	send_data.command = cmd;
	send_data.addr = addr;
	lcd_send_data();
}

void lcd_process::lcd_send_data(unsigned int n, unsigned long addr, unsigned char cmd)
{
  lcd_send_data((int)n, addr, cmd);
}

void lcd_process::lcd_send_data(float n, unsigned long addr, unsigned char cmd)
{
  lcd_send_data((int)n, addr, cmd);
}

void lcd_process::lcd_send_data(long n, unsigned long addr, unsigned char cmd)
{
  lcd_send_data((unsigned long)n, addr, cmd);
}

void lcd_process::lcd_send_data(unsigned long n, unsigned long addr, unsigned char cmd/*= WRITE_VARIABLE_ADDR*/)
{
	if(cmd == WRITE_VARIABLE_ADDR )
	{
		if(n > 0xFFFF)
		{
			send_data.data[0] = n >> 16;
			send_data.data[1] = n & 0xFFFF;
			send_data.len = 7;
		}
		else
		{
			send_data.data[0] = n;
			send_data.len = 5;
		}
	}
	else if(cmd == READ_VARIABLE_ADDR)
	{
		send_data.bytelen = n;
		send_data.len = 4;
	}
	send_data.command = cmd;
	send_data.addr = addr;
	lcd_send_data();
}

void lcd_process::lcd_send_temperature(int tempbed, int tempbedt, int temphotend, int temphotendt)
{
  lcd_send_data(tempbed, TEMP_HOTEND_ADDR);
  lcd_send_data(tempbedt, TEMP_HOTEND_TARGET_ADDR);
  lcd_send_data(temphotend, TEMP_BED_ADDR);
  lcd_send_data(temphotendt, TEMP_BED_TARGET_ADDR);
}



void lcd_process::icon_update(void)
{
  millis_t ms = millis();
  if(ms > update_time && icon_update_status)
  {
    if(start_icon_count < 100)
    {
      lcd_send_data(start_icon_count, START_ICON_ADDR);
    }
    if((start_icon_count += 1) > 100)
    {
      icon_update_status = 0;
      lcd_send_data(PAGE_BASE +1, PAGE_ADDR);
    }
    update_time = ms +10;
  }
}

void lcd_process::next_page_clear(void)
{
  if(LcdFile.page_count > 1 && current_page != 0)
  {
    if(LcdFile.page_count == current_page + 1)
    {
      for(int i = 0; i < 4; i++)
      {
        lcd_send_data_clear((FILE_TEXT_ADDR_9 + 16*i), 30, WRITE_VARIABLE_ADDR);
        lcd_send_data(2,(FILE_ICON_ADDR + i));
      }
    }
    else
    {
      if(current_page % 2)
      {
        for(int i = 0; i < 4; i++)
        {
          lcd_send_data_clear((FILE_TEXT_ADDR_5 + 16*i), 30, WRITE_VARIABLE_ADDR);
          lcd_send_data(2,(FILE_ICON_ADDR + i));
        }
      }
      else
      {
        for(int i = 0; i < 4; i++)
        {
          lcd_send_data_clear((FILE_TEXT_ADDR_5 + 16*i), 30, WRITE_VARIABLE_ADDR);
          lcd_send_data(2,(FILE_ICON_ADDR + i));
        }
      }
    }
  }
  if(current_page == 0)
  {
    for(int i = 0; i < 4; i++)
    {
      lcd_send_data_clear((FILE_TEXT_ADDR_1 + 16*i), 30, WRITE_VARIABLE_ADDR);
      lcd_send_data(2,(FILE_ICON_ADDR + i));
    }
  }

}

void lcd_process::last_page_clear(void)
{
  if(LcdFile.page_count > 2)
  {
    if(current_page % 2)
    {
      for(int i = 0; i < 4; i++)
      {
        lcd_send_data_clear((FILE_TEXT_ADDR_5 + 16*i), 30, WRITE_VARIABLE_ADDR);
        lcd_send_data(2,(FILE_ICON_ADDR + i));
      }
    }
    else
    {
      for(int i = 0; i < 4; i++)
      {
        lcd_send_data_clear((FILE_TEXT_ADDR_1 + 16*i), 30, WRITE_VARIABLE_ADDR);
        lcd_send_data(2,(FILE_ICON_ADDR + i));
      }
    }
  }
  //LcdFile.page_count == 2
  else
  {
    for(int i = 0; i < 4; i++)
    {
      lcd_send_data_clear((FILE_TEXT_ADDR_1 + 16*i), 30, WRITE_VARIABLE_ADDR);
      lcd_send_data(2,(FILE_ICON_ADDR + i));
    }
  }
}


void lcd_process::send_first_page_data(void)
{
  next_page_clear();
  pfile_list_t temp = NULL;
  if((LcdFile.page_count > 1) && current_page == 0)
  {
    for(int i = 0; i < 4; i++)
    {
      temp = LcdFile.file_list_index((i+1));
      if(temp->IsDir == 1)
        lcd_send_data(1,(FILE_ICON_ADDR + i));
      else
        lcd_send_data(0,(FILE_ICON_ADDR + i));
      lcd_send_data(temp->file_name,(FILE_TEXT_ADDR_1 + 16*i));
    }
    lcd_send_data(PAGE_BASE +3, PAGE_ADDR);
  }
  if((LcdFile.page_count == 1) && current_page == 0)
  {
    for(int i = 0; i < LcdFile.last_page_file_count; i++)
    {
      temp = LcdFile.file_list_index((i+1));
      if(temp->IsDir == 1)
        lcd_send_data(1,(FILE_ICON_ADDR + i));
      else
        lcd_send_data(0,(FILE_ICON_ADDR + i));
      lcd_send_data(temp->file_name,(FILE_TEXT_ADDR_1 + 16*i));
    }
    lcd_send_data(PAGE_BASE + 2, PAGE_ADDR);
  }
  current_page = 1;
}


void lcd_process::send_next_page_data(void)
{
  pfile_list_t temp = NULL;
  next_page_clear();
  if(LcdFile.page_count == (current_page + 1))
  {
    for(int i = current_page*4; i < (current_page*4 + LcdFile.last_page_file_count); i++)
    {
      temp = LcdFile.file_list_index((i+1));
      if(temp->IsDir == 1)
        lcd_send_data(1,(FILE_ICON_ADDR + (i - current_page*4)));
      else
        lcd_send_data(0,(FILE_ICON_ADDR + (i - current_page*4)));
      lcd_send_data(temp->file_name,(FILE_TEXT_ADDR_9 + 16*(i - current_page*4)));
    }
    lcd_send_data(PAGE_BASE +6, PAGE_ADDR);
  }
  else if(LcdFile.page_count > current_page + 1)
  {
    if(current_page % 2)
    {
      for(int i = current_page*4; i < (current_page*4 + 4); i++)
      {
        SERIAL_PRINTF(" i = %d .\r\n",i);
        temp = LcdFile.file_list_index((i+1));
        if(temp->IsDir == 1)
          lcd_send_data(1,(FILE_ICON_ADDR + (i - current_page*4)));
        else
          lcd_send_data(0,(FILE_ICON_ADDR + (i - current_page*4)));
        lcd_send_data(temp->file_name,(FILE_TEXT_ADDR_5 + 16*(i - current_page*4)));
      }
      lcd_send_data(PAGE_BASE +5, PAGE_ADDR);
    }
    else
    {
      for(int i = current_page*4; i < (current_page*4 + 4); i++)
      {
        temp = LcdFile.file_list_index((i+1));
        if(temp->IsDir == 1)
          lcd_send_data(1,(FILE_ICON_ADDR + (i - current_page*4)));
        else
          lcd_send_data(0,(FILE_ICON_ADDR + (i - current_page*4)));
        lcd_send_data(temp->file_name,(FILE_TEXT_ADDR_1 + 16*(i - current_page*4)));
      }
      lcd_send_data(PAGE_BASE +4, PAGE_ADDR);
    }
  }
  else
  {
    type = CMD_NULL;
  }
  current_page += 1;
}


void lcd_process::send_last_page_data(void)
{
  pfile_list_t temp = NULL;
  last_page_clear();
  if((LcdFile.page_count > 2))
  {
    if(current_page % 2)
    {
      for(int i = (current_page - 2)*4; i < (current_page*4 -4); i++)
      {
        temp = LcdFile.file_list_index((i+1));
        if(temp->IsDir == 1)
          lcd_send_data(1,(FILE_ICON_ADDR + (i - current_page*4 + 8)));
        else
          lcd_send_data(0,(FILE_ICON_ADDR + (i - current_page*4 + 8)));
        lcd_send_data(temp->file_name,(FILE_TEXT_ADDR_5 + 16*(i - current_page*4 + 8)));
      }
      current_page -= 1;
      lcd_send_data(PAGE_BASE +5, PAGE_ADDR);
    }
    else
    {
      for(int i = (current_page - 2)*4; i < (current_page*4 -4); i++)
      {
        temp = LcdFile.file_list_index((i+1));
        if(temp->IsDir == 1)
          lcd_send_data(1,(FILE_ICON_ADDR + (i - current_page*4 + 8)));
        else
          lcd_send_data(0,(FILE_ICON_ADDR + (i - current_page*4 + 8)));
        lcd_send_data(temp->file_name,(FILE_TEXT_ADDR_1 + 16*(i - current_page*4 + 8)));
      }
      if(current_page == 2)
      {
        lcd_send_data(PAGE_BASE + 3, PAGE_ADDR);
      }
      else
      {
        lcd_send_data(PAGE_BASE + 4, PAGE_ADDR);
      }
      current_page -= 1;
    }
  }
  else if((LcdFile.page_count == 2))
  {
    for(int i = (current_page - 2)*4; i < (current_page*4 -4); i++)
    {
      temp = LcdFile.file_list_index((i+1));
      if(temp->IsDir == 1)
        lcd_send_data(1,(FILE_ICON_ADDR + (i - current_page*4 + 8)));
      else
        lcd_send_data(0,(FILE_ICON_ADDR + (i - current_page*4 + 8)));
      lcd_send_data(temp->file_name,(FILE_TEXT_ADDR_1 + 16*(i - current_page*4 + 8)));
    }
    current_page -= 1;
    lcd_send_data(PAGE_BASE +3, PAGE_ADDR);
  }
  else
  {
    type = CMD_NULL;
  }
}

void my_lcd_init(void)
{
  LcdFile.file_list_init();
}
void lcd_update(void)
{
  dwin_process.icon_update();
  dwin_process.process_lcd_command();
}

void lcd_process::process_lcd_command(void)
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
          if(recive_data.addr == ButtonAddr[i])
          {
            if(ButtonAddr[i] == MENU_BUTTONS)
              key = MenuFile;
            else if(ButtonAddr[i] == SELECT_BUTTONS)
              key = SelectFile;
            else if(ButtonAddr[i] == PRINT_BUTTONS)
              key = PrintFile;
            else if(ButtonAddr[i] >= AXIS_MOVE_BTN && ButtonAddr[i] <= HOME_MOVE_BTN)
            {
              key = AxisMove;
              SERIAL_PRINTF("\r\n this axis\r\n");
            }
            else if(ButtonAddr[i] == LANGUAGE_SET_BTN)
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
            if(recive_data.data[0] == 0x09)
            {
              lcd_send_temperature(102,200,50,80);
              udisk.ls(LS_GET_FILE_NAME, "");
              LcdFile.get_file_page_count();
              max_file_index = GET_MAX_INDEX(LcdFile);
              current_page = 0;
              send_first_page_data();
            }
            // return button
            else if(recive_data.data[0] == 0x0A)
            {
              lcd_send_data(PAGE_BASE +1, PAGE_ADDR);
            }
            // next file page button
            else if(recive_data.data[0] == 0x0B)
            {
              send_next_page_data();
            }
            //last file page button
            else if(recive_data.data[0] == 0x0C)
            {
              send_last_page_data();
            }
            break;

          case SelectFile:
            //first file select button
            if(recive_data.data[0] == 0x01)
            {
              current_status = out_printing;
              current_file_index = ((current_page-1)*4);
              if(current_file_index > max_file_index)
                break;
              pfile_list_t temp = NULL;
              temp = LcdFile.file_list_index((current_file_index+1));
              memset(select_file_name,0,FILE_NAME_LEN);
              strcpy(select_file_name,temp->file_name);
              if(temp->IsDir)
              {
                LcdFile.file_list_clear();
                LcdFile.list_test();
                LcdFile.get_file_page_count();
                current_page = 0;
                send_first_page_data();
              }
              else
              {
                lcd_send_data(select_file_name,(FILE_TEXT_ADDR_D));
                lcd_send_data(PAGE_BASE + 7, PAGE_ADDR);
                UserExecution.cmd_M2023(select_file_name);
              }
            }
            //second file select button
            else if(recive_data.data[0] == 0x02)
            {
            }
            //third file select button
            if(recive_data.data[0] == 0x03)
            {
            }
            //forth file select button
            if(recive_data.data[0] == 0x04)
            {
            }
            break;
          case PrintFile:
            if(recive_data.data[0] == 0x01)
            {
              if(current_status == out_printing)
              {
                current_status = on_printing;
                lcd_send_data(STOP_MESSAGE,START_STOP_ICON_ADDR);
                SERIAL_PRINTF("\r\n kaishi\r\n");
                //send print file to
                UserExecution.cmd_M2024();
              }
              else if(current_status == on_printing )
              {
                current_status = stop_printing;
                lcd_send_data(START_MESSAGE,START_STOP_ICON_ADDR);
                UserExecution.cmd_M2025();
              }
              else if(current_status == stop_printing)
              {
                current_status = on_printing;
                lcd_send_data(STOP_MESSAGE,START_STOP_ICON_ADDR);
                UserExecution.cmd_M2024();
              }
            }
            break;
          case AxisMove:
            if(recive_data.addr == AXIS_MOVE_BTN)
            {
              //move xaxis distance away;
              SERIAL_PRINTF("AXIS_MOVE_BTN...\r\n");
              lcd_send_data(0,X_AXIS_MOVE_BTN);
              lcd_send_data(0,Y_AXIS_MOVE_BTN);
              lcd_send_data(0,Z_AXIS_MOVE_BTN);
              lcd_send_data(PAGE_BASE + 9, PAGE_ADDR);
              UserExecution.cmd_g92(0, 0, 0, 0);
            }
            else if((recive_data.addr == X_AXIS_MOVE_BTN)||
				           (recive_data.addr == Y_AXIS_MOVE_BTN) ||
				           (recive_data.addr == Z_AXIS_MOVE_BTN))
            {
              UserExecution.cmd_g1((float)recive_data.data[0]/10, (float)recive_data.data[0]/10, (float)recive_data.data[0]/10, 0);
            }
            else if(recive_data.addr == HOME_MOVE_BTN)
            {
              //go home;
              SERIAL_PRINTF("go hmoenow ...\r\n");
			  UserExecution.cmd_g28();
            }
            break;
          case SetLanguage:
            if(recive_data.data[0] == 0x01)
            {
              lcd_send_data(PAGE_BASE + 1, PAGE_ADDR);
            }
            if(recive_data.data[0] == 0x02)
            {
              lcd_send_data(PAGE_BASE + 11, PAGE_ADDR);
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
#endif // USB_DISK_SUPPORT
#endif // USE_DWIN_LCD
#endif // TARGET_LPC1768
