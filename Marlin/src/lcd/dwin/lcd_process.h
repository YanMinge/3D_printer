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

#ifndef _LCD_PROCESS_H_
#define _LCD_PROCESS_H_

#ifdef TARGET_LPC1768
#include "../Marlin.h"

#if ENABLED(USE_DWIN_LCD)
#include "dwin.h"
#include "lcd_file.h"

enum lcd_cmd_type : unsigned char {
  CMD_NULL,
  CMD_ERROR,
  CMD_OK,
  CMD_WRITE_REG_OK,
  CMD_WRITE_VAR_OK,
  CMD_READ_REG,
  CMD_READ_VAR,

  CMD_MENU_FILE,
  CMD_SELECT_FILE,
  CMD_PRINT_FILE,
  CMD_AXIS_MOVE,
  CMD_SET_LANGUAGE,
};

typedef struct lcd_data_buffer
{
    unsigned char head[2];
    unsigned char len;
    unsigned char command;
    unsigned long addr;
    unsigned long bytelen;
    unsigned short data[32];
} lcd_buffer_t;

class lcd_process
{
public:
  lcd_process();
  virtual ~lcd_process(void) { }

  void clear_lcd_data_buf(void);
  void clear_lcd_data_buf1(void);
  void clear_recevie_buf(void);
  void clear_send_data_buf(void);
  void lcd_receive_data(void);
  void lcd_receive_data_clear(void);
  void lcd_send_data(void);
  void lcd_send_data(const String &, unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void lcd_send_data(const char[], unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void lcd_send_data_clear(unsigned long addr,int len, unsigned char cmd = WRITE_VARIABLE_ADDR);
  void lcd_send_data(char, unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void lcd_send_data(unsigned char*, unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void lcd_send_data(int, unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void lcd_send_data(float, unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void lcd_send_data(unsigned int,unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void lcd_send_data(long,unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void lcd_send_data(unsigned long,unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void lcd_show_picture(unsigned short n,unsigned short n1, unsigned long addr, unsigned char cmd = WRITE_VARIABLE_ADDR);

  void lcd_send_temperature(int tempbed, int tempbedt, int temphotend, int temphotendt);

  inline void clear_page(unsigned long addr, unsigned char cmd = WRITE_VARIABLE_ADDR);
  inline void send_page(unsigned long addr,int page,int num, unsigned char cmd = WRITE_VARIABLE_ADDR);

  void icon_update(void);
  void send_first_page_data(void);
  void send_next_page_data(void);
  void send_last_page_data(void);

  inline bool is_have_command(){ return is_command;}
  inline void reset_command(){ is_command = 0;}
  inline void reset_command_type(){ type = CMD_NULL;}
  inline lcd_cmd_type get_command_type(){      return type;}
  inline unsigned long get_receive_addr(){ return recive_data.addr;}
  inline unsigned short get_receive_data(){ return recive_data.data[0];}

  void lcd_send_image_test(int len,int times, unsigned char cmd = WRITE_VARIABLE_ADDR);
  void send_image(void);
  void lcd_loop(void);
  void set_image_count(void);
  void set_loop_status(bool status){ loop_status = status;}
  void set_file_status(bool status){ file_status = status;}
  void reset_image_parameters(void);

  uint8_t get_language_type(void);
  void set_language_type(unsigned char type);
  void language_init(void);
  void get_image_data(int len);

private:
  bool is_command; /*whether receive a lcd command*/
  lcd_cmd_type type; /*the type of the lcd command*/

  bool icon_update_status;
  millis_t update_time;
  unsigned int start_icon_count;

  //data
  lcd_buffer_t recive_data;
  lcd_buffer_t send_data;
  unsigned char receive_num;
  unsigned char recevie_data_buf[DATA_BUF_SIZE];
  unsigned char send_data_buf[MAX_SEND_BUF];

  //languge
  uint8_t language_type;

  //image show
  bool loop_status;
  bool file_status;
  uint8_t image_send_count;
  uint8_t image_current_send_count;
  uint8_t image_last_count_len;
  uint8_t send_file_num;

  //file_list_t
  pfile_list_t current_file;
  uint32_t offset;
};

extern lcd_process dwin_process;

#endif // USE_DWIN_LCD
#endif // TARGET_LPC1768
#endif // _LCD_PROCESS_H_