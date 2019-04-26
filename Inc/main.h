/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define MVS3_Pin GPIO_PIN_2
#define MVS3_GPIO_Port GPIOE
#define MVS3_EXTI_IRQn EXTI2_IRQn
#define MVS2_Pin GPIO_PIN_3
#define MVS2_GPIO_Port GPIOE
#define MVS2_EXTI_IRQn EXTI3_IRQn
#define MVS1_Pin GPIO_PIN_4
#define MVS1_GPIO_Port GPIOE
#define MVS1_EXTI_IRQn EXTI4_IRQn
#define RAM_CS_Pin GPIO_PIN_5
#define RAM_CS_GPIO_Port GPIOE
#define ESC_Pin GPIO_PIN_6
#define ESC_GPIO_Port GPIOE
#define FLED_BLUE_Pin GPIO_PIN_13
#define FLED_BLUE_GPIO_Port GPIOC
#define FLED_GREEN_Pin GPIO_PIN_14
#define FLED_GREEN_GPIO_Port GPIOC
#define FLED_RED_Pin GPIO_PIN_15
#define FLED_RED_GPIO_Port GPIOC
#define TOUCH1_Pin GPIO_PIN_0
#define TOUCH1_GPIO_Port GPIOC
#define TOUCH2_Pin GPIO_PIN_1
#define TOUCH2_GPIO_Port GPIOC
#define M1_CHA_Pin GPIO_PIN_0
#define M1_CHA_GPIO_Port GPIOA
#define M1_CHB_Pin GPIO_PIN_1
#define M1_CHB_GPIO_Port GPIOA
#define RAM_HOLD_Pin GPIO_PIN_4
#define RAM_HOLD_GPIO_Port GPIOA
#define E1_Pin GPIO_PIN_0
#define E1_GPIO_Port GPIOB
#define E2_Pin GPIO_PIN_1
#define E2_GPIO_Port GPIOB
#define M1_INA_Pin GPIO_PIN_2
#define M1_INA_GPIO_Port GPIOB
#define M1_EN_Pin GPIO_PIN_7
#define M1_EN_GPIO_Port GPIOE
#define M1_INB_Pin GPIO_PIN_8
#define M1_INB_GPIO_Port GPIOE
#define M1_PWM_Pin GPIO_PIN_9
#define M1_PWM_GPIO_Port GPIOE
#define M1_CS_Pin GPIO_PIN_10
#define M1_CS_GPIO_Port GPIOE
#define M2_PWM_Pin GPIO_PIN_11
#define M2_PWM_GPIO_Port GPIOE
#define M2_EN_Pin GPIO_PIN_12
#define M2_EN_GPIO_Port GPIOE
#define M2_INA_Pin GPIO_PIN_13
#define M2_INA_GPIO_Port GPIOE
#define M2_INB_Pin GPIO_PIN_14
#define M2_INB_GPIO_Port GPIOE
#define M2_CS_Pin GPIO_PIN_15
#define M2_CS_GPIO_Port GPIOE
#define LCD_CS_Pin GPIO_PIN_14
#define LCD_CS_GPIO_Port GPIOB
#define LCD_RST_Pin GPIO_PIN_15
#define LCD_RST_GPIO_Port GPIOB
#define SW1_Pin GPIO_PIN_8
#define SW1_GPIO_Port GPIOD
#define SW2_Pin GPIO_PIN_9
#define SW2_GPIO_Port GPIOD
#define SW3_Pin GPIO_PIN_10
#define SW3_GPIO_Port GPIOD
#define ST_MOTOR_CH4_Pin GPIO_PIN_12
#define ST_MOTOR_CH4_GPIO_Port GPIOD
#define ST_MOTOR_CH3_Pin GPIO_PIN_13
#define ST_MOTOR_CH3_GPIO_Port GPIOD
#define ST_MOTOR_CH2_Pin GPIO_PIN_14
#define ST_MOTOR_CH2_GPIO_Port GPIOD
#define ST_MOTOR_CH1_Pin GPIO_PIN_15
#define ST_MOTOR_CH1_GPIO_Port GPIOD
#define M2_CHA_Pin GPIO_PIN_6
#define M2_CHA_GPIO_Port GPIOC
#define M2_CHB_Pin GPIO_PIN_7
#define M2_CHB_GPIO_Port GPIOC
#define CL_Pin GPIO_PIN_8
#define CL_GPIO_Port GPIOC
#define DP1_Pin GPIO_PIN_10
#define DP1_GPIO_Port GPIOA
#define DP2_Pin GPIO_PIN_11
#define DP2_GPIO_Port GPIOA
#define LED_RST_Pin GPIO_PIN_12
#define LED_RST_GPIO_Port GPIOA
#define BUZZER_Pin GPIO_PIN_15
#define BUZZER_GPIO_Port GPIOA
#define MVSIG1_Pin GPIO_PIN_10
#define MVSIG1_GPIO_Port GPIOC
#define MVSIG2_Pin GPIO_PIN_11
#define MVSIG2_GPIO_Port GPIOC
#define MVSIG3_Pin GPIO_PIN_12
#define MVSIG3_GPIO_Port GPIOC
#define PING1_Pin GPIO_PIN_0
#define PING1_GPIO_Port GPIOD
#define PING2_Pin GPIO_PIN_1
#define PING2_GPIO_Port GPIOD
#define PING3_Pin GPIO_PIN_2
#define PING3_GPIO_Port GPIOD
#define PING4_Pin GPIO_PIN_3
#define PING4_GPIO_Port GPIOD
#define PING5_Pin GPIO_PIN_4
#define PING5_GPIO_Port GPIOD
#define PING6_Pin GPIO_PIN_5
#define PING6_GPIO_Port GPIOD
#define RANGE_Pin GPIO_PIN_6
#define RANGE_GPIO_Port GPIOD
#define CK_Pin GPIO_PIN_7
#define CK_GPIO_Port GPIOD
#define GATE_Pin GPIO_PIN_3
#define GATE_GPIO_Port GPIOB
#define DOUT_Pin GPIO_PIN_4
#define DOUT_GPIO_Port GPIOB
#define FET1_Pin GPIO_PIN_8
#define FET1_GPIO_Port GPIOB
#define FET2_Pin GPIO_PIN_9
#define FET2_GPIO_Port GPIOB

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
