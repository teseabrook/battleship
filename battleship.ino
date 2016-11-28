//Battleship Game v1.0
//By Thomas Seabrook
//OLED Display and Rotary Encoder setup code taken from classwork
#include <Interrupt.h>
#include <TimerListener.h>
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
int8_t player1ShipsRemaining = 5;
int8_t player2ShipsRemaining = 5;
bool gameComplete = false;

int8_t shipPos[2][5][2][5] = {{{{0,0,0,0,0}, {0,0,0,0,0}},
  {{0,0,0,0,NULL}, {0,0,0,0,NULL}},
  {{0,0,0,NULL, NULL}, {0,0,0,NULL, NULL}},
  {{0,0,0,NULL, NULL}, {0,0,0,NULL, NULL}},
  {{0,0,NULL, NULL, NULL}, {0,0,NULL, NULL, NULL}}},
  
  {{{0,0,0,0,0}, {0,0,0,0,0}},
  {{0,0,0,0,NULL}, {0,0,0,0,NULL}},
  {{0,0,0,NULL, NULL}, {0,0,0,NULL, NULL}},
  {{0,0,0,NULL, NULL}, {0,0,0,NULL, NULL}},
  {{0,0,NULL, NULL, NULL}, {0,0,NULL, NULL, NULL}}}};
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
void loadInToArray(int8_t[2], int8_t[2], int8_t, int8_t, int8_t);
void fire(int8_t);
void getHit(int8_t[2], int8_t);
void complete();
void hasWon(int8_t);
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
}

void loop()
{
  menu();
}

void menu()
{
  int choice = 0;
  buttons = 0;
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Battleship Game v1.0");
  display.println("[Play]");
  display.println(" Controls ");
  display.println(" Exit ");
  display.display();
  while (true)
  {
    do
    {
      buttons = module.getButtons();
    } while (buttons == 0);
    if (buttons == 1)
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
    else if (buttons == 2)
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
    else if (buttons == 4)
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
    else if (buttons == 128 && choice == 0)
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
    bool placed = false;
    while(!placed)
    {
      do
      {
        buttons = module.getButtons();
      } while (buttons == 0);
      if(buttons == 128)
      {
        play = false;
        display.clearDisplay();
        drawBoard(0,0);
        delay(1000);
        placeShip(0, 0);
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("       Player 1");
        display.println("Place your cruiser");
        delay(1000);
        drawBoard(0,0);
        placeShip(0, 1);
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("       Player 1");
        display.println("Place your Submarine");
        delay(1000);
        drawBoard(0,0);
        placeShip(0, 2);
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("       Player 1");
        display.println("Place your Frigate");
        delay(1000);
        drawBoard(0,0);
        placeShip(0, 3);
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("       Player 1");
        display.println("Place your Destroyer");
        delay(1000);
        drawBoard(0,0);
        placeShip(0, 4);
      }
    }
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("       Player 2");
    display.println("Press S8 to begin placing your Aircraft Carrier.");
    display.display();
    delay(1000);
    do
      {
        buttons = module.getButtons();
      } while (buttons == 0);
      if(buttons == 128)
      {
        play = false;
        display.clearDisplay();
        drawBoard(1,1);
        delay(1000);
        placeShip(1, 0);
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("       Player 1");
        display.println("Place your cruiser");
        delay(1000);
        drawBoard(1,1);
        placeShip(1, 1);
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("       Player 1");
        display.println("Place your Submarine");
        delay(1000);
        drawBoard(1,1);
        placeShip(1, 2);
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("       Player 1");
        display.println("Place your Frigate");
        delay(1000);
        drawBoard(1,1);
        placeShip(1, 3);
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("       Player 1");
        display.println("Place your Destroyer");
        delay(1000);
        drawBoard(1,1);
        placeShip(1, 4);
        placed = true;
      }
  }
}

