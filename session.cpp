/*
	session.cpp
*/
#include "user_define.h"
#include <string.h>
#include <ctype.h>

/* ---------- Global vars ---------- */
#define CMDLINE_BUFSIZE 40
#define I2C_BUFSIZE 64

uint8_t	CMDLINE[CMDLINE_BUFSIZE];
uint8_t 	TARGET_ADR;
uint16_t	RD_CNT;
uint16_t	WR_CNT;
uint8_t	I2C_RD_BUF[I2C_BUFSIZE];
uint8_t	I2C_WR_BUF[I2C_BUFSIZE];


/* hex string to int (up to 4digit)*/
uint16_t hex2val(uint8_t *str){
  uint16_t ret,m,p;
  p=0;
  ret=0;
  while ((str[p]!=0)&&(p<5)){ p++; }
  if (p>=5){ return 0; }
  m=1;
  while (p>0){
    p--;
    if (toupper(str[p]) >= 'A'){
      ret += m * ((toupper(str[p])-'A') + 10);
    } else {
      ret += m * (str[p]-'0');
    }
    m = m << 4;
  }
  return ret;
}
/* int to hex string (up to 4digit)*/
void hexstr(uint16_t val, char *str){
  char *hextbl="0123456789ABCDEF?";
  str[0] = hextbl[val >> 12];
  str[1] = hextbl[(val >> 8) & 0x000f];
  str[2] = hextbl[(val >> 4) & 0x000f];
  str[3] = hextbl[val & 0x000f];
  str[4] = 0;
}
/* ---------- Terminal session functions ---------- */
void show_help()
{
	const char *cp PROGMEM =
		" a : set slave address\r\n"
		" s : set byte(s) data to write\r\n"
		" c : set count bytes to read\r\n"
		" r : read access\r\n"
		" w : write access\r\n"
		" p : show current parameters\r\n"
		" l : list all address response\r\n"
		" 0 : reset i2c I/F\r\n"
		" ? : show help\r\n"
		"read/write bytes count is 64 bytes max.\r\n";
	Serial.print(cp);
}

/* --- get one command line --- */
/*   loop in this function until enter key pressed
     return: string length
*/
int8_t cmd_line(uint8_t *cmdlineptr){
	uint8_t cin;
	uint8_t len;
	bool enterkey;
	uint8_t cptr = 0;

	enterkey = false;
	while (!enterkey){
		while(!(recv_len() > 0)){		/* wait for anykey loop */
			//check_switch();		/* check push sw event (it's not related this session) */
      //digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
		}
		//uart_sendstr("(enter-pressed)\r\n");
	
		if (uart_getchar(&cin)){
			if (cin == BS){
				if (cptr >0){
					Serial.print((char)BS);
					Serial.print((char)' ');
					Serial.print((char)BS);
					cptr--;
				} else {
					Serial.print((char)BEL);
				}
			}
			else if (cin == CR){
				Serial.print((char)CR);
				Serial.print((char)LF);
				cmdlineptr[cptr] = 0;
				len = cptr;
				cptr = 0;
				enterkey = true;
			}
			else {
				if (cptr < (CMDLINE_BUFSIZE-1)){
					Serial.print((char)cin);
					cmdlineptr[cptr++] = cin;
				} else {
					Serial.print((char)BEL);
				}
			}
		}
	} /* while(.. */
	return len;
}

