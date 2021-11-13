#include "BleOutputCallbacks.h"

#if defined(CONFIG_ARDUHAL_ESP_LOG)
  #include "esp32-hal-log.h"
  #define LOG_TAG ""
#else
  #include "esp_log.h"
  static const char* LOG_TAG = "BLEDevice";
#endif

uint8_t gamepadOutData[GP_VDATAOUT_LEN];
uint8_t gamepadDataGet=0;
uint8_t gamepadOutDataLen;

GamepadOutputCallbacks::GamepadOutputCallbacks(void) 
{
}

void GamepadOutputCallbacks::onWrite(BLECharacteristic* me) 
{
    uint8_t* value = (uint8_t*)(me->getValue().c_str());    
    gamepadOutDataLen = me->getValue().length();
    uint8_t i;
    if(gamepadOutDataLen>GP_VDATAOUT_LEN)
        gamepadOutDataLen = GP_VDATAOUT_LEN;
    for(i=0;i<gamepadOutDataLen;i++)
    {
        gamepadOutData[i] = value[i];
    }
    //ESP_LOGI(LOG_TAG, "special keys: %d", *value);
    gamepadDataGet = 1;
}
