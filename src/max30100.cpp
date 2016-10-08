#include "Arduino.h"
#include <Wire.h>
#include "MAX30100.h"

MAX30100::MAX30100(){

}

void MAX30100::reset(void){
	uint8_t cfg = 0;
	cfg = cfg | 0x40;
  SendToSensor(MODE_CONFIG, cfg);
}

void MAX30100::init(void){
	uint8_t cfg = 0;

  // read old and set new mode
  ReadFromSensor(MODE_CONFIG, &cfg, 1);
  cfg = ( cfg & ~0x07 ) | HR_ONLY;
  cfg = ( cfg & ~0x07 ) | SPO2_EN | TEMP_EN;
  SendToSensor(MODE_CONFIG, cfg);

  ReadFromSensor(SPO2_CONFIG, &cfg, 1); // read SpO2 configuration
  cfg |= SPO2_HI_RES_EN; // enable High Resolution SpO2
  cfg |= SAMPLES_100; // set samples per second
  cfg |= PULSE_WIDTH_1600; // set 16-bit ADC resolution
  SendToSensor(SPO2_CONFIG, cfg); // send new SpO2 configuration

  ReadFromSensor(LED_CONFIG, &cfg, 1); // read LED current control config
  cfg |= IR_CURRENT_500; // set IR LED current to be 50 mA
  cfg |= RED_CURRENT_500; // set RED LED current to be 24 mA
  SendToSensor(LED_CONFIG, cfg); // send new LED current control config

  // interrupt settings
  ReadFromSensor(INT_ENABLE, &cfg, 1);
  cfg |= ENA_HR_RDY;
  cfg |= ENA_SO2_RDY;
  cfg |= ENA_TEP_RDY;// send new interrupt settings
  SendToSensor(INT_ENABLE, cfg);
}

void MAX30100::GetTemp() {
  uint8_t cfg = 0;
  // read old and set new mode
  ReadFromSensor(MODE_CONFIG, &cfg, 1);
  cfg |= TEMP_EN;
  SendToSensor(MODE_CONFIG, cfg);
}

// read calibration temperature from the sensor
// @param tempValue data from temperature sensor
// MAX30100_ReadTemp()
float MAX30100::ReadTemp() {
  uint8_t tempInt, tempFrac;
  float temp = 0;
  ReadFromSensor(TEMP_INTEGER, &tempInt, 1); // read temperature - integer part
  ReadFromSensor(TEMP_FRACTION, &tempFrac, 1); // read temperature - fraction part

  temp = tempInt + (tempFrac * 0.0625);
  return temp;
}

void MAX30100::readSensor(void){
  uint8_t temp[4] = {0};  // Temporary buffer for read values
  ReadFromSensor(FIFO_DATA_REG, &temp[0], 4);  // Read four times from the FIFO
  IR = (temp[0]<<8) | temp[1];    // Combine values to get the actual number
  RED = (temp[2]<<8) | temp[3];   // Combine values to get the actual number
}

void MAX30100::ReadFromSensor (uint8_t rAddr, uint8_t* rxBuff, uint8_t dataNum)
{
	Wire.beginTransmission(MAX30100_I2C_ADR);   // Initialize the Tx buffer
	// Next send the register to be read. OR with 0x80 to indicate multi-read.
	Wire.write(rAddr);     // Put slave register address in Tx buffer
	Wire.endTransmission(false);       // Send the Tx buffer, but send a restart to keep connection alive
	uint8_t i = 0;
	Wire.requestFrom(MAX30100_I2C_ADR, dataNum);  // Read bytes from slave register address
	while (Wire.available())
	{
		rxBuff[i++] = Wire.read(); // Put read results in the Rx buffer
	}
}

uint8_t MAX30100::GetStatus() {
  uint8_t readData;
  ReadFromSensor(INT_STATUS, &readData, 1);
  return readData;
}

// initialize the sensor
void MAX30100::SendToSensor (uint8_t wrAddr, uint8_t wrData) {
  uint8_t dataToSend[2];
  dataToSend[0] = wrAddr;
  dataToSend[1] = wrData;
  // send address to write to and then data to be written
  Wire.beginTransmission(MAX30100_I2C_ADR);  // Initialize the Tx buffer
	Wire.write(wrAddr);           // Put slave register address in Tx buffer
	Wire.write(wrData);                 // Put data in Tx buffer
	Wire.endTransmission();           // Send the Tx buffer
}
