/*
  user_define.h
*/
#include     "M5Unified.h"


//============ general definitioin ============
#define	CR	((char)0x0d)
#define	LF	((char)0x0a)
#define	BEL	((char)0x07)
#define	BS	((char)0x08)
#define	ESC	((char)0x1b)


//==================================================
// uart.cpp related definitions,vars,functions
//==================================================
//---------- define for uart ----------
#define  UART_BPS  115200
//---------- uart functions ----------
int8_t recv_len(void);
void uart_sendchar(uint8_t c);
bool uart_getchar(uint8_t *c);
void init_uart(void);


//==================================================
// i2cmaster.cpp related definitions,vars,functions
//==================================================
//---------- define for i2cmaster ----------
#define ENDTX_OK      0     // success (endTransmissioin() result, arduino definitioin)
#define ENDTX_TOLONG  1     // data too long
#define ENDTX_NACK_ADR   2  // NACK (address)
#define ENDTX_NACK_DAT   3  // NACK (data)
#define ENDTX_ERR_OTHER  4  // other error
//---------- i2cmaster functions ----------
void init_i2c(void);
uint8_t i2c_write(uint8_t adr, uint8_t *data, int8_t len);
size_t i2c_read(uint8_t adr, uint8_t *data, int8_t len);
uint8_t i2c_stopCond(void);
void i2c_deviceList(void);


//==================================================
// session.cpp related definitions,vars,functions
//==================================================
//---------- session functions ----------
void init_session(void);
