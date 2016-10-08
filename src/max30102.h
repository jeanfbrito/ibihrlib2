#define MAX30100_I2C_ADR 0x57

// registers' addresses
#define INT_STATUS       0x00
#define INT_ENABLE       0x01
#define FIFO_WRITE_PTR   0x02
#define OVER_FLOW_CNT    0x03
#define FIFO_READ_PTR    0x04
#define FIFO_DATA_REG    0x05
#define MODE_CONFIG      0x06
#define SPO2_CONFIG      0x07
#define LED_CONFIG       0x09
#define TEMP_INTEGER     0x16
#define TEMP_FRACTION    0x17
#define REVISION_ID      0xFE
#define PART_ID          0xFF

// mode configuration bits
#define TEMP_EN          0x08
#define HR_ONLY          0x02
#define SPO2_EN          0x03

// SpO2 configuration bits
#define SPO2_HI_RES_EN   0x40

// interrupt enable bits
#define ENA_A_FULL       0x80
#define ENA_TEP_RDY      0x40
#define ENA_HR_RDY       0x20
#define ENA_SO2_RDY      0x10

// interrupt status bits
#define PWR_RDY          0x01

// sample rate control bits [samples per second]
#define SAMPLES_50       0x00
#define SAMPLES_100      0x04
#define SAMPLES_167      0x08
#define SAMPLES_200      0x0C
#define SAMPLES_400      0x10
#define SAMPLES_600      0x14
#define SAMPLES_800      0x18
#define SAMPLES_1000     0x1C

// LED pulse width control bits - pulse width [us]
#define PULSE_WIDTH_200  0x00 // 13-bit ADC resolution
#define PULSE_WIDTH_400  0x01 // 14-bit ADC resolution
#define PULSE_WIDTH_800  0x02 // 15-bit ADC resolution
#define PULSE_WIDTH_1600 0x03 // 16-bit ADC resolution

// LED current control bits [ma]
#define IR_CURRENT_0     0x00 // 0.0 mA
#define IR_CURRENT_44    0x01 // 4.4 mA
#define IR_CURRENT_76    0x02 // 7.6 mA
#define IR_CURRENT_110   0x03 // 11.0 mA
#define IR_CURRENT_142   0x04 // 14.2 mA
#define IR_CURRENT_174   0x05 // 17.4 mA
#define IR_CURRENT_208   0x06 // 20.8 mA
#define IR_CURRENT_240   0x07 // 24.0 mA
#define IR_CURRENT_271   0x08 // 27.1 mA
#define IR_CURRENT_306   0x09 // 30.6 mA
#define IR_CURRENT_338   0x0A // 33.8 mA
#define IR_CURRENT_370   0x0B // 37.0 mA
#define IR_CURRENT_402   0x0C // 40.2 mA
#define IR_CURRENT_436   0x0D // 43.6 mA
#define IR_CURRENT_468   0x0E // 46.8 mA
#define IR_CURRENT_500   0x0F // 50.0 mA

#define RED_CURRENT_0    0x00 // 0.0 mA
#define RED_CURRENT_44   0x10 // 4.4 mA
#define RED_CURRENT_76   0x20 // 7.6 mA
#define RED_CURRENT_110  0x30 // 11.0 mA
#define RED_CURRENT_142  0x40 // 14.2 mA
#define RED_CURRENT_174  0x50 // 17.4 mA
#define RED_CURRENT_208  0x60 // 20.8 mA
#define RED_CURRENT_240  0x70 // 24.0 mA
#define RED_CURRENT_271  0x80 // 27.1 mA
#define RED_CURRENT_306  0x90 // 30.6 mA
#define RED_CURRENT_338  0xA0 // 33.8 mA
#define RED_CURRENT_370  0xB0 // 37.0 mA
#define RED_CURRENT_402  0xC0 // 40.2 mA
#define RED_CURRENT_436  0xD0 // 43.6 mA
#define RED_CURRENT_468  0xE0 // 46.8 mA
#define RED_CURRENT_500  0xF0 // 50.0 mA

#include <Wire.h>

class MAX30100 {
public:
  uint16_t IR = 0;      // Last IR reflectance datapoint
  uint16_t RED = 0;     // Last Red reflectance datapoint

  MAX30100();
  void  reset(void);
  void  init(void);
  void  GetTemp(void);
  float  ReadTemp(void);
  void  readSensor(void);       // Updates the values
  uint8_t   GetStatus(void);  // Gets part ID

private:
  void 	SendToSensor (uint8_t wrAddr, uint8_t wrData);
  void  ReadFromSensor (uint8_t rAddr, uint8_t* rxBuff, uint8_t dataNum);
};