void fire(int8_t player)
{
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Player ");
  display.print(player);
  display.println("'s turn");
  switch(player)
  {
    case 0:
      drawBoard(1, 0);
      break;
    case 1:
      drawBoard(0, 1);
      break;
  }
  int8_t desiredPos[2] = {0, 0};
  do
  {
    getPos(&desiredPos[0], &desiredPos[1]);
  } while (desiredPos[0] > 127 || desiredPos[1] > 127);
  gameBoards[player][desiredPos[0]][desiredPos[1]] += 128;
  getHit(desiredPos, player);
}

void getHit(int8_t pos[2], int8_t player)
{
  if(player == 1)
  {
    player = 0;
  }
  else
  {
    player = 1;
  }
  for(int j = 0; j < 5; j++)
  {
    for(int i = 0; i < 5; i++)
    {
       if(pos[0] == shipPos[player][j][0][i] && pos[1] == shipPos[player][j][1][i])
       {
        shipPos[player][j][0][i] = NULL;
        shipPos[player][j][1][i] = NULL;
        hasWon();
      }
    }
  }
}

void hasWon()
{
  for(int i = 0; i < 2; i++)
  {
    int8_t playerRemaining = 5;
    for(int j = 0; j < 5; j++)
    {
      if(shipPos[i][j][0][0] == NULL && shipPos[i][j][0][1] == NULL && shipPos[i][j][0][2] == NULL && shipPos[i][j][0][3] == NULL && shipPos[i][j][0][4] == NULL)
      {
        playerRemaining--;
      }
    }
    if(playerRemaining == 0)
    {
      complete(i);
    }
  }
}

void complete(int8_t player)
{
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("Player ");
  display.print(player + 1);
  display.println(" wins!");
  gameComplete = true;
  delay(5000);
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
  int turns = 0;
  while(!gameComplete)
  {
    if(turns % 2 == 1)
    {
      fire(1);
    }
    else
    {
      fire(0);
    }
    turns++;
    //Stops things like being able to fire after the game ends
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
    Serial.println("Getting Input");
    getPos(p1, p2);
    Serial.print("Debug ");
    Serial.println(startPos[0]);
    getPos(p3, p4);
    Serial.print("Debug2 ");
    Serial.println(endPos[0]);
    if(((startPos[0] == endPos[0]) || (startPos[1] == endPos[1])) && ((startPos[0] - endPos[0] == (len-1)) || (startPos[1] - endPos[1] == (len-1)) || (endPos[0] - startPos[0] == (len-1)) || (endPos[1] - startPos[1] == (len-1))))
    {
      Serial.println("Valid");
      cont = false;
    }
    if(cont)
    {
      display.clearDisplay();
      display.setCursor(0,0);
      Serial.println("Invalid input");
      delay(1000);
      display.clearDisplay();
      display.display();
      drawBoard(player, player);
    }
  } while (cont);

  loadInToArray(startPos, endPos, player, ship, len);
  
}

void loadInToArray(int8_t startPos[2], int8_t endPos[2], int8_t player, int8_t ship, int8_t len)
{
  Serial.println("Loading");
  if(startPos[0] == endPos[0])
  {
    Serial.println("Branch 1");
    if(startPos[1] > endPos[1])
    {
      for(int8_t i = 0; i < len; i++)
      {
        Serial.println("Loading.");
        shipPos[player][ship][0][i] = startPos[0];
        shipPos[player][ship][1][i] = endPos[1] + i;
      }
    }
    else
    {
      for(int8_t i = 0; i > len; i++)
      {
        Serial.println("Loading.");
        shipPos[player][ship][0][i] = startPos[0];
        shipPos[player][ship][1][i] = startPos[1] + i;
      }
    }
  }
  else
  {
    Serial.println("Branch 2");
    if(startPos[0] > endPos[0])
    {
      for(int8_t i = 0; i < len; i++)
      {
        Serial.println("Loading.");
        shipPos[player][ship][1][i] = startPos[1];
        shipPos[player][ship][0][i] = endPos[0] + i;
      }
    }
    else
    {
      for(int8_t i = 0; i > len; i++)
      {
        Serial.println("Loading.");
        shipPos[player][ship][1][i] = startPos[1];
        shipPos[player][ship][0][i] = startPos[0] + i;
      }
    }
  }
  Serial.println(shipPos[player][ship][0][0]);
  for(int8_t i = 0; i > len; i++)
  {
    gameBoards[player][shipPos[player][ship][0][i]][shipPos[player][ship][1][i]] += 1;
  }
}

