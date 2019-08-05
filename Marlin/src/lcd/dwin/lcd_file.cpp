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

#ifdef TARGET_LPC1768
#include "../Marlin.h"

#if ENABLED(USE_DWIN_LCD)
#include "lcd_file.h"

lcd_file LcdFile;

lcd_file::lcd_file()
{
  current_page = 0;
  current_status = out_printing;
  file_page_num = 0;
  last_page_file_count = 0;

  current_file_index = 0;
  directory_stack_init();
}

/**
 * @breif  create list head for file_list
 */
void lcd_file::file_list_init(void)
{
  pfile_list_t m;
  m =(pfile_list_t) new char[sizeof(file_list_t)];
  memset(m,0,sizeof(file_list_t));
  if(NULL == m)
  {
    return;
  }
  file_list = m;
  return;
}

/**
 * @breif  insert list a member after the linklist head
 */
void lcd_file::file_list_insert(pfile_list_t m)
{
  m->next_file = file_list->next_file;
  file_list->next_file = m;
}

/**
 * @breif  insert list a member in the linklist tail
 */
void lcd_file::file_list_insert_tail(pfile_list_t m)
{
  pfile_list_t t;
  t = file_list;
  while( t->next_file != NULL)
  {
    t = t->next_file;
  }
  t->next_file = m;
}

/**
 * @breif  delete a list member after the linklist head
 */
void lcd_file::file_list_del(void)
{
  pfile_list_t t;
  if(NULL == file_list->next_file)
  {
    return;
  }
  t = file_list->next_file;
  file_list->next_file = t->next_file;
  delete (t);
}

/**
 * @breif  delete all the member in the linklist,except the linklist head
 */
void lcd_file::file_list_clear(void)
{
  pfile_list_t t,temp;
  t = file_list->next_file;
  while( t != NULL)
  {
    temp = t;
    t = t->next_file;
    delete (temp);
  }
  file_list->next_file = NULL;
}

/**
 * @breif  get a linklist member through the index
 */
pfile_list_t lcd_file::file_list_index(int index)
{
  int i = 0;
  pfile_list_t t = NULL;
  t = file_list;
  while( (t != NULL) && (i != index))
  {
    i++;
    t = t->next_file;
  }
  if(i == index)
  {
    return t;
  }
  else
  {
    return NULL;
  }
}

bool lcd_file::file_list_is_empty(void)
{
  if(file_list->next_file == NULL)
  {
    return true;
  }
  else
  {
    return false;
  }
}

int lcd_file::file_list_len(void)
{
  int i = 0;
  pfile_list_t head = file_list->next_file;
  for(; head; head = head->next_file,i++);
  return i;
}

/**
 * @breif  set page info through the linklist
 * @detail  set file_page_num
 * @detail  set last_page_file_count
 */
void lcd_file::set_file_page_info(void)
{
  if(file_list_is_empty())
  {
    list_len = 0;
    file_page_num = 0;
    last_page_file_count = 0;
    return;
  }
  list_len = file_list_len();
  file_page_num = (list_len / PAGE_FILE_NUM);
  last_page_file_count = (list_len % PAGE_FILE_NUM);
  if(last_page_file_count)
  {
    file_page_num += 1;
  }
  else
  {
    last_page_file_count = PAGE_FILE_NUM;
  }
}

#endif // USE_DWIN_LCD
#endif // TARGET_LPC1768
