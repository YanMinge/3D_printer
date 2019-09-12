/**
 * \par Copyright (C), 2018-2019, MakeBlock
 * \class   no
 * \brief   head file for dwin lcd.
 * @file    dwin.h
 * @author  Akingpan
 * @version V1.0.0
 * @date    2019/05/15
 * @brief   head file for dwin lcd.
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
 * This file is the head file for dwin lcd.
 *
 * \par Method List:
 *
 *
 * \par History:
 * <pre>
 * `<Author>`         `<Time>`        `<Version>`        `<Descr>`
 * Akingpan           2019/05/15       1.0.0              Initial function design.
 * </pre>
 *
 */

#if ENABLED(USE_DWIN_LCD)

#ifndef _DWIN_H_
#define _DWIN_H_

#define IMAGE_SHOW_DELAY            300   //delay ms before show image
#define FILE_DIRECTORY_DEPTH        5
#define UNLOAD_TIME_SHOW_INTERVAL   200 //200MS
#define UNLOAD_TIME_SHOW_PERCENTAGE (100 - FILAMENT_UNLOAD_DELAY/UNLOAD_TIME_SHOW_INTERVAL)
#define MATERIAL_CHECK_TIME_PERIOD  100

typedef struct file_directory_stack{
    int page_num[FILE_DIRECTORY_DEPTH];
    int top;
}file_directory_stack_t;

enum machine_status_type : uint8_t {

  PRINT_MACHINE_STATUS_NO_USB_DISK_CH = 0,
  PRINT_MACHINE_STATUS_UNKNOW_ERROR_CH,
  PRINT_MACHINE_STATUS_NO_FILAMENT_CH,
  PRINT_MACHINE_STATUS_NO_UPDATE_FILE_CH,
  PRINT_MACHINE_STATUS_FIRMWARE_EXCEPTION_CH,
  PRINT_MACHINE_STATUS_LOAD_FILAMENT_CH,
  PRINT_MACHINE_STATUS_STOP_UNLOAD_FILAMENT_CH,
  PRINT_MACHINE_STATUS_UNLOAD_SUCCESS_CH,
  PRINT_MACHINE_STATUS_PRINT_SUCCESS_CH,
  PRINT_MACHINE_STATUS_CANCEL_PRINT_CH,
  PRINT_MACHINE_STATUS_PRINT_CONTINUE_CH,
  PRINT_MACHINE_STATUS_LOAD_FILAMENT_SUCCESS_CH,
  LASER_MACHINE_STATUS_FOCUS_CONFIRM_CH,
  LASER_MACHINE_STATUS_FOCUS_FINISHED_CH,
  LASER_MACHINE_STATUS_ENGRAVE_FINISHED_CH,
  PRINT_MACHINE_STATUS_XYZ_HOME_OK_CH, //15
  PRINT_MACHINE_STATUS_RESTORE_FACOTORY_CH,
  PRINT_MACHINE_STATUS_CALIBRATION_OK_CH,
  PRINT_MACHINE_STATUS_LEVELING_OK_CH,
  PIRNT_MACHINE_STATUS_PRINT_LOAD_FILAMENT_CH,
  PRINT_MACHINE_STATUS_TASK_CANCEL_CH,
  PRINT_MACHINE_STATUS_MATERIAL_RUN_OUT_CH,
  PRINT_MACHINE_STATUS_NO_FIRMWARE_FILE_CH,
  PRINT_MACHINE_STATUS_SAME_FIRMWARE_FILE_CH,//23
  PRINT_MACHINE_STATUS_RESTORE_FACTORY_HINT_CH,//24
  LASER_MACHINE_STATUS_ENGRAVE_SIZE_BIG_CH,//25

