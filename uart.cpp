/*
	uart.cpp
*/
#include "user_define.h"
#include <string.h>


/* ---------- UART access functions ---------- */

void init_uart(){
  Serial.begin(UART_BPS);
}

void uart_sendchar(uint8_t c){
  Serial.print((char)c);
}

/* get a received char from receive buffer */
bool uart_getchar(uint8_t *c){
  bool f;
  f = false;
  if (Serial.available() > 0){
    f = true;
    *c = (uint8_t)(Serial.read());
  } 
  return f;
}

int8_t recv_len(){
	return Serial.available();
}

