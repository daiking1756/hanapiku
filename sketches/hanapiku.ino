#include <M5Stack.h>
// アバターを表示するライブラリの読み込み
#include <Avatar.h>

// 曲げセンサを35番ピンのADコンバータにつなぐ
#define VOL_PIN 35

// センサの値の平滑化に用いる定数
//　0<a<1の範囲で設定する(大きいほど効果が大きくなる)
#define a 0.0

using namespace m5avatar;

// Avatarクラスの変数定義
Avatar avatar;

// センサの値の変数定義
int vol_value = 0;
float rc = 0;

void setup() {
  M5.begin();
  M5.Speaker.begin(); // これが無いとmuteしても無意味です。
  M5.Speaker.mute();
  Serial.begin(115200);

  // 35番ピンを入力モードにする
  pinMode(VOL_PIN, INPUT);

  // avatarオブジェクトを初期化
  avatar.init();
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

  // 鼻がピクピクしたとき
  if (rc > 350.00) { 
    // 怒り顔の表示
    avatar.setExpression(Expression::Angry);
  } else { 
    // ニコニコ顔の表示
    avatar.setExpression(Expression::Happy);
  }

}
