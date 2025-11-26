#include "tof.h"

VL53L0X_Dev_t dev;
VL53L0X_RangingMeasurementData_t measure;
uint8_t data;
uint8_t VHV;
uint8_t Phase;
uint32_t refSpadCount;
uint8_t isAperture;
uint8_t dataReady = 0;

void VL53L0X_InitSensor() {

    // for(uint8_t a = 0; a < 255; a+=2) {
    //   if (HAL_I2C_IsDeviceReady(&hi2c1, a, 1, 10) == HAL_OK)
    //       printf("Device found at 0x%X\n", a);
    // }
    
    dev.I2cDevAddr = 0x52;
    dev.I2cHandle = &hi2c1;
    
    if(HAL_I2C_Mem_Read(&hi2c1, 0x52, 0xC0, 1, &data, 1, 100) == HAL_OK)
        printf("Read ID = 0x%X\n", data);
    else
        printf("Read failed\n");

    int init = VL53L0X_DataInit(&dev);
    if(init != VL53L0X_ERROR_NONE) printf("DataInit ERROR\n");
    int stat = VL53L0X_StaticInit(&dev);
    if(stat != VL53L0X_ERROR_NONE) printf("StaticInit ERROR\n");

    VL53L0X_PerformRefCalibration(&dev, &VHV, &Phase);

    VL53L0X_PerformRefSpadManagement(&dev, &refSpadCount, &isAperture);
    int set = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(&dev, 33000);
    if(set != VL53L0X_ERROR_NONE) printf("SetM ERROR\n");
    VL53L0X_SetDeviceMode(&dev, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING);
    int start = VL53L0X_StartMeasurement(&dev);
    if(start != VL53L0X_ERROR_NONE) printf("Start ERROR\n");

}

void VL53L0X_ReadDistance() {

  VL53L0X_GetMeasurementDataReady(&dev, &dataReady);

  if (dataReady) {
      VL53L0X_GetRangingMeasurementData(&dev, &measure);
      printf("Distance = %d mm\n", measure.RangeMilliMeter);
      VL53L0X_ClearInterruptMask(&dev, VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_NEW_SAMPLE_READY);
  }
  else {
      printf("No new measure\n");
  }
}
