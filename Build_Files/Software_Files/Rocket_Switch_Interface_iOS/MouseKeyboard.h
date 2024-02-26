/* 
* File: MouseKeyboard.h
* Firmware: Rocket_Switch_Interface_iOS.ino 
* Developed by: MakersMakingChange
* Version: 1.1 (31/07/2023)
* Copyright Neil Squire Society 2023. 
* License: This work is licensed under the CC BY SA 4.0 License: http://creativecommons.org/licenses/by-sa/4.0 .
*/

#include "Adafruit_TinyUSB.h"
#pragma once

#define ATTRIBUTE_PACKED  __attribute__((packed, aligned(1)))

#define RID_KEYBOARD 1
#define RID_MOUSE 2
  
#define MOUSE_LEFT 1
#define MOUSE_RIGHT 2
#define MOUSE_MIDDLE 4
#define MOUSE_ALL (MOUSE_LEFT | MOUSE_RIGHT | MOUSE_MIDDLE)

//uint8_t const _ascii2keycode[128][2] = {HID_ASCII_TO_KEYCODE};

uint8_t const desc_hid_report[] =
{
    TUD_HID_REPORT_DESC_KEYBOARD( HID_REPORT_ID(RID_KEYBOARD) ),
    TUD_HID_REPORT_DESC_MOUSE( HID_REPORT_ID(RID_MOUSE) )
};


class USBMouse {
  private:
    void mouseReport(signed char b, signed char x, signed char y, signed char wheel = 0,signed char pan = 0); 
  public:
    inline USBMouse(void);
    inline void begin(void);
    inline void end(void);
	  inline void wakeup(void);
    inline void move(signed char x, signed char y);
    inline void moveAll(signed char x, signed char y, signed char wheel = 0,signed char pan = 0); 
    inline void scroll(signed char wheel = 0);
    inline void pan(signed char pan = 0); 
    inline void click(uint8_t b = MOUSE_LEFT);
    inline void press(uint8_t b = MOUSE_LEFT);   // press LEFT by default
    inline void release(uint8_t b = MOUSE_LEFT); // release LEFT by default
    inline bool isPressed(uint8_t b = MOUSE_LEFT); // check LEFT by default
	  inline bool isReady(void);
  protected:
    uint8_t _buttons;
    void buttons(uint8_t b);
    Adafruit_USBD_HID usb_hid;
};

typedef struct
{
  uint8_t modifiers;
  uint8_t reserved;
  uint8_t keys[6];
} KeyReport;


class USBKeyboard : public Print
{
	private:
		KeyReport _keyReport;
		void keyboardReport(KeyReport* keys);
    uint8_t const _ascii2keycode[128][2] = {HID_ASCII_TO_KEYCODE};
	public:
		inline USBKeyboard(void);
		inline void begin(void);
		inline void end(void);
		inline void wakeup(void);
		inline size_t write(uint8_t k);
		inline size_t write(const uint8_t *buffer, size_t size);
		inline size_t press(uint8_t m, uint8_t k);
		inline size_t release(uint8_t m, uint8_t k);
		inline void releaseAll(void);
		inline bool isReady(void);
  protected:
    Adafruit_USBD_HID usb_hid;
};

/*****************************
 *   MOUSE SECTION
 *****************************/ 

USBMouse::USBMouse(void)
{
  _buttons = 0;
  this->usb_hid.setPollInterval(1);
  this->usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));

}

void USBMouse::begin(void)
{
  this->usb_hid.begin();
  //while( !USBDevice.mounted() ) delay(1);
}


void USBMouse::end(void)
{
}

void USBMouse::wakeup(void)
{
	if ( USBDevice.suspended() )  {
      USBDevice.remoteWakeup();
    }
}

void USBMouse::mouseReport(int8_t b, int8_t x, int8_t y, int8_t wheel, int8_t pan) 
{
	wakeup();
    while(!isReady()) delay(1);
    usb_hid.mouseReport(RID_MOUSE,b,x,y,wheel,pan);
}

void USBMouse::move(int8_t x, int8_t y) 
{
    mouseReport(_buttons, x, y, 0, 0);
}
void USBMouse::moveAll(int8_t x, int8_t y, int8_t wheel, int8_t pan) 
{
    mouseReport(_buttons, x, y, wheel, pan);
}

