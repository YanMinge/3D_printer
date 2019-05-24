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
 *    2.  void          lcd_file::file_list_insert(pfile_list_t m);
 *    3.  void          lcd_file::file_list_insert_tail(pfile_list_t m);
 *    4.  void          lcd_file::file_list_del(void);
 *    5.  void          lcd_file::file_list_clear(void);
 *    6.  void          lcd_file::get_file_page_count(void);
 *    7.  bool          lcd_file::file_list_is_empty(void);
 *    8.  int           lcd_file::file_list_len(void);
 *    9.  pfile_list_t  lcd_file::file_list_index(void);
 *    10. void          lcd_file::list_test(void);
 *    11. int           lcd_file::get_page_count(void);
 *    12. int           lcd_file::get_last_page_file_num(void);
 *
 * \par History:
 * <pre>
 * `<Author>`         `<Time>`        `<Version>`        `<Descr>`
 * Akingpan           2019/05/15       1.0.0              Initial function design.
 * </pre>
 *
 */

#ifndef _LCD_FILE_H_
#define _LCD_FILE_H_

#ifdef TARGET_LPC1768
#include "../Marlin.h"

#if ENABLED(USE_DWIN_LCD)
#include "dwin.h"

typedef struct usb_file
{
  bool IsDir;
  unsigned long Time;
  struct usb_file* next_file;
  char file_name[FILE_NAME_LEN];
}file_list_t, *pfile_list_t;

class lcd_file
{
public:

  lcd_file(void);
  virtual ~lcd_file(void) { }

  void file_list_init(void);
  void file_list_insert(pfile_list_t m);
  void file_list_insert_tail(pfile_list_t m);
  void file_list_del(void);
  void file_list_clear(void);
  void get_file_page_count(void);
  bool file_list_is_empty(void);
  int file_list_len(void);
  pfile_list_t file_list_index(int index);
  void list_test(void);

  int get_page_count(void);
  int get_last_page_file_num(void);

  int page_count;
  int last_page_file_count;

private:
  char sort;
  pfile_list_t file_list;
};

extern lcd_file LcdFile;

#endif // USE_DWIN_LCD
#endif // TARGET_LPC1768
#endif // _LCD_FILE_H_