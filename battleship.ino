//Battleship Game v1.0
//By Thomas Seabrook
//OLED Display and Rotary Encoder setup code taken from classwork
#include <Interrupt.h>
#include <TimerListener.h>
#include <RifIntEncoder.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <InvertedTM1638.h>
#include <TM1638.h>
#include <TM1638QYF.h>
#include <TM1640.h>
#include <TM16XX.h>

//I would use enums/variables to make code readability easier, but I ran out of space
//Global Variables/Classes
Adafruit_SSD1306 display(9, 10, 12);
TM1638 module(7, 6, 5);
RifIntEncoder knob;
int8_t player1ShipsRemaining = 5;
int8_t player2ShipsRemaining = 5;
bool gameComplete = false;

int8_t shipPos[2][5][2][5] = {{{{0,0,0,0,0}, {0,0,0,0,0}},
  {{0,0,0,0,NULL}, {0,0,0,0,NULL}},
  {{0,0,0,NULL, NULL}, {0,0,0,NULL, NULL}},
  {{0,0,0,NULL, NULL}, {0,0,0,NULL, NULL}},
  {{0,0,NULL, NULL, NULL}, {0,0,NULL, NULL, NULL}}},
  
  {{{0,0,0,0,0}, {0,0,0,0,0}},
  {{0,0,0,0,0}, {0,0,0,0,0}},
  {{0,0,0,0,0}, {0,0,0,0,0}},
  {{0,0,0,0,0}, {0,0,0,0,0}},
  {{0,0,0,0,0}, {0,0,0,0,0}}}};
//First is player, second is ship, third is x or y coords and 4th is the coords themselves

int8_t gameBoards[2][10][10] = {{{0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0}
},
{{0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0}
}};
/* 0-127 is an empty tile
 * 128-255 is a ship 
 * Odd has been shot, even has not
 */
  
byte buttons;
bool go = true;

//function prototypes
void menu();
void play();
void controls();
void quit();
void controlsPageTwo();
void controlsPageThree();
void controlsPageFour();
void printOne();
void printTwo();
void printThree();
void printFour();
void drawBoard(int8_t, int8_t);
void drawSquare(int8_t, int8_t);
void placeShip(int8_t, int8_t);
int8_t displayToSevenSeg(int8_t);
void displayToSevenSeg(int8_t, int8_t);
void getPos(int8_t *p1, int8_t *p2);
// ------------------------------------------------------------------------------------
void setup()
{
  //Dunno what this does but it's important
  display.begin(SSD1306_SWITCHCAPVCC);
  display.display();
  delay(1000);
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC);
  display.display();
  delay(1000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  Interrupt::initialiseStaticISRVectorTable() ;
  // -- encoders ------------------------------------------------
  knob.init(3, 4);
  knob.setMax(180);
  knob.setMin(0);
}

void loop()
{
  menu();
}

void menu()
{
  int choice = 0;

  while (go)
  {
    delay(500);
    int knobPos = knob.getPos();

      if (knobPos % 3 == 0)
      {
        choice = 0;
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Battleship Game v1.0");
        display.println("[Play]");
        display.println(" Controls ");
        display.println(" Exit ");
        display.display();

      }
      else if (knobPos % 2 == 0)
      {
        choice = 1;
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Battleship Game v1.0");
        display.println(" Play ");
        display.println("[Controls]");
        display.println(" Exit ");
        display.display();

      }
      else
      {
        choice = 2;
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Battleship Game v1.0");
        display.println(" Play ");
        display.println(" Controls ");
        display.println("[Exit]");
        display.display();

      }
    
    buttons = module.getButtons();

    if (buttons == 128 && choice == 0)
    {
      play();
    }
    else if (buttons == 128 && choice == 1)
    {
      controls();
    }
    else if (buttons == 128 && choice == 2)
    { 
      quit();
    }
  }
}

void play()
{
  bool play = true;
  while (play)
  {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("       Player 1");
    display.println("Press S8 to begin placing your Aircraft Carrier.");
    display.display();
    delay(1000);
    while(true)
    {
      buttons = module.getButtons();
      if(buttons == 128)
      {
        play = false;
        display.clearDisplay();
        drawBoard(0,0);
        delay(1000);
        placeShip(0, 0);
      }
    }
  }
}

void drawBoard(int8_t board, int8_t player)
{
  int8_t k = 0;
  int8_t l = 0;
  if(board == player)
  {
    for(int8_t i = 1; i < 56; i += 6)
    {
      for(int8_t j = 1; j < 29; j += 3)
      {
        if(gameBoards[board][k][l] < 128)
        {
          drawSquare(i, j);
          k++;
          l++;
        }
      }
    }
  }
  else
  {
    for(int8_t i = 1; i < 56; i += 6)
    {
      for(int8_t j = 1; j < 29; j += 3)
      {
        if(gameBoards[board][k][l] < 128 || gameBoards[board][k][l] % 2 == 0)
        {
          drawSquare(i, j);
          k++;
          l++;
        }
      }
    }
  }
}

