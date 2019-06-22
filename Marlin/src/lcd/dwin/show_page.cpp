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

#if PIN_EXISTS(BEEPER)
#include "../../libs/buzzer.h"
#endif

void lcd_process::show_start_up_page(void)
{
  DEBUGPRINTF("print start_up_page\r\n");
  switch (ui_machine)
  {
    case MACHINE_TYPE_NULL:
      lcd_send_data(PAGE_BASE + EXCEPTION_NO_HEAD_PAGE_CH, PAGE_ADDR);
      break;

    case MACHINE_TYPE_PRINT:
      change_lcd_page(PRINT_HOME_PAGE_EN,PRINT_HOME_PAGE_CH);
      break;

    case MACHINE_TYPE_LASER:
      change_lcd_page(LASER_HOME_PAGE_EN,LASER_HOME_PAGE_CH);
      break;

    default:
      break;
  }
}

void lcd_process::change_lcd_page(int en_page_num, int ch_page_num)
{
  if(LAN_NULL == ui_language)
  {
    lcd_send_data(PAGE_BASE + START_UP_LANGUAGE_SET_PAGE, PAGE_ADDR);
    machine_status = PRINT_MACHINE_STATUS_NO_SET_LANGUAGE;
  }
  else if(LAN_CHINESE == ui_language)
  {
    lcd_send_data(PAGE_BASE + ch_page_num, PAGE_ADDR);
  }
  else if(LAN_NULL == ui_language)
  {
    lcd_send_data(PAGE_BASE + en_page_num, PAGE_ADDR);
  }
}

void lcd_process::clear_page(unsigned long addr, unsigned char cmd/*= WRITE_VARIABLE_ADDR*/)
{
  for(int i = 0; i < PAGE_FILE_NUM; i++)
  {
    lcd_text_clear((addr + FILE_TEXT_LEN*i), FILE_NAME_LCD_LEN);
    lcd_send_data(TYPE_NULL,(PRINT_FILE_SIMAGE_ICON_ADDR + i));
  }
}

inline void lcd_process::send_page(unsigned long addr,int page,int num, unsigned char cmd/*= WRITE_VARIABLE_ADDR*/)
{
#define PAGE(a,b) (a - PAGE_FILE_NUM*b)

  pfile_list_t temp = NULL;
  for(int i = page*PAGE_FILE_NUM; i < (page*PAGE_FILE_NUM + num); i++)
  {
    temp = LcdFile.file_list_index((i+1));
    lcd_send_data(TYPE_LOAD,(PRINT_FILE_SIMAGE_ICON_ADDR + PAGE(i,page)));
    lcd_send_data(temp->file_name,addr + FILE_TEXT_LEN*PAGE(i,page));
  }
}

void lcd_process::send_first_page_data(void)
{
  DEBUGPRINTF("fil_page = %d, last_page = %d\r\n",file_info.page_count,file_info.last_page_file);

  get_file_info();
  clear_page(PRINT_FILE_TEXT_BASE_ADDR);

  if((file_info.page_count > 1) && file_info.current_page == 0)
  {
    send_page(PRINT_FILE_TEXT_BASE_ADDR,file_info.current_page,PAGE_FILE_NUM);
    lcd_send_data(PAGE_BASE +PRINT_FILE_LIST_FIRST_PAGE, PAGE_ADDR);
  }
  else if((file_info.page_count == 1) && file_info.current_page == 0)
  {
    if(0 == file_info.last_page_file)
    {
      file_info.last_page_file = PAGE_FILE_NUM;
    }
    send_page(PRINT_FILE_TEXT_BASE_ADDR,file_info.current_page,file_info.last_page_file);
    lcd_send_data(PAGE_BASE + PRINT_FILE_LIST_ONLY_ONE_PAGE, PAGE_ADDR);
  }
  else if((file_info.page_count == 0) && (file_info.current_page == 0))
  {
    lcd_send_data(PAGE_BASE + PRINT_FILE_LIST_ONLY_ONE_PAGE, PAGE_ADDR);
  }
  LcdFile.set_current_page(1);
}

void lcd_process::send_next_page_data(void)
{
  get_file_info();
  clear_page(PRINT_FILE_TEXT_BASE_ADDR);

  if(file_info.page_count == (file_info.current_page + 1))
  {
    send_page(PRINT_FILE_TEXT_BASE_ADDR,file_info.current_page,file_info.last_page_file);
    lcd_send_data(PAGE_BASE +PRINT_FILE_LIST_END_PAGE, PAGE_ADDR);
    LcdFile.set_current_page(file_info.current_page + 1);
  }
  else if(file_info.page_count > file_info.current_page + 1)
  {
    send_page(PRINT_FILE_TEXT_BASE_ADDR,file_info.current_page,PAGE_FILE_NUM);
    lcd_send_data(PAGE_BASE + PRINT_FILE_LIST_MIDDLE_PAGE, PAGE_ADDR);
    LcdFile.set_current_page(file_info.current_page + 1);
  }
}

