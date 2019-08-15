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
#include "../../module/planner.h"
#include "../../gcode/gcode.h"

#if ENABLED(USE_DWIN_LCD)
#include "dwin.h"
#include "lcd_file.h"
#include "lcd_process.h"
#include "lcd_parser.h"

#if ENABLED(USB_DISK_SUPPORT)
#include "udisk_reader.h"
#include "user_execution.h"

#if PIN_EXISTS(BEEPER)
#include "../../libs/buzzer.h"
#endif

#if ENABLED(FACTORY_MACHINE_INFO)
#include "machine_info.h"
#endif

#if ENABLED(POWER_LOSS_RECOVERY)
#include "../../feature/power_loss_recovery.h"
#endif

void lcd_process::show_start_up_page(void)
{
  head_t head_machine;
  head_machine = MachineInfo.get_head_type();
  if(HEAD_NULL == head_machine && LAN_NULL == ui_language)
  {
    return;
  }
  if(LAN_NULL == ui_language)
  {
    lcd_send_data(PAGE_BASE + START_UP_LANGUAGE_SET_PAGE, PAGE_ADDR);
    machine_status = PRINT_MACHINE_STATUS_NO_SET_LANGUAGE;
    return;
  }
  if(HEAD_PRINT == head_machine)
  {
    change_lcd_page(PRINT_HOME_PAGE_EN,PRINT_HOME_PAGE_CH);
  }
  else if(HEAD_LASER == head_machine)
  {
    change_lcd_page(LASER_HOME_PAGE_EN,LASER_HOME_PAGE_CH);
  }
}

void lcd_process::change_lcd_page(int en_page_num, int ch_page_num)
{
  if(LAN_CHINESE == ui_language)
  {
    lcd_send_data(PAGE_BASE + ch_page_num, PAGE_ADDR);
  }
  else if(LAN_ENGLISH == ui_language)
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
  get_file_info();
  clear_page(PRINT_FILE_TEXT_BASE_ADDR);
  send_file_list_page_num(file_info.current_page + 1, file_info.page_count); //send page num to lcd

  if((file_info.page_count > 1) && file_info.current_page == 0)
  {
    send_page(PRINT_FILE_TEXT_BASE_ADDR,file_info.current_page,PAGE_FILE_NUM);  //send load icon to lcd
    SEND_PAGE(PRINT, _FILE_LIST_FIRST_PAGE, PAGE_BASE, LASER);  // show first file_list page
  }
  else if((file_info.page_count == 1) && file_info.current_page == 0)
  {
    if(0 == file_info.last_page_file)
    {
      file_info.last_page_file = PAGE_FILE_NUM;
    }
    send_page(PRINT_FILE_TEXT_BASE_ADDR,file_info.current_page,file_info.last_page_file);
    SEND_PAGE(PRINT, _FILE_LIST_ONLY_ONE_PAGE, PAGE_BASE, LASER);
  }
  else if((file_info.page_count == 0) && (file_info.current_page == 0))
  {
    SEND_PAGE(PRINT, _FILE_LIST_ONLY_ONE_PAGE, PAGE_BASE, LASER);
  }
  LcdFile.set_current_page(1);
}

void lcd_process::send_next_page_data(void)
{
  get_file_info();
  clear_page(PRINT_FILE_TEXT_BASE_ADDR);
  send_file_list_page_num(file_info.current_page + 1, file_info.page_count);

  if(file_info.page_count == (file_info.current_page + 1))
  {
    send_page(PRINT_FILE_TEXT_BASE_ADDR,file_info.current_page,file_info.last_page_file);
    //lcd_send_data(PAGE_BASE +PRINT_FILE_LIST_END_PAGE, PAGE_ADDR);
    SEND_PAGE(PRINT, _FILE_LIST_END_PAGE, PAGE_BASE, LASER);
    LcdFile.set_current_page(file_info.current_page + 1);
  }
  else if(file_info.page_count > file_info.current_page + 1)
  {
    send_page(PRINT_FILE_TEXT_BASE_ADDR,file_info.current_page,PAGE_FILE_NUM);
    //lcd_send_data(PAGE_BASE + PRINT_FILE_LIST_MIDDLE_PAGE, PAGE_ADDR);
    SEND_PAGE(PRINT, _FILE_LIST_MIDDLE_PAGE, PAGE_BASE, LASER);
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
      SEND_PAGE(PRINT, _FILE_LIST_FIRST_PAGE, PAGE_BASE, LASER);
    }
    else
    {
      SEND_PAGE(PRINT, _FILE_LIST_MIDDLE_PAGE, PAGE_BASE, LASER);
    }
  }
  else if((file_info.page_count == 2))
  {
    SEND_PAGE(PRINT, _FILE_LIST_FIRST_PAGE, PAGE_BASE, LASER);
  }
  LcdFile.set_current_page(file_info.current_page - 1);
  get_file_info();
  send_file_list_page_num(file_info.current_page, file_info.page_count);
}

