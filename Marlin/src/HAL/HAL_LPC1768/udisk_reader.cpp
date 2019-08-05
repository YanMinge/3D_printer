/**
 * \par Copyright (C), 2018-2019, MakeBlock
 * \class   udisk_reader
 * \brief   USB HOST mass storge driver.
 * @file    udisk_reader.cpp
 * @author  Mark Yan
 * @version V1.0.0
 * @date    2019/05/15
 * @brief   source code for USB HOST mass storge driver.
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

#ifdef TARGET_LPC1768
#include "../../module/planner.h"
#include "../../module/printcounter.h"

#include "lcd_process.h"
#include "lcd_parser.h"
#include "../../gcode/queue.h"

#include HAL_PATH(.., HAL.h)

#if ENABLED(USB_DISK_SUPPORT)
#include "udisk_reader.h"
#include "machine_info.h"

#if ENABLED(POWER_LOSS_RECOVERY)
#include "../../feature/power_loss_recovery.h"
#endif

udisk_reader udisk;

//#define DEBUGPRINTF(...)
#define DEBUGPRINTF(...) SERIAL_OUT(printf, __VA_ARGS__)

extern "C" bool is_usb_connected(void);
extern "C" usb_error_info_type get_usb_error_info(void);
extern "C"  void set_disk_status(DSTATUS status);

union
{
  uint8_t byteVal[4];
  float floatVal;
  uint32_t uintVal;
  int32_t intVal;
}val4byte;

udisk_reader::udisk_reader(void)
{
  detected = false;
  Initialized = false;
  udisk_printing = abort_udisk_printing = false;
  is_file_opened = false;
  opened_file_name = NULL;
  opened_file_type = TYPE_NULL;
  memset(current_file_path, 0, sizeof(current_file_path));
}

void udisk_reader::init(void)
{
  SetupHardware();
  file_count = 0;
  memset(file_list_array, 0, sizeof(file_list_array));
  Initialized = true;
}

bool udisk_reader::is_usb_detected(void)
{
  return detected;
}

bool udisk_reader::is_usb_Initialized(void)
{
  return Initialized;
}

void udisk_reader::usb_status_polling(void)
{
  static bool pre_usb_status;
  static uint16_t pre_error_code;
  usb_error_info_type error_info = get_usb_error_info();
  uint16_t error_code = (error_info.ErrorCode << 8) & error_info.SubErrorCode;
  if((error_code != pre_error_code) && (error_info.ErrorCode != 0 || error_info.SubErrorCode != 0))
  {
    DEBUGPRINTF(("Dev Enum Error\r\n"
                  " -- Error port %d\r\n"
                  " -- Error Code %d\r\n"
                  " -- Sub Error Code %d\r\n"),
                 error_info.corenum, error_info.ErrorCode, error_info.SubErrorCode);
    pre_error_code = error_code;
    return;
  }

  bool usb_status = is_usb_connected();
  if(usb_status != pre_usb_status)
  {
    dwin_parser.set_file_read_status(false);
    if(usb_status == false)
    {
      f_unmount("/");
      set_disk_status(STA_NOINIT);
      detected = false;
    }
    else
    {
      f_mount(&fatFS, "/" , 0);     /* Register volume work area (never fails) */
      detected = true;
    }
    pre_usb_status = usb_status;
  }
}

uint16_t udisk_reader::ls(is_action_t action, const char *path, const char * const match)
{
  is_action = action;
  return ls_dive(path, match);
}

void udisk_reader::file_list_sort(void)
{
  if(file_count != 0)
  {
    file_info_t file_info_temp;
    int i, j;
    for(i=0; i<(file_count-1); i++)
    {
      for (j=0; j<(file_count-1-i); j++)
      {
        if (file_list_array[j].ftime > file_list_array[j+1].ftime)
        {
          file_info_temp = file_list_array[j];
          file_list_array[j] = file_list_array[j+1];
          file_list_array[j+1] = file_info_temp;
        }
      }
    }
  }
}

