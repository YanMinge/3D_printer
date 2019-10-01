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
#include "lcd_parser.h"

#if ENABLED(USB_DISK_SUPPORT)
#include "udisk_reader.h"
#include "user_execution.h"

#if ENABLED(POWER_LOSS_RECOVERY)
#include "../../feature/power_loss_recovery.h"
#endif

lcd_process dwin_process;

lcd_process::lcd_process()
{
  start_icon_count = 0;

  is_lcd_command = false;
  lcd_command_type = CMD_NULL;

  recive_data.head[0] = send_data.head[0] = HEAD_ONE;
  recive_data.head[1] = send_data.head[1] = HEAD_TWO;

  memset(recevie_data_buf,0, sizeof(recevie_data_buf));
  memset(send_data_buf,0, sizeof(send_data_buf));
  memset(&file_info,0,sizeof(data_info_t));

  lcd_temp_show_status = false;
}

/**
 * @breif  clear recevie_data_buf
 * @detail  have not receive a command
 */
void lcd_process::clear_lcd_data_buf(void)
{
  receive_num = 0;
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

void lcd_process::lcd_receive_data_clear(void)
{
  while(lcd_data_available())
  {
    read_lcd_data();
  }
}

uint8_t lcd_process::lcd_receive_data_correct(void)
{
  if((HEAD_ONE != recevie_data_buf[0]) || (receive_num > DATA_BUF_SIZE))
  {
    return LCD_DATA_ERROR;
  }
  else if((receive_num >= 2) && (HEAD_TWO != recevie_data_buf[1]))
  {
    return LCD_DATA_ERROR;
  }
  else if((receive_num > 3) &&  receive_num > (recevie_data_buf[2]+3))
  {
    return LCD_DATA_ERROR;
  }
  else if((receive_num > 3) && receive_num == (recevie_data_buf[2]+3))
  {
    return LCD_DATA_FULL;
  }
  else
  {
    return LCD_DATA_NO_FULL;
  }
}

bool lcd_process::lcd_data_available(void)
{
  return(MYSERIAL2.available() > 0);
}

uint8_t lcd_process::read_lcd_data(void)
{
  uint8_t c;
  c = MYSERIAL2.read();
  return c;
}

void lcd_process::write_lcd_data(uint8_t c)
{
  MYSERIAL2.write(c);
}

void lcd_process::lcd_receive_data(void)
{
  while(lcd_data_available())
  {
    recevie_data_buf[receive_num++] = read_lcd_data();
    //MYSERIAL0.write(recevie_data_buf[receive_num-1]);
    if(LCD_DATA_ERROR == lcd_receive_data_correct())
    {
      clear_lcd_data_buf();
      is_lcd_command = false;
      continue;
    }
    //recevie a command
    else if(LCD_DATA_FULL == lcd_receive_data_correct())
    {
      clear_recevie_buf();
      recive_data.len = recevie_data_buf[2];
      recive_data.command = recevie_data_buf[3];

      switch (recive_data.command)
      {
        case WRITE_REGISTER_ADDR:
          if(0x03 == recive_data.len)
          {
            clear_lcd_data_buf();
            is_lcd_command = false;
            continue;
          }
          break;

        case WRITE_VARIABLE_ADDR:
          if(0x03 == recive_data.len)
          {
            lcd_command_type = CMD_WRITE_VAR_OK;
          }
          break;

        case READ_VARIABLE_ADDR:
          recive_data.addr = recevie_data_buf[4];
          recive_data.addr = (recive_data.addr << 8 ) | recevie_data_buf[5];
          recive_data.bytelen = recevie_data_buf[6];
          for(int i = 0;i < (signed long)recive_data.bytelen;i+=2)
          {
            recive_data.data[i/2]= recevie_data_buf[7+i];
            recive_data.data[i/2]= (recive_data.data[i/2] << 8 ) | recevie_data_buf[8+i];
          }
          lcd_command_type = CMD_READ_VAR;
          clear_lcd_data_buf();
          is_lcd_command = true;
          return;

        case READ_REGISTER_ADDR:
          recive_data.addr = recevie_data_buf[4];
          recive_data.bytelen = recevie_data_buf[5];

          for(int i = 0;i < (int)recive_data.bytelen;i++)
          {
            recive_data.data[i]= recevie_data_buf[6+i];
          }
          lcd_command_type = CMD_READ_REG;
          break;

        default:
          lcd_command_type = CMD_ERROR;
          break;
      }

      clear_lcd_data_buf();
      is_lcd_command = false;
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
      //MYSERIAL2.write(send_data_buf[i]);
      write_lcd_data(send_data_buf[i]);
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
      //MYSERIAL2.write(send_data_buf[i]);
      write_lcd_data(send_data_buf[i]);
    }
    clear_send_data_buf();
	}
}

void lcd_process::lcd_text_clear(unsigned long addr,int len, unsigned char cmd/*= WRITE_VARIABLE_ADDR*/)
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
      //MYSERIAL2.write(send_data_buf[i]);
      write_lcd_data(send_data_buf[i]);
    }
    clear_send_data_buf();
	}
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

