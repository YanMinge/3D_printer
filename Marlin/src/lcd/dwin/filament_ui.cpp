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
  if(progress_status.start_stop_status)
  {
    if(progress_status.load_return_status)
    {
      dwin_process.move_main_page();
    }
    else
    {
      dwin_process.lcd_send_data(PAGE_BASE +18, PAGE_ADDR);
    }
    reset_progress_status();
  }
}

void filament_ui_show::show_unload_end_page(void)
{
  if(progress_status.start_stop_status)
  {
    dwin_process.lcd_send_data(PAGE_BASE +18, PAGE_ADDR);
    reset_progress_status();
  }
}

void filament_ui_show::show_heat_prepare_page(void)
{
  dwin_process.lcd_send_data(PAGE_BASE +17, PAGE_ADDR);
}

void filament_ui_show::show_load_unload_start_page(void)
{
  if(progress_status.start_stop_status)
  {
    if(progress_status.load_return_status)
    {
      dwin_process.move_main_page();
      reset_progress_status();
    }
    else
    {
      if(progress_status.heat_cool_status) //load the filament
      {
        dwin_process.lcd_send_data(PAGE_BASE +15, PAGE_ADDR);
      }
      else //unload the filament
      {
        dwin_process.lcd_send_data(PAGE_BASE +19, PAGE_ADDR);
      }
    }
  }
}

#endif // USB_DISK_SUPPORT
#endif // USE_DWIN_LCD
#endif // TARGET_LPC1768