void lcd_process::send_given_page_data(void)
{
  get_file_info();
  clear_page(PRINT_FILE_TEXT_BASE_ADDR);
  send_file_list_page_num(file_info.current_page, file_info.page_count);

  if(1 == file_info.current_page)
  {
    if(1 == file_info.page_count)
    {
      if(0 == file_info.last_page_file) file_info.last_page_file = PAGE_FILE_NUM;
      send_page(PRINT_FILE_TEXT_BASE_ADDR,file_info.current_page - 1,file_info.last_page_file);
      SEND_PAGE(PRINT, _FILE_LIST_ONLY_ONE_PAGE, PAGE_BASE, LASER);
    }
    else if(file_info.page_count >= 2)
    {
      send_page(PRINT_FILE_TEXT_BASE_ADDR,file_info.current_page - 1,PAGE_FILE_NUM);
      SEND_PAGE(PRINT, _FILE_LIST_FIRST_PAGE, PAGE_BASE, LASER);
    }
    else if(file_info.page_count == 0)
    {
      SEND_PAGE(PRINT, _FILE_LIST_ONLY_ONE_PAGE, PAGE_BASE, LASER);
    }
  }

  else if(file_info.current_page >= 2)
  {
    if(file_info.current_page == file_info.page_count)
    {
      send_page(PRINT_FILE_TEXT_BASE_ADDR,file_info.current_page - 1,file_info.last_page_file);
      SEND_PAGE(PRINT, _FILE_LIST_END_PAGE, PAGE_BASE, LASER);
    }
    else if(file_info.current_page < file_info.page_count)
    {
      send_page(PRINT_FILE_TEXT_BASE_ADDR,file_info.current_page - 1, PAGE_FILE_NUM);
      SEND_PAGE(PRINT, _FILE_LIST_MIDDLE_PAGE, PAGE_BASE, LASER);
    }
  }
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

void lcd_process::show_laser_set_page(void)
{
#if PIN_EXISTS(BEEPER)
  if(LAN_CHINESE == ui_language)
  {
    if(buzzer.get_buzzer_switch())
    {
      lcd_send_data(PAGE_BASE + LASER_BEEP_ON_SET_PAGE_CH, PAGE_ADDR);
    }
    else
    {
      lcd_send_data(PAGE_BASE + LASER_BEEP_OFF_SET_PAGE_CH, PAGE_ADDR);
    }
  }
  else if(LAN_ENGLISH == ui_language)
  {
    if(buzzer.get_buzzer_switch())
    {
      lcd_send_data(PAGE_BASE + LASER_BEEP_ON_SET_PAGE_EN, PAGE_ADDR);
    }
    else
    {
      lcd_send_data(PAGE_BASE + LASER_BEEP_OFF_SET_PAGE_EN, PAGE_ADDR);
    }
  }
#endif
}

void lcd_process::show_machine_set_page(void)
{
  if(HEAD_PRINT == MachineInfo.get_head_type())
  {
    dwin_process.show_print_set_page();
  }
  else
  {
    dwin_process.show_laser_set_page();
  }
}

void lcd_process::show_machine_status(uint8_t ch_type)
{

  if(LAN_CHINESE == ui_language)
  {
    if(ch_type <= MACHINE_STATUS_NUM)
      lcd_send_data(ch_type,PRINT_MACHINE_STATUS_ICON_ADDR);
    else
      lcd_send_data(ch_type - MACHINE_STATUS_NUM,PRINT_PREPARE_TEXT_ICON_ADDR);
  }
  else if(LAN_ENGLISH == ui_language)
  {
    lcd_send_data(ch_type + MACHINE_STATUS_NUM,PRINT_MACHINE_STATUS_ICON_ADDR);
  }
}

void lcd_process::show_machine_continue_print_page(void)
{
  if(HEAD_PRINT == MachineInfo.get_head_type())
  {
    show_machine_status(PRINT_MACHINE_STATUS_PRINT_CONTINUE_CH);
  }
  else
  {
    return;
  }
  change_lcd_page(PRINT_CONFIRM_CANCEL_HINT_PAGE_EN,PRINT_CONFIRM_CANCEL_HINT_PAGE_CH);
  machine_status = PRINT_MACHINE_STATUS_PRINT_CONTINUE_CH;
}

void lcd_process::show_machine_recovery_print_page(void)
{
  dwin_process.reset_image_send_parameters();
  LcdFile.set_current_status(out_printing);

#if ENABLED(POWER_LOSS_RECOVERY)
  if(udisk.job_recover_file_exists())
  {
    malloc_current_file();
    int16_t value = udisk.ls(LS_GET_FILE_NAME, dwin_parser.current_path, ".gcode");
    if(USB_NOT_DETECTED == value || value)
    {
      dwin_process.show_usb_pull_out_page();
      return;
    }
    dwin_parser.set_file_read_status(true);
    dwin_parser.set_file_list_open_status(true);
    LcdFile.set_file_page_info();
    LcdFile.set_current_page(1);

    strcpy(current_file->file_name,recovery.info.udisk_filename);
    if(udisk.check_gm_file(current_file->file_name))
    {
      current_file->file_type = TYPE_MAKEBLOCK_GM;
      dwin_process.lcd_text_clear(PRINT_FILE_PRINT_TEXT_ADDR, FILE_NAME_LCD_LEN);
      dwin_process.lcd_send_data(current_file->file_name,PRINT_FILE_PRINT_TEXT_ADDR);
      CHANGE_PAGE(PRINT, LASER, _FILE_PRINT_STANDARD_START_PAGE_, EN, CH);
      dwin_process.limage_send_start();
    }
    else
    {
      current_file->file_type = TYPE_DEFAULT_FILE;
      dwin_process.lcd_send_data(TYPE_DEFAULT_FILE,PRINT_FILE_LIMAGE_ICON_ADDR);
      dwin_process.lcd_text_clear(PRINT_FILE_PRINT_TEXT_ADDR, FILE_NAME_LCD_LEN);
      dwin_process.lcd_send_data(current_file->file_name,PRINT_FILE_PRINT_TEXT_ADDR);
      CHANGE_PAGE(PRINT, LASER, _FILE_PRINT_NOSTANDARD_START_PAGE_, EN, CH);
    }
  }
#endif
}
void lcd_process::show_machine_status_page(machine_status_type print_status, \
                                                    machine_status_type laser_status, int page_en, int page_ch)
{
  if(HEAD_PRINT == MachineInfo.get_head_type())
  {
    show_machine_status(print_status);
    machine_status = print_status;
  }
  else
  {
    show_machine_status(laser_status);
    machine_status = laser_status;
  }
  change_lcd_page(page_en,page_ch);
}

void lcd_process::show_machine_status_page(machine_status_type status, int page_en, int page_ch)
{
  show_machine_status(status);
  machine_status = status;
  change_lcd_page(page_en,page_ch);
}

void lcd_process::show_start_print_file_page(pfile_list_t temp)
{
  if( TYPE_DEFAULT_FILE == temp->file_type)
  {
    CHANGE_PAGE(PRINT, LASER, _FILE_PRINT_NOSTANDARD_START_PAGE_, EN, CH)
  }
  else
  {
    CHANGE_PAGE(PRINT, LASER, _FILE_PRINT_STANDARD_START_PAGE_, EN, CH)
    lcd_show_picture(PRINT_LIMAGE_X_POSITION,PRINT_LIMAGE_Y_POSITION,PICTURE_ADDR,0X82);
  }
}

void lcd_process::show_stop_print_file_page(pfile_list_t temp)
{
  if(machine_status == PRINT_MACHINE_STATUS_PRINT_CONTINUE_CH)
  {
    //send file name ,image to the page;
  }
  if( TYPE_DEFAULT_FILE == temp->file_type)
  {
    CHANGE_PAGE(PRINT, LASER, _FILE_PRINT_NOSTANDARD_STOP_PAGE_, EN, CH)
  }
  else
  {
    CHANGE_PAGE(PRINT, LASER, _FILE_PRINT_STANDARD_STOP_PAGE_, EN, CH)
    lcd_show_picture(PRINT_LIMAGE_X_POSITION,PRINT_LIMAGE_Y_POSITION,PICTURE_ADDR,0X82);
  }
}

void lcd_process::show_usb_pull_out_page(void)
{
  dwin_parser.set_file_list_open_status(false);
  dwin_parser.refresh_current_path();
  LcdFile.directory_stack_init();

  reset_image_send_parameters();
  show_machine_status(PRINT_MACHINE_STATUS_NO_USB_DISK_CH);
  set_machine_status(PRINT_MACHINE_STATUS_NO_USB_DISK_CH);
  change_lcd_page(EXCEPTION_SURE_HINT_PAGE_EN,EXCEPTION_SURE_HINT_PAGE_CH);
}

void lcd_process::show_no_firmware_page(void)
{
  show_machine_status(PRINT_MACHINE_STATUS_NO_UPDATE_FILE_CH);
  set_machine_status(PRINT_MACHINE_STATUS_NO_UPDATE_FILE_CH);
  change_lcd_page(EXCEPTION_SURE_HINT_PAGE_EN,EXCEPTION_SURE_HINT_PAGE_CH);
}

void lcd_process::show_firmware_upate_page(void)
{
  temperature_progress_update(0);
  send_progress_percentage(0);
  change_lcd_page(PRINT_UPDATE_PAGE_EN,PRINT_UPDATE_PAGE_CH);

  show_machine_status(PRINT_MACHINE_STATUS_NO_UPDATE_FILE_CH);
  set_machine_status(PRINT_MACHINE_STATUS_NO_UPDATE_FILE_CH);
  change_lcd_page(EXCEPTION_SURE_HINT_PAGE_EN,EXCEPTION_SURE_HINT_PAGE_CH);
}

#endif // USB_DISK_SUPPORT
#endif // USE_DWIN_LCD
#endif // TARGET_LPC1768