void lcd_process::send_last_page_data(void)
{
  get_file_info();

  clear_page(PRINT_FILE_TEXT_BASE_ADDR);
  send_page(PRINT_FILE_TEXT_BASE_ADDR,file_info.current_page-2,PAGE_FILE_NUM);
  if((file_info.page_count > 2))
  {
    if(file_info.current_page == 2)
    {
      lcd_send_data(PAGE_BASE + PRINT_FILE_LIST_FIRST_PAGE, PAGE_ADDR);
    }
    else
    {
      lcd_send_data(PAGE_BASE +PRINT_FILE_LIST_MIDDLE_PAGE, PAGE_ADDR);
    }
  }
  else if((file_info.page_count == 2))
  {
    lcd_send_data(PAGE_BASE +PRINT_FILE_LIST_FIRST_PAGE, PAGE_ADDR);
  }
  LcdFile.set_current_page(file_info.current_page - 1);
}

void lcd_process::show_print_set_page(void)
{
#if PIN_EXISTS(BEEPER)
  if(LAN_CHINESE == ui_language)
  {
    if(buzzer.get_buzzer_switch())
    {
      lcd_send_data(PAGE_BASE + PRINT_BEEP_ON_SET_PAGE_CH, PAGE_ADDR);
    }
    else
    {
      lcd_send_data(PAGE_BASE + PRINT_BEEP_OFF_SET_PAGE_CH, PAGE_ADDR);
    }
  }
  else if(LAN_ENGLISH == ui_language)
  {
    if(buzzer.get_buzzer_switch())
    {
      lcd_send_data(PAGE_BASE + PRINT_BEEP_ON_SET_PAGE_EN, PAGE_ADDR);
    }
    else
    {
      lcd_send_data(PAGE_BASE + PRINT_BEEP_OFF_SET_PAGE_EN, PAGE_ADDR);
    }
  }
#endif
}

void lcd_process::show_machine_status(uint8_t ch_type)
{

  if(LAN_CHINESE == ui_language)
  {
    lcd_send_data(ch_type,PRINT_MACHINE_STATUS_ICON_ADDR);
  }
  else if(LAN_ENGLISH == ui_language)
  {
    lcd_send_data(ch_type + MACHINE_STATUS_NUM,PRINT_MACHINE_STATUS_ICON_ADDR);
  }
  if(LAN_NULL == ui_language)
  {
    lcd_send_data(PAGE_BASE + START_UP_LANGUAGE_SET_PAGE, PAGE_ADDR);
    machine_status = PRINT_MACHINE_STATUS_NO_SET_LANGUAGE;
  }
}

void lcd_process::show_start_print_file_page(pfile_list_t temp)
{
  if( TYPE_DEFAULT_FILE == temp->file_type)
  {
    change_lcd_page(PRINT_FILE_PRINT_NOSTANDARD_START_PAGE_EN,PRINT_FILE_PRINT_NOSTANDARD_START_PAGE_CH);
  }
  else
  {
    change_lcd_page(PRINT_FILE_PRINT_STANDARD_START_PAGE_EN,PRINT_FILE_PRINT_STANDARD_START_PAGE_CH);
    lcd_show_picture(PRINT_LIMAGE_X_POSITION,PRINT_LIMAGE_Y_POSITION,PICTURE_ADDR,0X82);
  }
}

void lcd_process::show_stop_print_file_page(pfile_list_t temp)
{
  if( TYPE_DEFAULT_FILE == temp->file_type)
  {
    change_lcd_page(PRINT_FILE_PRINT_NOSTANDARD_STOP_PAGE_EN,PRINT_FILE_PRINT_NOSTANDARD_STOP_PAGE_CH);
  }
  else
  {
    change_lcd_page(PRINT_FILE_PRINT_STANDARD_STOP_PAGE_EN,PRINT_FILE_PRINT_STANDARD_STOP_PAGE_CH);
    lcd_show_picture(PRINT_LIMAGE_X_POSITION,PRINT_LIMAGE_Y_POSITION,PICTURE_ADDR,0X82);
  }
}


#endif // USB_DISK_SUPPORT
#endif // USE_DWIN_LCD
#endif // TARGET_LPC1768
