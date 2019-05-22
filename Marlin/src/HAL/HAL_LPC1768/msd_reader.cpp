/**   
 * \par Copyright (C), 2018-2019, MakeBlock
 * \class   msd_reader
 * \brief   USB HOST mass storge driver.
 * @file    msd_reader.h
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
 *    1.  void    msd_reader::begin(int32_t baud);
 *
 * \par History:
 * <pre>
 * `<Author>`         `<Time>`        `<Version>`        `<Descr>`
 * Mark Yan         2019/05/15     1.0.0            Initial function design.
 * </pre>
 *
 */

#ifdef TARGET_LPC1768
#include "msd_reader.h"
#include "../../gcode/lcd_file.h"
#include HAL_PATH(.., HAL.h)

#if ENABLED(USBMSCSUPPORT)
msd_reader MsdReader;

//#define DEBUGPRINTF(...) 
#define DEBUGPRINTF(...) SERIAL_OUT(printf, __VA_ARGS__)

extern "C" bool is_usb_connected(void);
extern "C" usb_error_info_type get_usb_error_info(void);
extern "C"  void set_disk_status(DSTATUS status);

msd_reader::msd_reader(void)
{
  detected = false;
  Initialized = false;
}

void msd_reader::init(void)
{
  SetupHardware();
  Initialized = true;
}

bool msd_reader::is_usb_detected(void)
{
  return detected;
}

bool msd_reader::is_usb_Initialized(void)
{
  return Initialized;
}


void msd_reader::usb_status_polling(void)
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

void msd_reader::ls(LsAction Action, const char *path, const char * const match)
{
  lsAction = Action;
  DEBUGPRINTF("list dir!\r\n");
  lsDive(path, match);
}

void msd_reader::lsDive(const char *path, const char * const match/*=NULL*/)
{
  FRESULT rc; 	/* Result code */
  DIR dir;        /* Directory object */
  char *debugBuf = NULL;
  FILINFO fno;    /* File information object */
  pfile_list file_list_data;
  rc = f_opendir(&dir, path);
  if (rc) 
  {
    DwinLcdFile.file_list_clear();
    DEBUGPRINTF("can't open dir(%s)\r\n", path);
  }
  else
  {
    file_count = 0;
	if(lsAction == LS_SerialPrint)
	{
      debugBuf = new char[11+255];	
      memset(debugBuf, 0, sizeof(11+255));
	}
	if(lsAction == LS_GetFilename)
	{
      DwinLcdFile.file_list_clear();
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
      if(lsAction == LS_SerialPrint && debugBuf != NULL)
      {
        if (fno.fattrib & AM_DIR)
        {
          sprintf(debugBuf, " <Dir>  %s\r\n", fno.fname);
        }
        else 
        {
          sprintf(debugBuf, " <File> %s\r\n", fno.fname);
        }
        DEBUGPRINTF(debugBuf);
      }
	  else if(lsAction == LS_GetFilename)
      {
        file_list_data = (pfile_list) new char[(sizeof(file_list))];
        memset(file_list_data, 0, sizeof(file_list));
        if(fno.fattrib & AM_DIR)
        {
          file_list_data->IsDir = true;
		}
		else
        {
          file_list_data->IsDir = false;
		}	
		if(strlen(fno.fname) <= FileNameLen)
        {
		  strcpy(file_list_data->UsbFlieName, fno.fname);
        }
		else
        {
          memcpy(file_list_data->UsbFlieName, fno.fname, FileNameLen);
		  file_list_data->UsbFlieName[FileNameLen] = '\0'; 
		}
		DwinLcdFile.file_list_insert(file_list_data);
      }
    }
    if (rc)
    {
      DEBUGPRINTF("f_readdir error(%d)\r\n", rc);
    }
    else if(lsAction == LS_Count)
    {
      DEBUGPRINTF("LS_Count(%d)\r\n",file_count);
    }
  }
}

void msd_reader::test_code(void)
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

#endif // USBMSCSUPPORT
#endif // TARGET_LPC1768
