#include <M5Stack.h>

//reference = https://lang-ship.com/reference/unofficial/M5StickC/Class/Button/

void setup() {
  // init lcd, serial, but don't init sd card
//  M5.begin(true, false, true);
  M5.begin();

   //Power chipがgpio21, gpio22, I2Cにつながれたデバイスに接続される。
  //バッテリー動作の場合はこの関数を読んでください（バッテリーの電圧を調べるらしい）
//  M5.Power.begin();

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setTextSize(3);
}


void loop() {
  M5.update();// update button state
   
  M5.Lcd.setCursor(0,0);
  if (M5.BtnA.wasPressed()) {
    M5.Lcd.printf("BtnA pressed!");
  } else if (M5.BtnA.wasReleased()) {
    M5.Lcd.print("BtnA Released!");
  }else{
    M5.Lcd.print("BtnA None     ");
  }
  

  M5.Lcd.setCursor(0,60);
  if (M5.BtnB.isPressed()) {
    M5.Lcd.printf("BtnB pressed!");
  } else if (M5.BtnB.isReleased()) {
    M5.Lcd.print("BtnB Released!");
  }else{
    M5.Lcd.print("BtnB None     ");
  }
  
   
  M5.Lcd.setCursor(0, 120);
  if (M5.BtnC.pressedFor(1000)) {
    M5.Lcd.print("BtnC pressed for 1sec  ");
  }else if(M5.BtnC.releasedFor(1000)){
    M5.Lcd.print("BtnC released for 1sec ");
  }else{
    M5.Lcd.print("BtnC None              ");
  }
}
