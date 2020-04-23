#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#define Continue 1
#define MAX_SIZE 9
#define MAX_COUNT 10000

struct Time {
  char hour;
  char minutes;
  char seconds;
};

void getRtcValue();
void setRtcValue();
void setAlarm();
void resetAlarm();
void delay();

void interrupt(*alarmInterrupt) (...);
void interrupt newAlarmInterrupt(...) {

  puts("The right time is now");


  outp(0xA1, inp(0xA1) | 0x01);

  outp(0x70, 0x0B);
  outp(0x71, inp(0x71) & 0xDF);

  alarmInterrupt();
  resetAlarm();
}


unsigned int delayTime;

void interrupt(*RTCInterrupt) (...);
void interrupt newRTCInterrupt(...) {
  delayTime--;
  RTCInterrupt();
}



int main() {

  char userInput;

  while(Continue) {
    rewind(stdin);
    printf("Select from the above\n");
    printf("1. Get time\n");
    printf("2. Set time\n");
    printf("3. Delay\n");
    printf("4. Set alarm\n");
    printf("5. Exit\n");

    userInput = getch();
    rewind(stdin);
    switch(userInput) {
      case '1':
        getRtcValue();
        break;
      case '2':
        setRtcValue();
        break;
      case '3':
        delay();
        break;
      case '4':
        setAlarm();
        break;
      case '5':
        return 0;
    }
  }
}

int bcdToInt(char* source) {
  return( ((*source) & 0x0f) + 10 * (((*source) & 0xf0) >> 4));
}


struct Time getNewTime() {
  struct Time temp;
  char *newTime;
  newTime = (char*)malloc(MAX_SIZE);
  printf("Enter time (02:45:08) << ");
  scanf("%8s", newTime);

  temp.hour = (char)((newTime[0]- 0x30) * 0x10 + newTime[1] - 0x30);
  temp.minutes = (char)((newTime[3]- 0x30) * 0x10 + newTime[4] - 0x30);
  temp.seconds = (char)((newTime[6]- 0x30) * 0x10 + newTime[7] - 0x30);

  free(newTime);
  return temp;
}


void setAlarm() {
  struct Time alarmTime = getNewTime();

  outp(0x70, 0x05);
  outp(0x71, alarmTime.hour);

  outp(0x70, 0x03);
  outp(0x71, alarmTime.minutes);

  outp(0x70, 0x01);
  outp(0x71, alarmTime.seconds);

  _disable();
  alarmInterrupt = getvect(0x4A);
  setvect(0x4A, newAlarmInterrupt);
  _enable();

  outp(0x70, 0x0B);
  outp(0x71, (inp(0x71) | 0x20));

  outp(0xA1, (inp(0xA0) & 0xFE));

}

void resetAlarm() {

  _disable();
  setvect(0x4A, alarmInterrupt);
  _enable();

  outp(0x70, 0x05);
  outp(0x71, 0);

  outp(0x70, 0x03);
  outp(0x71, 0);

  outp(0x70, 0x01);
  outp(0x71, 0);
}

void setRtcValue() {
  struct Time newTime = getNewTime();
  disable();

  unsigned int count = 0;

  while(count++ != MAX_COUNT) {
    outp(0x70, 0x0A);
    if(!(inp(0x71) & 0x80)) {
      break;
    }
  }

  outp(0x70, 0x0B);
  outp(0x71, inp(0x71) | 0x80);

  outp(0x70, 0x04);
  outp(0x71, newTime.hour);

  outp(0x70, 0x02);
  outp(0x71,  newTime.minutes);

  outp(0x70, 0x00);
  outp(0x71, newTime.seconds);

  outp(0x70, 0x0B);
  outp(0x71, inp(0x71) & 0x7F);

  enable();
}

void getRtcValue() {
  struct Time currentTime;

  outp(0x70, 0x4);
  currentTime.hour = inp(0x71);

  outp(0x70, 0x2);
  currentTime.minutes = inp(0x71);

  outp(0x70, 0x0);
  currentTime.seconds = inp(0x71);

  printf("RTC time >> %d:%d:%d\n", bcdToInt(&currentTime.hour), bcdToInt(&currentTime.minutes), bcdToInt(&currentTime.seconds));

}

void delay() {
  rewind(stdin);
  printf("Enter delay time in ms: ");

  scanf("%d", &delayTime);

  _disable();
  RTCInterrupt = getvect(0x70);
  setvect(0x70, newRTCInterrupt);
  _enable();

  printf("Program has stopped for %dms\n", delayTime);

  outp(0xA1, inp(0xA1) & 0xFE);

  outp(0x70, 0x0B);
  outp(0x71, inp(0x71) | 0x40);

  while(delayTime != 0);

  outp(0x70, 0x0B);
  outp(0x71, inp(0x71) & 0xBF);

  outp(0xA1, inp(0xA1) | 0x01);

  _disable();
  setvect(0x70, RTCInterrupt);
  _enable();

  printf("You can continue working\n");

}
