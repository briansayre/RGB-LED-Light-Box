#include "IRremote.h"
#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

#define REDPIN 8
#define GREENPIN 9
#define BLUEPIN 10
#define RECEIVER 12

/*-----( Declare objects )-----*/
IRrecv irrecv(RECEIVER);     // create instance of 'irrecv'
decode_results results;      // create instance of 'decode_results'

int red = 0;
int blue = 0;
int green = 0;
int state = 999;
int prevState = 999;

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
  irrecv.blink13(false);
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
  delay(3000); // wait for console opening
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}


void loop() {
  // continuously checks the state of the machine
  checkState();
}


// Gets the IR code and changes the state if there is a signal
void checkRemote() {
  if (irrecv.decode(&results)) {
    switch (results.value) {
      case 0xFFA25D: Serial.println("POWER"); state = 999; break;
      case 0xFFE21D: Serial.println("FUNC/STOP"); state = 690; break;
      case 0xFF629D: Serial.println("VOL+"); state = 0; break;
      case 0xFF22DD: Serial.println("FAST BACK"); state = 10; break;
      case 0xFF02FD: Serial.println("PAUSE"); state = 20; break;
      case 0xFFC23D: Serial.println("FAST FORWARD"); state = 30;  break;
      case 0xFFE01F: Serial.println("DOWN"); state = 0;  break;
      case 0xFFA857: Serial.println("VOL-"); state = 0;  break;
      case 0xFF906F: Serial.println("UP"); state = 0;  break;
      case 0xFF9867: Serial.println("EQ"); state = 0;  break;
      case 0xFFB04F: Serial.println("ST/REPT"); state = 0; break;
      case 0xFF6897: Serial.println("0"); state = 0;  break;
      case 0xFF30CF: Serial.println("1"); state = 1; break;
      case 0xFF18E7: Serial.println("2"); state = 2; break;
      case 0xFF7A85: Serial.println("3"); state = 3;  break;
      case 0xFF10EF: Serial.println("4"); state = 4; break;
      case 0xFF38C7: Serial.println("5"); state = 5; break;
      case 0xFF5AA5: Serial.println("6"); state = 6; break;
      case 0xFF42BD: Serial.println("7"); state = 7; break;
      case 0xFF4AB5: Serial.println("8"); state = 8; break;
      case 0xFF52AD: Serial.println("9"); state = 9; break;
      case 0xFFFFFFFF: Serial.println(" REPEAT"); break;
      default: Serial.println(" other button   ");
    }
    delay(500); // Do not get immediate repeat
    irrecv.resume(); // receive the next value
  }
}


// Checks and sets the current states
void checkState() {
  prevState = state;
  checkRemote();
  Serial.println(state);
  if (state == 999) {
    setLights(0, 0, 0);
  }
  if (state != prevState) {
    switch (state) {
      case 0:
        //white();
        break;
      case 1:
        setLights(255, 0, 0);
        break;
      case 2:
        setLights(0, 255, 0);
        break;
      case 3:
        setLights(0, 0, 255);
        break;
      case 4:
        setLights(210, 55, 0);
        break;
      case 5:
        setLights(0, 255, 40);
        break;
      case 6:
        setLights(0, 50, 180);
        break;
      case 7:
        setLights(220, 110, 20);
        break;
      case 8:
        setLights(0, 255, 230);
        break;
      case 9:
        setLights(150, 0, 170);
        break;
      case 10:
        redFade();
        break;
      case 20:
        greenFade();
        break;
      case 30:
        blueFade();
        break;
      case 690:
        dayCycle();
        break;
      case 999:
        setLights(0, 0, 0);
        break;
    }
  }
}
/*
   COLORS:

   0 - WHITE - 255, 255, 255
   
   1 - RED - 255, 0, 0
   4 - ORANGE - 210, 55, 0
   7 - YELLOW - 220, 110, 20

   2 - GREEN - 0, 255, 0
   5 - LIGHT GREEN - 0, 255, 40
   8 - TEAL - 0, 255, 230

   3 - BLUE - 0, 0, 255
   6 - LIGHT BLUE - 0, 50, 180
   9 - PURPLE - 150, 0, 170
*/


// Sets the lights using the parameters and uses a delay to slowly or quickly set the lights
void setLights(int r, int g, int b, int d) {
  if (d == 0) {
    red = r;
    green = g;
    blue = b;
    analogWrite(REDPIN, red);
    analogWrite(GREENPIN, green);
    analogWrite(BLUEPIN, blue);
  }
  while (r != red || g != green || b != blue) {
    checkState();
    if (state == 999) {
      red = r;
      green = g;
      blue = b;
    }
    if (r > red) {
      red++;
    } else if (r < red) {
      red--;
    }
    if (g > green) {
      green++;
    } else if (g < green) {
      green--;
    }
    if (b > blue) { 
      blue++;
    } else if (b < blue) {
      blue--;
    }
    analogWrite(REDPIN, red);
    analogWrite(GREENPIN, green);
    analogWrite(BLUEPIN, blue);
    Serial.print("Red: ");
    Serial.println(red);
    Serial.print("Green: ");
    Serial.println(green);
    Serial.print("Blue: ");
    Serial.println(blue);
    Serial.println("");
    delay(d);
  }

}

// Calls set lights with a zero delay
void setLights(int r, int g, int b) {
  setLights(r, g, b, 0);
}

void white() {
  while (state == 0) {
    setLights(200, 0, 0);
    setLights(0, 200, 0);
    setLights(0, 0, 200);
  }
}


// Models the daylights to act as a foe window
void dayCycle() {
  while (state == 690) {
    DateTime now = rtc.now();
    int hour = now.hour();
    int min = now.minute();
    if (hour == 7 && min == 0) {
      setLights(210, 55, 0, 1500);
    } else if (hour == 8 && min == 0) {
      setLights(220, 110, 20, 1500);
    } else if (hour == 12 && min == 0) {
      setLights(0, 50, 180, 1500);
    } else if (hour == 18 && min == 0) {
      setLights(0, 255, 230, 1500);
    } else if (hour == 19 && min == 0) {
      setLights(210, 55, 0, 1500);
    } else if (hour == 20 && min == 0) {
      setLights(255, 0, 0, 1500);
    } else if (hour == 21 && min == 0) { //21
      setLights(150, 0, 170, 1500);
    } else if (hour == 0 && min == 0) {
      setLights(0, 0, 0, 1500);
    }
  }
}


// Fades from red to orange nonstop
void redFade() {
  while (state == 10) {
    setLights(255, 0, 0, 20);
    setLights(210, 55, 0, 20);
  }
}


// Fades from green to teal nonstop
void greenFade() {
  while (state == 20) {
    setLights(0, 30, 200, 20);
    setLights(0, 200, 30, 20);
  }
}


// Fades from blue to orange purple
void blueFade() {
  while (state == 30) {
    setLights(150, 0, 170, 20);
    setLights(200, 0, 80, 20);
  }
}

