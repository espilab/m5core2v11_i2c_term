/*
 * i2cmaster.cpp
 */
#include "user_define.h"
#include  <Wire.h>


void init_i2c(){
    Wire.begin(21,22);
    Wire.setClock(100000);
}


// i2c write
uint8_t i2c_write(uint8_t adr, uint8_t *data, int8_t len){
  uint8_t j,ret;
  Wire.beginTransmission((uint8_t)adr);
  for (j=0; j<len; j++){
    Wire.write(data[j]);
  }
  ret = Wire.endTransmission();
  /*
  if (ret == 0){    // DEBUG
    Serial.println("ok");
  } else {
    Serial.print("i2c_write, result=");  Serial.println(ret);
  }
  */
  return ret;
}

// i2c read
size_t i2c_read(uint8_t adr, uint8_t *data, int8_t len){
  uint8_t j;
  size_t ret;
  j = 0;
  ret = Wire.requestFrom((int)adr, (int)len);
  while (Wire.available()){
    char c = Wire.read();
    data[j] = (uint8_t)c;
    j++;
  }
  return ret;
}

// i2c stop condition
uint8_t i2c_stopCond(){
  uint8_t ret;
  ret = Wire.endTransmission(true);
  Serial.print("endTransmission, result=");  Serial.println(ret);
  return ret;
}


// i2c list all address
void i2c_deviceList(){
  uint8_t adr, data[2],ret;

  char *cp PROGMEM =
    "x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xA xB xC xD xE xF\r\n"
    "-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --";
  Serial.print(cp);
  for (adr = 0x00; adr < 0x80; adr++){
    if (!(adr & 0x0F)) Serial.println("");
    ret = i2c_write(adr, data, 0);    // check slave by write
    //ret = i2c_read(adr, data, 1);   // check slave by read
    //if (adr==0x05){ ret=1; }//DEBUG
    //if (ret > 0){
    if (ret == ENDTX_OK){
      if (adr < 0x10){   Serial.print("0"); }
      Serial.print(adr, HEX);  Serial.print(" ");
    } else {
      Serial.print("** ");
    }
  }//for
  Serial.println("");
}
