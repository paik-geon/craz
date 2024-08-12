#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Stepper.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

const int STEPS = 2048;
Stepper stepper0(STEPS, 2, 4, 3, 5);
Stepper stepper1(STEPS, 6, 8, 7, 9);
Stepper stepper2(STEPS, 10, 12, 11, 13);
Stepper stepper3(STEPS, 23, 27, 25, 29);
Stepper stepper4(STEPS, 22, 26, 24, 28);
Stepper stepper_chain(STEPS, 46, 50, 48, 52);

int num = 0;
bool state = 0;
bool test = 0;
int head = 2;
bool under = 0;
bool sw1 = 0;
bool sw2 = 0;

byte back2[] = {
  B01000,
  B11111,
  B00000,
  B01110,
  B00010,
  B01110,
  B01000,
  B01110
};

byte back1[] = {
  B01000,
  B11111,
  B00000,
  B00100,
  B01100,
  B00100,
  B00100,
  B01110
};

byte back3[] = {
  B01000,
  B11111,
  B00000,
  B01110,
  B00010,
  B01110,
  B00010,
  B01110
};

byte back4[] = {
  B01000,
  B11111,
  B00000,
  B01010,
  B01010,
  B01110,
  B00010,
  B00010
};

byte if0[] = {
  B11111,
  B10001,
  B10101,
  B10001,
  B11111,
  B00100,
  B10101,
  B01110
};

byte if1[] = {
  B11111,
  B11011,
  B11011,
  B11011,
  B11111,
  B00100,
  B10101,
  B01110
};

byte ifblank[] = {
  B11111,
  B10001,
  B10001,
  B10001,
  B11111,
  B00100,
  B10101,
  B01110
};

byte invert_one[] = {
  B11011,
  B10011,
  B11011,
  B11011,
  B11011,
  B11011,
  B10001,
  B00000
};

byte invert_zero[] = {
  B10001,
  B01110,
  B01100,
  B01010,
  B00110,
  B01110,
  B10001,
  B00000
};

byte invert_ifblank[] = {
  B00000,
  B01110,
  B01110,
  B01110,
  B00000,
  B11011,
  B01010,
  B10001
};

byte invert_if1[] = {
  B00000,
  B01100,
  B00100,
  B01110,
  B00000,
  B11011,
  B01010,
  B10001
};

byte invert_if0[] = {
  B00000,
  B01110,
  B01010,
  B01110,
  B00000,
  B11011,
  B01010,
  B10001
};

byte invert_99[] = {
  B11101,
  B11011,
  B10111,
  B01111,
  B10111,
  B11011,
  B11101,
  B00000
};

byte invert_101[] = {
  B01111,
  B10111,
  B11011,
  B11101,
  B11011,
  B10111,
  B01111,
  B00000
};

int tape[12][5] = {0, 0, 0, 1, 0,
                   0, -2, 0, 1, 1,
                   0, 1, 0, 1, 1,
                   0, 1, 1, 1, 1
                  };

int realtape[12][5] = {0, 0, 0, 1, 0,
                       0, -2, 0, 1, 1,
                       0, 1, 0, 1, 1,
                       0, 1, 1, 1, 1
                      };

int goal[12][5] = {0, 1, 0, 1, 1,
                   0, 0, 0, 1, 1,
                   0, 0, 0, 1, 1,
                   1, 0, 0, 0, 0
                  };
int actline[12][7] = {99, 0, 101, 101, 101, 1, -1,
                      99, 201, 1, -1, -1, -1, -1,
                      201, 201, 201, 99, 302, -1
                     };
int underactline[12][7] = { -1, -1, -1, -1, -1, -1, -1,
                            -2, 0, -1, -1, -1, -1, -1,
                            0, 101, 101, 101, -1, -1, -1
                          };
int buttonnum[12][6] = {1, 5, -1, -1, -1, -1,
                        1, -1, -1, -1, -1, -1,
                        4, -1, -1, -1, -1, -1
                       };
