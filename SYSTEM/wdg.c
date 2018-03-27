#include "wdg.h"
#include "stm32f4xx.h"
#include "stm32f4xx_iwdg.h"

void wdg_activate(unsigned short num_of_ms) {
    uint8_t prescale_reg;
    uint8_t prescale_val;

    if (num_of_ms < 1) {
        num_of_ms = 1;
        prescale_reg = IWDG_Prescaler_32;
        prescale_val = 1;
    }
    else if (num_of_ms <= 4096) {
        prescale_reg = IWDG_Prescaler_32;
        prescale_val = 1;
    }
    else if (num_of_ms <= 8192) {
        prescale_reg = IWDG_Prescaler_64;
        prescale_val = 2;
    }
    else if (num_of_ms <= 16384) {
        prescale_reg = IWDG_Prescaler_128;
        prescale_val = 4;
    }
    else if (num_of_ms <= 32768) {
        prescale_reg = IWDG_Prescaler_256;
        prescale_val = 8;
    }
    else {
        num_of_ms = 32768;
        prescale_reg = IWDG_Prescaler_256;
        prescale_val = 8;
    }

    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    while (IWDG_GetFlagStatus(IWDG_FLAG_PVU));
    IWDG_SetPrescaler(prescale_reg);
    while (IWDG_GetFlagStatus(IWDG_FLAG_RVU));
    IWDG_SetReload(num_of_ms/prescale_val-1);
    IWDG_Enable();
}

void wdg_reactivate() {
    IWDG_ReloadCounter();
}