void lcd_process::lcd_show_picture(unsigned short x, unsigned short y, unsigned long addr,unsigned char cmd/*= WRITE_VARIABLE_ADDR*/)
{
	send_data.data[0] = PICTURE_DISPLAY_BASE1 >> 16;
	send_data.data[1] = PICTURE_DISPLAY_BASE1 & 0xFFFF;

	send_data.data[2] = x;
	send_data.data[3] = y;
	send_data.len = 11;
	send_data.command = cmd;
	send_data.addr = addr;
	lcd_send_data();
}

void lcd_process::send_current_temperature(int tempbed, int temphotend)
{
  char str[2][8];
  char str1[5]={0xA1,0xe3,0xff,0xff,0};

  itoa(tempbed,str[0],10);
  itoa(temphotend,str[1],10);
  strcat(str[0],str1);
  strcat(str[1],str1);

  lcd_send_data(str[0],PRINT_TEMP_BED_ADDR);
  lcd_send_data(str[1],PRINT_TEMP_HOTEND_ADDR);
}

void lcd_process::temperature_progress_update(unsigned int percentage)
{
  send_temperature_percentage((uint16_t)percentage);
}

void lcd_process::send_file_list_page_num(int current_page, int page_num)
{
  char str[10];
  char str1[4];
  char str2[3]={0xff,0xff,0};
  sprintf_P(str1, PSTR("%d"), page_num);
  sprintf_P(str, PSTR("%2d"), current_page);
  strcat(str,"/");
  strcat(str,str1);
  strcat(str,str2);

  lcd_send_data(str,PRINT_FILE_PAGE_NUM_ADDR);
}

void lcd_process::get_file_info(void)
{
  file_info.last_page_file = LcdFile.get_last_page_file_num();
  file_info.page_count = LcdFile.get_page_count();
  file_info.current_page = LcdFile.get_current_page_num();
}

void lcd_process::set_simage_count(void)
{
  if(image_status.simage_set_status)
  {
    int file_size;
    get_file_info();
    file_info.current_index = PAGE_FILE_NUM*(file_info.current_page - 1) + (file_info.send_file_num+1);
    current_file = LcdFile.file_list_index(file_info.current_index);
    DEBUGPRINTF("file_name = %s file_type = %d\r\n",current_file->file_name, current_file->file_type);

    if(TYPE_FOLDER == current_file->file_type)
    {
      lcd_send_data(TYPE_FOLDER,(PRINT_FILE_SIMAGE_ICON_ADDR + file_info.send_file_num)); //send folder icon to file_list page
    }
    if(TYPE_DEFAULT_FILE == current_file->file_type)
    {
      if(udisk.check_gm_file(current_file->file_name))
      {
        current_file->file_type = TYPE_MAKEBLOCK_GM;
      }
      else
      {
        lcd_send_data(TYPE_DEFAULT_FILE,(PRINT_FILE_SIMAGE_ICON_ADDR + file_info.send_file_num)); //send default icon to file_list page
      }
    }

    if(TYPE_MAKEBLOCK_GM == current_file->file_type)
    {
      image_status.simage_delay_status = false;
      file_size = udisk.get_simage_size(current_file->file_name);
      if(-1 == file_size)
      {
        show_usb_pull_out_page();
        return;
      }
      file_info.image_send_count = file_size/SEND_IMAGE_LEN;
      file_info.image_last_count_len = file_size % SEND_IMAGE_LEN;
      if(file_info.image_last_count_len > 0) file_info.image_send_count += 1;
      image_status.simage_set_status = false;
      image_status.simage_send_status = true;
      int32_t offset = udisk.get_simage_offset(current_file->file_name);  //set the file index for send simage;
      if(-1 == offset)
      {
        show_usb_pull_out_page();
        return;
      }
      udisk.set_index(offset);
      DEBUGPRINTF("read image_send_count = %d image_last_count_len = %d\r\n",file_info.image_send_count,file_info.image_last_count_len);
    }
    else
    {
      image_status.simage_set_status = true;
      image_status.simage_send_status = false;
      file_info.send_file_num += 1;
      if(file_info.send_file_num > 2) simage_send_end();
    }
  }
}

