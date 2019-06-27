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
 *    1.  void          lcd_file::file_list_init(void);
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
#include "filament_ui.h"

#if ENABLED(USB_DISK_SUPPORT)
#include "udisk_reader.h"
#include "user_execution.h"

filament_ui_show filament_show;

filament_ui_show::filament_ui_show()
{
  memset(&progress_status,0,sizeof(progress_show_status_t));
}

void filament_ui_show::show_load_end_page(void)
{
  if(PRINT_MACHINE_STATUS_NO_FILAMENT_CH == dwin_process.get_machine_status())
  {
    dwin_process.show_machine_status(PRINT_MACHINE_STATUS_NO_FILAMENT_CH);
    dwin_process.change_lcd_page(EXCEPTION_SURE_HINT_PAGE_EN, EXCEPTION_SURE_HINT_PAGE_CH);
    return;
  }
  if(PRINT_MACHINE_STATUS_UNKNOW_ERROR_CH == dwin_process.get_machine_status())
  {
    dwin_process.show_machine_status(PRINT_MACHINE_STATUS_UNKNOW_ERROR_CH);
    dwin_process.change_lcd_page(EXCEPTION_SURE_HINT_PAGE_EN, EXCEPTION_SURE_HINT_PAGE_CH);
    return;
  }
  if(progress_status.start_stop_status)
  {
    dwin_process.change_lcd_page(EXCEPTION_SURE_HINT_PAGE_EN,EXCEPTION_SURE_HINT_PAGE_CH);
    dwin_process.show_machine_status(PRINT_MACHINE_STATUS_LOAD_FILAMENT_SUCCESS_CH);
    dwin_process.set_machine_status(PRINT_MACHINE_STATUS_LOAD_FILAMENT_SUCCESS_CH);

    reset_progress_status();
    UserExecution.cmd_M300(300,500);
    UserExecution.cmd_M300(500,1000);
  }
}

void filament_ui_show::show_unload_end_page(void)
{
  if(progress_status.start_stop_status)
  {
    dwin_process.change_lcd_page(EXCEPTION_SURE_HINT_PAGE_EN,EXCEPTION_SURE_HINT_PAGE_CH);
    dwin_process.show_machine_status(PRINT_MACHINE_STATUS_UNLOAD_SUCCESS_CH);
    dwin_process.set_machine_status(PRINT_MACHINE_STATUS_UNLOAD_SUCCESS_CH);
    reset_progress_status();
  }
  UserExecution.cmd_M2034(true);
  UserExecution.cmd_M2032(true);
}

void filament_ui_show::show_heat_prepare_page(void)
{
  dwin_process.lcd_send_data(PAGE_BASE +17, PAGE_ADDR);
}

void filament_ui_show::show_file_prepare_page(void)
{
  dwin_process.lcd_send_data(PAGE_BASE +21, PAGE_ADDR);
}

void filament_ui_show::show_file_print_end_page(void)
{
  if(progress_status.file_print_status)
  {
    pfile_list_t temp = NULL;
    temp = LcdFile.file_list_index(dwin_parser.get_current_page_index());
    if(PRINT_MACHINE_STATUS_NO_FILAMENT_CH == dwin_process.get_machine_status())
    {
      dwin_process.show_machine_status(PRINT_MACHINE_STATUS_NO_FILAMENT_CH);
      dwin_process.change_lcd_page(EXCEPTION_SURE_HINT_PAGE_EN, EXCEPTION_SURE_HINT_PAGE_CH);
      return;
    }
    if(progress_status.load_return_status)
    {
      dwin_process.show_start_print_file_page(temp);
      LcdFile.set_current_status(out_printing);
    }
    else
    {
      dwin_process.show_stop_print_file_page(temp);
      LcdFile.set_current_status(on_printing);
      UserExecution.cmd_M2023(temp->file_name);
      UserExecution.cmd_M2024();
    }
    reset_progress_status();
  }
}

void filament_ui_show::show_load_unload_start_page(void)
{
  if(PRINT_MACHINE_STATUS_NO_FILAMENT_CH == dwin_process.get_machine_status())
  {
    dwin_process.show_machine_status(PRINT_MACHINE_STATUS_NO_FILAMENT_CH);
    dwin_process.change_lcd_page(EXCEPTION_SURE_HINT_PAGE_EN, EXCEPTION_SURE_HINT_PAGE_CH);
    return;
  }
  if(PRINT_MACHINE_STATUS_UNKNOW_ERROR_CH == dwin_process.get_machine_status())
  {
    dwin_process.show_machine_status(PRINT_MACHINE_STATUS_NO_FILAMENT_CH);
    dwin_process.change_lcd_page(EXCEPTION_SURE_HINT_PAGE_EN, EXCEPTION_SURE_HINT_PAGE_CH);
    return;
  }
  if(progress_status.start_stop_status)
  {
    if(progress_status.load_return_status)
    {
      dwin_process.change_lcd_page(PRINT_HOME_PAGE_EN,PRINT_HOME_PAGE_CH);
      reset_progress_status();
    }
    else
    {
      if(progress_status.heat_cool_status) //load the filament
      {
        dwin_process.change_lcd_page(EXCEPTION_COMPLETE_HINT_PAGE_EN,EXCEPTION_COMPLETE_HINT_PAGE_CH);
        dwin_process.show_machine_status(PRINT_MACHINE_STATUS_LOAD_FILAMENT_CH);
        dwin_process.set_machine_status(PRINT_MACHINE_STATUS_LOAD_FILAMENT_CH);
      }
      else //unload the filament
      {
        dwin_process.change_lcd_page(PRINT_PREPARE_HEAT_PAGE,PRINT_PREPARE_HEAT_PAGE);
        dwin_process.lcd_send_data(4,PRINT_PREPARE_TEXT_ICON_ADDR);
      }
    }
    UserExecution.cmd_M2034(true);
  }
}

#endif // USB_DISK_SUPPORT
#endif // USE_DWIN_LCD
#endif // TARGET_LPC1768
