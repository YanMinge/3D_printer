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
 *    1.  void    udisk_reader::init(void);
 *    2.  bool    udisk_reader::is_usb_detected(void);
 *    3.  void    udisk_reader::usb_status_polling(void);
 *    4.  void    udisk_reader::test_code(void);
 *    5.  uint16_t udisk_reader::ls(is_action_t action, const char *path = "", const char * const match = NULL);
 *    6.  uint16_t udisk_reader::is_dive(const char *path = "", const char * const match = NULL);
 *    7.  uint16_t udisk_reader::get_num_Files(const char *path = "", const char * const match = NULL);
 *
 * \par History:
 * <pre>
 * `<Author>`         `<Time>`        `<Version>`        `<Descr>`
 * Mark Yan         2019/05/15     1.0.0            Initial function design.
 * </pre>
 *
 */

#ifdef TARGET_LPC1768
#include "udisk_reader.h"
#include "lcd_process.h"
#include "../../gcode/queue.h"

#include HAL_PATH(.., HAL.h)

#if ENABLED(USB_DISK_SUPPORT)

udisk_reader udisk;

//#define DEBUGPRINTF(...)
#define DEBUGPRINTF(...) SERIAL_OUT(printf, __VA_ARGS__)

extern "C" bool is_usb_connected(void);
extern "C" usb_error_info_type get_usb_error_info(void);
extern "C"  void set_disk_status(DSTATUS status);

udisk_reader::udisk_reader(void)
{
  detected = false;
  Initialized = false;
  udisk_printing = abort_udisk_printing = false;
  is_file_opened = false;
  opened_file_name = NULL;
}

void udisk_reader::init(void)
{
  SetupHardware();
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
    if(usb_status == false)
    {
      f_unmount("/");
      set_disk_status(STA_NOINIT);
      detected = false;
    }
    else
    {
      f_mount(&fatFS, "/" , 0);     /* Register volume work area (never fails) */
      detected = true;;
    }
    DEBUGPRINTF("usb_status(%d)\r\n", usb_status);
    pre_usb_status = usb_status;
  }
}

uint16_t udisk_reader::ls(is_action_t action, const char *path, const char * const match)
{
  is_action = action;
  return ls_dive(path, match);
}

uint16_t udisk_reader::ls_dive(const char *path, const char * const match/*=NULL*/)
{
  FRESULT rc = FR_OK; 	/* Result code */
  DIR dir;        /* Directory object */
  char *lsdata = NULL;
  FILINFO fno;    /* File information object */
  pfile_list_t file_list_data;
  rc = f_opendir(&dir, path);
  if (rc)
  {
    LcdFile.file_list_clear();
    DEBUGPRINTF("can't open dir(%s)\r\n", path);
    if(!is_usb_detected())
    {
      dwin_process.lcd_send_data(PAGE_BASE +12, PAGE_ADDR);
      return USB_NOT_DETECTED;
    }
    return rc;
  }
  else
  {
    file_count = 0;
    if(is_action == LS_SERIAL_PRINT)
    {
      SERIAL_PRINTF("List files in path: %s\r\n", path);
      lsdata = new char[100];
      memset(lsdata, 0, sizeof(100));
    }
    if(is_action == LS_GET_FILE_NAME)
    {
      LcdFile.file_list_clear();
    }
    for(;; )
    {
      /* Read a directory item */
      rc = f_readdir(&dir, &fno);
      if (rc || !fno.fname[0])
      {
        break;                  /* Error or end of dir */
      }
      file_count++;
      if(is_action == LS_SERIAL_PRINT && lsdata != NULL)
      {
        uint16_t year = ((fno.fdate >> 9) & 0x3f) + 1980;
        uint16_t month = ((fno.fdate >> 5) & 0x0f);
        uint16_t date = (fno.fdate & 0x1f);

        uint16_t hour = ((fno.ftime >> 11) & 0x1f);
        uint16_t min = ((fno.ftime >> 5) & 0x3f);
        if (fno.fattrib & AM_DIR)
        {
          sprintf(lsdata, " <DIR> %d\\%02d\\%02d %02d:%02d  %12ld    %.32s\r\n", year, month, date, hour, min, fno.fsize, fno.fname);
        }
        else
        {
          sprintf(lsdata, "       %d\\%02d\\%02d %02d:%02d  %12ld    %.32s\r\n", year, month, date, hour, min, fno.fsize, fno.fname);
        }
        SERIAL_PRINTF(lsdata);
        delete[] lsdata;
      }
      else if(is_action == LS_GET_FILE_NAME)
      {
        file_list_data = (pfile_list_t) new char[(sizeof(file_list_t))];
        memset(file_list_data, 0, sizeof(file_list_t));
        if(fno.fattrib & AM_DIR)
        {
          file_list_data->IsDir = true;
        }
        else
        {
          file_list_data->IsDir = false;
        }
        if(strlen(fno.fname) <= FILE_NAME_LEN)
        {
          strcpy(file_list_data->file_name, fno.fname);
        }
        else
        {
          memcpy(file_list_data->file_name, fno.fname, FILE_NAME_LEN);
          file_list_data->file_name[FILE_NAME_LEN] = '\0';
        }
        LcdFile.file_list_insert(file_list_data);
      }
    }
    if (rc)
    {
      DEBUGPRINTF("f_readdir error(%d)\r\n", rc);
    }
    else if(is_action == LS_COUNT)
    {
      SERIAL_PRINTF("file_count(%d)\r\n",file_count);
      return file_count;
    }
    return rc;
  }
}

uint16_t udisk_reader::get_num_Files(const char *path, const char * const match)
{
  return ls(LS_COUNT, path, match);
}

void udisk_reader::open_file(char * const path, const bool read)
{
  FRESULT rc;
  uint8_t doing = 0;
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
    doing = 1;
  }
  else
  {
    doing = 2;
  }

  if (doing)
  {
    SERIAL_ECHO_START();
    SERIAL_ECHOPGM("Now ");
    serialprintPGM(doing == 1 ? PSTR("doing") : PSTR("fresh"));
    SERIAL_ECHOLNPAIR(" file: ", path);
  }

  stop_udisk_print();

  if(read)
  {
    rc = f_open(&file_obj, path, FA_READ);
    if (rc)
    {
      DEBUGPRINTF("Unable to open file: %s from USB Disk\r\n", path);
      return;
    }
    else
    {
      if(strlen(path) <= FILE_NAME_LEN * 4)
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

void udisk_reader::print_file_name()
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

void udisk_reader::report_status()
{
  DEBUGPRINTF("report_status\r\n");
  for(uint32_t i = 0; i< file_size; i++)
  {
    int16_t c = get();
    DEBUGPRINTF("%c (%d)", c, file_obj.fptr);
  }
  DEBUGPRINTF("\r\n");
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
  }
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

bool udisk_reader::check_gm_file(char * const path)
{
  if (!is_usb_detected())
  {
    return false;
  }
  if(!is_gm_file_type(path))
  {
    return false;
  }
  if(file_size == (0x50 + get_simage_size(path) + get_limage_size(path) + get_gcode_size(path)))
  {
    return true;
  }
  else
  {
    return false;
  }
}

#endif // USB_DISK_SUPPORT
#endif // TARGET_LPC1768
