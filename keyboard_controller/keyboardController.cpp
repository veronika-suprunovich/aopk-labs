#include <dos.h>
#include <conio.h>
#include <stdio.h>

#define true 1
#define false 0
#define ATTEMPTS_COUNT 3

struct VIDEO {
  unsigned char symbol;
  unsigned char attribute;
};

const unsigned char defaultAttribute = 0x40;

void printScanCode(int scanCode);

void setIndicator(unsigned char mask);
void blink(void);

int resendInfo = true;
int bContinue = true;

void interrupt(*keyboardInterruptHandler)(...);
void interrupt newKeyboardInterruptHandler(...) {
  unsigned char scanCode = 0;
  scanCode = inp(0x60);
  if (scanCode == 0x01) bContinue = false;

  if (scanCode == 0xFE) {
    resendInfo = true;
  } else resendInfo = false;

  printScanCode(scanCode);
  outp(0x20, 0x20);							// Сброс контроллера прерываний
}

void setKeyboardInterruptHandler();
void resetKeyboardInterruptHandler();

int main() {

  setKeyboardInterruptHandler();

  blink();

	while (bContinue);

  resetKeyboardInterruptHandler();

	return 0;
}

int writeToPort(unsigned char mask) {

  for(int i = 0; i <= ATTEMPTS_COUNT; i++) {
    while(inp(0x64) & 0x02);
    outp(0x60, mask);
    delay(50);
    if(resendInfo == 0) return true;
  }

  return false;
}

void setIndicator(unsigned char mask) {
	resendInfo = 1;
  if(!writeToPort(0xED)) {
    return;
  }
	resendInfo = 1;
  if(!writeToPort(mask)) {
    return;
  }
}

unsigned char mask[] = {
  0x06,  0x02, 0x02, 0x02, 0x02, 0x06, 0x06, 0x02,
  0x02, 0x02, 0x02, 0x06, 0x02, 0x02, 0x06,
  0x02, 0x06, 0x02, 0x06, 0x06, 0x06,
  0x06, 0x06, 0x02, 0x06, 0x06, 0
};

void blink() {

  for(int i = 0; mask[i] != 0; i++) {
    setIndicator(mask[i]);
    delay(200);
    setIndicator(0x00);
    delay(200);
  }
}

void setKeyboardInterruptHandler() {
  _disable();
  keyboardInterruptHandler = getvect(0x09); // keyboard
  setvect(0x09, newKeyboardInterruptHandler);
  _enable();
}

void resetKeyboardInterruptHandler() {
  _disable();
  setvect(0x09, keyboardInterruptHandler);
  _enable();
}

void printScanCode(int scanCode) {
  VIDEO far* screen = (VIDEO far*)MK_FP(0xB800, 0);

  for(int i = 1; i >= 0; i--) {
    int currentBits = ((scanCode >> (i * 4)) & 0x0F);
    if (currentBits < 10) {
      screen->symbol = currentBits + '0';
    } else {
      screen->symbol = currentBits - 10 + 'A';
    }

    screen->attribute = defaultAttribute;
    screen++;
  }
}