void drawSquare(int8_t x, int8_t y)
{
  int8_t xOffset = 30;
    display.drawPixel(x + xOffset, y, WHITE);
    display.drawPixel(x + xOffset, y + 1, WHITE);
    display.drawPixel(x + 2 + xOffset, y, WHITE);
    display.drawPixel(x + 2 + xOffset, y + 1, WHITE);
    display.display();
}

void placeShip(int8_t player, int8_t ship)
{
  int8_t len = 0;
  switch(ship)
  {
    case 0:
      len = 5;
      break;
    case 1:
      len = 4;
      break;
    case 2:
      len = 3;
      break;
    case 3:
      len = 3;
      break;
    case 4:
      len = 2;
      break;
    default:
      break;
  }
  int8_t startPos[2] = {0,0};
  int8_t endPos[2] = {0,0};
  int8_t *p1 = &startPos[0];
  int8_t *p2 = &startPos[1];
  int8_t *p3 = &endPos[0];
  int8_t *p4 = &endPos[1];
  bool cont;
  do
  {
    cont = true;
    getPos(p1, p2);
    getPos(p3, p4);
    if(((startPos[0] != endPos[0]) || (startPos[1] != endPos[1])) || ((startPos[0] - endPos[0] != len) || (startPos[1] - endPos[1] != len) || (endPos[0] - startPos[0] != len) || (endPos[1] - startPos[1] != len)))
    {
      cont = false;
    }
    if(cont)
    {
      display.clearDisplay();
      display.setCursor(0,0);
      display.println("Invalid input");
      delay(1000);
      drawBoard(player, player);
    }
  } while (cont);
  
}

void getPos(int8_t *p1, int8_t *p2)
{
    int8_t desiredPos[2] = {0,0};
  int8_t buttonPos[5] = {1, 2, 3, 4, 5};
  bool confirm = false;
  while(desiredPos[1] == 0 && !confirm)
  {
    confirm = false;
    buttons = module.getButtons();
    switch(buttons)
    {
      case 1:
        if(desiredPos[0] == 0)
        {
          desiredPos[0] = buttonPos[0];
          displayToSevenSeg(desiredPos[0]);
        }
        else
        {
          desiredPos[1] = buttonPos[0];
          displayToSevenSeg(desiredPos[0], desiredPos[1]);
        }
        delay(500);
        break;
       case 2:
        if(desiredPos[0] == 0)
        {
          desiredPos[0] = buttonPos[1];
          displayToSevenSeg(desiredPos[0]);
        }
        else
        {
          desiredPos[1] = buttonPos[1];
          displayToSevenSeg(desiredPos[0], desiredPos[1]);
        }
        delay(500);
        break;
      case 4:
        if(desiredPos[0] == 0)
        {
          desiredPos[0] = buttonPos[2];
          displayToSevenSeg(desiredPos[0]);
        }
        else
        {
          desiredPos[1] = buttonPos[2];
          displayToSevenSeg(desiredPos[0], desiredPos[1]);
        }
        delay(500);
        break;
      case 8:
        if(desiredPos[0] == 0)
        {
          desiredPos[0] = buttonPos[3];
          displayToSevenSeg(desiredPos[0]);
        }
        else
        {
          desiredPos[1] = buttonPos[3];
          displayToSevenSeg(desiredPos[0], desiredPos[1]);
        }
        delay(500);
        break;
      case 16:
        if(desiredPos[0] == 0)
        {
          desiredPos[0] = buttonPos[4];
          displayToSevenSeg(desiredPos[0]);
        }
        else
        {
          desiredPos[1] = buttonPos[4];
          displayToSevenSeg(desiredPos[0], desiredPos[1]);
        }
        delay(500);
        break;
      case 32:
        if(buttonPos[0] == 1)
        {
          for(int8_t i = 0; i < 5; i++)
          {
            buttonPos[i] += 5;
          }
        }
        else
        {
          for(int8_t i = 0; i < 5; i++)
          {
            buttonPos[i] -= 5;
          }
        }
        delay(500);
        break;
      case 64:
        if(buttonPos[0] == 1)
        {
          for(int8_t i = 0; i < 5; i++)
          {
            buttonPos[i] += 5;
          }
        }
        else
        {
          for(int8_t i = 0; i < 5; i++)
          {
            buttonPos[i] -= 5;
          }
        }
        delay(500);
        break;
      case 128:
        confirm = true;
        delay(500);
        break;
      default:
        break;
    }
    Serial.println(confirm);
  }
  int8_t temp[8] = {0,0,0,0,0,0,0,0};
 //   module.setDisplay(temp);
    *p1 = desiredPos[0];
    *p2 = desiredPos[1];
}

void controls()
{
  printOne();
  bool stay = true;
  while (stay)
  {
    printOne();
    buttons = module.getButtons();
    if (buttons == 64)
    {
      controlsPageTwo();
    }
    else if (buttons == 32)
    {
      delay(1000);
      stay = false;
    }
  }
}

