#include "Keypad.h"
#include "LiquidCrystal_I2C.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

//Initialize keyboard
const byte ROWS = 2; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'}
};

// Initialize screen
byte rowPins[ROWS] = {9, 8}; 
byte colPins[COLS] = {7, 6, 5, 4}; 

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
LiquidCrystal_I2C lcd(0x3F, 16, 2); 

// Declaration of variables
char keyInput;
const int len_password = 4;
char master_password[len_password] = {'1','2','3','4'};
char attempt_password[len_password];
int cursor_pass = 0;
bool unlocked = false;
bool initialized = false;
bool exitLoop = false;
int screenID = 0;
String screens [3][2] = {{"Choose an option","Hum 1, Temp 2"}, {"Humidity", "X %"}, {"Temperature","X Celsius"}};
int i = 0;

// Declaration of functions
void menu(){
  lcd.setCursor(0,0);
  lcd.print(screens[screenID][0]);
  lcd.setCursor(0,1);
  lcd.print(screens[screenID][1]);
  
  keyInput = keypad.getKey();
  //keyPress(keyInput);

    switch (keyInput) {
      case '1':
        lcd.clear();
        screenID = 1;
        break;
      case '2':
        lcd.clear();
        screenID = 2;
        break;
      case 'B':
        lcd.clear();
        screenID = 0;
      default:
        break;
    }
}

void keyPress(char key){
  
  if (key){
        //Serial.println(key);
        //lcd.clear();
        lcd.setCursor(0,1);
        lcd.print(key);
        delay(100);
    
  }
}

void checkPASS()
{
   int correct=0;
   int i;
   for (i=0; i<len_password; i++) {
    if (attempt_password[i]==master_password[i]) {
      correct++;
      }
    }
   if (correct==len_password && cursor_pass==len_password){
    lcd.setCursor(0,1);
    lcd.print("Correct Key");
    delay(3000);
    unlocked = true;
    cursor_pass=0;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Insert Password");
   }
   else
   {
    lcd.setCursor(0,1);
    lcd.print("Incorrect Key");
    delay(3000);
    cursor_pass=0;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Insert Password");
   }
   for (int zz=0; zz<len_password; zz++) {
    attempt_password[zz]=0;
   }

}

// Main script

void setup() {
   Serial.begin(115200);
   lcd.begin(16,2);
   lcd.init();
   lcd.backlight();
   lcd.setCursor(0,0);
   lcd.print("Initializing setup");
   delay(500);
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("Enter password:");
}

void loop() {
  

  while(unlocked == false){
    keyInput = keypad.getKey();
    lcd.setCursor(cursor_pass-1,1);
    lcd.print("*");
    if (keyInput){
      switch(keyInput){
        case 'A':
          cursor_pass=0;
          lcd.setCursor(0,1);
          lcd.print("                ");
          break;
        case 'B':
          delay(100); // added debounce
          checkPASS();
          break;
        default:
          attempt_password[cursor_pass]=keyInput;
          cursor_pass++;
        }
    }

  };
  
  //lcd.clear();
  
  if(initialized ==false){ // entry message
    lcd.setCursor(0,0);
    lcd.print("Device unlocked");
    delay(1000);
    initialized = true;
    lcd.clear();
  }

  menu();


  // keyInput = keypad.getKey();
  // keyPress(keyInput);


}
