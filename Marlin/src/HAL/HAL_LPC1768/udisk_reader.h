/**
 * \par Copyright (C), 2018-2019, MakeBlock
 * \class   udisk_reader
 * \brief   USB HOST mass storge driver.
 * @file    udisk_reader.h
 * @author  Mark Yan
 * @version V1.0.0
 * @date    2019/05/15
 * @brief   Header file for USB HOST mass storge driver.
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
 * This file is a drive for usb host mass storge.
 *
 * \par Method List:
 *
 *    1.  void     udisk_reader::init(void);
 *    2.  bool     udisk_reader::is_usb_detected(void);
 *    3.  bool     udisk_reader::is_usb_Initialized(void);
 *    4.  void     udisk_reader::usb_status_polling(void);
 *    5.  uint16_t udisk_reader::ls(is_action_t action, const char *path = "", const char * const match = NULL);
 *    6.  void     udisk_reader::file_list_sort(void);
 *    7.  uint16_t udisk_reader::is_dive(const char *path = "", const char * const match = NULL);
 *    8.  uint16_t udisk_reader::get_num_Files(const char *path = "", const char * const match = NULL);
 *    9.  char *   udisk_reader::get_file_name(void);
 *    10. void     udisk_reader::open_file(char * const path, const bool read);
 *    11. void     udisk_reader::print_file_name();
 *    12. void     udisk_reader::report_status();
 *    13. int16_t  udisk_reader::get(void);
 *    14. void     udisk_reader::start_udisk_print(void);
 *    15. void     udisk_reader::pause_udisk_print(void);
 *    16. void     udisk_reader::stop_udisk_print(void);
 *    17. void     udisk_reader::printing_has_finished();
 *    18. bool     udisk_reader::get_udisk_printing_flag(void);
 *    19. bool     udisk_reader::is_gm_file_type(char * const path);
 *    20. bool     udisk_reader::check_gm_file(char * const path);
 *    21. uint32_t udisk_reader::get_simage_size(char * const path);
 *    22. uint32_t udisk_reader::get_simage_offset(char * const path);
 *    23. uint32_t udisk_reader::get_limage_size(char * const path);
 *    24. uint32_t udisk_reader::get_limage_offset(char * const path);
 *    25. uint32_t udisk_reader::get_gcode_size(char * const path);
 *    26. uint32_t udisk_reader::get_gcode_offset(char * const path);
 *    27. uint32_t udisk_reader::get_print_time(char * const path);
 *    28. uint32_t udisk_reader::get_print_time_dynamic(void);
 *    29. void     udisk_reader::print_time_countdown(void);
 *
 * \par History:
 * <pre>
 * `<Author>`         `<Time>`        `<Version>`        `<Descr>`
 * Mark Yan         2019/05/15     1.0.0            Initial function design.
 * </pre>
 *
 */

#ifndef _UDISK_READER_H_
#define _UDISK_READER_H_

#ifdef TARGET_LPC1768
#include <stdarg.h>
#include <stdio.h>
#include <Arduino.h>
#include "../../inc/MarlinConfigPre.h"
#include "../../inc/MarlinConfig.h"

#if ENABLED(USB_DISK_SUPPORT)
#include "MassStorageLib.h"
#include "lcd_file.h"

enum is_action_t : uint8_t { LS_SERIAL_PRINT, LS_COUNT, LS_GET_FILE_NAME };

#define USB_NOT_DETECTED             100

#define MAX_ELEMENT_FOR_FILES_LIST   50


#define GCODE_SIZE_INDEX             4
#define SIMAGE_OFFSET_INDEX          12
#define LIMAGE_OFFSET_INDEX          20
#define GCODE_OFFSET_INDEX           28
#define PRINT_DIST_INDEX             40
#define PRINT_TIME_INDEX             44
#define PRINT_MATERIAL_INDEX         48
#define LASER_FRAM_POSITION_INDEX    52

typedef struct
{
  uint32_t ftime;
  uint32_t fsize;
  file_type_t ftype;
  char fname[FILE_NAME_LEN];
}file_info_t;

class udisk_reader
{
public:

  udisk_reader(void);
  virtual ~udisk_reader(void) { }

  void init(void);
  bool is_usb_detected(void);
  bool is_usb_Initialized(void);
  void usb_status_polling(void);
  uint16_t ls(is_action_t action, const char *path = "", const char * const match = NULL);
  void file_list_sort(void);
  uint16_t ls_dive(const char *path = "", const char * const match = NULL);
  uint16_t get_num_Files(const char *path = "", const char * const match = NULL);
  char* get_file_name(void);
  void open_file(char * const path, const bool read);
  void print_file_name();
  void report_status();
  int16_t get(void);
  void start_udisk_print(void);
  void pause_udisk_print(void);
  void stop_udisk_print(void);
  void printing_has_finished();
  bool get_udisk_printing_flag(void);
  bool is_gm_file_type(char * const path);
  bool check_gm_file(char * const path);
  uint32_t get_simage_size(char * const path);
  uint32_t get_simage_offset(char * const path);
  uint32_t get_limage_size(char * const path);
  uint32_t get_limage_offset(char * const path);
  uint32_t get_gcode_size(char * const path);
  uint32_t get_gcode_offset(char * const path);
  uint32_t get_print_time(char * const path);
  void recovery_print_time_dynamic(uint32_t time);
  uint32_t get_print_time_dynamic(void);
  void print_time_countdown(void);
  file_type_t get_opened_file_type(void);
  uint32_t get_opened_file_size(void);
  inline bool eof() { return udisk_pos >= file_size; }
  inline bool is_file_open() { return is_file_opened;}
  inline void set_index(const uint32_t index) { udisk_pos = index; f_lseek(&file_obj, index); }
  inline uint32_t get_index() { return udisk_pos; }
  bool abort_udisk_printing;
#if ENABLED(POWER_LOSS_RECOVERY)
  bool job_recover_file_exists(void);
  void open_job_recovery_file(const bool read);
  void remove_job_recovery_file(void);
#endif
  bool firmware_upate_file_exists(void);
  bool is_firmware_same(void);
  uint32_t caculate_file_crc32(FIL* fp);

  bool get_fram_xy_position(char * const path);
  uint8_t get_directory_depth(char *dir_path);
private:
  //Variable definitions
  FATFS fatFS; /* File system object */

  bool detected;
  bool Initialized;
  bool udisk_printing;
  bool is_file_opened;

  is_action_t is_action;
  file_type_t opened_file_type;

  uint16_t file_count;  //counter for the files in the current directory

  uint32_t udisk_pos;
  uint32_t file_size;
  uint32_t file_gcode_offset;
  uint32_t print_time_dynamic;
  uint32_t file_print_time;

  FIL file_obj;
  char *opened_file_name;
  file_info_t file_list_array[MAX_ELEMENT_FOR_FILES_LIST];
  char current_file_path[FILE_DIRECTORY_DEPTH * FILE_NAME_LEN];
  char abs_file_name[(FILE_DIRECTORY_DEPTH + 1) * FILE_NAME_LEN];
};

#define IS_UDISK_PRINTING() udisk.get_udisk_printing_flag()
#define IS_UDISK_FILE_OPEN()

extern udisk_reader udisk;
#endif // USB_DISK_SUPPORT
#endif // TARGET_LPC1768
#endif // _UDISK_READER_H_