void USBMouse::scroll(int8_t wheel) 
{
    mouseReport(_buttons, 0, 0, wheel, 0);
}

void USBMouse::pan(int8_t pan) 
{
    mouseReport(_buttons, 0, 0, 0, pan);
}

void USBMouse::click(uint8_t b)
{
  _buttons = b;
  mouseReport(_buttons, 0, 0, 0, 0);
  _buttons = 0;
  mouseReport(_buttons, 0, 0, 0, 0);
}

void USBMouse::buttons(uint8_t b)
{
	if (b != _buttons)
	{
	  _buttons = b;
	  mouseReport(_buttons, 0, 0, 0, 0);
	}
}

void USBMouse::press(uint8_t b) 
{
	buttons(_buttons | b);
}

void USBMouse::release(uint8_t b)
{
	buttons(_buttons & ~b);
}

bool USBMouse::isPressed(uint8_t b)
{
	if ((b & _buttons) > 0) 
	  return true;
	return false;
}

bool USBMouse::isReady(void)
{
	if (usb_hid.ready()) 
	  return true;
	return false;
}

/*****************************
 *   KEYBOARD SECTION
 *****************************/ 

USBKeyboard::USBKeyboard(void) 
{
}

void USBKeyboard::begin(void)
{
	usb_hid.setPollInterval(2);
	usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
	usb_hid.begin();
	while( !USBDevice.mounted() ) delay(1);
}

void USBKeyboard::end(void)
{
}

void USBKeyboard::keyboardReport(KeyReport* keys)
{
	wakeup();
	while(!isReady()) delay(1);
	usb_hid.keyboardReport(RID_KEYBOARD,keys->modifiers,keys->keys);
	delay(2);
}

void USBKeyboard::wakeup(void)
{
	if ( USBDevice.suspended() )  {
      USBDevice.remoteWakeup();
    }
}

size_t USBKeyboard::press(uint8_t m, uint8_t k) 
{
	uint8_t i;
	_keyReport.modifiers = m;

	//Add key if the it's not already present and if there is an empty spot.
	if (_keyReport.keys[0] != k && _keyReport.keys[1] != k && 
	  _keyReport.keys[2] != k && _keyReport.keys[3] != k &&
	  _keyReport.keys[4] != k && _keyReport.keys[5] != k) {
	  
	  for (i=0; i<6; i++) {
		if (_keyReport.keys[i] == 0x00) {
		  _keyReport.keys[i] = k;
		  break;
		}
	  }
	  if (i == 6) {
		setWriteError();
		return 0;
	  } 
	}
	keyboardReport(&_keyReport);
	return 1;
}


size_t USBKeyboard::release(uint8_t m, uint8_t k) 
{
	uint8_t i;
	_keyReport.modifiers = 0x00;
	//Check to see if the key is present and clear it if it exists.
	for (i=0; i<6; i++) {
	  if (0 != k && _keyReport.keys[i] == k) {
		_keyReport.keys[i] = 0x00;
	  }
	}
	keyboardReport(&_keyReport);
	return 1;
}

void USBKeyboard::releaseAll(void)
{
	_keyReport.keys[0] = 0;
	_keyReport.keys[1] = 0; 
	_keyReport.keys[2] = 0;
	_keyReport.keys[3] = 0; 
	_keyReport.keys[4] = 0;
	_keyReport.keys[5] = 0; 
	_keyReport.modifiers = 0;
	keyboardReport(&_keyReport);
}
  
size_t USBKeyboard::write(uint8_t c)
{
  uint8_t keycode = 0;
  uint8_t modifier = 0;
  uint8_t uch = (uint8_t)c;
  
  if (_ascii2keycode[c][0]) {
    modifier = KEYBOARD_MODIFIER_LEFTSHIFT;
  }
  
  keycode = _ascii2keycode[uch][1];
  
	uint8_t p = press(modifier, keycode);  // Keydown
	release(modifier, keycode);            // Keyup
	return p;              // Return the result of press() 
}

size_t USBKeyboard::write(const uint8_t *buffer, size_t size) {
	size_t n = 0;
	while (size--) {
	  if (*buffer != '\r') {
		if (write(*buffer)) {
		  n++;
		} else {
		  break;
		}
	  }
	  buffer++;
	}
	return n;
}

bool USBKeyboard::isReady(void)
{
	if (usb_hid.ready()) 
	  return true;
	return false;
}
