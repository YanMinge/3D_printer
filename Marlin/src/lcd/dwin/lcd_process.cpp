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

lcd_process::lcd_process()
{
  start_icon_count = 0;
  icon_update_status = 1;

  is_command = 0;
  type = CMD_NULL;

  recive_data.head[0] = send_data.head[0] = HEAD_ONE;
  recive_data.head[1] = send_data.head[1] = HEAD_TWO;

  memset(recevie_data_buf,0, sizeof(recevie_data_buf));
  memset(send_data_buf,0, sizeof(send_data_buf));
}

void lcd_process::clear_lcd_data_buf(void)
{
  receive_num = 0;
  is_command = 0;
  memset(recevie_data_buf,0,sizeof(recevie_data_buf));
}

void lcd_process::clear_lcd_data_buf1(void)
{
  receive_num = 0;
  is_command = 1;
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
    recevie_data_buf[receive_num++] = MYSERIAL2.read();
    //MYSERIAL1.write(recevie_data_buf[receive_num-1]);
    //data head is not correct
    if(recevie_data_buf[0] != HEAD_ONE)    //recevie data is wrong
    {
      clear_lcd_data_buf();
      continue;
    }
    //recive data num is overflow
    else if(receive_num > DATA_BUF_SIZE)
    {
      clear_lcd_data_buf();
    }
    //recevie a command
    else if((receive_num > 3) && receive_num == (recevie_data_buf[2]+3))
    {
      clear_recevie_buf();
      if((recive_data.head[0] == recevie_data_buf[0]) && (recive_data.head[1] == recevie_data_buf[1]) && receive_num > 2)
      {
        recive_data.len = recevie_data_buf[2];
        recive_data.command = recevie_data_buf[3];

        if((0x03 == recive_data.len) && (WRITE_REGISTER_ADDR == recive_data.command) && (TAIL_ONE == recevie_data_buf[4]) && (TAIL_TWO == recevie_data_buf[5]))
        {
          type = CMD_WRITE_REG_OK;
        }
        else if((0x03 == recive_data.len) &&(WRITE_VARIABLE_ADDR == recive_data.command) && (TAIL_ONE == recevie_data_buf[4]) && (TAIL_TWO == recevie_data_buf[5]))
        {
          type = CMD_WRITE_VAR_OK;
        }
        else if(READ_VARIABLE_ADDR == recive_data.command)
        {
          recive_data.addr = recevie_data_buf[4];
          recive_data.addr = (recive_data.addr << 8 ) | recevie_data_buf[5];
          recive_data.bytelen = recevie_data_buf[6];

          for(int i = 0;i < (signed long)recive_data.bytelen;i+=2)
          {
            recive_data.data[i/2]= recevie_data_buf[7+i];
            recive_data.data[i/2]= (recive_data.data[i/2] << 8 ) | recevie_data_buf[8+i];
          }
          type = CMD_READ_VAR;
          clear_lcd_data_buf1();
          return;
        }
        else if(READ_REGISTER_ADDR == recive_data.command)
        {
          recive_data.addr = recevie_data_buf[4];
          recive_data.bytelen = recevie_data_buf[5];

          for(int i = 0;i < (signed long)recive_data.bytelen;i++)
          {
            recive_data.data[i]= recevie_data_buf[6+i];
          }
          type = CMD_READ_REG;
        }
        else
        {
          type = CMD_ERROR;
          clear_lcd_data_buf();
          return;
        }
        clear_lcd_data_buf1();
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
    if(WRITE_REGISTER_ADDR == send_data.command)
    {
      send_data_buf[4] = send_data.addr;
      for(int i = 0; i < (send_data.len - 2); i++)
        send_data_buf[5+i] = send_data.data[i];
    }
    // read data from register
    else if((READ_REGISTER_ADDR == send_data.command) && (3 ==send_data.len))
    {
      send_data_buf[4] = send_data.addr;
      send_data_buf[5] = send_data.bytelen;
    }
    // write data to var
    else if(WRITE_VARIABLE_ADDR == send_data.command)
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
    else if((READ_VARIABLE_ADDR == send_data.command) && 4 == send_data.len)
    {
      send_data_buf[4] = send_data.addr >> 8;
      send_data_buf[5] = send_data.addr & 0xFF;
      send_data_buf[6] = send_data.bytelen;
    }
    //error command
    else
    {
      clear_send_data_buf();
      return;
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
	{
    return;
	}
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
		{
			send_data_buf[6 + i] = str[i];
		}
    for(int i = 0; i < (len + 6); i++)
    {
      MYSERIAL2.write(send_data_buf[i]);
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
		{
      send_data_buf[6 + i] = 0xff;
		}
    for(int i = 0; i < (len + 6); i++)
    {
      MYSERIAL2.write(send_data_buf[i]);
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

inline void lcd_process::clear_page(unsigned long addr, unsigned char cmd/*= WRITE_VARIABLE_ADDR*/)
{
  for(int i = 0; i < PAGE_FILE_NUM; i++)
  {
    lcd_send_data_clear((addr + FILE_TEXT_LEN*i), FILE_NAME_LEN);
    lcd_send_data(2,(FILE_ICON_ADDR + i));
  }
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

inline void lcd_process::send_page(unsigned long addr,int page,int num, unsigned char cmd/*= WRITE_VARIABLE_ADDR*/)
{
  pfile_list_t temp = NULL;

#define PAGE(a,b) (a - 4*b)

  for(int i = page*4; i < (page*4 + num); i++)
  {
    temp = LcdFile.file_list_index((i+1));
    if(temp->IsDir == 1)
    {
      lcd_send_data(1,(FILE_ICON_ADDR + PAGE(i,page)));
    }
    else
    {
      lcd_send_data(0,(FILE_ICON_ADDR + PAGE(i,page)));
    }
    lcd_send_data(temp->file_name,addr + FILE_TEXT_LEN*PAGE(i,page));
  }
}


void lcd_process::send_first_page_data(void)
{
  int page_count;
  int current_page;
  int last_file;

  last_file = LcdFile.get_last_page_file_num();
  page_count = LcdFile.get_page_count();
  current_page = LcdFile.get_current_page_num();
  clear_page(FILE_TEXT_ADDR_1);

  if((page_count > 1) && current_page == 0)
  {
    send_page(FILE_TEXT_ADDR_1,current_page,PAGE_FILE_NUM);
    lcd_send_data(PAGE_BASE +3, PAGE_ADDR);
  }
  else if((page_count == 1) && current_page == 0)
  {
    if(0 == last_file)
    {
      last_file = PAGE_FILE_NUM;
    }
    send_page(FILE_TEXT_ADDR_1,current_page,last_file);
    lcd_send_data(PAGE_BASE + 2, PAGE_ADDR);
  }
  LcdFile.set_current_page(1);
}

void lcd_process::send_next_page_data(void)
{
  int page_count;
  int current_page;
  int last_file;

  last_file = LcdFile.get_last_page_file_num();
  page_count = LcdFile.get_page_count();
  current_page = LcdFile.get_current_page_num();

  if(page_count == (current_page + 1))
  {
    clear_page(FILE_TEXT_ADDR_9);
    send_page(FILE_TEXT_ADDR_9,current_page,last_file);
    lcd_send_data(PAGE_BASE +6, PAGE_ADDR);
    LcdFile.set_current_page(current_page + 1);
  }
  else if(page_count > current_page + 1)
  {
    if(current_page % 2)
    {
      clear_page(FILE_TEXT_ADDR_5);
      send_page(FILE_TEXT_ADDR_5,current_page,PAGE_FILE_NUM);
      lcd_send_data(PAGE_BASE +5, PAGE_ADDR);
    }
    else
    {
      clear_page(FILE_TEXT_ADDR_1);
      send_page(FILE_TEXT_ADDR_1,current_page,PAGE_FILE_NUM);
      lcd_send_data(PAGE_BASE +4, PAGE_ADDR);
    }
    LcdFile.set_current_page(current_page + 1);
  }
  else
  {
    type = CMD_NULL;
  }
}

void lcd_process::send_last_page_data(void)
{
  int page_count;
  int current_page;

  page_count = LcdFile.get_page_count();
  current_page = LcdFile.get_current_page_num();

  if((page_count > 2))
  {
    if(current_page % 2)
    {
      clear_page(FILE_TEXT_ADDR_5);
      send_page(FILE_TEXT_ADDR_5,current_page-2,PAGE_FILE_NUM);
      lcd_send_data(PAGE_BASE +5, PAGE_ADDR);
    }
    else
    {
      clear_page(FILE_TEXT_ADDR_1);
      send_page(FILE_TEXT_ADDR_1,current_page-2,PAGE_FILE_NUM);
      if(current_page == 2)
      {
        lcd_send_data(PAGE_BASE + 3, PAGE_ADDR);
      }
      else
      {
        lcd_send_data(PAGE_BASE + 4, PAGE_ADDR);
      }
    }
    LcdFile.set_current_page(current_page - 1);
  }
  else if((page_count == 2))
  {
    clear_page(FILE_TEXT_ADDR_1);
    send_page(FILE_TEXT_ADDR_1,current_page-2,PAGE_FILE_NUM);
    lcd_send_data(PAGE_BASE +3, PAGE_ADDR);
    LcdFile.set_current_page(current_page - 1);
  }
  else
  {
    type = CMD_NULL;
  }
}

bool lcd_process::is_have_command(void)
{
  return is_command;
}

void lcd_process::reset_command(void)
{
  is_command = 0;
}

lcd_cmd_type lcd_process::get_command_type(void)
{
  return type;
}

void lcd_process::reset_command_type(void)
{
  type = CMD_NULL;
}

unsigned long lcd_process::get_receive_addr(void)
{
  return recive_data.addr;
}

unsigned short lcd_process::get_receive_data(void)
{
  return recive_data.data[0];
}

#endif // USB_DISK_SUPPORT
#endif // USE_DWIN_LCD
#endif // TARGET_LPC1768
