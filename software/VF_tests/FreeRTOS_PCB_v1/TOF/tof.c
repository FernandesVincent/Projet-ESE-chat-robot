#include "tof.h"
#include "main.h"
#include "stm32g4xx_hal.h"
#include "vl53l0x_api.h"
#include "vl53l0x_def.h"
#include "vl53l0x_platform.h"

VL53L0X_RangingMeasurementData_t measure;
uint8_t data;
uint8_t VHV;
uint8_t Phase;
uint32_t refSpadCount;
uint8_t isAperture;
uint8_t dataReady = 0;

// Une instance de VL53L0X_Dev_t par capteur
VL53L0X_Dev_t tof_dev[NUMBER_OF_TOFS] = {
    { .I2cDevAddr = TOF_DEFAULT_DEV_ADDR, .I2cHandle = &TOF_I2C },
    { .I2cDevAddr = TOF_DEFAULT_DEV_ADDR, .I2cHandle = &TOF_I2C },
    { .I2cDevAddr = TOF_DEFAULT_DEV_ADDR, .I2cHandle = &TOF_I2C },
    { .I2cDevAddr = TOF_DEFAULT_DEV_ADDR, .I2cHandle = &TOF_I2C },
    { .I2cDevAddr = TOF_DEFAULT_DEV_ADDR, .I2cHandle = &TOF_I2C },
    { .I2cDevAddr = TOF_DEFAULT_DEV_ADDR, .I2cHandle = &TOF_I2C }
};

// Pins XSHUT pour chaque capteur
TOF_PinPort xshut_pins[NUMBER_OF_TOFS] = {
    { .Pin = XSHUT0_Pin, .Port = XSHUT0_GPIO_Port },
    { .Pin = XSHUT1_Pin, .Port = XSHUT1_GPIO_Port },
    { .Pin = XSHUT2_Pin, .Port = XSHUT2_GPIO_Port },
    { .Pin = XSHUT3_Pin, .Port = XSHUT3_GPIO_Port },
    { .Pin = XSHUT4_Pin, .Port = XSHUT4_GPIO_Port },
    { .Pin = XSHUT5_Pin, .Port = XSHUT5_GPIO_Port }
};

// Pins GPIO (interrupt) pour chaque capteur
TOF_PinPort gpio_pins[NUMBER_OF_TOFS] = {
    { .Pin = INT0_Pin, .Port = INT0_GPIO_Port },
    { .Pin = INT1_Pin, .Port = INT1_GPIO_Port },
    { .Pin = INT2_Pin, .Port = INT2_GPIO_Port },
    { .Pin = INT3_Pin, .Port = INT3_GPIO_Port },
    { .Pin = INT4_Pin, .Port = INT4_GPIO_Port },
    { .Pin = INT5_Pin, .Port = INT5_GPIO_Port }
};

// Tableau des TOF avec adresse unique
TOF tof[NUMBER_OF_TOFS] = {
    {.dev = &tof_dev[0], .new_adress = 0x54, .xshut = &xshut_pins[0], .gpio = &gpio_pins[0]},
    {.dev = &tof_dev[1], .new_adress = 0x56, .xshut = &xshut_pins[1], .gpio = &gpio_pins[1]},
    {.dev = &tof_dev[2], .new_adress = 0x58, .xshut = &xshut_pins[2], .gpio = &gpio_pins[2]},
    {.dev = &tof_dev[3], .new_adress = 0x60, .xshut = &xshut_pins[3], .gpio = &gpio_pins[3]},
    {.dev = &tof_dev[4], .new_adress = 0x62, .xshut = &xshut_pins[4], .gpio = &gpio_pins[4]},
    {.dev = &tof_dev[5], .new_adress = 0x64, .xshut = &xshut_pins[5], .gpio = &gpio_pins[5]}
};

///////////////////////////////////////////////////////////////////////////////////

// Initialise un capteur individuel
void VL53L0X_InitSingleSensor(int index) {
    TOF *t = &tof[index];
    VL53L0X_Init(t);
    VL53L0X_Calibration(t);
    VL53L0X_Param(t, TIMING_BUDGET_US, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING);
    VL53L0X_StartMeasure(index);
}

