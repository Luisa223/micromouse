#include "rangeSensor.h"
#include <Libpic30.h>
#include "IOconfig.h"

#define ADDRESS_1 0x29
#define ADDRESS_2 0x30
#define ADDRESS_3 0x31
#define ADDRESS_4 0x32
#define ADDRESS_5 0x33


void changeSensorAdressesOnPowerUp()
{
    // Sensor 1 is left on 0x29
    SEN_SHDN1 = 1;
    SEN_SHDN3 = 1;
    SEN_SHDN4 = 1;
    SEN_SHDN5 = 1;
    
    
    VL6180X_write(0x012, ADDRESS_2);
    SEN_SHDN1 = 1;
    SEN_SHDN2 = 1;
    SEN_SHDN4 = 1;
    SEN_SHDN5 = 1;
    
    VL6180X_write(0x012, ADDRESS_3);
    SEN_SHDN1 = 1;
    SEN_SHDN2 = 1;
    SEN_SHDN3 = 1;
    SEN_SHDN5 = 1;
    VL6180X_write(0x012, ADDRESS_4);
    SEN_SHDN1 = 1;
    SEN_SHDN2 = 1;
    SEN_SHDN3 = 1;
    SEN_SHDN4 = 1;
    VL6180X_write(0x012, ADDRESS_5);
}

// Inititalisation necessary
void VL6180X_init(uint8_t address)
{
    
    I2CInit();
    
    uint8_t status = 0;
    
    do{
        status = VL6180X_read(SYSTEM__FRESH_OUT_OF_RESET);
        
    }while(status != 1);
     
    
    VL6180X_write(address, SYSTEM__FRESH_OUT_OF_RESET, 0x00);
    __delay32(10); 
   
    VL6180X_write(address, 0x0207, 0x01);
    
    
  
    VL6180X_write(address, 0x0208, 0x01);
    
    
    VL6180X_write(address, 0x0096, 0x00);
    
    VL6180X_write(address, 0x0097, 0xfd);
    
    VL6180X_write(address, 0x00e3, 0x00);
    VL6180X_write(address, 0x00e4, 0x04);
    
    VL6180X_write(address, 0x00e5, 0x02);
    
    VL6180X_write(address, 0x00e6, 0x01);
    
    VL6180X_write(address, 0x00e7, 0x03);
    
    VL6180X_write(address, 0x00f5, 0x02);
    
    VL6180X_write(address, 0x00d9, 0x05);
    VL6180X_write(address, 0x00db, 0xce);
    VL6180X_write(address, 0x00dc, 0x03);
    VL6180X_write(address, 0x00dd, 0xf8);
    VL6180X_write(address, 0x009f, 0x00);
    VL6180X_write(address, 0x00a3, 0x3c);
    
    
    
    VL6180X_write(address, 0x00b7, 0x00);
    VL6180X_write(address, 0x00bb, 0x03c);
    VL6180X_write(address, 0x00b2, 0x09);
    VL6180X_write(address, 0x00ca, 0x09);
    VL6180X_write(address, 0x0198, 0x01);
    VL6180X_write(address, 0x01b0, 0x17);
    VL6180X_write(address, 0x01ad, 0x00);
    VL6180X_write(address, 0x00ff, 0x05);
    VL6180X_write(address, 0x0100, 0x05);
    VL6180X_write(address, 0x0199, 0x05);
    VL6180X_write(address, 0x01a6, 0x1b);
    VL6180X_write(address, 0x01ac, 0x3e);
    VL6180X_write(address, 0x01a7, 0x1f);
    VL6180X_write(address, 0x0030, 0x00);
    
    
    
    //recommended : public registers
    VL6180X_write(address, SYSTEM__MODE_GPIO1, 0x10);	//Interrupt, Active Low
    VL6180X_write(address, READOUT__AVERAGING_SAMPLE_PERIOD, 0x30);
    VL6180X_write(address, SYSALS__ANALOGUE_GAIN, 0x46);
    VL6180X_write(address, SYSRANGE__VHV_REPEAT_RATE, 0x0f);
    VL6180X_write(address, SYSALS__INTEGRATION_PERIOD, 0x63);
    VL6180X_write(address, SYSRANGE__VHV_RECALIBRATE, 0x01);

    //optional : public registers
    VL6180X_write(address, SYSRANGE__INTERMEASUREMENT_PERIOD, 0x09);
    VL6180X_write(address, SYSALS__INTERMEASUREMENT_PERIOD, 0x31);
    VL6180X_write(address, SYSTEM__INTERRUPT_CONFIG_GPIO, RANGE_INT_NEW_SAMPLE_READY);
    VL6180X_write(address, SYSRANGE__RANGE_CHECK_ENABLES, 0x00);

}


// I2C functions are in i2c header defined
void VL6180X_write(uint8_t address, uint16_t index, uint8_t data)
{
    
    I2CIdle();
    I2CStart();

    // Device write address
    I2CIdle();
    I2CWrite(address);

    // Register address first byte
    I2CIdle();
    I2CWrite((index>>8) & 0xFF);

    // Register address second byte
    I2CIdle();
    I2CWrite(index & 0xFF);

    //Data byte
    I2CIdle();
    I2CWrite(data & 0xFF);


    // P
    I2CIdle();

    I2CStop();
   
	return;

}

uint8_t VL6180X_read(uint16_t index)
{
    
	uint8_t tempByte;

	I2CIdle();
	I2CStart();

    // Device write address --> address: 0x29 plus 0 for write indication
    I2CIdle();
    I2CWrite(0x52);

    // Register address first byte
    I2CIdle();
    I2CWrite(index>>8);

	// Register address second byte
	I2CIdle();
    I2CWrite(index & 0xFF);

	// R
    I2CIdle();
    I2CRestart();
    
	// Read operation
	I2CIdle();
    I2CWrite(0x53);

	//Read byte
	I2CIdle();
    
	tempByte =  I2CRead();
 
	return tempByte;

}



void startRangeSingleShot(uint16_t address)
{
    // check if error code is zero and last bit is set to 1 --> ready
    while(!(VL6180X_read(RESULT__RANGE_STATUS) & 0x01));
    
    // 0: single shot, 1: continuous
    VL6180X_write(address, SYSRANGE__START, 0x01);
    
}

void waitRangeCompleteSingle()
{
    char status;
    char range_status;
    
    status = VL6180X_read(RESULT__RANGE_STATUS);
    
    // 0x04: new sample ready event
    while(range_status != 0x04)
    {
        status  = VL6180X_read(RESULT__RANGE_STATUS);
        range_status = status & 0x07;
        __delay_ms(1); 
    }
        
}

uint8_t readRangeResultSingle(uint16_t address)
{
    // read range in mm
  uint8_t range = VL6180X_read(RESULT__RANGE_VAL);

  // clear interrupt
  VL6180X_write(address, SYSTEM__INTERRUPT_CLEAR, 0x07);

  return range;
}
