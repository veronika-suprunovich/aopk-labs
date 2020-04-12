#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#define CONTINUE 1
#define nTimes 9999

#define EXIT '0'
#define PLAY_SOUND '1'
#define PRINT_DIVISION_RATIO '2'
#define PRINT_STATUS_WORDS '3'
#define GENERATE_RANDOM_NUMBER '4'

#define ODD_NUMBER  1
#define EVEN_NUMBER 2

int octaveNoteFrequency[] = {
  277, 277, 494, 440, 392,
  392, 440, 494, 277,
  329, 440, 494, 294, 277,
  494, 440, 440, 393, 440, 494, 494,
   0
};

int clock[] = {
	150, 150, 70, 70, 70,
  150, 100, 100, 100,
  150, 100, 100, 100, 100,
  150, 100, 100, 100, 100, 100, 100,
   0
};

int pause[] = {
  300, 300, 200, 200, 200,
  200, 200, 200, 200,
  200, 200, 200, 200, 200,
  200, 200, 200, 200, 200, 200, 200,
   0
};

void printDivisionRatio(void);
void setFrequency(unsigned int octaveNoteFrequency);
void play_sound(void);
void printStatusWords(void);
void generateRandom(void);

int main() {
  char userInput;

  while(CONTINUE) {
      printf("Select from the above\n");
      printf("1. Play sound\n");
      printf("2. Print division ratio\n");
      printf("3. Print status words\n");
      printf("4. Get random number\n");
      printf("0. Exit\n");

      userInput = getch();

      switch(userInput) {
      	case PLAY_SOUND:
          play_sound();
      	  break;
        case PRINT_DIVISION_RATIO:
          printDivisionRatio();
          break;
        case PRINT_STATUS_WORDS:
          printf("Status word\n");
          printStatusWords();
          break;
        case GENERATE_RANDOM_NUMBER:
          generateRandom();
          break;
      	case EXIT:
      	  return 0;
        }
    }
}

void setFrequency(unsigned int octaveNoteFrequency) {
	long timerFrequency = 1193180, frequencyDivisionRation;
	outp(0x43, 0xB6);  // 10110110

	frequencyDivisionRation = timerFrequency / octaveNoteFrequency;
	outp(0x42, frequencyDivisionRation % 256);
	frequencyDivisionRation /= 256;
	outp(0x42, frequencyDivisionRation);
}

void play_sound(void) {
    int i;
    for(i = 0; octaveNoteFrequency[i] != 0 || clock[i] != 0; i++) {

      setFrequency(octaveNoteFrequency[i]);
      delay(pause[i]);
      outp(0x61, inp(0x61) | 3);
      delay(clock[i]);

      outp(0x61, inp(0x61) & 0xfc); //11111100
    }
}

void printDivisionRatio(void) {
    int ports[] = {0x40, 0x41, 0x42};
    int controlWords[] = {0, 64, 128};
    unsigned lowDivisionRatio, highDivisionRatio, maxDivisionRatio, temp;
    int channelNum, count;
    for(channelNum = 0; channelNum < 3; channelNum++) {
      maxDivisionRatio = 0;
      temp = 0;
      if (channelNum == 2) {
        outp(0x61, inp(0x61) | 3);
      }
      for (count = 0; count  < nTimes; count++) {

        outp(0x43, controlWords[channelNum]); // select a channel
        lowDivisionRatio = inp(ports[channelNum]);
        highDivisionRatio = inp(ports[channelNum]);

        temp = highDivisionRatio * 256 + lowDivisionRatio;

        if(temp > maxDivisionRatio) {
          maxDivisionRatio = temp;
        }
        }
        if(channelNum == 2) {
          outp(0x61, inp(0x61) & 0xfc);
        }
        printf("Channel %d - %4X\n", channelNum, maxDivisionRatio);
    }
}

void printStatusWords(void) {
  unsigned char state, stateWord[8];
  int ports[] = {0x40, 0x41, 0x42};
  //11100010
  int controlWords[] = {226, 228, 232};
  int i, channelNum;
  for(channelNum = 0; channelNum < 3; channelNum++) {
    outp(0x43, controlWords[channelNum]);
    state = inp(ports[channelNum]);
    for (i = 7; i >= 0; i--) {
      stateWord[i] = state%2 + '0';
      state /= 2;
    }
    stateWord[8] = '\0';
    printf("Channel %d: %s\n", channelNum, stateWord);
    }

}


void randomSet(int maxValue, int parity);
int randomGet(void);

void generateRandom(void) {
  int userInput, parity;
  printf("Enter the maximum value of the number\n");
  scanf("%d", &userInput);

  printf("1. odd number\t2. even number\n");
  scanf("%d", &parity);

  randomSet(userInput, parity);

  printf("Press any key to get result..\n");
  getch();

  printf("Number: %u\n", randomGet());
}

void randomSet(int maxValue, int parity) {
  switch(parity) {
    case ODD_NUMBER:
        outp(0x43, 0xb4);
        break;
    case EVEN_NUMBER:
        outp(0x43, 0xb6);
        break;
    default:
        outp(0x43, 0xb4);
        break;
  }

  outp(0x42, maxValue & 0x00ff);
  outp(0x42, (maxValue & 0xff00) >> 8);

  outp(0x61, inp(0x61) | 1);
}


int randomGet(void) {
  int number;

  outp(0x43, 0x128);

  number = inp(0x42);
  number = (inp(0x42) << 8) + number;

  outp(0x61, inp(0x61) & 0xfc);

  return number;
}
