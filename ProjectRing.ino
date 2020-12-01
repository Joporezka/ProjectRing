#define OLED_SOFT_BUFFER_64
#include <GyverOLED.h>
GyverOLED oled;

#include <GyverEncoder.h>
#define CLK 4
#define DT 3
#define SW 2
Encoder enc(CLK, DT, SW);

#include <microDS3231.h>
MicroDS3231 rtc;

#define ITEMS 4


const uint8_t ptr_bmp[] PROGMEM = {
  0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0xFF, 0xFF, 0x7E, 0x3C, 0x18,
};
uint8_t dataSettings[1]; // массив значений настроек
bool MainSignal = false;

void setup() {
  oled.init(OLED128x64, 500);
  oled.setContrast(255);
  Serial.begin(9600);
}

void loop() {
  static int8_t pointer = 0;


  enc.tick();

  if (enc.isLeft() or enc.isFastL()) {
    pointer = constrain(pointer - 1, 0, ITEMS - 1);
  }

  if (enc.isRight() or enc.isFastR()) {
    pointer = constrain(pointer + 1, 0, ITEMS - 1);
  }

  if (enc.isClick()) {
    switch (pointer) {
      case 0: CurrentTime(); break;
      case 1: TimeSerial(); break;
      case 2: break;
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
       "  Выключение дисплея и сигнала:\n"
     ));
  }

  printPointer(pointer);
  oled.update();
}



void printPointer(uint8_t pointer) {
  oled.setCursor(0, pointer);
  oled.print(">");

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
    oled.print(rtc.getHours());
    oled.print(":");
    oled.print(rtc.getMinutes());
    oled.print(":");
    oled.print(rtc.getSeconds());

    if (MainSignal == 0){
        oled.print("Ожидание...\n");
    }

    oled.update();
    oled.scale1x();
  }
}
