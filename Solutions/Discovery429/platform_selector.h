////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//  Copyright (c) Microsoft Corporation. All rights reserved.
//  Implementation for Discovery429 board (STM32F4): Copyright (c) Oberon microsystems, Inc.
//
//  Discovery429 specific definitions
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _PLATFORM_Discovery429_SELECTOR_H_
#define _PLATFORM_Discovery429_SELECTOR_H_ 1

/////////////////////////////////////////////////////////
//
// processor and features
//

#if defined(PLATFORM_ARM_Discovery429)
#define HAL_SYSTEM_NAME                     "Discovery429"

#define PLATFORM_ARM_STM32F4  // STM32F429 cpu
#define STM32F429_439xx       // enable F429 features

#define USB_ALLOW_CONFIGURATION_OVERRIDE  1


//
// processor and features
//
/////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////
//
// constants
//

// legal SYSCLK: 24, 30, 32, 36, 40, 42, 48, 54, 56, 60, 64, 72, 84, 96, 108, 120, 144, 168MHz
#define SYSTEM_CLOCK_HZ                 168000000  // 168MHz
#define SYSTEM_CYCLE_CLOCK_HZ           168000000  // 168MHz
#define SYSTEM_APB1_CLOCK_HZ            42000000   // 42MHz
#define SYSTEM_APB2_CLOCK_HZ            84000000   // 84MHz
#define SYSTEM_CRYSTAL_CLOCK_HZ         8000000    // 8MHz external clock
#define SUPPLY_VOLTAGE_MV               3000       // 3.0V supply

#define CLOCK_COMMON_FACTOR             1000000    // GCD(SYSTEM_CLOCK_HZ, 1M)

#define SLOW_CLOCKS_PER_SECOND          1000000    // 1MHz
#define SLOW_CLOCKS_TEN_MHZ_GCD         1000000    // GCD(SLOW_CLOCKS_PER_SECOND, 10M)
#define SLOW_CLOCKS_MILLISECOND_GCD     1000       // GCD(SLOW_CLOCKS_PER_SECOND, 1k)

#define FLASH_MEMORY_Base               0x08000000
#define FLASH_MEMORY_Size               0x00200000
#define SRAM1_MEMORY_Base               0x20000000 // region includes internal & external Ram
#define SRAM1_MEMORY_Size               0xC0000000

#define TXPROTECTRESISTOR               RESISTOR_DISABLED
#define RXPROTECTRESISTOR               RESISTOR_DISABLED
#define CTSPROTECTRESISTOR              RESISTOR_DISABLED
#define RTSPROTECTRESISTOR              RESISTOR_DISABLED

#define TOTAL_GPIO_PORT                 7 // PA - PG
#define INSTRUMENTATION_H_GPIO_PIN      GPIO_PIN_NONE

#define TOTAL_USART_PORT                6
#define USART_DEFAULT_PORT              COM1
#define USART_DEFAULT_BAUDRATE          115200

#define DEBUG_TEXT_PORT                 USB1
#define STDIO                           USB1
#define DEBUGGER_PORT                   USB1
#define MESSAGING_PORT                  USB1

#define TOTAL_USB_CONTROLLER            1
#define STM32F4_USB_HS                  1  // USB device on USB HS (PB12-15)
#define USB_MAX_QUEUES                  6  // 6 endpoints (EP0 + 5)

#define TOTAL_SOCK_PORT                 0


// System Timer Configuration
#define STM32F4_32B_TIMER 5
#define STM32F4_16B_TIMER 1


// Pin Configuration
#define STM32F4_ADC 1
#define STM32F4_AD_CHANNELS {4,5,6,7,10,11,12,13} // PA4,PA5,PA6,PA7,PC0,PC1,PC2,PC3

#define STM32F4_PWM_TIMER {2,3,4,9,9}
#define STM32F4_PWM_CHNL  {1,1,2,1,2}
#define STM32F4_PWM_PINS  {5,20,23,69,70} // PA5,PB4,PB7,PE5,PE6

#define STM32F4_SPI_SCLK_PINS {5,26,42,66,87} // PA5,PB10,PC10,PE2,PF7
#define STM32F4_SPI_MISO_PINS {6,34,43,69,88} // PA6,PC2, PC11,PE5,PF8
#define STM32F4_SPI_MOSI_PINS {7,35,44,70,89} // PA7,PC3, PC12,PE6,PF9

#define STM32F4_I2C_PORT     3
#define STM32F4_I2C_SCL_PIN  8  // PA8
#define STM32F4_I2C_SDA_PIN  41 // PC9

#define STM32F4_UART_RXD_PINS {10,3,27,43,50} // A10,A3,B11,C11,D2
#define STM32F4_UART_TXD_PINS { 9,2,26,42,44} // A9, A2,B10,C10,C12


//
// constants
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
//
// global functions
//

//
// global functions
//
/////////////////////////////////////////////////////////

#include <processor_selector.h>

#endif // PLATFORM_ARM_Discovery429
//
// drivers
/////////////////////////////////////////////////////////

#endif // _PLATFORM_Discovery429_SELECTOR_H_