// Change l'adresse I2C du capteur
void VL53L0X_ChangeDeviceAddress(TOF *t){
    if(VL53L0X_SetDeviceAddress(t->dev, t->new_adress) != VL53L0X_ERROR_NONE)
        printf("SetDeviceAddress ERROR\n");
    t->dev->I2cDevAddr = t->new_adress;
}

// Initialisation basique
void VL53L0X_Init(TOF *t){
    if(VL53L0X_DataInit(t->dev) != VL53L0X_ERROR_NONE)
        printf("DataInit ERROR\n");
    if(VL53L0X_StaticInit(t->dev) != VL53L0X_ERROR_NONE)
        printf("StaticInit ERROR\n");
}

// Paramètres
void VL53L0X_Param(TOF *t, uint32_t timing_budget_us, VL53L0X_DeviceModes mode){
    if(VL53L0X_SetMeasurementTimingBudgetMicroSeconds(t->dev, timing_budget_us) != VL53L0X_ERROR_NONE)
        printf("SetTimingBudget ERROR\n");
    if(VL53L0X_SetDeviceMode(t->dev, mode) != VL53L0X_ERROR_NONE)
        printf("SetDeviceMode ERROR\r\n");
}

// Calibration
void VL53L0X_Calibration(TOF *t){
    if(VL53L0X_PerformRefCalibration(t->dev, &VHV, &Phase) != VL53L0X_ERROR_NONE)
        printf("PerformCalibration ERROR\r\n");
    if(VL53L0X_PerformRefSpadManagement(t->dev, &refSpadCount, &isAperture) != VL53L0X_ERROR_NONE)
        printf("PerformRefSpadManagement ERROR\r\n");
}

// Démarre la mesure
void VL53L0X_StartMeasure(int index){
    TOF *t = &tof[index];
    if(VL53L0X_StartMeasurement(t->dev) != VL53L0X_ERROR_NONE)
        printf("Start ERROR\n");
}

///////////////////////////////////////////////////////////////////////////////////

// Initialisation de tous les capteurs correctement
void VL53L0X_InitAllSensor() {
    // 1. Éteindre tous les capteurs
    for(int i = 0; i < NUMBER_OF_TOFS; i++)
        HAL_GPIO_WritePin(tof[i].xshut->Port, tof[i].xshut->Pin, GPIO_PIN_RESET);

    HAL_Delay(100);

    // 2. Allumer et configurer chaque capteur un par un
    for(int i = 0; i < NUMBER_OF_TOFS; i++){
        HAL_GPIO_WritePin(tof[i].xshut->Port, tof[i].xshut->Pin, GPIO_PIN_SET);
        HAL_Delay(100); // temps pour que le capteur démarre
        VL53L0X_ChangeDeviceAddress(&tof[i]);
        VL53L0X_InitSingleSensor(i);
    }
}

///////////////////////////////////////////////////////////////////////////////////

// Lecture de la distance
int VL53L0X_ReadDistance(int index) {
    TOF *t = &tof[index];
    uint8_t dataReady = 0;
    if(VL53L0X_GetMeasurementDataReady(t->dev, &dataReady) != VL53L0X_ERROR_NONE){
        printf("Error data ready\n");
    }
    if (dataReady) {
        VL53L0X_RangingMeasurementData_t measure;
        if(VL53L0X_GetRangingMeasurementData(t->dev, &measure) != VL53L0X_ERROR_NONE){
            printf("Error reading measurement\n");
        }
        printf("TOF %d Distance = %d mm\n", index, measure.RangeMilliMeter);
        VL53L0X_ClearInterruptMask(t->dev, 0);
        return measure.RangeMilliMeter;
    }
    else {
        return ERROR;
    }
}

// Comparaison seuil
int VL53L0X_CompareThreshold(int index, int distance_mm, int threshold_mm){
    return (distance_mm > threshold_mm) ? 1 : 0;
}

// Seuil booléen
bool VL53L0X_IsAboveThreshold(int index){
    int distance = VL53L0X_ReadDistance(index);
    int result = VL53L0X_CompareThreshold(index, distance, 200);
    if(result == 1){
        printf("TOF %d Above Threshold !\r\n", index);
        return true;
    } else {
        printf("TOF %d Below Threshold !\r\n", index);
        return false;
    }
}
