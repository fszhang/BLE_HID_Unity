#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "BLE2902.h"
#include "BLEHIDDevice.h"
#include "HIDTypes.h"
#include "HIDKeyboardTypes.h"
#include <driver/adc.h>
#include "sdkconfig.h"
#include "BleConnectionStatus.h"
#include "BleGamepad.h"

#if defined(CONFIG_ARDUHAL_ESP_LOG)
#include "esp32-hal-log.h"
#define LOG_TAG "BLEGamepad"
#else
#include "esp_log.h"
static const char* LOG_TAG = "BLEGamepad";
#endif

BleGamepad bleGamepad;

static const uint8_t _hidReportDescriptor[] = {
    USAGE_PAGE(1),       0x01, // USAGE_PAGE (Generic Desktop)
    USAGE(1),            0x05, // USAGE (Gamepad)
    COLLECTION(1),       0x01, //   COLLECTION (Application)
        COLLECTION(1),       0x00, //   COLLECTION (Physical)
            REPORT_ID(1),       GAMEPAD_ID,
            // ------------------------------------------------- Dpad and Buttons (1 to 16)
            USAGE(1),            0x39, //     USAGE_PAGE (Dpad)
            LOGICAL_MINIMUM(1),  0x01, //     LOGICAL_MINIMUM (0)
            LOGICAL_MAXIMUM(1),  0x08, //     LOGICAL_MAXIMUM (1)
            REPORT_SIZE(1),      0x04, //     REPORT_SIZE (4)
            REPORT_COUNT(1),     0x01, //     REPORT_COUNT (1)
            HIDINPUT(1),         0x42, //     INPUT (Data, Variable, Absolute)  1 Dpad
            
            USAGE_PAGE(1),       0x09, //     USAGE_PAGE (Button)   12 Buttons
            USAGE_MINIMUM(1),    0x01, //     USAGE_MINIMUM (Button 1)
            USAGE_MAXIMUM(1),    0x0C, //     USAGE_MAXIMUM (Button 12)
            LOGICAL_MINIMUM(1),  0x00, //     LOGICAL_MINIMUM (0)
            LOGICAL_MAXIMUM(1),  0x01, //     LOGICAL_MAXIMUM (1)
            REPORT_SIZE(1),      0x01, //     REPORT_SIZE (1)
            REPORT_COUNT(1),     0x0C, //     REPORT_COUNT (12)
            HIDINPUT(1),         0x02, //     INPUT (Data, Variable, Absolute) ;12 button bits
            
            // ------------------------------------------------- X/Y/Z position
            USAGE_PAGE(1), 	    0x01,           //		USAGE_PAGE (Generic Desktop)
            USAGE(1), 		    0x30,           //      USAGE (X)
            USAGE(1), 		    0x31,           //      USAGE (Y)
            USAGE(1), 		    0x32,           //      USAGE (Z)  
            0x15, 			    0x00,           //      LOGICAL_MINIMUM (0)
            0x26, 			    0xFF, 0x00,     //      LOGICAL_MAXIMUM (255)
            REPORT_SIZE(1),     0x08,           //		REPORT_SIZE (8)
            REPORT_COUNT(1),    0x03,           //		REPORT_COUNT (3)
            HIDINPUT(1), 	    0x02,           //      INPUT (Data,Var,Abs)
            // ------------------------------------------------- rX/rY/rZ position
            USAGE(1),            0x33,  //     USAGE (rX) Left Trigger
            USAGE(1),            0x34,  //     USAGE (rY) Right Trigger
            USAGE(1),            0x35,  //     USAGE (rZ) 
            0x15, 0x00,                 // 	   Logical Minimum (0)
            0x26, 0xFF, 0x00,           // 	   Logical Maximum (255)
            REPORT_SIZE(1),      0x08,  //     REPORT_SIZE (8)
            REPORT_COUNT(1),     0x03,  //     REPORT_COUNT (3)
            HIDINPUT(1),         0x02,  //     INPUT (Data, Variable, Absolute) ;3 bytes (rX,rY,rZ)
            
            // ------------------------------------------------- Vendor defined
            0x09,  0x03,                // USAGE ID - Vendor defined
            0x15,  0x00,                // LOGICAL_MINIMUM (0)
            0x26,  0xff, 0x00,          // LOGICAL_MAXIMUM (255)
            REPORT_SIZE(1),0x08,        // REPORT_SIZE (8)
            REPORT_COUNT(1),GP_VDATAIN_LEN,         // REPORT_COUNT (10)
            0x81, 0x02,                 // INPUT (Data,Var,Abs)
            
            // The Output report
            0x09,   0x03,               //     USAGE ID - Vendor defined
            0x15,   0x00,               //     LOGICAL_MINIMUM (0)
            0x26,   0xff, 0x00,         //     LOGICAL_MAXIMUM (1)
            REPORT_SIZE(1), 0x08,       //     REPORT_SIZE (1)
            REPORT_COUNT(1),GP_VDATAOUT_LEN,         //     REPORT_COUNT (10)
            0x91, 0x02, // OUTPUT (Data,Var,Abs) 
        END_COLLECTION(0),         //     END_COLLECTION 
    END_COLLECTION(0),         //     END_COLLECTION
};