void lcd_process::send_simage(void)
{
  #define SEND_NUM(X) (X ? X:SEND_IMAGE_LEN)
  if(image_status.simage_send_status)
  {
    if(file_info.image_current_send_count == file_info.image_send_count -1)
    {
      get_image_data(SEND_NUM(file_info.image_last_count_len));
      lcd_send_image(SEND_NUM(file_info.image_last_count_len),file_info.image_current_send_count);
      lcd_send_data(TYPE_NULL,(PRINT_FILE_SIMAGE_ICON_ADDR + file_info.send_file_num));
      lcd_show_picture(PRINT_SIMAGE_X_POSITION(file_info.send_file_num),PRINT_SIMAGE_Y_POSITION,PICTURE_ADDR,0X82);

      image_status.simage_delay_status = true;
      file_info.image_current_send_count = 0;
      file_info.image_send_count = 0;
      file_info.image_last_count_len = 0;
      file_info.send_file_num += 1;
      image_status.simage_set_status = true;
      image_status.simage_send_status = false;

      // if file have send finish
      if((file_info.current_page == file_info.page_count) && (file_info.send_file_num > (file_info.last_page_file - 1)))
      {
          image_status.simage_status = false;
          image_status.simage_set_status = false;
          file_info.send_file_num = 0;
      }
      else if((file_info.current_page < file_info.page_count) && (file_info.send_file_num > (PAGE_FILE_NUM - 1)))
      {
        image_status.simage_status = false;
        image_status.simage_set_status = false;
        file_info.send_file_num = 0;
      }
    }

    if(file_info.image_current_send_count < file_info.image_send_count -1)
    {
      get_image_data(SEND_IMAGE_LEN);
      lcd_send_image(SEND_IMAGE_LEN,file_info.image_current_send_count);
      file_info.image_current_send_count += 1;
    }
  }
}

void lcd_process::set_limage_count(void)
{
  if(image_status.limage_set_status)
  {
    int32_t file_size;

    get_file_info();
    file_info.current_index = PAGE_FILE_NUM*(file_info.current_page - 1) + (file_info.select_file_num);
    current_file = LcdFile.file_list_index(file_info.current_index);

    lcd_send_data(TYPE_LOAD,PRINT_FILE_LIMAGE_ICON_ADDR);
    if(TYPE_MAKEBLOCK_GM == current_file->file_type)
    {
      file_size = udisk.get_limage_size(current_file->file_name);
      if(-1 == file_size)
      {
        show_usb_pull_out_page();
        return;
      }
      file_info.image_send_count = file_size/SEND_IMAGE_LEN;
      file_info.image_last_count_len = file_size % SEND_IMAGE_LEN;
      image_status.limage_set_status = false;
      image_status.limage_send_status = true;
      if(!udisk.get_fram_xy_position(current_file->file_name))
      {
        show_usb_pull_out_page();
        return;
      }
      int32_t offset = udisk.get_limage_offset(current_file->file_name);
      if(-1 == offset)
      {
        show_usb_pull_out_page();
        return;
      }
      udisk.set_index(offset);
      if(file_info.image_last_count_len > 0)
      {
        file_info.image_send_count += 1;
      }
    }
    else
    {
      image_status.limage_set_status = false;
      image_status.limage_send_status = false;
      image_status.limage_status = false;
    }
  }
  DEBUGPRINTF("read image_send_count = %d image_last_count_len = %d\r\n",file_info.image_send_count,file_info.image_last_count_len);
}

void lcd_process::send_limage(void)
{
  #define SEND_NUM(X) (X ? X:SEND_IMAGE_LEN)
  if(image_status.limage_send_status)
  {
    if(file_info.image_current_send_count == file_info.image_send_count -1)
    {
      get_image_data(SEND_NUM(file_info.image_last_count_len));
      lcd_send_image(SEND_NUM(file_info.image_last_count_len),file_info.image_current_send_count);
      lcd_send_data(TYPE_NULL,PRINT_FILE_LIMAGE_ICON_ADDR);
      if(!udisk.job_recover_file_exists())
      {
        lcd_show_picture(PRINT_LIMAGE_X_POSITION,PRINT_LIMAGE_Y_POSITION,PICTURE_ADDR,0X82);
        safe_delay(IMAGE_SHOW_DELAY); //avoid show picture in other page;
      }

      file_info.image_current_send_count = 0;
      file_info.image_send_count = 0;
      file_info.image_last_count_len = 0;
      image_status.limage_status = false;
      image_status.simage_delay_status = true;
      DEBUGPRINTF("read time = %d\r\n", file_info.image_current_send_count);
    }
    if(file_info.image_current_send_count < file_info.image_send_count -1)
    {
      get_image_data(SEND_IMAGE_LEN);
      lcd_send_image(SEND_IMAGE_LEN,file_info.image_current_send_count);
      file_info.image_current_send_count += 1;
    }
  }
}

