/*


   TO DO: убрать делэй на звонке(хуже этого нет ничего на этом свете)
          добавить EEPROM

  

   Pinout:
   S1 S2 KEY 3 2 4
   SCL SDA 5 4
   RELAY 5
*/
#define OLED_SOFT_BUFFER_64
#include <GyverOLED.h>
GyverOLED oled;

#include <GyverEncoder.h>
#define CLK 2
#define DT 3
#define SW 4
Encoder enc(CLK, DT, SW, TYPE2);

#include <microDS3231.h>
MicroDS3231 rtc;
#define ITEMS 4

#define RELAY 5
#define DURATION 4
#include <GyverTimers.h>



uint32_t StartTime = 480 * 60;                 // Время начала в минутах
uint8_t TimeSettings[] = {                // массив значений настроек
  40, 10, 10, 20, 10, 10           //продолжительность Урока + перемен
};

uint32_t timings[] =
{
  StartTime,
  timings[0] + TimeSettings[0] * 60,
  timings[1] + TimeSettings[1] * 60,
  timings[2] + TimeSettings[0] * 60,
  timings[3] + TimeSettings[2] * 60,
  timings[4] + TimeSettings[0] * 60,
  timings[5] + TimeSettings[3] * 60,
  timings[6] + TimeSettings[0] * 60,
  timings[7] + TimeSettings[4] * 60,
  timings[8] + TimeSettings[0] * 60,
  timings[9] + TimeSettings[5] * 60,
  timings[10] + TimeSettings[0] * 60,
};


void setup() {
  //rtc.setTime(COMPILE_TIME);
  oled.init(OLED128x64, 500);
  oled.setContrast(255);
  Serial.begin(9600);
  attachInterrupt(0, isrCLK, CHANGE);    // прерывание на 2 пине! CLK у энка
  attachInterrupt(1, isrDT, CHANGE);    // прерывание на 3 пине! DT у энка

  pinMode(RELAY, OUTPUT);
  /* ПОКАЗ ВРЕМЕНИ ЗВОНКОВ
    for(uint8_t i=0;i<=11;i++){
    Serial.print(GlobalSeconds2Hours(timings[i]));
    Serial.print(" ");
    Serial.println(GlobalSeconds2Minutes(timings[i]));
    }
  */
}

void loop() {
  Signal();
  static int8_t pointer = 0;


  enc.tick();
  if (enc.isLeft()) {
    pointer = constrain(pointer - 1, 0, ITEMS - 1);
  }
  if (enc.isRight()) {
    pointer = constrain(pointer + 1, 0, ITEMS - 1);
  }

  if (enc.isClick()) {
    switch (pointer) {
      case 0: CurrentTime(); break;
      case 1: TimeSerial(); break;
      case 2: settings(); break;
      case 3: saver(); break;

    }
  }


  oled.clear();
  oled.home();

  if (pointer < 8) {
    oled.print
    (F(
       "  Текущее время:\n"
       "  Ручной режим:\n"
       "  Настройки:\n"
       "  Сон:\n"
     ));
  }


  printPointer(pointer);
  oled.update();
}



void printPointer(uint8_t pointer) {
  oled.setCursor(0, pointer);
  oled.print(">");

}

void printActivePointer(uint8_t pointer) {
  oled.setCursor(0, pointer);
  oled.print(">!");
}

void isrCLK() {
  enc.tick();  // отработка в прерывании
}
void isrDT() {
  enc.tick();  // отработка в прерывании
}

// ==========================================ВЫВОД ВРЕМЕНИ В СЕРИАЛ====================================
void TimeSerial(void) {
  oled.clear();
  oled.home();
  oled.print(F("RTC SETTINGS SERIAL:"));
  oled.update();
  while (1) {
    enc.tick();
    
    printSerialTime();
    if (enc.isClick()) return;
  }
}

void printSerialTime() {
  Serial.print(rtc.getHours());
  Serial.print(":");
  Serial.print(rtc.getMinutes());
  Serial.print(":");
  Serial.print(rtc.getSeconds());
  Serial.print(" ");
  Serial.print(rtc.getDay());
  Serial.print(" ");
  Serial.print(rtc.getDate());
  Serial.print("/");
  Serial.print(rtc.getMonth());
  Serial.print("/");
  Serial.println(rtc.getYear());
}

//==========================================ВЫВОД ТЕКУЩЕГО ВРЕМЕНИ======================================
void CurrentTime(void) {
  while (1) {
    enc.tick();
    if (enc.isClick()) return;

    Signal();

    oled.clear();
    oled.home();
    oled.scale2X();
    oled.print(" ");
    oled.print(rtc.getHours());
    oled.print(":");
    oled.print(rtc.getMinutes());
    oled.print(":");
    oled.println(rtc.getSeconds());

    oled.scale1X();
    oled.print(rtc.getDate());
    oled.print(".");
    oled.print(rtc.getMonth());
    oled.print(".");
    oled.println(rtc.getYear());
    PrintWeekDay();
    //oled.println(rtc.getDay());
    if (!digitalRead(RELAY)) {
      oled.print("Ожидание...\n");
    } else {
      oled.print("Звоню!\n");
    }

    oled.update();
  }
}

