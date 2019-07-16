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

class lcd_process
{
public:
  lcd_process();
  virtual ~lcd_process(void) { }

  void clear_lcd_data_buf(void);
  void clear_recevie_buf(void);
  void clear_send_data_buf(void);
  bool lcd_data_available(void);
  uint8_t read_lcd_data(void);
  void write_lcd_data(uint8_t c);
  uint8_t lcd_receive_data_correct(void);
  void lcd_receive_data(void);
  void lcd_receive_data_clear(void);
  void lcd_send_data(void);
  void lcd_send_data(const String &, unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void lcd_send_data(const char[], unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void lcd_text_clear(unsigned long addr,int len, unsigned char cmd = WRITE_VARIABLE_ADDR);
  void lcd_send_data(unsigned char*, unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void lcd_send_data(int, unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void lcd_send_data(unsigned int,unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void lcd_send_data(long,unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void lcd_send_data(unsigned long,unsigned long, unsigned char = WRITE_VARIABLE_ADDR);
  void lcd_show_picture(unsigned short n,unsigned short n1, unsigned long addr, unsigned char cmd = WRITE_VARIABLE_ADDR);

  void send_current_temperature(int tempbed, int temphotend);
  void send_file_list_page_num(int current_page, int page_num);
  void get_file_info(void);

  void temperature_progress_update(unsigned int percentage,int tempbed, int tempbedt, int temphotend, int temphotendt);

  inline bool is_have_command(){ return is_lcd_command;}
  inline void reset_command(){ is_lcd_command = false;}
  inline void reset_command_type(){ lcd_command_type = CMD_NULL;}
  inline lcd_cmd_type get_command_type(){      return lcd_command_type;}
  inline unsigned long get_receive_addr(){ return recive_data.addr;}
  inline unsigned short get_receive_data(){ return recive_data.data[0];}

  void lcd_send_image(int len,int times, unsigned char cmd = WRITE_VARIABLE_ADDR);
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
  void reset_usb_pull_out_parameters(void);
  void image_send_delay(void);
  void send_print_time(uint32_t time);

  language_type get_language_type(void);
  void set_language_type(language_type type);
  void get_image_data(int len);

  void set_machine_status(machine_status_type type){machine_status = type;}
  machine_status_type get_machine_status(void){return machine_status;}
  inline void delete_current_file(void){ delete (current_file);}
  inline void malloc_current_file(void){    current_file =(pfile_list_t) new char[sizeof(file_list_t)];
    memset(current_file,0,sizeof(file_list_t));}

  //show_page.cpp
  void show_start_up_page(void);
  void change_lcd_page(int en_page_num, int ch_page_num);
  void clear_page(unsigned long addr, unsigned char cmd = WRITE_VARIABLE_ADDR);
  inline void send_page(unsigned long addr,int page,int num, unsigned char cmd = WRITE_VARIABLE_ADDR);
  void send_first_page_data(void);
  void send_next_page_data(void);
  void send_last_page_data(void);
  void show_print_set_page(void);
  void show_laser_set_page(void);
  void show_machine_set_page(void);
  void show_machine_status(uint8_t ch_type);
  void show_machine_continue_print_page();
  void show_machine_recovery_print_page(void);
  void show_machine_status_page(machine_status_type print_status, machine_status_type laser_status, \
                                         int page_en, int page_ch);
  void show_machine_status_page(machine_status_type status, int page_en, int page_ch);
  void show_start_print_file_page(pfile_list_t temp);
  void show_stop_print_file_page(pfile_list_t temp);
  void show_usb_pull_out_page(void);
  void show_laser_prepare_focus_page(void);
  void process_lcd_subcommands_now(PGM_P pgcode);
  void laser_walking_frame(void);
  bool lcd_subcommand_status;
  pfile_list_t current_file; /*the file_struct which now the file is selected*/

private:
  bool is_lcd_command; /*whether receive a lcd command*/
  lcd_cmd_type lcd_command_type; /*the type of the lcd command*/

  millis_t update_time;
  unsigned int start_icon_count;

  //data
  lcd_buffer_t recive_data;
  lcd_buffer_t send_data;
  unsigned char receive_num;
  unsigned char recevie_data_buf[DATA_BUF_SIZE];
  unsigned char send_data_buf[MAX_SEND_BUF];

  //image show
  send_status_t image_status;
  data_info_t file_info;
  uint32_t offset;

  //languge
  language_type ui_language;
  machine_status_type machine_status;
};

extern lcd_process dwin_process;

#define  LCD_PAGE(A, B, C)  (A##B + C)
#define  LCD_PAGE_EN(A, B, C)  (A##B##C)
#define  SEND_PAGE(A, B, C, D)  do { \
  if( IS_HEAD_PRINT()) {dwin_process.lcd_send_data(LCD_PAGE(A, B, C), PAGE_ADDR);}\
  else if(IS_HEAD_LASER()){dwin_process.lcd_send_data(LCD_PAGE(D, B, C), PAGE_ADDR);}\
}while(0);

#define  CHANGE_PAGE(A, B, C, D, E)  do { \
  if( IS_HEAD_PRINT()) {dwin_process.change_lcd_page(LCD_PAGE_EN(A, C, D), LCD_PAGE_EN(A, C, E));}\
  else if(IS_HEAD_LASER()){dwin_process.change_lcd_page(LCD_PAGE_EN(B, C, D), LCD_PAGE_EN(B, C, E));}\
}while(0);

#endif // USE_DWIN_LCD
#endif // TARGET_LPC1768
#endif // _LCD_PROCESS_H_