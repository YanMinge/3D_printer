#ifdef TARGET_LPC1768

extern "C" {
  #include <debug_frmwrk.h>
}

#include "../../sd/cardreader.h"
#include "../../inc/MarlinConfig.h"
#include "HAL.h"
#include "HAL_timers.h"

#if ENABLED(USB_DISK_SUPPORT)
  #include "udisk_reader.h"
#endif

#if ENABLED(FACTORY_MACHINE_INFO)
  #include "machine_info.h"
#endif

#ifdef USE_MATERIAL_MOTION_CHECK
  #include "material_check.h"
#endif

extern uint32_t MSC_SD_Init(uint8_t pdrv);
extern "C" int isLPC1769();
extern "C" void disk_timerproc(void);

#if ENABLED(USB_DISK_SUPPORT)
static int start_systick = false;
#endif

void SysTick_Callback() {

#if ENABLED(USB_DISK_SUPPORT)
  if(start_systick == true && udisk.is_usb_Initialized() == true)
  {
    udisk.usb_status_polling();
  }
  udisk.print_time_countdown();
#endif
  
#ifdef USE_MATERIAL_MOTION_CHECK
  MaterialCheck.code_wheel_step_update();
  MaterialCheck.material_extrusion_update();
#endif

  disk_timerproc();
}

void HAL_init() {

  // Support the 4 LEDs some LPC176x boards have
  #if PIN_EXISTS(LED)
    SET_DIR_OUTPUT(LED_PIN);
    WRITE_PIN_CLR(LED_PIN);
    #if PIN_EXISTS(LED2)
      SET_DIR_OUTPUT(LED2_PIN);
      WRITE_PIN_CLR(LED2_PIN);
      #if PIN_EXISTS(LED3)
        SET_DIR_OUTPUT(LED3_PIN);
        WRITE_PIN_CLR(LED3_PIN);
        #if PIN_EXISTS(LED4)
          SET_DIR_OUTPUT(LED4_PIN);
          WRITE_PIN_CLR(LED4_PIN);
        #endif
      #endif
    #endif

    // Flash status LED 3 times to indicate Marlin has started booting
    for (uint8_t i = 0; i < 6; ++i) {
      TOGGLE(LED_PIN);
      delay(100);
    }
  #endif

  //debug_frmwrk_init();
  //_DBG("\n\nDebug running\n");
  // Initialise the SD card chip select pins as soon as possible
  #if PIN_EXISTS(SS)
    WRITE(SS_PIN, HIGH);
    SET_OUTPUT(SS_PIN);
  #endif

  #if ENABLED(FACTORY_MACHINE_INFO)
    MachineInfo.init();
  #endif

  #ifdef USE_MATERIAL_MOTION_CHECK
    MaterialCheck.init();
  #endif

  #if defined(ONBOARD_SD_CS) && ONBOARD_SD_CS > -1
    WRITE(ONBOARD_SD_CS, HIGH);
    SET_OUTPUT(ONBOARD_SD_CS);
  #endif
  //USB_Init();                               // USB Initialization
  //USB_Connect(FALSE);                       // USB clear connection
  //delay(3000);                              // Give OS time to notice
  //USB_Connect(TRUE);

  #if DISABLED(USB_SD_DISABLED)
    MSC_SD_Init(0);                         // Enable USB SD card access
  #endif

  #if NUM_SERIAL > 0
    MYSERIAL0.begin(BAUDRATE);
    #if NUM_SERIAL > 1
      MYSERIAL1.begin(BAUDRATE);
    #endif
    SERIAL_PRINTF("\n\necho:%s (%dMhz) Initialized\n", isLPC1769() ? "LPC1769" : "LPC1768", SystemCoreClock / 1000000);
    SERIAL_FLUSHTX();
  #endif

  HAL_timer_init();
  start_systick = true;
}

// HAL idle task
void HAL_idletask(void) {
  #if BOTH(SDSUPPORT, SHARED_SD_CARD)
    // If Marlin is using the SD card we need to lock it to prevent access from
    // a PC via USB.
    // Other HALs use IS_SD_PRINTING() and IS_SD_FILE_OPEN() to check for access but
    // this will not reliably detect delete operations. To be safe we will lock
    // the disk if Marlin has it mounted. Unfortuately there is currently no way
    // to unmount the disk from the LCD menu.
    // if (IS_SD_PRINTING() || IS_SD_FILE_OPEN())
    if (card.isDetected())
      MSC_Aquire_Lock();
    else
      MSC_Release_Lock();
  #endif
  // Perform USB stack housekeeping
}

#endif // TARGET_LPC1768