int underbuttonnum[12][6] = { -1, -1, -1, -1, -1, -1,
                              -1, -1, -1, -1, -1, -1,
                              -1, -1, -1, -1, -1, -1
                            };
int buttonchange(int a)
{
  if (a == 0)
  {
    a = 1;
  }
  else if (a == 1)
  {
    a = 0;
  }
  else if (a == 99)
  {
    a = 101;
  }
  else if (a == 101)
  {
    a = 99;
  }
  else if (a == 199)
  {
    a++;
  }
  else if (a == 200)
  {
    a++;
  }
  else if (a == 201)
  {
    a = 199;
  }
  else if (a == 301)
  {
    a = 302;
  }
  else if (a == 302)
  {
    a = 303;
  }
  else if (a == 303)
  {
    a = 304;
  }
  else if (a == 304)
  {
    a = 301;
  }
  return a;
}

void printtape(int a[])
{
  lcd.setCursor(16, 0);
  for (int i = 0;; i++)
  {
    if (i != 5)
    {
      if (a[i] == -2)
      {
        lcd.print(" ");
        Serial.print("N");
      }
      else
      {
        lcd.print(a[i]);
        Serial.print(a[i]);
      }
    }
    else
    {
      Serial.println();
      break;
    }
  }
}

void printinvert(int a)
{
  if (a == -2)
  {
    lcd.print(" ");
  }
  else if (a == 0)
  {
    lcd.write(byte(3));
  }
  else if (a == 1)
  {
    lcd.write(byte(2));
  }
  else if (a == 99)
  {
    lcd.write(byte(7));
  }
  else if (a == 101)
  {
    lcd.write(byte(8));
  }
  else if (a == 199)
  {
    lcd.write(byte(6));
  }
  else if (a == 200)
  {
    lcd.write(byte(4));
  }
  else if (a == 201)
  {
    lcd.write(byte(5));
  }
}

void printnotinvert(int a)
{
  if (a == -1)
  {
    Serial.println();
  }
  else if (a == -2)
  {
    Serial.print("X");
    lcd.print(" ");
  }
  else if (!a)
  {
    lcd.print("0");
    Serial.print("0");
  }
  else if (a == 1)
  {
    lcd.print("1");
    Serial.print("1");
  }
  else if (a == 99)
  {
    lcd.print("<");
    Serial.print("<");
  }
  else if (a == 101)
  {
    lcd.print(">");
    Serial.print(">");
  }
  else if (a == 199)
  {
    Serial.print("A");
    lcd.write(byte(9));
  }
  else if (a == 200)
  {
    Serial.print("o");
    lcd.write(byte(0));
  }
  else if (a == 201)
  {
    Serial.print("y");
    lcd.write(byte(1));
  }
}


void printgoal(int a[])
{
  lcd.setCursor(16, 1);
  for (int i = 0;; i++)
  {
    if (i != 5)
    {
      lcd.print(a[i]);
      Serial.print(a[i]);
    }
    else
    {
      Serial.println();
      break;
    }
  }
}

void printactline(int a[])
{
  lcd.setCursor(0, 0);
  for (int i = 0;; i++)
  {
    if (a[i] == -1)
    {
      Serial.println();
      break;
    }
    else if (a[i] == -2)
    {
      Serial.print("X");
      lcd.print(" ");
    }
    else if (!a[i])
    {
      lcd.print("0");
      Serial.print("0");
    }
    else if (a[i] == 1)
    {
      lcd.print("1");
      Serial.print("1");
    }
    else if (a[i] == 99)
    {
      lcd.print("<");
      Serial.print("<");
    }
    else if (a[i] == 101)
    {
      lcd.print(">");
      Serial.print(">");
    }
    else if (a[i] == 199)
    {
      Serial.print("A");
      lcd.write(byte(2));
    }
    else if (a[i] == 200)
    {
      Serial.print("o");
      lcd.write(byte(0));
    }
    else if (a[i] == 201)
    {
      Serial.print("y");
      lcd.write(byte(1));
    }
    else if (a[i] == 301)
    {
      Serial.print("[<1]");
      lcd.write(byte(4));
    }
    else if (a[i] == 302)
    {
      Serial.print("[<2]");
      lcd.write(byte(5));
    }
    else if (a[i] == 303)
    {
      Serial.print("[<3]");
      lcd.write(byte(6));
    }
    else if (a[i] == 304)
    {
      Serial.print("[<4]");
      lcd.write(byte(7));
    }
  }
}

