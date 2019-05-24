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
  usb_error_info_type error_info = get_usb_error_info();
  if(error_info.ErrorCode != 0 || error_info.SubErrorCode != 0)
  {
    DEBUGPRINTF(("Dev Enum Error\r\n"
                  " -- Error port %d\r\n"
                  " -- Error Code %d\r\n"
                  " -- Sub Error Code %d\r\n"),
                 error_info.corenum, error_info.ErrorCode, error_info.SubErrorCode);
    return;
  }

  bool usb_status = is_usb_connected();
  if(usb_status != pre_usb_status)
  {
    if(usb_status == 0)
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
  char *debugBuf = NULL;
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
      SERIAL_PRINTF("List files in path:%s\r\n", path);
      debugBuf = new char[100];
      memset(debugBuf, 0, sizeof(100));
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
      if(is_action == LS_SERIAL_PRINT && debugBuf != NULL)
      {
        uint16_t year = ((fno.fdate >> 9) & 0x3f) + 1980;
		uint16_t month = ((fno.fdate >> 5) & 0x0f);
		uint16_t date = (fno.fdate & 0x1f);

		uint16_t hour = ((fno.ftime >> 11) & 0x1f);
		uint16_t min = ((fno.ftime >> 5) & 0x3f);
        if (fno.fattrib & AM_DIR)
        {
          sprintf(debugBuf, " <DIR> %d\\%02d\\%02d %02d:%02d  %12ld    %.32s\r\n", year, month, date, hour, min, fno.fsize, fno.fname);
        }
        else
        {
          sprintf(debugBuf, "       %d\\%02d\\%02d %02d:%02d  %12ld    %.32s\r\n", year, month, date, hour, min, fno.fsize, fno.fname);
        }
        SERIAL_PRINTF(debugBuf);
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

void udisk_reader::get(void)
{
  SERIAL_PRINTF("M2020 /\r\n");
  //VirtualSerial.printf_rx("M2020 /\r\n");
  enqueue_and_echo_command("M2020 /");
}

#if 0
void CardReader::openAndPrintFile(const char *name) {
  char cmd[4 + strlen(name) + 1]; // Room for "M23 ", filename, and null
  sprintf_P(cmd, PSTR("M23 %s"), name);
  for (char *c = &cmd[4]; *c; c++) *c = tolower(*c);
  enqueue_and_echo_command_now(cmd);
  enqueue_and_echo_commands_P(PSTR("M24"));
}
#endif

void udisk_reader::start_file_print(void)
{
  if (is_usb_detected())
  {
    udisk_printing = true;
  }
}

void udisk_reader::stop_udisk_Print(void)
{
  udisk_printing = abort_udisk_printing = false;
}

bool udisk_reader::get_udisk_printing_flag(void)
{
  return udisk_printing;
}

void udisk_reader::test_code(void)
{
  static uint8_t buffer[8 * 1024];
  static FIL fileObj; /* File object */
  FRESULT rc;     /* Result code */
  UINT i;
  UINT br;
  uint8_t *ptr;
  char debugBuf[300];
  DIR dir;        /* Directory object */
  FILINFO fno;    /* File information object */

  rc = f_open(&fileObj, "MESSAGE.TXT", FA_READ);
  if (rc)
  {
    DEBUGPRINTF("Unable to open MESSAGE.TXT from USB Disk\r\n");
  }
  else
  {
    DEBUGPRINTF("Opened file MESSAGE.TXT from USB Disk. Printing contents...\r\n\r\n");
    for (;; )
    {
      /* Read a chunk of file */
      rc = f_read(&fileObj, buffer, sizeof buffer, &br);
      if (rc || !br)
      {
        break;                  /* Error or end of file */
      }
      ptr = (uint8_t *) buffer;
      for (i = 0; i < br; i++)
      {
        /* Type the data */
        DEBUGPRINTF("%c", ptr[i]);
      }
    }
    if (rc)
    {
      DEBUGPRINTF("f_read error(%d)\r\n", rc);
    }

    DEBUGPRINTF("\r\n\r\nClose the file.\r\n");
    rc = f_close(&fileObj);
    if (rc)
    {
      DEBUGPRINTF("f_close error(%d)\r\n", rc);
    }
  }
  DEBUGPRINTF("\r\nOpen root directory.\r\n");
  rc = f_opendir(&dir, "");
  if (rc)
  {
    DEBUGPRINTF("f_opendir error(%d)\r\n", rc);
  }
  else
  {
    DEBUGPRINTF("\r\nDirectory listing...\r\n");
    for (;; )
    {
      /* Read a directory item */
      rc = f_readdir(&dir, &fno);
      if (rc || !fno.fname[0])
      {
        break;                  /* Error or end of dir */
      }
      if (fno.fattrib & AM_DIR)
      {
        sprintf(debugBuf, "   <Dir>  %s\r\n", fno.fname);
      }
      else
      {
        sprintf(debugBuf, "   <File> %s\r\n", fno.fname);
      }
      DEBUGPRINTF(debugBuf);
    }
    if (rc)
    {
      DEBUGPRINTF("f_readdir error(%d)\r\n", rc);
    }
  }
  DEBUGPRINTF("\r\nTest completed.\r\n");
}

#endif // USB_DISK_SUPPORT
#endif // TARGET_LPC1768
