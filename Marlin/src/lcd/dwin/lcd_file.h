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
 *    6.  void          lcd_file::set_file_page_info(void);
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

enum file_type_t : uint8_t
{
  TYPE_NULL = 0,
  TYPE_LOAD,
  TYPE_DEFAULT_FILE,
  TYPE_FOLDER,
  TYPE_MAKEBLOCK_GM,
  TYPE_FIRMWARE,
};

typedef struct usb_file
{
  file_type_t file_type;
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
  void set_file_page_info(void); //set the page o
  bool file_list_is_empty(void);
  int file_list_len(void);
  pfile_list_t file_list_index(int index);
  void list_test(void);
  bool set_current_page_via_filename(char * file_name);

  inline int get_page_count(){     return file_page_num;}
  inline int get_last_page_file_num(){return last_page_file_count;}
  inline int get_current_page_num(){ return current_page;}
  inline int get_file_list_len(){ return list_len;}
  inline void set_current_page(int num){ current_page = num;}
  inline void set_current_status(print_status status){ current_status = status;}
  inline print_status get_current_status(){ return current_status;}
  inline int get_current_index(){ return (current_page - 1)*PAGE_FILE_NUM;}

  void directory_stack_init(void)
  {
    memset(&page_directory, 0, sizeof(page_directory));
    page_directory.top = -1;
  }
  bool directory_stack_is_full(void)
  {
    if(page_directory.top == FILE_DIRECTORY_DEPTH - 1) return true;
    else return false;
  }
  bool directory_stack_is_empty(void)
  {
    if(page_directory.top ==  - 1) return true;
    else return false;
  }
  void directory_stack_push(void)
  {
    if(directory_stack_is_full()) return;
    page_directory.top ++;
    page_directory.page_num[page_directory.top] = current_page;
  }
  void directory_stack_pop(void)
  {
    if(directory_stack_is_empty())
    {
      current_page = 1;
      return;
    }
    current_page = page_directory.page_num[page_directory.top];
    page_directory.top --;
  }

private:
  char sort;
  int file_page_num; /* number of page that show in lcd*/
  int current_page; /* the current page that show in lcd*/
  int last_page_file_count; /* number of the last page files*/

  int list_len; /* number of files in the linklist*/
  int current_file_index;
  print_status current_status;

  pfile_list_t file_list; /*file linklist object*/

  file_directory_stack_t page_directory;
};

extern lcd_file LcdFile;

#endif // USE_DWIN_LCD
#endif // TARGET_LPC1768
#endif // _LCD_FILE_H_