void printunderactline(int a[])
{
  lcd.setCursor(0, 1);
  for (int i = 0;; i++)
  {
    if (a[i] == -1)
    {
      Serial.println();
      break;
    }
    else if (a[i] == -2)
    {
      Serial.print("X");
      lcd.print(" ");
    }
    else if (!a[i])
    {
      lcd.print("0");
      Serial.print("0");
    }
    else if (a[i] == 1)
    {
      lcd.print("1");
      Serial.print("1");
    }
    else if (a[i] == 99)
    {
      lcd.print("<");
      Serial.print("<");
    }
    else if (a[i] == 101)
    {
      lcd.print(">");
      Serial.print(">");
    }
    else if (a[i] == 199)
    {
      Serial.print("A");
      lcd.write(byte(2));
    }
    else if (a[i] == 200)
    {
      Serial.print("o");
      lcd.write(byte(0));
    }
    else if (a[i] == 201)
    {
      Serial.print("y");
      lcd.write(byte(1));
    }
    else if (a[i] == 301)
    {
      Serial.print("[<1]");
      lcd.write(byte(7));
    }
    else if (a[i] == 302)
    {
      Serial.print("[<2]");
      lcd.write(byte(8));
    }
    else if (a[i] == 303)
    {
      Serial.print("[<3]");
      lcd.write(byte(9));
    }
    else if (a[i] == 304)
    {
      Serial.print("[<4]");
      lcd.write(byte(10));
    }
  }
}


void printcurrentactline(int a)
{
  lcd.setCursor(0, 3);
  Serial.print("Current actline is");
  lcd.print("Current act : ");
  if (a == -1)
  {
    Serial.println();
  }
  else if (a == -2)
  {
    Serial.print("X");
    lcd.print(" ");
  }
  else if (!a)
  {
    lcd.print("0");
    Serial.print("0");
  }
  else if (a == 1)
  {
    lcd.print("1");
    Serial.print("1");
  }
  else if (a == 99)
  {
    lcd.print("<");
    Serial.print("<");
  }
  else if (a == 101)
  {
    lcd.print(">");
    Serial.print(">");
  }
  else if (a == 199)
  {
    Serial.print("A");
    lcd.write(byte(2));
  }
  else if (a == 200)
  {
    Serial.print("o");
    lcd.write(byte(0));
  }
  else if (a == 201)
  {
    Serial.print("y");
    lcd.write(byte(1));
  }
  else if (a == 301)
  {
    Serial.print("[<1]");
    lcd.write(byte(3));
  }
  else if (a == 302)
  {
    Serial.print("[<2]");
    lcd.write(byte(4));
  }
  else if (a == 303)
  {
    Serial.print("[<3]");
    lcd.write(byte(5));
  }
  else if (a == 304)
  {
    Serial.print("[<4]");
    lcd.write(byte(6));
  }
}