uint16_t udisk_reader::ls_dive(const char *path, const char * const match/*=NULL*/)
{
  FRESULT rc = FR_OK;   /* Result code */
  DIR dir;        /* Directory object */
  FILINFO fno;    /* File information object */
  uint8_t dir_depth = 0;
  pfile_list_t file_list_data;
  char lsdata[100];
  rc = f_opendir(&dir, path);
  if (rc)
  {
    LcdFile.file_list_clear();
    DEBUGPRINTF("can't open dir(%s)\r\n", path);
    if(!is_usb_detected())
    {
      return USB_NOT_DETECTED;
    }
    DEBUGPRINTF("f_opendir error(%d)\r\n", rc);
  }
  else
  {
    file_count = 0;
    memset(file_list_array, 0, sizeof(file_list_array));
    memset(current_file_path, 0, sizeof(current_file_path));
    strcpy(current_file_path, path);
    dir_depth = get_directory_depth(current_file_path);
    for(;; )
    {
      /* Read a directory item */
      rc = f_readdir(&dir, &fno);
      if (rc || !fno.fname[0])
      {
        break;                  /* Error or end of dir */
      }
      if((match != NULL) && (strstr(fno.fname, match) == NULL) && (!(fno.fattrib & AM_DIR)))
      {
        continue;
      }
      if(is_action != LS_COUNT)
      {
        file_list_array[file_count].ftime = ((fno.fdate & 0xffff) << 16) + (fno.ftime & 0xffff);
        file_list_array[file_count].fsize = fno.fsize;
        if(fno.fattrib & AM_DIR)
        {
          if(dir_depth > 5) continue;
          else file_list_array[file_count].ftype = TYPE_FOLDER;
        }
        else
        {
          char * file_path = new char[strlen(path) + strlen(fno.fname) + 1];
          strcpy(file_path, path);
          file_path[strlen(path)] = '/';
          strcpy(file_path + strlen(path) + 1, fno.fname);
          file_list_array[file_count].ftype = TYPE_DEFAULT_FILE;
          delete[] file_path;
        }

        if(strlen(fno.fname) < FILE_NAME_LEN)
        {
          strcpy(file_list_array[file_count].fname, fno.fname);
        }
        else
        {
          //memcpy(file_list_array[file_count].fname, fno.fname, FILE_NAME_LEN -1);
          //file_list_array[file_count].fname[FILE_NAME_LEN - 1] = '\0';
          continue;
        }
      }
      file_count++;
    }
    if (rc)
    {
      DEBUGPRINTF("f_readdir error(%d)\r\n", rc);
      return rc;
    }
    if(is_action == LS_SERIAL_PRINT)
    {
      file_list_sort();
      SERIAL_PRINTF("List files in path: %s\r\n", path);
      for(int i = 0; i < file_count; i++)
      {
        uint16_t fdata = (file_list_array[i].ftime >> 16) & 0xffff;
        uint16_t year = ((fdata >> 9) & 0x3f) + 1980;
        uint16_t month = ((fdata >> 5) & 0x0f);
        uint16_t date = (fdata & 0x1f);

        uint16_t ftime = file_list_array[i].ftime & 0xffff;
        uint16_t hour = ((ftime >> 11) & 0x1f);
        uint16_t min = ((ftime >> 5) & 0x3f);
        if(file_list_array[file_count].ftype == true)
        {
          sprintf(lsdata, " <DIR> %d\\%02d\\%02d %02d:%02d  %12ld    %.32s\r\n", year, month, date, hour, min, file_list_array[i].fsize, file_list_array[i].fname);
        }
        else
        {
          sprintf(lsdata, "       %d\\%02d\\%02d %02d:%02d  %12ld    %.32s\r\n", year, month, date, hour, min, file_list_array[i].fsize, file_list_array[i].fname);
        }
        SERIAL_PRINTF(lsdata);
      }
    }
    else if(is_action == LS_GET_FILE_NAME)
    {
      file_list_sort();
      LcdFile.file_list_clear();
      for(int i = 0; i < file_count; i++)
      {
        file_list_data = (pfile_list_t) new char[(sizeof(file_list_t))];
        memset(file_list_data, 0, sizeof(file_list_t));
        file_list_data->file_type = file_list_array[i].ftype;
        strcpy(file_list_data->file_name, file_list_array[i].fname);
        LcdFile.file_list_insert(file_list_data);
      }
    }
    else if(is_action == LS_COUNT)
    {
      return file_count;
    }
  }
  return rc;
}

uint16_t udisk_reader::get_num_Files(const char *path, const char * const match)
{
  return ls(LS_COUNT, path, match);
}

