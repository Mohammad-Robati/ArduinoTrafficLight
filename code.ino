#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Servo.h> 

LiquidCrystal lcd(53, 51, 49, 47, 45, 43);

// variables
int GREEN1 = 2;
int YELLOW1 = 3;
int RED1 = 4;
int GREEN2 = 5;
int YELLOW2 = 6;
int RED2 = 7;

int DELAY_YELLOW = 2000;
int DELAY_RED1 = 10000;
int DELAY_RED2 = 10000;

// 0 -> 1 is green, 2 is red
// 1 -> 1 is yellow, 2 is red
// 2 -> 1 is red, 2 is green
// 3 -> 1 is red, 2 is yellow
int state = 0;
unsigned long last_change;
unsigned long lcd_change;

int time_ratio = 1;

int humidity_pin = 13; // humidity

const byte ROWS = 4; // Four rows
const byte COLS = 3; // Three columns
// Define the Keymap
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

// Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte rowPins[ROWS] = { 39, 37, 35, 33 };
// Connect keypad COL0, COL1 and COL2 to these Arduino pins.
byte colPins[COLS] = { 27, 29, 31 };

// Create the Keypad
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// 0 -> remaining time
// 1 -> change state
// 2 -> increase or decrease DELAY_RED1
// 3 -> increase or decrease DELAY_RED2
// 4 -> reset timings
int menu = 0;
int change_state = 0;

int servoPin = 8; 
Servo Servo1;

// basic functions
void setup()
{
  Servo1.attach(servoPin); 
  
  Serial.begin(9600);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  
  pinMode(GREEN1, OUTPUT);
  pinMode(YELLOW1, OUTPUT);
  pinMode(RED1, OUTPUT);
  pinMode(GREEN2, OUTPUT);
  pinMode(YELLOW2, OUTPUT);
  pinMode(RED2, OUTPUT);

  pinMode(humidity_pin, INPUT);

  lcd_change = last_change = millis();
}

void loop()
{
  time_ratio = handle_humidity() + 1;
  
  unsigned long interval = millis() - last_change;

  int remaining_time;
  switch(state) {
    case 0: remaining_time = DELAY_RED2 / time_ratio - interval; break;
    case 1: remaining_time = DELAY_YELLOW / time_ratio - interval; break;
    case 2: remaining_time = DELAY_RED1 / time_ratio - interval; break;
    case 3: remaining_time = DELAY_YELLOW / time_ratio - interval; break;
  }

  if(millis() - lcd_change > 100) {
    lcd.clear();
    switch(menu) {
      case 0: lcd.print("REM: "); lcd.print(remaining_time/1000 + 1); break;
      case 1: lcd.print("CHANGE"); break;
      case 2: lcd.print("RED1: "); lcd.print(DELAY_RED1/1000); break;
      case 3: lcd.print("RED2: "); lcd.print(DELAY_RED2/1000); break;
      case 4: lcd.print("RESET"); break;
    }
    lcd_change = millis();
  }
  
  char key = kpd.getKey();
  if (key) {
    Serial.print(key);
    switch (key) {
      case '2': increase(); break;
      case '8': decrease(); break;
      case '6': left(); break;
      case '4': right();  break;
      case '5': ok(); break;
    }
  }
  
  switch(state) {
    case 0:
      green_light1();
      red_light2();
      if(remaining_time <= 0 || change_state == 1) {
        change_state = 0;
        state = 1;
        last_change = millis();
      }
      break;
    case 1:
      yellow_light1();
      red_light2();
      if(remaining_time <= 0) {
        change_state = 0;
        state = 2;
        last_change = millis(); 
        Servo1.write(45);   
      }
      break;
    case 2:
      red_light1();
      green_light2();
      if(remaining_time <= 0 || change_state == 1) {
        change_state = 0;
        state = 3;  
        last_change = millis(); 
      }
      break;
    case 3:
      red_light1();
      yellow_light2();
      if(remaining_time <= 0) {
        change_state = 0;
        state = 0;
        last_change = millis();  
        Servo1.write(135);  
      }
      break; 
  }
}

int handle_humidity()
{
  int valb = digitalRead(humidity_pin); // humidity calculation
  //Serial.print(valb);
  return valb;
}

void increase() {
  if(menu == 2)
    DELAY_RED1 += 1000;
  if(menu == 3)
    DELAY_RED2 += 1000;
}

void decrease() {
  if(menu == 2 && DELAY_RED1 > 1000)
    DELAY_RED1 -= 1000;
  if(menu == 3 && DELAY_RED2 > 1000) {
    DELAY_RED2 -= 1000;
  }
}

void left() {
  menu--;
  if(menu == -1)
    menu = 4;
}

void right() {
  menu = (menu + 1) % 5;
}

void ok() {
  if(menu == 1)
    change_state = 1;
  if(menu == 4) {
    DELAY_RED1 = 5000;
    DELAY_RED2 = 5000;
  }
}

void green_light1()
{
  digitalWrite(GREEN1, HIGH);
  digitalWrite(YELLOW1, LOW);
  digitalWrite(RED1, LOW);
}

void yellow_light1()
{
  digitalWrite(GREEN1, LOW);
  digitalWrite(YELLOW1, HIGH);
  digitalWrite(RED1, LOW);
}

void red_light1()
{
  digitalWrite(GREEN1, LOW);
  digitalWrite(YELLOW1, LOW);
  digitalWrite(RED1, HIGH);
}

void green_light2()
{
  digitalWrite(GREEN2, HIGH);
  digitalWrite(YELLOW2, LOW);
  digitalWrite(RED2, LOW);
}

void yellow_light2()
{
  digitalWrite(GREEN2, LOW);
  digitalWrite(YELLOW2, HIGH);
  digitalWrite(RED2, LOW);
}

void red_light2()
{
  digitalWrite(GREEN2, LOW);
  digitalWrite(YELLOW2, LOW);
  digitalWrite(RED2, HIGH);
}