BleGamepad::BleGamepad(std::string deviceName, std::string deviceManufacturer, uint8_t batteryLevel) :
_autoReport(false),
hid(0)
{
    this->deviceName = deviceName;
    this->deviceManufacturer = deviceManufacturer;
    this->batteryLevel = batteryLevel;
    this->connectionStatus = new BleConnectionStatus();
}

void BleGamepad::begin(bool autoReport)
{
    _autoReport = autoReport;
    xTaskCreate(this->taskServer, "server", 20000, (void *)this, 5, NULL);
}

void BleGamepad::end(void)
{
    
}

void BleGamepad::sendReport(uint8_t ID, uint8_t* reportData, uint8_t DataLen)
{
    if (this->isConnected())
    {        
        switch(ID)
        {
            case GAMEPAD_ID:
                this->inputGamepad->setValue(reportData, DataLen);
                this->inputGamepad->notify();
                break;  
            default:
                break;  
        }      
    }
}

void BleGamepad::setAutoReport(bool autoReport)
{
    _autoReport = autoReport;
}

bool BleGamepad::isConnected(void)
{
    return this->connectionStatus->connected;
}

void BleGamepad::setBatteryLevel(uint8_t level)
{
    this->batteryLevel = level;
    if (hid != 0)
        this->hid->setBatteryLevel(this->batteryLevel);
}

void BleGamepad::taskServer(void* pvParameter)
{
    BleGamepad* BleGamepadInstance = (BleGamepad *) pvParameter; //static_cast<BleGamepad *>(pvParameter);
    BLEDevice::init(BleGamepadInstance->deviceName);
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(BleGamepadInstance->connectionStatus);

    BleGamepadInstance->hid = new BLEHIDDevice(pServer);
    BleGamepadInstance->inputGamepad = BleGamepadInstance->hid->inputReport(GAMEPAD_ID); // <-- input REPORTID from report map
    BleGamepadInstance->outputGamePadData = BleGamepadInstance->hid->outputReport(GAMEPAD_ID);


    BleGamepadInstance->connectionStatus->inputGamepad = BleGamepadInstance->inputGamepad;
    BleGamepadInstance->connectionStatus->outputGamePadData = BleGamepadInstance->outputGamePadData;
    
    BleGamepadInstance->outputGamePadData->setCallbacks(new GamepadOutputCallbacks());

    BleGamepadInstance->hid->manufacturer()->setValue(BleGamepadInstance->deviceManufacturer);
    BleGamepadInstance->hid->pnp(0x02, 0x045e, 0x028e, 0x0002);
    BleGamepadInstance->hid->hidInfo(0x00, 0x02);
    
    BLESecurity *pSecurity = new BLESecurity();
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);
    
    BleGamepadInstance->hid->reportMap((uint8_t*)_hidReportDescriptor, sizeof(_hidReportDescriptor));
    BleGamepadInstance->hid->startServices();
    
    BleGamepadInstance->onStarted(pServer);
    
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->setAppearance(HID_GAMEPAD);
    pAdvertising->addServiceUUID(BleGamepadInstance->hid->hidService()->getUUID());
    //pAdvertising->setScanResponse(false);
    pAdvertising->start();
    BleGamepadInstance->hid->setBatteryLevel(100);
    
    ESP_LOGD(LOG_TAG, "Advertising started!");
    vTaskDelay(portMAX_DELAY); //delay(portMAX_DELAY);
}