char* udisk_reader::get_file_name(void)
{
  if(is_file_open())
  {
    if(opened_file_name != NULL)
    {
      return opened_file_name;
    }
  }
  return NULL;
}

void udisk_reader::open_file(char * const path, const bool read)
{
  FRESULT rc;
  uint8_t doing = 0;
  memset(abs_file_name, 0, sizeof(abs_file_name));
  if(strcmp(current_file_path, "/") == 0)
  {
    strcpy(abs_file_name, path);
  }
  else
  {
    strcat(abs_file_name,current_file_path);
    strcat(abs_file_name,"/");
    strcat(abs_file_name,path);
  }
  if (!is_usb_detected())
  {
    return;
  }

  if(is_file_open())
  {
    if(opened_file_name != NULL)
    {
      DEBUGPRINTF("The file: %s has been opened\r\n",opened_file_name);
    }

	if(strcmp(opened_file_name, path))
    {
      DEBUGPRINTF("opened_file_name not path\r\n");
      doing = 1;
    }
  }
  else
  {
    doing = 2;
  }

  if(doing)
  {
    SERIAL_ECHO_START();
    SERIAL_ECHOPGM("Now ");
    serialprintPGM(doing == 1 ? PSTR("doing") : PSTR("fresh"));
    SERIAL_ECHOLNPAIR(" file: ", abs_file_name);
  }
  else
  {
    return;
  }

  stop_udisk_print();

  if(read)
  {
    rc = f_open(&file_obj, abs_file_name, FA_READ);
    if (rc)
    {
      DEBUGPRINTF("Unable to open file: %s from USB Disk\r\n", abs_file_name);
      return;
    }
    else
    {
      if(strlen(abs_file_name) <= FILE_NAME_LEN * 4)
      {
        opened_file_name = new char[strlen(path)];
        strcpy(opened_file_name, path);
      }
      else
      {
        opened_file_name = new char[FILE_NAME_LEN * 4];
        memcpy(opened_file_name, path, FILE_NAME_LEN * 4);
        opened_file_name[FILE_NAME_LEN * 4] = '\0';
      }
      is_file_opened = true;
      file_size = file_obj.obj.objsize;
      udisk_pos = 0;
      SERIAL_ECHOLNPAIR(MSG_SD_FILE_OPENED, opened_file_name, MSG_SD_SIZE, file_size);
      SERIAL_ECHOLNPGM(MSG_SD_FILE_SELECTED);
    }
  }
}

void udisk_reader::print_file_name(void)
{
  if(opened_file_name != NULL)
  {
    SERIAL_ECHO(' ');
    SERIAL_ECHO(opened_file_name);
  }
  else
  {
    SERIAL_ECHOPGM("(no file)");
  }
  SERIAL_EOL();
}

void udisk_reader::report_status(void)
{
  if (IS_UDISK_PRINTING())
  {
    SERIAL_ECHOPGM(MSG_SD_PRINTING_BYTE);
    SERIAL_ECHO(udisk_pos - get_gcode_offset(get_file_name()));
    SERIAL_CHAR('/');
    SERIAL_ECHOLN(get_gcode_size(get_file_name()));
  }
  else
  {
    SERIAL_ECHOLNPGM("Not UDISK printing");
  }
}

int16_t udisk_reader::get(void)
{
  uint8_t data[1];
  UINT br;
  FRESULT rc;
  rc = f_read(&file_obj, data, 1, &br);
  if (rc)
  {
    DEBUGPRINTF("f_read error(%d)\r\n", rc);
    return -1;
  }
  udisk_pos = file_obj.fptr;
  return data[0];
}

void udisk_reader::start_udisk_print(void)
{
  if (is_usb_detected())
  {
    udisk_printing = true;
  }
}

void udisk_reader::pause_udisk_print(void)
{
  udisk_printing = false;
}

void udisk_reader::stop_udisk_print(void)
{
  FRESULT rc;
  udisk_printing = abort_udisk_printing = false;
  if(is_file_open())
  {
    rc = f_close(&file_obj);
    if (rc)
    {
      DEBUGPRINTF("f_close error(%d)\r\n", rc);
    }
    is_file_opened = false;
    delete[] opened_file_name;
    opened_file_name = NULL;
    opened_file_type = TYPE_NULL;
	file_size = 0;
  }
}

