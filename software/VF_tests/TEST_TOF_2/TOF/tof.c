#include "tof.h"
#include "vl53l0x_api.h"
#include "vl53l0x_def.h"
#include "vl53l0x_platform.h"

VL53L0X_Dev_t dev;
VL53L0X_RangingMeasurementData_t measure;
uint8_t data;
uint8_t VHV;
uint8_t Phase;
uint32_t refSpadCount;
uint8_t isAperture;
uint8_t dataReady = 0;

VL53L0X_Dev_t TOF_dev ={
    .I2cDevAddr = TOF_DEFAULT_DEV_ADDR,
    .I2cHandle = &TOF_I2C
};

TOF_PinPort xshut_pins[NUMBER_OF_TOFS] = {
    { .Pin = 0, .Port = 0 },
    { .Pin = 0, .Port = 0 },
    { .Pin = 0, .Port = 0 },
    { .Pin = 0, .Port = 0 },
    { .Pin = 0, .Port = 0 },
    { .Pin = 0, .Port = 0 }
};

TOF_PinPort gpio_pins[NUMBER_OF_TOFS] = {
    { .Pin = 0, .Port = 0 },
    { .Pin = 0, .Port = 0 },
    { .Pin = 0, .Port = 0 },
    { .Pin = 0, .Port = 0 },
    { .Pin = 0, .Port = 0 },
    { .Pin = 0, .Port = 0 }
};

TOF tof[NUMBER_OF_TOFS] = {
    {.dev = &TOF_dev, .new_adress = 0x54, .isenable = false, .isready = false, .xshut = &xshut_pins[0], .gpio = &gpio_pins[0]},
    {.dev = &TOF_dev, .new_adress = 0x56, .isenable = false, .isready = false, .xshut = &xshut_pins[1], .gpio = &gpio_pins[1]},
    {.dev = &TOF_dev, .new_adress = 0x58, .isenable = false, .isready = false, .xshut = &xshut_pins[2], .gpio = &gpio_pins[2]},
    {.dev = &TOF_dev, .new_adress = 0x60, .isenable = false, .isready = false, .xshut = &xshut_pins[3], .gpio = &gpio_pins[3]},
    {.dev = &TOF_dev, .new_adress = 0x62, .isenable = false, .isready = false, .xshut = &xshut_pins[4], .gpio = &gpio_pins[4]},
    {.dev = &TOF_dev, .new_adress = 0x64, .isenable = false, .isready = false, .xshut = &xshut_pins[5], .gpio = &gpio_pins[5]}
};



///////////////////////////////////////////////////////////////////////////////////

void VL53L0X_InitSingleSensor(int index) {
    TOF *t = &tof[index];
    // if(HAL_I2C_Mem_Read(t->dev->I2cHandle, t->dev->I2cDevAddr, 0xC0, 1, &data, 1, 100) == HAL_OK){
    //     printf("Read ID = 0x%X\n", data);
    // }
    // else{
    //     printf("Read failed\n");
    // }
    VL53L0X_EnableSingleSensor(index);
    HAL_Delay(30);
    VL53L0X_ChangeDeviceAddress(t);
    VL53L0X_Init(t);
    VL53L0X_Calibration(t);
    VL53L0X_Param(t, 33000,VL53L0X_DEVICEMODE_CONTINUOUS_RANGING);
    // VL53L0X_Start(t);
}

void VL53L0X_ChangeDeviceAddress(TOF *t){
    if(VL53L0X_SetDeviceAddress(t->dev, t->new_adress) != VL53L0X_ERROR_NONE)
        printf("SetDeviceAddress ERROR\n");
    t->dev->I2cDevAddr = t->new_adress;
}
void VL53L0X_Init(TOF *t){
    if(VL53L0X_DataInit(t->dev) != VL53L0X_ERROR_NONE) 
        printf("DataInit ERROR\n");
    if(VL53L0X_StaticInit(t->dev) != VL53L0X_ERROR_NONE) 
        printf("StaticInit ERROR\n");
}
void VL53L0X_Param(TOF *t, uint32_t timing_budget_us, VL53L0X_DeviceModes mode){
    if(VL53L0X_SetMeasurementTimingBudgetMicroSeconds(t->dev, timing_budget_us)!= VL53L0X_ERROR_NONE) 
        printf("SetTimingBudget ERROR\n");
    if(VL53L0X_SetDeviceMode(t->dev, mode ) != VL53L0X_ERROR_NONE) 
        printf("SetDeviceMode ERROR\r\n");
}

void VL53L0X_Calibration(TOF *t){
    if(VL53L0X_PerformRefCalibration(t->dev, &VHV, &Phase) != VL53L0X_ERROR_NONE)
        printf("PerformCalibration ERROR");
    if(VL53L0X_PerformRefSpadManagement(t->dev, &refSpadCount, &isAperture) != VL53L0X_ERROR_NONE)
        printf("PerformRefSpadManagement ERROR");
}

void VL53L0X_StartMeasure(TOF *t){
    if(VL53L0X_StartMeasurement(t->dev) != VL53L0X_ERROR_NONE) 
        printf("Start ERROR\n");
}

void VL53L0X_EnableSingleSensor(int index){
    TOF *t = &tof[index];
    for (int i = 0; i < NUMBER_OF_TOFS; i++) {
        TOF *tmp = &tof[i];
        HAL_GPIO_WritePin(tmp->xshut->Port, tmp->xshut->Pin, GPIO_PIN_RESET);
    }
    HAL_GPIO_WritePin(t->xshut->Port, t->xshut->Pin, GPIO_PIN_SET);
}

void VL53L0X_EnableAllSensors(){
    for (int i = 0; i < NUMBER_OF_TOFS; i++) {
        TOF *tmp = &tof[i];
        HAL_GPIO_WritePin(tmp->xshut->Port, tmp->xshut->Pin, GPIO_PIN_SET);
    }
}

void VL53L0X_InitAllSensor(){
    for(int i = 0; i<NUMBER_OF_TOFS; i++){
        VL53L0X_InitSingleSensor(i);
    }
    VL53L0X_EnableAllSensors();
}

///////////////////////////////////////////////////////////////////////////////////
int VL53L0X_ReadDistance(int index) {
    TOF *t = &tof[index];
    int distance;
    VL53L0X_GetMeasurementDataReady(t->dev, &dataReady);
    if (dataReady) {
        VL53L0X_GetRangingMeasurementData(t->dev, &measure);
        printf("Distance = %d mm\n", measure.RangeMilliMeter);
        distance = measure.RangeMilliMeter;
        VL53L0X_ClearInterruptMask(t->dev, VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_NEW_SAMPLE_READY);
        return distance;
    }
    else {
        printf("No new measure\n");
        return ERROR;
    }
}

int VL53L0X_CompareThreshold(int index, int distance, int threshold){
    if(distance > threshold ){
        return 1;
    }
    else{
        return 0;
    }
}