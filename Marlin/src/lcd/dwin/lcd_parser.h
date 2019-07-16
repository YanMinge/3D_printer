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

#ifndef _LCD_PARSER_H_
#define _LCD_PARSER_H_

#ifdef TARGET_LPC1768
#include "../Marlin.h"

#if ENABLED(USE_DWIN_LCD)
#include "dwin.h"
#include "lcd_process.h"

class lcd_parser
{
public:
  lcd_parser(void);
  virtual ~lcd_parser(void) { }

  void lcd_update();
  void parser_lcd_command(void);
  void get_command_type(void);

  void response_print_button(void);
  void response_return_button(void);
  void response_next_page_button(void);
  void response_last_page_button(void);
  void response_menu_file(void);
  void response_select_file(void);
  void response_print_file(void);
  void response_print_move_axis(void);
  void response_cancel_set_language(void);
  void response_set_language_ch(void);
  void response_set_language_en(void);
  void response_set_languag(void);
  void response_set_buzzer(void);
  void response_print_set(void);
  void response_filament(void);
  void response_print_machine_status(void);
  void next_path_fresh(char* name);
  bool last_path_fresh(void);

  inline int get_current_page_index(void){ return current_page_index;}
  inline void set_file_read_status(bool status){ file_read_status = status;}

  void refresh_current_path(void);
  int laser_focus;
private:
  lcd_cmd_type type;
  unsigned short receive_data;
  unsigned long receive_addr;

  char * current_path;

  bool file_read_status;
  int current_page_index;

};

extern lcd_parser dwin_parser;

#endif // USE_DWIN_LCD
#endif // TARGET_LPC1768
#endif // _LCD_PARSER_H_