void udisk_reader::printing_has_finished()
{
  planner.synchronize();
  stop_udisk_print();

#if ENABLED(POWER_LOSS_RECOVERY)
	remove_job_recovery_file();
#endif

#if ENABLED(UDISK_FINISHED_STEPPERRELEASE) && defined(UDISK_FINISHED_RELEASECOMMAND)
  planner.finish_and_disable();
#endif

  print_job_timer.stop();
  if (print_job_timer.duration() > 60)
  {
    enqueue_and_echo_commands_P(PSTR("M31"));
  }

#if ENABLED(NEWPANEL)
  if(IS_HEAD_PRINT())
  {
    dwin_process.set_machine_status(PRINT_MACHINE_STATUS_PRINT_SUCCESS_CH);
    dwin_process.show_machine_status(PRINT_MACHINE_STATUS_PRINT_SUCCESS_CH);
    dwin_process.change_lcd_page(EXCEPTION_SURE_HINT_PAGE_EN, EXCEPTION_SURE_HINT_PAGE_CH);
  }
  else if(IS_HEAD_LASER())
  {
    dwin_process.set_machine_status(LASER_MACHINE_STATUS_ENGRAVE_FINISHED_CH);
    dwin_process.show_machine_status(LASER_MACHINE_STATUS_ENGRAVE_FINISHED_CH);
    dwin_process.change_lcd_page(LASER_EXCEPTION_SURE_PAGE_EN, LASER_EXCEPTION_SURE_PAGE_CH);
  }
#endif
}

bool udisk_reader::get_udisk_printing_flag(void)
{
  return udisk_printing;
}

bool udisk_reader::is_gm_file_type(char * const path)
{
  open_file(path, true);
  set_index(0);
  char lable_char_1 =  get();
  char lable_char_2 =  get();
  if((lable_char_1 != 'G') || (lable_char_2 != 'M'))
  {
    return false;
  }
  else
  {
    return true;
  }
}

uint32_t udisk_reader::get_simage_size(char * const path)
{
  if (!is_usb_detected())
  {
    return -1;
  }
  if(!is_gm_file_type(path))
  {
    return -1;
  }
  set_index(SIMAGE_OFFSET_INDEX);
  val4byte.byteVal[0] = get();
  val4byte.byteVal[1] = get();
  val4byte.byteVal[2] = get();
  val4byte.byteVal[3] = get();
  uint32_t simage_offset = val4byte.uintVal;

  set_index(LIMAGE_OFFSET_INDEX);
  val4byte.byteVal[0] = get();
  val4byte.byteVal[1] = get();
  val4byte.byteVal[2] = get();
  val4byte.byteVal[3] = get();
  uint32_t limage_offset = val4byte.uintVal;
  DEBUGPRINTF("simage_size(%d)\r\n", limage_offset - simage_offset);
  return limage_offset - simage_offset;
}

uint32_t udisk_reader::get_simage_offset(char * const path)
{
  if (!is_usb_detected())
  {
    return -1;
  }
  if(!is_gm_file_type(path))
  {
    return -1;
  }
  set_index(SIMAGE_OFFSET_INDEX);
  val4byte.byteVal[0] = get();
  val4byte.byteVal[1] = get();
  val4byte.byteVal[2] = get();
  val4byte.byteVal[3] = get();
  uint32_t simage_offset = val4byte.uintVal;
  DEBUGPRINTF("simage_offset(%d)\r\n", simage_offset);
  return simage_offset;
}


uint32_t udisk_reader::get_limage_size(char * const path)
{
  if (!is_usb_detected())
  {
    return -1;
  }
  if(!is_gm_file_type(path))
  {
    return -1;
  }
  set_index(LIMAGE_OFFSET_INDEX);
  val4byte.byteVal[0] = get();
  val4byte.byteVal[1] = get();
  val4byte.byteVal[2] = get();
  val4byte.byteVal[3] = get();
  uint32_t limage_offset = val4byte.uintVal;

  set_index(GCODE_OFFSET_INDEX);
  val4byte.byteVal[0] = get();
  val4byte.byteVal[1] = get();
  val4byte.byteVal[2] = get();
  val4byte.byteVal[3] = get();
  uint32_t gcode_offset = val4byte.uintVal;
  DEBUGPRINTF("limage_size(%d)\r\n", gcode_offset - limage_offset);
  return gcode_offset - limage_offset;
}