int8_t displayToSevenSeg(int8_t num)
{
  byte sevenSegData[8] = {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000};
  switch(num)
  {
    case 1: //A
      sevenSegData[0] = 0b01110111;
      module.setDisplay(sevenSegData, 8);
      Serial.println("a");
      break;
    case 2://b
      sevenSegData[0] = 0b01111100;
      module.setDisplay(sevenSegData, 8);
      Serial.println("b");
      break;
    case 3://C
      sevenSegData[0] = 0b00111001;
      module.setDisplay(sevenSegData, 8);
      Serial.println("c");
      break;
    case 4://d
      sevenSegData[0] = 0b01011110;
      module.setDisplay(sevenSegData, 8);
      Serial.println("d");
      break;
   case 5://E
      sevenSegData[0] = 0b01111001;
      module.setDisplay(sevenSegData, 8);
      Serial.println("e");
      break;
   case 6://F
      sevenSegData[0] = 0b01110001;
      module.setDisplay(sevenSegData, 8);
      Serial.println("f");
      break;
   case 7://g
      sevenSegData[0] = 0b01101111;
      module.setDisplay(sevenSegData, 8);
      Serial.println("g");
      break;
   case 8://H
      sevenSegData[0] = 0b01110110;
      module.setDisplay(sevenSegData, 8);
      Serial.println("h");
      break;
   case 9://I
      sevenSegData[0] = 0b00000110;
      module.setDisplay(sevenSegData, 8);
      Serial.println("i");
      break;
   case 10://J
      sevenSegData[0] = 0b00001110;
      module.setDisplay(sevenSegData, 8);
      Serial.println("j");
      break;
   default:
      break;
  }
  return sevenSegData[0];
}

void displayToSevenSeg(int8_t num1, int8_t num2)
{
  byte sevenSegData[8] = {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000};
  sevenSegData[0] = displayToSevenSeg(num1);
  switch(num2)
  {
    case 1: //1
      sevenSegData[1] = 0b00110000;
      module.setDisplay(sevenSegData, 8);
      Serial.println("1");
      break;
    case 2://2
      sevenSegData[1] = 0b01011011;
      module.setDisplay(sevenSegData, 8);
      Serial.println("2");
      break;
    case 3://3
      sevenSegData[1] = 0b01001111;
      module.setDisplay(sevenSegData, 8);
      Serial.println("3");
      break;
    case 4://4
      sevenSegData[4] = 0b01100110;
      module.setDisplay(sevenSegData, 8);
      Serial.println("4");
      break;
   case 5://5
      sevenSegData[0] = 0b01101101;
      module.setDisplay(sevenSegData, 8);
      Serial.println("5");
      break;
   case 6://6
      sevenSegData[0] = 0b01111101;
      module.setDisplay(sevenSegData, 8);
      Serial.println("6");
      break;
   case 7://7
      sevenSegData[0] = 0b00000111;
      module.setDisplay(sevenSegData, 8);
      Serial.println("7");
      break;
   case 8://8
      sevenSegData[0] = 0b01111111;
      module.setDisplay(sevenSegData, 8);
      Serial.println("8");
      break;
   case 9://9
      sevenSegData[0] = 0b01101111;
      module.setDisplay(sevenSegData, 8);
      Serial.println("9");
      break;
   case 10://0
      sevenSegData[0] = 0b00111111;
      module.setDisplay(sevenSegData, 8);
      Serial.println("10");
      break;
   default:
      break;
  }
}

/*
 * Controls Functions
 * All the next functions pertain to the controls subroutine
 * They all follow the same basic structure so I'll just comment it here
 * Displays the text to screen, then waits for a button press
 * Left for previous, right for next
 */
void printOne()
{
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("       Controls");
  display.println("Use buttons S6 and S7 to navigate the menu");
  display.println("<-- Exit     Next -->");
  display.display();
}

void controlsPageTwo()
{
  printTwo();
  delay(500);
  while (true)
  {
    printTwo();
    buttons = module.getButtons();
    if (buttons == 64)
    {
      controlsPageThree();
    }
    else if (buttons == 32)
    {
      delay(500);
      return;
    }
  }
}

void printTwo()
{
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Use button S8 to confirm");
  display.println("\n<-- Prev     Next -->");
  display.display();
}

void controlsPageThree()
{
  printThree();
  delay(500);
  while (true)
  {
    printThree();
    buttons = module.getButtons();
    if (buttons == 32)
    {
      delay(500);
      return;
    }
    else if (buttons == 64)
    {
      delay(500);
      controlsPageFour();
    }
  }
}

void printThree()
{
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Use buttons S1 - S5 to select first a row, then a column.");
  display.println("<-- Prev     Next -->");
  display.display();
}

void controlsPageFour()
{
  printFour();
  while (true)
  {
    printFour();
    buttons = module.getButtons();
    if (buttons == 32)
    {
      delay(500);
      return;
    }
  }
}
void printFour()
{
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("S6 & 7 change from 1-5 to 6-10");
  display.println("\n<-- Prev");
  display.display();
}
//End Control Functions

void quit()
{
    display.clearDisplay();
    go = false;
    display.display();
 // Serial.println("I'm the ass hole function");
}
