#ifndef BLE_OUTPUT_CALLBACKS_H
#define BLE_OUTPUT_CALLBACKS_H
#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)

#include <BLEServer.h>
#include "BLE2902.h"
#include "BLECharacteristic.h"
#include "BleGamepad.h"
#include "GamepadCtrl.h"

extern uint8_t gamepadOutData[GP_VDATAOUT_LEN];
extern uint8_t gamepadDataGet;
extern uint8_t gamepadOutDataLen;


class GamepadOutputCallbacks : public BLECharacteristicCallbacks
{
public:
    GamepadOutputCallbacks(void);
    void onWrite(BLECharacteristic* me);
};


#endif // CONFIG_BT_ENABLED
#endif // BLE_KEYBOARD_OUTPUT_CALLBACKS_H