void lcd_process::send_print_time(uint32_t time)
{
  char str[12];
  memset(str, ' ', sizeof(str));
  if(udisk.get_opened_file_type() != TYPE_MAKEBLOCK_GM)
  {
    sprintf_P(str,"%.1f%%     ", (udisk.get_index() * 100.0) / udisk.get_opened_file_size());
    lcd_send_data(str,PRINT_FILE_PRINT_TIME_ADDR);
  }
  else
  {
    uint16_t hour = time/3600;
    int8_t min = (time % 3600) / 60;
    sprintf_P(str,"%.2d:%02d   ", hour, min);
    lcd_send_data(str,PRINT_FILE_PRINT_TIME_ADDR);
  }
}

void lcd_process::send_print_time(bool is_standard_file, uint32_t time)
{
  char str[12];
  memset(str, ' ', sizeof(str));
  if(!is_standard_file)
  {
    sprintf_P(str,"%.1f%%     ", (float)time);
    lcd_send_data(str,PRINT_FILE_PRINT_TIME_ADDR);
  }
  else
  {
    uint16_t hour = time/3600;
    int8_t min = (time % 3600) / 60;
    sprintf_P(str,"%.2d:%02d   ", hour, min);
    lcd_send_data(str,PRINT_FILE_PRINT_TIME_ADDR);
  }
}

void lcd_process::send_temperature_percentage(uint16_t percentage)
{
  char str[12];
  char str1[4] = {0x25,0xff,0xff,0x0};
  if(pre_percentage <= percentage)
  {
    sprintf_P(str, PSTR("%2d"), percentage);
    strcat(str,str1);
    lcd_send_data(str,PRINT_PREPARE_PERCENTAGE_ADDR);
    lcd_send_data(percentage/5 > 19 ? 19 : percentage/5, PRINT_PREPARE_PROGRESS_ICON_ADDR);
    pre_percentage = percentage;
  }
}

void lcd_process::send_progress_percentage(uint16_t percentage)
{
  char str[8];
  char str1[4] = {0x25,0xff,0xff,0x0};
  itoa(percentage,str,10);
  strcat(str,str1);
  lcd_send_data(str,PRINT_PREPARE_PERCENTAGE_ADDR);
}

void lcd_process::lcd_send_home_offset(float height)
{
  char str[8];
  sprintf_P(str,"%2.1f  ", height);
  lcd_send_data(str,PRINT_MACHINE_HOME_OFFSET_ADDR);
}

void lcd_process::lcd_loop(void)
{
  if(image_status.simage_status)
  {
    set_simage_count();
    send_simage();
  }
  else
  {
    image_status.simage_delay_status = false;
  }
  if(image_status.limage_status)
  {
    set_limage_count();
    send_limage();
  }
}

language_type lcd_process::get_language_type(void)
{
  if(LAN_CHINESE == ui_language)return LAN_CHINESE;
  else if(LAN_ENGLISH == ui_language)return LAN_ENGLISH;
  else return LAN_NULL;
}

void lcd_process::set_language_type(language_type type)
{
  if(LAN_CHINESE == type)ui_language = LAN_CHINESE;
  else if(LAN_ENGLISH == type)ui_language = LAN_ENGLISH;
  else ui_language = LAN_NULL;
}

void lcd_process::reset_image_parameters(void)
{
  file_info.image_send_count = 0;
  file_info.image_current_send_count = 0;
  file_info.image_last_count_len = 0;
  file_info.send_file_num = 0;
}

void lcd_process::reset_image_send_parameters(void)
{
  if(image_status.simage_delay_status) delay(IMAGE_SHOW_DELAY);
  memset(&file_info,0,sizeof(file_info));
  memset(&image_status,0,sizeof(image_status));
}

void lcd_process::get_image_data(int len)
{
  int16_t c;
  for(int i = 0; i < len; i++)
  {
  	c = udisk.get();
    if(-1 == c)
    {
      dwin_process.show_usb_pull_out_page();
      return;
    }
    send_data_buf[6+i] = c;
  }
}

void lcd_process::lcd_send_image(int len, int times,unsigned char cmd/*= WRITE_VARIABLE_ADDR*/)
{
	send_data_buf[0] = HEAD_ONE;
	send_data_buf[1] = HEAD_TWO;
	send_data_buf[2] = len+3;
	send_data_buf[3] = cmd;
	send_data_buf[4] = (0x80 + (125 * times)/256);
	send_data_buf[5] = (125 * times)%256;
  for(int i = 0; i < (len + 6); i++)
  {
    //MYSERIAL2.write(send_data_buf[i]);
    write_lcd_data(send_data_buf[i]);
  }
  clear_send_data_buf();
}

#endif // USB_DISK_SUPPORT
#endif // USE_DWIN_LCD
#endif // TARGET_LPC1768