void motorturn(int a, int val)
{
  val = map(val, 0, 360, 0, 2048);
  if (a == 0)
  {
    Serial.println("1st motor turned.");
    stepper0.step(val);
    delay(1000);
  }
  else if (a == 1)
  {
    Serial.println("2nd motor turned.");
    stepper1.step(val);
    delay(1000);
  }
  else if (a == 2)
  {
    Serial.println("3rd motor turned.");
    stepper2.step(val);
    delay(1000);
  }
  else if (a == 3)
  {
    Serial.println("4th motor turned.");
    stepper3.step(val);
    delay(1000);
  }
  else if (a == 4)
  {
    Serial.println("5th motor turned.");
    stepper4.step(val);
    delay(1000);
  }
  else if (a == 5)
  {
    Serial.println("chain motor turned.");
    stepper_chain.step(val);
    delay(1000);
  }
}



void zero_one(int head)
{
  int val = map(180, 0, 360, 0, 2048);
  if (head == 0)
  {
    stepper0.step(val);
  }
  else if (head == 1)
  {
    stepper1.step(val);
  }
  else if (head == 2)
  {
    stepper2.step(val);
  }
  else if (head == 3)
  {
    stepper3.step(val);
  }
  else if (head == 4)
  {
    stepper4.step(val);
  }
}

void one_zero(int head)
{
  int val = map(180, 0, 360, 0, 2048);
  if (head == 0)
  {
    stepper0.step(val);
  }
  else if (head == 1)
  {
    stepper1.step(val);
  }
  else if (head == 2)
  {
    stepper2.step(val);
  }
  else if (head == 3)
  {
    stepper3.step(val);
  }
  else if (head == 4)
  {
    stepper4.step(val);
  }
}

void one_blank(int head)
{
  int val = map(90, 0, 360, 0, 2048);
  if (head == 0)
  {
    stepper0.step(val);
  }
  else if (head == 1)
  {
    stepper1.step(val);
  }
  else if (head == 2)
  {
    stepper2.step(val);
  }
  else if (head == 3)
  {
    stepper3.step(val);
  }
  else if (head == 4)
  {
    stepper4.step(val);
  }
}

void blank_one(int head)
{
  int val = map(-90, 0, 360, 0, 2048);
  if (head == 0)
  {
    stepper0.step(val);
  }
  else if (head == 1)
  {
    stepper1.step(val);
  }
  else if (head == 2)
  {
    stepper2.step(val);
  }
  else if (head == 3)
  {
    stepper3.step(val);
  }
  else if (head == 4)
  {
    stepper4.step(val);
  }
}

void zero_blank(int head)
{
  int val = map(-90, 0, 360, 0, 2048);
  if (head == 0)
  {
    stepper0.step(val);
  }
  else if (head == 1)
  {
    stepper1.step(val);
  }
  else if (head == 2)
  {
    stepper2.step(val);
  }
  else if (head == 3)
  {
    stepper3.step(val);
  }
  else if (head == 4)
  {
    stepper4.step(val);
  }
}

void blank_zero(int head)
{
  int val = map(90, 0, 360, 0, 2048);
  if (head == 0)
  {
    stepper0.step(val);
  }
  else if (head == 1)
  {
    stepper1.step(val);
  }
  else if (head == 2)
  {
    stepper2.step(val);
  }
  else if (head == 3)
  {
    stepper3.step(val);
  }
  else if (head == 4)
  {
    stepper4.step(val);
  }
}

void goheadleft()
{
  stepper_chain.step(map(-320, 0, 360, 0, 2048));
}

void goheadright()
{
  stepper_chain.step(map(320, 0, 360, 0, 2048));
}

void changeblock(int a, int b, int head)
{
  if (a == 0 && b == 1)
  {
    zero_one(head);
  }
  else if (a == 0 && b == -2)
  {
    zero_blank(head);
  }
  else if (a == 1 && b == 0)
  {
    one_zero(head);
  }
  else if (a == 1 && b == -2)
  {
    one_blank(head);
  }
  else if (a == -2 && b == 0)
  {
    blank_zero(head);
  }
  else if (a == -2 && b == 1)
  {
    blank_one(head);
  }
}