void getPos(int8_t *p1, int8_t *p2)
{
  int8_t desiredPos[2] = {0,0};
  int8_t buttonPos[5] = {1, 2, 3, 4, 5};
  bool confirm = false;
  while(desiredPos[1] == 0 && confirm == false)
  {
    confirm = false;
    delay(500);
    buttons = 0;
    do
    {
      buttons = module.getButtons();
    } while (buttons == 0);
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
  }
  
    Serial.print(desiredPos[0]);
    Serial.println(desiredPos[1]);
    Serial.println("Exit");
  int8_t temp[8] = {0,0,0,0,0,0,0,0};
   module.setDisplay(temp);
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
    buttons = 0;
    do
    {
      buttons = module.getButtons();
    } while (buttons == 0);
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
      break;
    case 2://b
      sevenSegData[0] = 0b01111100;
      module.setDisplay(sevenSegData, 8);
      break;
    case 3://C
      sevenSegData[0] = 0b00111001;
      module.setDisplay(sevenSegData, 8);
      break;
    case 4://d
      sevenSegData[0] = 0b01011110;
      module.setDisplay(sevenSegData, 8);
      break;
   case 5://E
      sevenSegData[0] = 0b01111001;
      module.setDisplay(sevenSegData, 8);
      break;
   case 6://F
      sevenSegData[0] = 0b01110001;
      module.setDisplay(sevenSegData, 8);
      break;
   case 7://g
      sevenSegData[0] = 0b01101111;
      module.setDisplay(sevenSegData, 8);
      break;
   case 8://H
      sevenSegData[0] = 0b01110110;
      module.setDisplay(sevenSegData, 8);
      break;
   case 9://I
      sevenSegData[0] = 0b00000110;
      module.setDisplay(sevenSegData, 8);
      break;
   case 10://J
      sevenSegData[0] = 0b00001110;
      module.setDisplay(sevenSegData, 8);
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
      break;
    case 2://2
      sevenSegData[1] = 0b01011011;
      module.setDisplay(sevenSegData, 8);
      break;
    case 3://3
      sevenSegData[1] = 0b01001111;
      module.setDisplay(sevenSegData, 8);
      break;
    case 4://4
      sevenSegData[4] = 0b01100110;
      module.setDisplay(sevenSegData, 8);
      break;
   case 5://5
      sevenSegData[0] = 0b01101101;
      module.setDisplay(sevenSegData, 8);
      break;
   case 6://6
      sevenSegData[0] = 0b01111101;
      module.setDisplay(sevenSegData, 8);
      break;
   case 7://7
      sevenSegData[0] = 0b00000111;
      module.setDisplay(sevenSegData, 8);
      break;
   case 8://8
      sevenSegData[0] = 0b01111111;
      module.setDisplay(sevenSegData, 8);
      break;
   case 9://9
      sevenSegData[0] = 0b01101111;
      module.setDisplay(sevenSegData, 8);
      break;
   case 10://0
      sevenSegData[0] = 0b00111111;
      module.setDisplay(sevenSegData, 8);
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
    buttons = 0;
    do
    {
      buttons = module.getButtons();
    } while (buttons == 0);
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
    buttons = 0;
    do
    {
      buttons = module.getButtons();
    } while (buttons == 0);
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