uint32_t udisk_reader::get_limage_offset(char * const path)
{
  if (!is_usb_detected())
  {
    return -1;
  }
  if(!is_gm_file_type(path))
  {
    return -1;
  }
  set_index(LIMAGE_OFFSET_INDEX);
  val4byte.byteVal[0] = get();
  val4byte.byteVal[1] = get();
  val4byte.byteVal[2] = get();
  val4byte.byteVal[3] = get();
  uint32_t limage_offset = val4byte.uintVal;
  DEBUGPRINTF("limage_offset(%d)\r\n", limage_offset);
  return limage_offset;
}

uint32_t udisk_reader::get_gcode_size(char * const path)
{
  if (!is_usb_detected())
  {
    return -1;
  }
  if(!is_gm_file_type(path))
  {
    return -1;
  }
  set_index(GCODE_SIZE_INDEX);
  val4byte.byteVal[0] = get();
  val4byte.byteVal[1] = get();
  val4byte.byteVal[2] = get();
  val4byte.byteVal[3] = get();
  uint32_t gcode_size = val4byte.uintVal;
  DEBUGPRINTF("gcode_size(%d)\r\n", gcode_size);
  return gcode_size;
}

uint32_t udisk_reader::get_gcode_offset(char * const path)
{
  if (!is_usb_detected())
  {
    return -1;
  }
  if(!is_gm_file_type(path))
  {
    return -1;
  }
  set_index(GCODE_OFFSET_INDEX);
  val4byte.byteVal[0] = get();
  val4byte.byteVal[1] = get();
  val4byte.byteVal[2] = get();
  val4byte.byteVal[3] = get();
  uint32_t gcode_offset = val4byte.uintVal;
  DEBUGPRINTF("gcode_offset(%d)\r\n", gcode_offset);
  return gcode_offset;
}

uint32_t udisk_reader::get_print_time(char * const path)
{
  if (!is_usb_detected())
  {
    return -1;
  }
  if(!is_gm_file_type(path))
  {
    return -1;
  }
  set_index(PRINT_TIME_INDEX);
  val4byte.byteVal[0] = get();
  val4byte.byteVal[1] = get();
  val4byte.byteVal[2] = get();
  val4byte.byteVal[3] = get();
  uint32_t print_time = val4byte.uintVal;
  DEBUGPRINTF("print_time(%d)\r\n", print_time);
  print_time_dynamic = print_time;
  return print_time;
}

void udisk_reader::recovery_print_time_dynamic(uint32_t time)
{
  print_time_dynamic = print_time_dynamic - time;
}


uint32_t udisk_reader::get_print_time_dynamic(void)
{
  //Enable algorithm equalization time when the time is less than 6 minutes.
  if((print_time_dynamic < 360) && ((file_size - udisk_pos) > 0))
  {
    print_time_dynamic = uint32_t(ceil(print_time_dynamic * 0.5 + print_time_dynamic * ((file_size - udisk_pos)/file_size) * 0.5));
  }
  return print_time_dynamic;
}

void udisk_reader::print_time_countdown(void)
{
  static uint16_t print_time_count = 0;
  //count every 1s clock
  if(IS_UDISK_PRINTING())
  {
    if((print_time_count >= 1000) && (print_time_dynamic > 0))
    {
      print_time_dynamic--;
      print_time_count = 0;
    }
    print_time_count++;
  }
}

bool udisk_reader::check_gm_file(char * const path)
{
  if (!is_usb_detected())
  {
    return false;
  }
  if(!is_gm_file_type(path))
  {
    opened_file_type = TYPE_DEFAULT_FILE;
    return false;
  }
  if(file_size == (0x50 + get_simage_size(path) + get_limage_size(path) + get_gcode_size(path)))
  {
    opened_file_type = TYPE_MAKEBLOCK_GM;
    return true;
  }
  else
  {
    opened_file_type = TYPE_DEFAULT_FILE;
    return false;
  }
}

file_type_t udisk_reader::get_opened_file_type(void)
{
  return opened_file_type;
}

uint32_t udisk_reader::get_opened_file_size(void)
{
  return file_size;
}

#if ENABLED(POWER_LOSS_RECOVERY)
constexpr char job_recovery_file_name[13] = "RECOVERY.bin";
bool udisk_reader::job_recover_file_exists(void)
{
  FILINFO fno;
  FRESULT f_result = f_stat(job_recovery_file_name, &fno);
  if(f_result)
  {
    DEBUGPRINTF("job_recover_file_exists f_stat(%d)\r\n", f_result);
    return false;
  }
  return true;
}

