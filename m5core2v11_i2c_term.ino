/*
   m5core2v11_i2c_term.ino

   2025-11-7  一応使えるっぽい。
   2025-12-11 m5stack core2 v1.1 用にコンパイル、動作OK
   2026-01-06 変数の型をstdint.hの形式に変更, GitHubに登録
*/

#include  "user_define.h"


void setup() {
  init_uart();
  init_i2c();
  init_session();  // loop forever
}

void loop() {

  delay(5000);
  //uart_sendstr("loop.. ");
}
