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

#ifndef _FILAMENT_UI_H_
#define _FILAMENT_UI_H_

#ifdef TARGET_LPC1768
#include "../Marlin.h"

#if ENABLED(USE_DWIN_LCD)
#include "dwin.h"

typedef struct
{
  bool start_stop_status;
  bool load_return_status;
  bool heat_cool_status;
} progress_show_status_t;

class filament_ui_show
{
public:
  filament_ui_show();
  virtual ~filament_ui_show(void) { }

  void show_heat_prepare_page(void);
  void show_load_unload_start_page(void);
  void show_load_end_page(void);
  void show_unload_end_page(void);

  void reset_progress_status(void){memset(&progress_status,0,sizeof(progress_show_status_t)); }
  void set_progress_start_status(bool status){ progress_status.start_stop_status         = status;}
  void set_progress_load_return_status(bool status){  progress_status.load_return_status = status        ;}
  void set_progress_heat_cool_status(bool status){  progress_status.heat_cool_status = status        ;}
  bool get_progress_start_status(void){ return progress_status.start_stop_status        ;}
  bool get_progress_load_return_status(void){ return progress_status.load_return_status         ;}
  bool get_progress_heat_cool_status(void){ return progress_status.heat_cool_status         ;}

private:
  progress_show_status_t progress_status;
};

extern filament_ui_show filament_show;
#endif // _FILAMENT_UI_H_
#endif // TARGET_LPC1768
#endif // _FILAMENT_UI_H_