void udisk_reader::open_job_recovery_file(const bool read)
{
  if (!is_usb_detected())
  {
    return;
  }

  if (recovery.is_file_opened)
  {
    return;
  }
  FRESULT f_result = f_open(&recovery.file_obj, job_recovery_file_name, read ? FA_READ : FA_READ | FA_WRITE | FA_CREATE_NEW | FA_OPEN_ALWAYS);
  if(f_result)
  {
    DEBUGPRINTF("open_job_recovery_file f_open(%d)\r\n", f_result);
	SERIAL_ECHOLNPAIR(MSG_SD_OPEN_FILE_FAIL, job_recovery_file_name, ".");
  }
  else
  {
    DEBUGPRINTF("open_job_recovery_file\r\n");
    recovery.is_file_opened = true;
  }

  if(!read)
  {
    SERIAL_ECHOLNPAIR(MSG_SD_WRITE_TO_FILE, job_recovery_file_name);
  }
}

// Removing the job recovery file currently requires closing
// the file being printed, so during udsik printing the file should
// be zeroed and written instead of deleted.
void udisk_reader::remove_job_recovery_file(void)
{
  FRESULT rc;
  if (job_recover_file_exists())
  {
    if(recovery.is_file_opened)
    {
      rc = f_close(&file_obj);
      if (rc)
      {
        DEBUGPRINTF("remove_job_recovery_file f_close(%d)\r\n", rc);
      }
      recovery.is_file_opened = false;
	}
    rc = f_unlink(job_recovery_file_name);
	if (rc)
	{
	  DEBUGPRINTF("remove_job_recovery_file f_unlink(%d)\r\n", rc);
	}

#if ENABLED(DEBUG_POWER_LOSS_RECOVERY)
    SERIAL_ECHOPGM("Power-loss file delete");
    serialprintPGM(job_recover_file_exists() ? PSTR(" failed.\n") : PSTR("d.\n"));
#endif
  }
}
#endif // POWER_LOSS_RECOVERY

bool udisk_reader::get_fram_xy_position(char * const path)
{
  float laser_postion;
  if (!is_usb_detected())
  {
    return false;
  }
  if(!is_gm_file_type(path))
  {
    return false;
  }
  set_index(LASER_FRAM_POSITION_INDEX);
  val4byte.byteVal[0] = get();
  val4byte.byteVal[1] = get();
  val4byte.byteVal[2] = get();
  val4byte.byteVal[3] = get();
  laser_postion = val4byte.intVal;
  dwin_process.laser_fram_xy_position.upper_left_x_position = laser_postion;
  DEBUGPRINTF("gcode_offset(%f)\r\n", laser_postion);

  val4byte.byteVal[0] = get();
  val4byte.byteVal[1] = get();
  val4byte.byteVal[2] = get();
  val4byte.byteVal[3] = get();
  laser_postion = val4byte.intVal;
  dwin_process.laser_fram_xy_position.upper_left_y_position = laser_postion;
  DEBUGPRINTF("gcode_offset(%f)\r\n", laser_postion);

  val4byte.byteVal[0] = get();
  val4byte.byteVal[1] = get();
  val4byte.byteVal[2] = get();
  val4byte.byteVal[3] = get();
  laser_postion = val4byte.intVal;
  dwin_process.laser_fram_xy_position.buttom_right_x_position = laser_postion;
  DEBUGPRINTF("gcode_offset(%f)\r\n", laser_postion);

  val4byte.byteVal[0] = get();
  val4byte.byteVal[1] = get();
  val4byte.byteVal[2] = get();
  val4byte.byteVal[3] = get();
  laser_postion = val4byte.intVal;
  dwin_process.laser_fram_xy_position.buttom_right_y_position = laser_postion;
  DEBUGPRINTF("gcode_offset(%f)\r\n", laser_postion);
  return true;
}

uint8_t udisk_reader::get_directory_depth(char *dir_path)
{
  uint8_t depth = 0;
  while(*dir_path != '\0')
  {
    if(*dir_path == '/')
      depth ++;
    dir_path ++;
  }
  return depth;
}

#endif // USB_DISK_SUPPORT
#endif // TARGET_LPC1768