void PrintWeekDay() {
  switch (rtc.getDay()) {
    case 1: oled.println("Понедельник"); break;
    case 2: oled.println("Вторник"); break;
    case 3: oled.println("Среда"); break;
    case 4: oled.println("Четверг"); break;
    case 5: oled.println("Пятница"); break;
    case 6: oled.println("Суббота"); break;
    case 7: oled.println("Воскресенье"); break;

  }

}

//==============================МЕНЮ НАСТРОЕК===========================
void settings(void) {
  while (1) {
    static uint8_t pointerSettings = 0;
    static bool flag = 0;

    oled.clear();
    oled.home();
    oled.print
    (F(
       "   Начало: \n"
       "   Длительн.: \n"
       "   Перемена 1: \n"
       "   Перемена 2: \n"
       "   Перемена 3: \n"
       "   Перемена 4: \n"
       "   Перемена 5: \n"
     ));
    for (uint8_t i = 0; i <= 6; i++) {
      oled.setCursor(15, i);
      if (i == 0) {
        oled.print(GlobalSeconds2Hours(StartTime));
        oled.print(":");
        oled.print(GlobalSeconds2Minutes(StartTime));
      } else {
        oled.print(TimeSettings[i - 1]);
      }

    }

    enc.tick();
    if (enc.isLeft()) {
      pointerSettings = constrain(pointerSettings - 1, 0, 6);
    }
    if (enc.isRight()) {
      pointerSettings = constrain(pointerSettings + 1, 0, 6);
    }

    if (enc.isRightH()) {
      if (pointerSettings == 0) {
        StartTime += 60;
      } else {
        TimeSettings[pointerSettings - 1]++;
      }
    }
    if (enc.isLeftH()) {
      if (pointerSettings == 0) {
        StartTime -= 60;
      } else {
        TimeSettings[pointerSettings - 1]--;
      }
    }
    printPointer(pointerSettings);


    oled.update();
    if (enc.isHolded()) {
      //запись в массив времен звонка (в секундах)





      return;
    }

  }
}

uint32_t Time2Seconds(uint32_t hours, uint32_t minutes, uint32_t seconds) {
  return seconds + minutes * 60 + hours * 3600;
}

uint32_t GlobalSeconds2Minutes(uint32_t seconds) {
  return (seconds / 60) % 60;
}

uint32_t GlobalSeconds2Seconds(uint32_t seconds) {
  return seconds % 60;
}

uint32_t GlobalSeconds2Hours(uint32_t seconds) {
  return seconds / 3600;
}

//=======================ФУНКЦИЯ ЗВОНКА==================================

void Signal() {
  if (
    Time2Seconds(rtc.getHours(), rtc.getMinutes(), rtc.getSeconds()) == timings[0] or
    Time2Seconds(rtc.getHours(), rtc.getMinutes(), rtc.getSeconds()) == timings[1] or
    Time2Seconds(rtc.getHours(), rtc.getMinutes(), rtc.getSeconds()) == timings[2] or
    Time2Seconds(rtc.getHours(), rtc.getMinutes(), rtc.getSeconds()) == timings[3] or
    Time2Seconds(rtc.getHours(), rtc.getMinutes(), rtc.getSeconds()) == timings[4] or
    Time2Seconds(rtc.getHours(), rtc.getMinutes(), rtc.getSeconds()) == timings[5] or
    Time2Seconds(rtc.getHours(), rtc.getMinutes(), rtc.getSeconds()) == timings[6] or
    Time2Seconds(rtc.getHours(), rtc.getMinutes(), rtc.getSeconds()) == timings[7] or
    Time2Seconds(rtc.getHours(), rtc.getMinutes(), rtc.getSeconds()) == timings[8] or
    Time2Seconds(rtc.getHours(), rtc.getMinutes(), rtc.getSeconds()) == timings[9] or
    Time2Seconds(rtc.getHours(), rtc.getMinutes(), rtc.getSeconds()) == timings[10] or
    Time2Seconds(rtc.getHours(), rtc.getMinutes(), rtc.getSeconds()) == timings[11] or
    Time2Seconds(rtc.getHours(), rtc.getMinutes(), rtc.getSeconds()) == timings[12]
    //or enc.isHold()
  ) {
    digitalWrite(RELAY, HIGH);
    delay(2000);
    digitalWrite(RELAY, LOW);
  }

}
//=========================СКРИНСЕЙВЕР===================
void saver() {
  oled.clear();
  oled.update();
  while (1) {
    enc.tick();
    if (enc.isHold()) {
      return;
    }
  }
}
