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

  CMD_FILAMENT,
};

enum language_type : uint8_t {
  LAN_NULL = 0,
  LAN_CHINESE,
  LAN_ENGLISH,
};

enum usb_status : char {
  USB_NO_INSERT = 0,
  USB_INSERT,
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

typedef struct
{
  uint8_t image_send_count; /*the number of times needed for send a picture*/
  uint8_t image_current_send_count; /*the number of times have already sended*/
  uint8_t image_last_count_len; /*the length of bytes needed to be send in the last send var(0-250)*/
  uint8_t send_file_num; /*the number of pictures have already send  var(0-4)*/
  uint8_t select_file_num; /*file number of the current page which is clicked*/

  int page_count; /*the number of page need to show in lcd pannel*/
  int current_page; /*the page number which now showing in lcd pannel*/
  int last_page_file; /*the number of files in last page*/
  int current_index; /*the file number which now is selected*/
} data_info_t;

typedef struct
{
  bool simage_status;
  bool limage_status;
  bool simage_send_status;
  bool limage_send_status;
  bool simage_set_status;
  bool limage_set_status;
  bool simage_delay_status;
} send_status_t;

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
  void lcd_text_clear(unsigned long addr,int len, unsigned char cmd = WRITE_VARIABLE_ADDR);
  void lcd_send_data(char, unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void lcd_send_data(unsigned char*, unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void lcd_send_data(int, unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void lcd_send_data(float, unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void lcd_send_data(unsigned int,unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void lcd_send_data(long,unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void lcd_send_data(unsigned long,unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void lcd_show_picture(unsigned short n,unsigned short n1, unsigned long addr, unsigned char cmd = WRITE_VARIABLE_ADDR);

  void lcd_send_temperature(int tempbed, int tempbedt, int temphotend, int temphotendt);
  void send_current_temperature(int tempbed, int temphotend);

  inline void clear_page(unsigned long addr, unsigned char cmd = WRITE_VARIABLE_ADDR);
  inline void send_page(unsigned long addr,int page,int num, unsigned char cmd = WRITE_VARIABLE_ADDR);
  void get_file_info(void);

  void icon_update(void);
  void temperature_progress_update(unsigned int percentage,int tempbed, int tempbedt, int temphotend, int temphotendt);
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
  void send_simage(void);
  void send_limage(void);
  void lcd_loop(void);
  void send_temperature_percentage(uint16_t percentage);

  void set_limage_count(void);
  void set_simage_count(void);
  void set_simage_status(bool status){ image_status.simage_status = status;}
  void set_limage_status(bool status){ image_status.limage_status = status;}
  bool get_limage_status(void){ return image_status.limage_status;}
  void set_simage_send_status(bool status){ image_status.simage_send_status = status;}
  void set_limage_send_status(bool status){ image_status.limage_send_status = status;}
  void set_simage_set_status(bool status){ image_status.simage_set_status = status;}
  void set_limage_set_status(bool status){ image_status.limage_set_status = status;}
  void simage_send_start(){ image_status.simage_status = true;image_status.simage_set_status = true;}
  void limage_send_start(){ image_status.limage_status = true;image_status.limage_set_status = true;}
  void simage_send_end(){ image_status.simage_status = false;image_status.simage_set_status = false;}
  void limage_send_end(){ image_status.limage_status = false;image_status.limage_set_status = false;}
  void set_select_file_num(uint8_t index){ file_info.select_file_num = index;}
  void reset_image_parameters(void);
  void image_send_delay(void);
  void send_print_time(uint32_t time);

  language_type get_language_type(void);
  void set_language_type(language_type type);
  void move_main_page(void);
  void move_usb_hint_page(void);
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
  language_type ui_language;

  //image show
  send_status_t image_status;

  data_info_t file_info;
  pfile_list_t current_file; /*the file_struct which now the file is selected*/

  uint32_t offset;
};

extern lcd_process dwin_process;

#endif // USE_DWIN_LCD
#endif // TARGET_LPC1768
#endif // _LCD_PROCESS_H_