/* decode and process command */
void process_command(uint8_t *cmdline, int8_t len){
	uint16_t adr,j;
	char str[8];
	bool quit;
	uint8_t data;
	uint8_t stat;



	switch (cmdline[0]){
		case '?':
			show_help();
			break;
		case 'a':
			Serial.print("Enter slave address of target(hex):");
			while (cmd_line(CMDLINE) < 0)
				;
			adr = hex2val(CMDLINE);
			TARGET_ADR = (uint8_t)adr;
			hexstr(TARGET_ADR, str);
			Serial.print("adr=0x");	Serial.println(&str[2]);
			break;
		case 'c':
			Serial.print("Enter count(bytes) to read(hex):");
			while (cmd_line(CMDLINE) < 0)
				;
			data = (uint8_t)hex2val(CMDLINE);
			if (data <= I2C_BUFSIZE){
				RD_CNT = data;
				hexstr(RD_CNT, str);
				Serial.print("count=0x");	Serial.println(&str[2]);
			} else {
				Serial.println("Illegal value!");
			}
			break;
		case 'r':
			stat = i2c_read(TARGET_ADR, I2C_RD_BUF, RD_CNT);
			//hexstr(stat, str);	
			//uart_sendstr("return=0x");	uart_sendstr(str);	uart_sendstr(" \r\n");
      //Serial.print("(return=");  Serial.print(stat);  Serial.println(")");
			if (stat > 0){
				Serial.println("ok");
			} else {
			  Serial.println("No response");
			}
			Serial.print("read data: ");
			for (j=0; j<RD_CNT; j++){
				hexstr(I2C_RD_BUF[j], str);
				if ((j & 0x000F)==0){	Serial.println("");	}
				Serial.print(&str[2]); Serial.print(" ");
			}
			Serial.println("");
			break;
		case 'w':
			stat = i2c_write(TARGET_ADR, I2C_WR_BUF, WR_CNT);
			//hexstr(stat, str);	
			//uart_sendstr("return=0x");	uart_sendstr(str);	uart_sendstr(" \r\n");
			switch (stat){
				case ENDTX_OK :
					Serial.println("ok");
					break;
				case ENDTX_TOLONG:
					Serial.println("data too long");
					break;
        case ENDTX_NACK_ADR:
          Serial.println("NACK(address)");
          break;
        case ENDTX_NACK_DAT:
          Serial.println("NACK(data)");
          break;
        case ENDTX_ERR_OTHER:
          Serial.println("Error(other)");
          break;
			}
			break;

		case 'p':
			hexstr(TARGET_ADR, str);
			Serial.print("target adr = 0x");	Serial.println(&str[2]);	
			Serial.print("data for write: ");

			for (j=0; j<WR_CNT; j++){
				hexstr(I2C_WR_BUF[j], str);
				Serial.print(&str[2]);  Serial.print(" ");
			}
			Serial.println("");
			hexstr(RD_CNT, str);
			Serial.print("read count = 0x");	Serial.println(&str[2]);
			break;


		case 's':
			Serial.println("Enter data bytes to write, or '.' for quit.");
			j = 0;
			quit = false;
			while (!quit){
				hexstr(j, str);
				Serial.print("[");	Serial.print(&str[2]);	Serial.print("]>");
				while (cmd_line(CMDLINE) < 0)
					;
				if (CMDLINE[0] == '.'){
					break;
				} else {
					data = (uint8_t)hex2val(CMDLINE);
					I2C_WR_BUF[j++] = data;
				}
				if (j >= I2C_BUFSIZE){	break;	}
			}
			WR_CNT = j;
			hexstr(WR_CNT, str);
			Serial.print("data: 0x");	Serial.print(&str[2]);	Serial.println(" bytes");
			break;

    case '0':
      init_i2c();
      break;
      
    case 'l':
      i2c_deviceList();
      break;
      
		dafault:
			break;
	}
}
/* Session --- loop infinite here */
void session(){
	uint8_t len;
	TARGET_ADR = 0x70; 	/* defualt uC */
	
	while (1){
		//digitalWrite(13, LOW);
    Serial.print('>');
		len = cmd_line(CMDLINE);
		process_command(CMDLINE, len);
	}
}

void init_session(void){
	const char *cp PROGMEM =
	  "-----\r\nI2C device access tool ver 1.0  2026-1-6 by espi_lab.";
	Serial.println(cp);

	session();
}
