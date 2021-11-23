#include <M5StickC.h>

// 曲げセンサを36番ピンのADコンバータにつなぐ
#define VOL_PIN 36
   
// センサの値の平滑化に用いる定数
//　0<a<1の範囲で設定する(大きいほど効果が大きくなる)
#define a 0.0

// センサの値の変数定義
int vol_value = 0;
float rc = 0;

void setup() {
  M5.begin();
  Serial.begin(115200);

  // 36番ピンを入力モードにする
  pinMode(VOL_PIN, INPUT);
}

void loop() {
  // 35番ピンの電圧値を読む
  vol_value = analogRead(VOL_PIN);

  // 「センサの値」と「前回の出力値」の内分点を出力値とするRCフィルタ値の計算をする
  rc = a * rc + (1-a) * (float)vol_value;

  //フィルタ前の値をシリアルモニタに表示
  Serial.print("raw: ");
  Serial.print(vol_value);
  Serial.print(", ");

  //フィルタ後の値をシリアルモニタに表示
  Serial.print("filterd: ");
  Serial.print(rc);
  Serial.print("\n");

  // 0.5秒に1回計測
  delay(500);
}