  PRINT_MACHINE_STATUS_NO_USB_DISK_EN,
  PRINT_MACHINE_STATUS_UNKNOW_ERROR_EN,
  PRINT_MACHINE_STATUS_NO_FILAMENT_EN,
  PRINT_MACHINE_STATUS_NO_UPDATE_FILE_EN,
  PRINT_MACHINE_STATUS_FIRMWARE_EXCEPTION_EN,
  PRINT_MACHINE_STATUS_LOAD_FILAMENT_EN,
  PRINT_MACHINE_STATUS_STOP_UNLOAD_FILAMENT_EN,
  PRINT_MACHINE_STATUS_UNLOAD_SUCCESS_EN,
  PRINT_MACHINE_STATUS_PRINT_SUCCESS_EN,
  PRINT_MACHINE_STATUS_CANCEL_PRINT_EN,
  PRINT_MACHINE_STATUS_PRINT_CONTINUE_EN,
  PRINT_MACHINE_STATUS_LOAD_FILAMENT_SUCCESS_EN,
  LASER_MACHINE_STATUS_FOCUS_CONFIRM_EN,
  LASER_MACHINE_STATUS_FOCUS_FINISHED_EN,
  LASER_MACHINE_STATUS_ENGRAVE_FINISHED_EN,
  PRINT_MACHINE_STATUS_XYZ_HOME_OK_EN,
  PRINT_MACHINE_STATUS_RESTORE_FACOTORY_EN,
  PRINT_MACHINE_STATUS_CALIBRATION_OK_EN,
  PRINT_MACHINE_STATUS_LEVELING_OK_EN,
  PIRNT_MACHINE_STATUS_PRINT_LOAD_FILAMENT_EN,
  PRINT_MACHINE_STATUS_TASK_CANCEL_EN,
  PRINT_MACHINE_STATUS_MATERIAL_RUN_OUT_EN,
  PRINT_MACHINE_STATUS_NO_FIRMWARE_FILE_EN,
  PRINT_MACHINE_STATUS_SAME_FIRMWARE_FILE_EN,//47
  PRINT_MACHINE_STATUS_RESTORE_FACTORY_HINT_EN, //48
  LASER_MACHINE_STATUS_ENGRAVE_SIZE_BIG_EN, //49

  PRINT_MACHINE_STATUS_PREPARE_PRINT_CH,
  PRINT_MACHINE_STATUS_FIRMWARE_UPDATE_CH,
  PRINT_MACHINE_STATUS_PREPARE_LOAD_CH,
  PRINT_MACHINE_STATUS_PREPARE_UNLOAD_CH,
  LASER_MACHINE_STATUS_PREPARE_ENGRAVE_CH,
  LASER_MACHINE_STATUS_PREPARE_FOCUS_CH,
  PRINT_MACHINE_STATUS_PREPARE_UPDATE_CH,
  PRINT_MACHINE_STATUS_PREPARE_STOP_TASK_CH,
  PRINT_MACHINE_STATUS_PREPARE_QUIT_TASK_CH,
  PRINT_MACHINE_STATUS_PREPARE_HOMEING_CH,
  PRINT_MACHINE_STATUS_PREPARE_SAVE_OFFSET_CH,
  PRINT_MACHINE_STATUS_PREPARE_CALIBRATION_CH,
  PRINT_MACHINE_STATUS_PREPARE_LEVELING_CH,
  PRINT_MACHINE_STATUS_PREPARE_RESTORE_CH,
  LASER_MACHINE_STATUS_PREPARE_WALKING_BORDER_CH,

  PRINT_MACHINE_STATUS_PREPARE_PRINT_EN,
  PRINT_MACHINE_STATUS_FIRMWARE_UPDATE_EN,
  PRINT_MACHINE_STATUS_PREPARE_LOAD_EN,
  PRINT_MACHINE_STATUS_PREPARE_UNLOAD_EN,
  LASER_MACHINE_STATUS_PREPARE_ENGRAVE_EN,
  LASER_MACHINE_STATUS_PREPARE_FOCUS_EN,
  PRINT_MACHINE_STATUS_PREPARE_UPDATE_EN,
  PRINT_MACHINE_STATUS_PREPARE_STOP_TASK_EN,
  PRINT_MACHINE_STATUS_PREPARE_QUIT_TASK_EN,
  PRINT_MACHINE_STATUS_PREPARE_HOMEING_EN,
  PRINT_MACHINE_STATUS_PREPARE_SAVE_OFFSET_EN,
  PRINT_MACHINE_STATUS_PREPARE_CALIBRATION_EN,
  PRINT_MACHINE_STATUS_PREPARE_LEVELING_EN,
  PRINT_MACHINE_STATUS_PREPARE_RESTORE_EN,
  LASER_MACHINE_STATUS_PREPARE_WALKING_BORDER_EN,

  PRINT_PREPARE_STATUS_LOAD,
  PRINT_PREPARE_STATUS_UNLOAD,
  PRINT_MACHINE_STATUS_NULL,
  PRINT_MACHINE_STATUS_NO_SET_LANGUAGE,
  PRINT_MACHINE_STATUS_NO_SET_LAN_NO_HEAD,
};

