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
#define DURATION 4


uint16_t StartTime = 480;                 // Время начала в минутах
uint8_t TimeSettings[] = {                // массив значений настроек
  40, 10, 10, 20, 10, 10           //продолжительность Урока + перемен
};
bool MainSignal = false;

void setup() {
  oled.init(OLED128x64, 500);
  oled.setContrast(255);
  Serial.begin(9600);
  attachInterrupt(0, isrCLK, CHANGE);    // прерывание на 2 пине! CLK у энка
  attachInterrupt(1, isrDT, CHANGE);    // прерывание на 3 пине! DT у энка
}

void loop() {
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
      case 3: break;

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
  oled.print(F("Press OK to return"));
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
    PrintWeekDay();
    //oled.println(rtc.getDay());
    if (MainSignal == 0) {
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
       "   Длительность: \n"
       "   Перемена 1: \n"
       "   Перемена 2: \n"
       "   Перемена 3: \n"
       "   Перемена 4: \n"
       "   Перемена 5: \n"
     ));
    for (uint8_t i = 0; i <= 6; i++) {
      oled.setCursor(16, i);
      if(i==0){
        oled.print(GlobalMinutes2Hours(StartTime));
        oled.print(":");
        oled.print(GlobalMinutes2Minutes(StartTime));
      }else{
        oled.print(TimeSettings[i-1]);
      }
      
    }

    enc.tick();
    if (enc.isLeft()) {
      pointerSettings = constrain(pointerSettings - 1, 0, 6);
    }
    if (enc.isRight()) {
      pointerSettings = constrain(pointerSettings + 1, 0, 6);
    }
    
    if (enc.isRightH()){
      if (pointerSettings == 0){
        StartTime++;
      }else{
        TimeSettings[pointerSettings-1]++;
      }
    }
    if (enc.isLeftH()){
      if (pointerSettings == 0){
        StartTime--;
      }else{
        TimeSettings[pointerSettings-1]--;
      }
    }
    printPointer(pointerSettings);


    oled.update();
    if (enc.isHolded()){
      return;
    }
  }
}

uint16_t Time2Minutes(uint16_t hours, uint16_t minutes){
  return hours*60+minutes;
}

uint16_t GlobalMinutes2Minutes(uint16_t minutes){
  return minutes%60;
}

uint16_t GlobalMinutes2Hours(uint16_t minutes){
  return minutes/60;  
}