void mapchange(int a[], int b)
{
  for (int i = 0; i < 5; i++)
  {
    changeblock(a[i], realtape[b][i], i);
  }
}

int spd = 17;

void setup()
{
  // initialize the LCD
  pinMode(30, INPUT); // 1버튼
  pinMode(32, INPUT); // 2버튼
  pinMode(39, INPUT); // 0번맵
  pinMode(41, INPUT); // 1번맵
  pinMode(43, INPUT); // 2번맵
  pinMode(36, INPUT); // 행동표 실행 버튼
  lcd.begin();
  lcd.createChar(0, if0);
  lcd.createChar(1, if1);
  lcd.createChar(2, invert_one);
  lcd.createChar(3, invert_zero);
  lcd.createChar(4, back1);
  lcd.createChar(5, back2);
  lcd.createChar(6, back3);
  lcd.createChar(7, back4);
  stepper0.setSpeed(spd);
  stepper1.setSpeed(spd);
  stepper2.setSpeed(spd);
  stepper3.setSpeed(spd);
  stepper4.setSpeed(spd);
  stepper_chain.setSpeed(spd);
  Serial.begin(9600);
  // Turn on the blacklight and print a message.
  lcd.backlight();
  //lcd.print("Hello, world!");
}

void loop()
{
  printactline(actline[num]);
  printunderactline(underactline[num]);
  lcd.setCursor(9, 0);
  lcd.print(num);
  Serial.print(num);
  Serial.println("th tape");
  printtape(tape[num]);
  printgoal(goal[num]);
  delay(1000);
  lcd.clear();
  state = 0;
  if (digitalRead(39))//0번맵
  {
    mapchange(tape[num], 0);
    num = 0;
  }
  if (digitalRead(41))//1번맵
  {
    mapchange(tape[num], 1);
    num = 1;
  }
  if (digitalRead(43))//2번맵
  {
    mapchange(tape[num], 2);
    num = 2;
  }
  if (digitalRead(30))
  {
    actline[num][buttonnum[num][0]] = buttonchange(actline[num][buttonnum[num][0]]);
  }
  if (digitalRead(32))
  {
    actline[num][buttonnum[num][1]] = buttonchange(actline[num][buttonnum[num][1]]);
  }
  if (digitalRead(36))
  {
    Serial.println("A0 was inputed.");
    state = 1;
  }
  if (Serial.available() > 0)
  {
    int buttoninput = Serial.read() - 48;
    if (buttoninput == 9)
    {
      state = 1;
    }
    else if (buttoninput == 0)
    {
      actline[num][buttonnum[num][buttoninput]] = buttonchange(actline[num][buttonnum[num][buttoninput]]);
      buttoninput = 2;
    }
    else if (buttoninput == 1)
    {
      actline[num][buttonnum[num][buttoninput]] = buttonchange(actline[num][buttonnum[num][buttoninput]]);
      buttoninput = 2;
    }
    else if (buttoninput == 5)
    {
      Serial.println("10 was inputed.");
      mapchange(tape[num], 0);
      for (int j = 0; j < 5; j++)
      {
        if (tape[num][j] != realtape[num][j])
        {
          tape[num][j] = realtape[num][j];
        }
      }
      num = 0;
    }
    else if (buttoninput == 6)
    {
      Serial.println("11 was inputed.");
      mapchange(tape[num], 1);
      for (int j = 0; j < 5; j++)
      {
        if (tape[num][j] != realtape[num][j])
        {
          tape[num][j] = realtape[num][j];
        }
      }
      num = 1;
    }
    else if (buttoninput == 7)
    {
      printactline(actline[num]);
      printunderactline(underactline[num]);
      Serial.println("12 was inputed.");
      mapchange(tape[num], 2);
      for (int j = 0; j < 5; j++)
      {
        if (tape[num][j] != realtape[num][j])
        {
          tape[num][j] = realtape[num][j];
        }
      }
      num = 2;
    }
  }
  if (state)
  {
    Serial.println("actline act");
    for (int i = 0;; i++)
    {
      if (under == 0)
      {
        Serial.print(i);
        Serial.println("th actline");
        printactline(actline[num]);
        printunderactline(underactline[num]);
        printtape(tape[num]);
        printgoal(goal[num]);
        printcurrentactline(actline[num][i]);
        /*for (int j = 0;; j++)
          {
          if (actline[num][j] == -1)
          {
            break;
          }
          if (i == j)
          {
            printinvert(actline[num][i]);
          }
          else
          {
            printnotinvert(actline[num][i]);
          }
          }*/
        if (actline[num][i] == -1)
        {
          Serial.println("actline's done.");
          break;
        }
        else if (actline[num][i] == 0 || actline[num][i] == 1)
        {
          if ((tape[num][i] == 0 && actline[num][i] == 1) || (tape[num][i] == 1 && actline[num][i] == 0))
          {
            Serial.println("motor turn.");
            zero_one(head);
          }
          else if (tape[num][i] == -2)
          {
            if (actline[num][i] == 0)
            {
              blank_zero(head);
            }
            else
            {
              blank_one(head);
            }
          }
          Serial.print(head);
          Serial.print(":");
          Serial.print(tape[num][head]);
          Serial.print("->");
          Serial.println(actline[num][i]);
          tape[num][head] = actline[num][i];
        }
        else if (actline[num][i] == 99)
        {
          Serial.print("head : ");
          Serial.print(head);
          Serial.print("->");
          Serial.println(--head);
          goheadleft();
        }
        else if (actline[num][i] == 101)
        {
          Serial.print("head : ");
          Serial.print(head);
          Serial.print("->");
          Serial.println(++head);
          goheadright();
        }
        else if (actline[num][i] == 199)
        {
          if (tape[num][head] == -2)
          {
            Serial.print(head);
            Serial.println("th is blank. And then go UNDER!");
            i--;
            under = 1;
          }
          else
          {
            Serial.print(head);
            Serial.print("th is ");
            Serial.print(tape[num][head]);
            Serial.println(",,, keep going");
          }
        }
        else if (actline[num][i] == 200)
        {
          if (tape[num][head] == 0)
          {
            Serial.print(head);
            Serial.println("th is 0. And then go UNDER!");
            i--;
            under = 1;
          }
          else
          {
            Serial.print(head);
            Serial.print("th is ");
            Serial.print(tape[num][head]);
            Serial.println(",,, keep going");
          }
        }
        else if (actline[num][i] == 201)
        {
          if (tape[num][head] == 1)
          {
            Serial.print(head);
            Serial.println("th is 1. And then go UNDER!");
            i--;
            under = 1;
          }
          else
          {
            Serial.print(head);
            Serial.print("th is ");
            Serial.print(tape[num][head]);
            Serial.println(",,, keep going");
          }
        }
        else if (actline[num][i] == 301)
        {
          i -= 2;
        }
        else if (actline[num][i] == 302)
        {
          i -= 3;
        }
        else if (actline[num][i] == 303)
        {
          i -= 4;
        }
        else if (actline[num][i] == 304)
        {
          i -= 5;
        }
        printtape(tape[num]);
        delay(2000);
      }
      else if (under == 1)
      {
        printactline(actline[num]);
        printunderactline(underactline[num]);
        printtape(tape[num]);
        printgoal(goal[num]);
        Serial.print(i);
        Serial.println("th underactline");
        printcurrentactline(underactline[num][i]);
        if (underactline[num][i] == -1)
        {
          Serial.println("it's done.");
          break;
        }
        else if (underactline[num][i] == 0 || underactline[num][i] == 1)
        {
          Serial.print(head);
          Serial.print(":");
          Serial.print(tape[num][head]);
          Serial.print("->");
          Serial.println(underactline[num][i]);
          if (tape[num][head] == -2)
          {
            if (underactline[num][i] == 0)
            {
              blank_zero(head);
            }
            else if (underactline[num][i] == 1)
            {
              blank_one(head);
            }
          }
          else
          {
            zero_one(head);
          }
          tape[num][head] = underactline[num][i];
        }
        else if (underactline[num][i] == 99)
        {
          goheadleft();
          Serial.print("head : ");
          Serial.print(head);
          Serial.print("->");
          Serial.println(--head);
        }
        else if (underactline[num][i] == 101)
        {
          goheadright();
          Serial.print("head : ");
          Serial.print(head);
          Serial.print("->");
          Serial.println(++head);
        }
        else if (underactline[num][i] == 199)
        {
          if (tape[head] == -2)
          {
            Serial.print(head);
            Serial.println("th is blank. And then go UP!");
            i--;
            under = 0;
            break;
          }
        }
        else if (underactline[num][i] == 200)
        {
          if (tape[head] == 0)
          {
            Serial.print(head);
            Serial.println("th is 0. And then go UP!");
            i--;
            under = 0;
            break;
          }
        }
        else if (underactline[num][i] == 201)
        {
          if (tape[head] == 1)
          {
            Serial.print(head);
            Serial.println("th is 1. And then go UP!");
            i--;
            under = 1;
            break;
          }
        }
        printtape(tape[num]);
        delay(500);
      }
    }
    under = 0;
    //이제 검사(테이프랑 목표 코드랑 일치하는지)
    for (int i = 0; i < 5; i++)
    {
      printactline(actline[num]);
      printunderactline(underactline[num]);
      printgoal(goal[num]);
      if (tape[num][i] != goal[num][i])
      {
        test = 1;
        Serial.print(i);
        Serial.println("th is WRONG!!");
        break;
      }
      else
      {
        Serial.print(i);
        Serial.println("th is Complete");
      }
      delay(500);
    }
    if (test)
    {
      for (int j = 0; j < 5; j++)
      {
        if (tape[num][j] != realtape[num][j])
        {
          changeblock(tape[num][j], realtape[num][j], j);
          tape[num][j] = realtape[num][j];
        }
      }
      test = 0;
    }
    else
    {
      lcd.setCursor(7, 2);
      lcd.print("SUCCESS!!");
      Serial.println("SUCCESS!!");
      while (1)
      {
        if (digitalRead(39) || digitalRead(41) || digitalRead(43) || Serial.available())
        {
          int ekdmaaoq = Serial.parseInt();
          if (ekdmaaoq >= 0 && ekdmaaoq <= 2)
          {
            Serial.println("mapchange :");
            Serial.println(ekdmaaoq);
            mapchange(tape[num], ekdmaaoq);
            num = ekdmaaoq;
            break;
          }
          if (digitalRead(39))
          {
            mapchange(tape[num], 0);
            num = 0; //첫번째 맵
            break;
          }
          else if (digitalRead(41))
          {
            mapchange(tape[num], 1);
            num = 1; //두번째맵
            break;
          }
          else if (digitalRead(43))
          {
            mapchange(tape[num], 2);
            num = 2; //세번째맵
            break;
          }
        }
      }
      for (int abc = 0; abc < 3; abc++)
      {
        for (int j = 0; j < 5; j++)
        {
          if (tape[abc][j] != realtape[abc][j])
          {
            tape[abc][j] = realtape[abc][j];
          }
        }
      }
      test = 0;
    }
    if (head > 2)
    {
      int asdf = head - 2;
      for (int gpemehfflrl = 0; gpemehfflrl < asdf; gpemehfflrl++)
      {
        goheadleft();
      }
    }
    else if (head < 2)
    {
      int asdf = 2 - head;
      for (int gpemehfflrl = 0; gpemehfflrl < asdf; gpemehfflrl++)
      {
        goheadright();
      }
    }
    head = 2;
    state = 0;
  }
}