#define MACHINE_STATUS_NUM            51
#define MACHINE_STATUS_PREPARE_NUM    15

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
  CMD_PRINT_AXIS_MOVE,
  CMD_PRINT_SETTING,
  CMD_FILAMENT,
  CMD_PRINT_MACHINE_STATUS,
  CMD_PAUSE_CONTINUE,
};

enum language_type : uint8_t {
  LAN_NULL = 0,
  LAN_CHINESE,
  LAN_ENGLISH,
};

enum usb_status : char {
  NULL_INSERT = 0,
  USB_INSERT,
  PC_CABLE_INSERT,
  FILAMENT_INSERT,
};

enum machine_type : uint8_t {
  MACHINE_TYPE_NULL = 0,
  MACHINE_TYPE_PRINT,
  MACHINE_TYPE_LASER,
};

enum lcd_data_status : uint8_t {
  LCD_DATA_ERROR,
  LCD_DATA_FULL,
  LCD_DATA_NO_FULL,
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

#define GET_MAX_INDEX(X) (PAGE_FILE_NUM*X.page_count + X.last_page_file_count)
//#define DEBUGPRINTF(...)
#define DEBUGPRINTF(...) SERIAL_OUT(printf, __VA_ARGS__)

//marco defines have relation with UI layout
#define PAGE_FILE_NUM                               3
#define SEND_IMAGE_LEN                              250
#define FILE_NAME_LEN                               32
#define FILE_NAME_LCD_LEN                           (FILE_NAME_LEN + 2)
#define FILE_TEXT_LEN                               (FILE_NAME_LEN/2 + 1)
#define FILE_NUM_MAX                                40
#define DATA_BUF_SIZE		                            10  //lcd_recive_buf_size
#define MAX_SEND_BUF		                            256
#define FILE_NUM_MAX                                40

//marco defines have relation with lcd command
#define	WRITE_REGISTER_ADDR	                        0x80
#define	READ_REGISTER_ADDR	                        0x81
#define	WRITE_VARIABLE_ADDR	                        0x82
#define	READ_VARIABLE_ADDR	                        0x83
#define HEAD_ONE                                    (0x5A)
#define HEAD_TWO                                    (0xA5)
#define TAIL_ONE                                    (0x4F)
#define TAIL_TWO                                    (0x4B)
#define PRINT_SIMAGE_X_POSITION_BASE                84
#define PRINT_SIMAGE_X_OFFSET                       80
#define PRINT_SIMAGE_X_POSITION(A)                  (PRINT_SIMAGE_X_POSITION_BASE + A*PRINT_SIMAGE_X_OFFSET)
#define PRINT_SIMAGE_Y_POSITION                     380
#define PRINT_LIMAGE_X_POSITION                     124
#define PRINT_LIMAGE_Y_POSITION                     320
#define PAGE_BASE	                                  (unsigned long)0x5A010000
#define PICTURE_DISPLAY_BASE1                       (unsigned long)0x5A018000
#define PICTURE_DISPLAY_BASE2                       (unsigned long)0x00000000
#define PICTURE_ADDR                                0x00A6
#define	PAGE_ADDR	                                  0x0084
#define UUID_ADDR                                   0x16B8

//print page chinese
#define START_UP_LANGUAGE_SET_PAGE                  2
#define PRINT_HOME_PAGE_CH                          4
#define PRINT_FILE_LIST_ONLY_ONE_PAGE               6
#define PRINT_FILE_LIST_FIRST_PAGE                  7
#define PRINT_FILE_LIST_END_PAGE                    8
#define PRINT_FILE_LIST_MIDDLE_PAGE                 9
#define PRINT_FILE_PRINT_STANDARD_START_PAGE_CH     10
#define PRINT_FILE_PRINT_NOSTANDARD_START_PAGE_CH   11
#define PRINT_FILE_PRINT_STANDARD_STOP_PAGE_CH      13
#define PRINT_FILE_PRINT_NOSTANDARD_STOP_PAGE_CH    14
#define PRINT_BEEP_OFF_SET_PAGE_CH                  18
#define PRINT_BEEP_ON_SET_PAGE_CH                   19
#define PRINT_XYZ_MOVE_PAGE_CH                      21
#define PRINT_LANGUAGE_SET_PAGE_CH                  23
#define SET_LANGUAGE_CONFIRM_PAGE_CH                25
#define PRINT_INFO_PAGE_CH                          27
#define EXCEPTION_NO_HEAD_PAGE_CH                   28
#define PRINT_FIRMWARE_EXCEPTION_PAGE_CH            31
#define PRINT_FIRMWARE_UPDATE_FAIL_PAGE_CH          32
#define PRINT_CALIBRATION_PAGE_CH                   33
#define PRINT_PREPARE_BLOCK_PAGE_CH                 34
#define PRINT_UPDATE_PAGE_CH                        35
#define PRINT_PREPARE_HEAT_PAGE_CH                  36
#define PRINT_EXCEPTION_SURE_PAGE_CH                38
#define PRINT_CONFIRM_CANCEL_HINT_PAGE_CH           40
#define PRINT_EXCEPTION_COMPLETE_HINT_PAGE_CH       41
#define HOST_COMPUTER_PRINT_PAGE_CH                 43
#define PRINT_UNLOAD_FIILMENT_PAGE_CH               44
#define PRINT_EXCEPTION_SURE_RETURN_PAGE_CH         69

//print page english
#define PRINT_HOME_PAGE_EN                          60
#define PRINT_FILE_PRINT_STANDARD_START_PAGE_EN     60
#define PRINT_FILE_PRINT_NOSTANDARD_START_PAGE_EN   60
#define PRINT_FILE_PRINT_STANDARD_STOP_PAGE_EN      60
#define PRINT_FILE_PRINT_NOSTANDARD_STOP_PAGE_EN    60
#define PRINT_BEEP_OFF_SET_PAGE_EN                  60
#define PRINT_BEEP_ON_SET_PAGE_EN                   60
#define PRINT_XYZ_MOVE_PAGE_EN                      60
#define PRINT_LANGUAGE_SET_PAGE_EN                  60
#define SET_LANGUAGE_CONFIRM_PAGE_EN                60
#define PRINT_INFO_PAGE_EN                          27
#define EXCEPTION_NO_HEAD_PAGE_EN                   60
#define PRINT_FIRMWARE_EXCEPTION_PAGE_EN            31
#define PRINT_FIRMWARE_UPDATE_FAIL_PAGE_EN          32
#define PRINT_CALIBRATION_PAGE_EN                   33
#define PRINT_PREPARE_BLOCK_PAGE_EN                 34
#define PRINT_UPDATE_PAGE_EN                        35
#define PRINT_PREPARE_HEAT_PAGE_EN                  36
#define PRINT_EXCEPTION_SURE_PAGE_EN                60
#define PRINT_CONFIRM_CANCEL_HINT_PAGE_EN           60
#define PRINT_EXCEPTION_COMPLETE_HINT_PAGE_EN       60
#define HOST_COMPUTER_PRINT_PAGE_EN                 60
#define PRINT_EXCEPTION_SURE_RETURN_PAGE_EN         69

//laser page chinese
#define LASER_HOME_PAGE_CH                          46
#define LASER_FILE_LIST_ONLY_ONE_PAGE               47
#define LASER_FILE_LIST_FIRST_PAGE                  48
#define LASER_FILE_LIST_END_PAGE                    49
#define LASER_FILE_LIST_MIDDLE_PAGE                 50
#define LASER_FILE_PRINT_STANDARD_START_PAGE_CH     51
#define LASER_FILE_PRINT_NOSTANDARD_START_PAGE_CH   51
#define LASER_FILE_PRINT_STANDARD_STOP_PAGE_CH      52
#define LASER_FILE_PRINT_NOSTANDARD_STOP_PAGE_CH    52
#define LASER_AXIS_MOVE_AJUST_PAGE_CH               54
#define LASER_BEEP_OFF_SET_PAGE_CH                  57
#define LASER_BEEP_ON_SET_PAGE_CH                   58
#define LASER_LINE_CHOICE_PAGE_CH                   60
#define LASER_PREPARE_HEAT_PAGE_CH                  61
#define LASER_EXCEPTION_SURE_PAGE_CH                62
#define LASER_EXCEPTION_SURE_RETURN_PAGE_CH         63
#define LASER_INFO_PAGE_CH                          64
#define LASER_PREPARE_BLOCK_PAGE_CH                 65
#define LASER_CONFIRM_CANCEL_HINT_PAGE_CH           67
#define LASER_EXCEPTION_COMPLETE_HINT_PAGE_CH       1000

//laser page english
#define LASER_HOME_PAGE_EN                          46
#define LASER_FILE_PRINT_STANDARD_START_PAGE_EN     51
#define LASER_FILE_PRINT_NOSTANDARD_START_PAGE_EN   52
#define LASER_FILE_PRINT_STANDARD_STOP_PAGE_EN      51
#define LASER_FILE_PRINT_NOSTANDARD_STOP_PAGE_EN    52
#define LASER_AXIS_MOVE_AJUST_PAGE_EN               54
#define LASER_BEEP_OFF_SET_PAGE_EN                  57
#define LASER_BEEP_ON_SET_PAGE_EN                   58
#define LASER_LINE_CHOICE_PAGE_EN                   60
#define LASER_PREPARE_HEAT_PAGE_EN                  61
#define LASER_EXCEPTION_SURE_PAGE_EN                62
#define LASER_EXCEPTION_SURE_RETURN_PAGE_EN         63
#define LASER_INFO_PAGE_EN                          64
#define LASER_PREPARE_BLOCK_PAGE_EN                 65
#define LASER_CONFIRM_CANCEL_HINT_PAGE_EN           67
#define LASER_EXCEPTION_COMPLETE_HINT_PAGE_EN       1000

//print icon_addr
#define PRINT_STATUS_BAR_USB_ICON_ADDR              0X1000
#define PRINT_STATUS_BAR_PC_ICON_ADDR               0X1001
#define PRINT_FILE_SIMAGE_ICON_ADDR                 0X1002
#define PRINT_FILE_LIMAGE_ICON_ADDR                 0X1008
#define PRINT_MACHINE_STATUS_ICON_ADDR              0X1009
#define PRINT_PREPARE_PROGRESS_ICON_ADDR            0X100A
#define PRINT_PREPARE_TEXT_ICON_ADDR                0X100B
#define PRINT_HOST_PRINT_ICON_ADDR                  0X100C
#define PRINT_RETURN_ICON_ADDR                      0X100D
#define PRINT_STATUS_BAR_FILAMENT_ICON_ADDR         0X100E
#define PRINT_STATUS_ALL_BAR_STATUS_ADDR            0X100F

//print text addr
#define PRINT_TEMP_HOTEND_ADDR                      0X1040
#define PRINT_TEMP_BED_ADDR                         0X1044
#define PRINT_FILE_TEXT_BASE_ADDR                   0X1050  //list_box and print file name
#define PRINT_FILE_PRINT_TEXT_ADDR                  0X1083  //(PRINT_FILE_TEXT_BASE_ADDR + FILE_NAME_LEN*PAGE_FILE_NUM)
#define PRINT_FILE_PAGE_NUM_ADDR                    0X1170
#define PRINT_FILE_PRINT_TIME_ADDR                  0X1174
#define PRINT_PREPARE_PERCENTAGE_ADDR               0X1179
#define PRINT_MACHINE_INFO_ID_ADDR                  0X117C
#define PRINT_MACHINE_INFO_FIRMWARE_ADDR            0X1186
#define PRINT_MACHINE_INFO_UPTIME_ADDR              0X1192
#define PRINT_MACHINE_INFO_PRINT_TIME_ADDR          0X1199
#define PRINT_MACHINE_INFO_LASER_TIME_ADDR          0X11A1

//print button addr
#define PRINT_FILE_LIST_BUTTONS                     0x1200
#define PRINT_FILE_SELECT_BUTTONS                   0x1202
#define PRINT_FILE_PRINT_BUTTONS                    0x1204
#define PRINT_SET_PAGE_XYZ_AXIS_BTN_RELEASE         0x120E
#define PRINT_SET_PAGE_X_AXIS_MOVE_MIN_BTN          0x1210
#define PRINT_SET_PAGE_X_AXIS_MOVE_ADD_BTN          0x1211
#define PRINT_SET_PAGE_Y_AXIS_MOVE_MIN_BTN          0x1212
#define PRINT_SET_PAGE_Y_AXIS_MOVE_ADD_BTN          0x1213
#define PRINT_SET_PAGE_Z_AXIS_MOVE_MIN_BTN          0x1214
#define PRINT_SET_PAGE_Z_AXIS_MOVE_ADD_BTN          0x1215
#define PRINT_SET_PAGE_HOME_MOVE_BTN                0x1216
#define LASER_SET_XY_AXIS_ZERO_BTN                  0x1217
#define HOME_OFFSET_BTN                             0x121C

#define PRINT_SET_PAGE_SET_BTN                      0x1218
#define PRINT_HOME_PAGE_FILEMENT_BTN   	            0x121A
#define PRINT_MACHINE_STATUS_BTN      	            0x121B

enum print_status {
  out_printing,prepare_printing,on_printing,stop_printing,
};

#endif // USE_DWIN_LCD
#endif // _LCD_FILE_H_