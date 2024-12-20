/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
uint8_t us100_buffer[2] = {0};
volatile uint16_t distance;
volatile uint8_t us100_Rx_flag = 0;
uint8_t cmd_dist = 0x55;
uint8_t msg_buffer[64] = {0};
int rpm_tick_count = 0;
uint8_t wall_clock_hr_update_flag = 0;
uint8_t rx_buffer[3]; // Buffer to store received characters
uint8_t rx_index = 0;              // Current index in the buffer
uint8_t rx_byte;                   // Temporary variable to store received byte
uint8_t txd_msg_buffer[64] = {0};
float rpm = 0;
int rpm_p = 0;
int new_hour = 0;
int halftime = 0;
uint8_t percent_pwm = 0;
float percent_pwm_p = 0;
uint8_t zone = 0;
bool done_setting = false;
int man_mode;
uint8_t DIG_A;
uint8_t DIG_B;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim5;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart6;

/* USER CODE BEGIN PV */
volatile uint8_t clock_hours = 0;
volatile uint8_t clock_mins = 0;
volatile uint8_t clock_secs = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART6_UART_Init(void);
static void MX_TIM5_Init(void);
/* USER CODE BEGIN PFP */
static void ADC_Select_CH(int CH);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t rcv_intpt_flag = 0;
void ADC_Select_CH(int CH)
{
ADC_ChannelConfTypeDef sConfig = {0};
sConfig.Channel = ADC_CHANNEL_4;
sConfig.Rank = 1;
if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
	Error_Handler();
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  int mode = 0;
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_USART6_UART_Init();
  MX_TIM5_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Init(&htim3);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
  TIM3->PSC = 15;
  TIM3->ARR = 2000;
  TIM3->CCR1 = 0; //fwd
  TIM3->CCR3 = 0; //bckwd
  HAL_TIM_Base_Start(&htim2);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  TIM2->PSC = 16-1;
  TIM2->ARR = 20000-1;
  TIM2->CCR1 = 500;
  int rpm_tick_start = 0;
  int timer_start = 1;
  int on_off = 0;
  int input_array[8];
  uint8_t clock_inputs[9];
  HAL_GPIO_WritePin(GPIOA, LD2_Pin, GPIO_PIN_RESET);
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    //user inputs for pipeplines 
	  if (mode == 0){

		  HAL_UART_Transmit(&huart6, "\r\n",strlen("\r\n"), 1000);
			HAL_UART_Transmit(&huart6, "***SETUP MODE*** \r\n",strlen("***SETUP MODE*** \r\n"), 1000);
			HAL_UART_Transmit(&huart6, "ENTER SETUP PARAMETERS \r\n",strlen("ENTER SETUP PARAMETERS \r\n"), 1000);

			HAL_UART_Transmit(&huart6, "FIRST PIPELINE CHOICE FOR CONNECTION:",strlen("FIRST PIPELINE CHOICE FOR CONNECTION:"), 1000);
			rcv_intpt_flag = 00;
			HAL_UART_Receive_IT(&huart6, &rx_byte, 1);
			while(rcv_intpt_flag == (00)){}
			input_array[0] = atoi((char*)rx_buffer);
			HAL_UART_Transmit(&huart6, "\r\n",strlen("\r\n"), 1000);

			HAL_UART_Transmit(&huart6, "FIRST PIPELINE CHOICE FOR MOTOR PWM:",strlen("FIRST PIPELINE CHOICE FOR MOTOR PWM:"), 1000);
			rcv_intpt_flag = 00;
			HAL_UART_Receive_IT(&huart6, &rx_byte, 1);
			while(rcv_intpt_flag == (00)){}
			input_array[1] = atoi((char*)rx_buffer);
			HAL_UART_Transmit(&huart6, "\r\n",strlen("\r\n"), 1000);

			HAL_UART_Transmit(&huart6, "SECOND ZONE CHOICE FOR CONNECTION:",strlen("SECOND ZONE CHOICE FOR CONNECTION:"), 1000);
			rcv_intpt_flag = 00;
			HAL_UART_Receive_IT(&huart6, &rx_byte, 1);
			while(rcv_intpt_flag == (00)){}
			input_array[2] = atoi((char*)rx_buffer);
			HAL_UART_Transmit(&huart6, "\r\n",strlen("\r\n"), 1000);

			HAL_UART_Transmit(&huart6, "SECOND ZONE CHOICE FOR MOTOR PWM:",strlen("SECOND ZONE CHOICE FOR MOTOR PWM:"), 1000);
			rcv_intpt_flag = 00;
			HAL_UART_Receive_IT(&huart6, &rx_byte, 1);
			while(rcv_intpt_flag == (00)){}
			input_array[3] = atoi((char*)rx_buffer);
			HAL_UART_Transmit(&huart6, "\r\n",strlen("\r\n"), 1000);

			HAL_UART_Transmit(&huart6, "THIRD ZONE CHOICE FOR CONNECTION:",strlen("THIRD ZONE CHOICE FOR CONNECTION:"), 1000);
			rcv_intpt_flag = 00;
			HAL_UART_Receive_IT(&huart6, &rx_byte, 1);
			while(rcv_intpt_flag == (00)){}
			input_array[4] = atoi((char*)rx_buffer);
			HAL_UART_Transmit(&huart6, "\r\n",strlen("\r\n"), 1000);

			HAL_UART_Transmit(&huart6, "THIRD ZONE CHOICE FOR MOTOR PWM:",strlen("THIRD ZONE CHOICE FOR MOTOR PWM:"), 1000);
			rcv_intpt_flag = 00;
			HAL_UART_Receive_IT(&huart6, &rx_byte, 1);
			while(rcv_intpt_flag == (00)){}
			input_array[5] = atoi((char*)rx_buffer);
			HAL_UART_Transmit(&huart6, "\r\n",strlen("\r\n"), 1000);

			HAL_UART_Transmit(&huart6, "FOURTH ZONE CHOICE FOR CONNECTION:",strlen("FOURTH ZONE CHOICE FOR CONNECTION:"), 1000);
			rcv_intpt_flag = 00;
			HAL_UART_Receive_IT(&huart6, &rx_byte, 1);
			while(rcv_intpt_flag == (00)){}
			input_array[6] = atoi((char*)rx_buffer);
			HAL_UART_Transmit(&huart6, "\r\n",strlen("\r\n"), 1000);

			HAL_UART_Transmit(&huart6, "FOURTH ZONE CHOICE FOR MOTOR PWM:",strlen("FOURTH ZONE CHOICE FOR MOTOR PWM:"), 1000);
			rcv_intpt_flag = 00;
			HAL_UART_Receive_IT(&huart6, &rx_byte, 1);
			while(rcv_intpt_flag == (00)){}
			input_array[7] = atoi((char*)rx_buffer);
			HAL_UART_Transmit(&huart6, "\r\n",strlen("\r\n"), 1000);

      //user inputs for wall clock settings

      HAL_UART_Transmit(&huart6, "CURRENT WALL CLOCK TIME:",strlen("CURRENT WALL CLOCK TIME:"), 1000);
			rcv_intpt_flag = 00;
			HAL_UART_Receive_IT(&huart6, &rx_byte, 1);
			while(rcv_intpt_flag == (00)){}
      clock_inputs[0] = atoi((char*)rx_buffer);
			HAL_UART_Transmit(&huart6, "\r\n",strlen("\r\n"), 1000);

      HAL_UART_Transmit(&huart6, "INLET WALL CLOCK START TIME:",strlen("INLET WALL CLOCK START TIME:"), 1000);
			rcv_intpt_flag = 00;
			HAL_UART_Receive_IT(&huart6, &rx_byte, 1);
			while(rcv_intpt_flag == (00)){}
      clock_inputs[1] = atoi((char*)rx_buffer);
			HAL_UART_Transmit(&huart6, "\r\n",strlen("\r\n"), 1000);

      HAL_UART_Transmit(&huart6, "INLET WALL CLOCK STOP TIME:",strlen("INLET WALL CLOCK STOP TIME:"), 1000);
			rcv_intpt_flag = 00;
			HAL_UART_Receive_IT(&huart6, &rx_byte, 1);
			while(rcv_intpt_flag == (00)){}
      clock_inputs[2] = atoi((char*)rx_buffer);
			HAL_UART_Transmit(&huart6, "\r\n",strlen("\r\n"), 1000);

      HAL_UART_Transmit(&huart6, "FIRST ZONE CHOICE WALL CLOCK START TIME:",strlen("FIRST ZONE CHOICE WALL CLOCK START TIME:"), 1000);
			rcv_intpt_flag = 00;
			HAL_UART_Receive_IT(&huart6, &rx_byte, 1);
			while(rcv_intpt_flag == (00)){}
      clock_inputs[3] = atoi((char*)rx_buffer);
			HAL_UART_Transmit(&huart6, "\r\n",strlen("\r\n"), 1000);

      HAL_UART_Transmit(&huart6, "FIRST ZONE CHOICE WALL CLOCK STOP TIME:",strlen("FIRST ZONE CHOICE WALL CLOCK STOP TIME:"), 1000);
			rcv_intpt_flag = 00;
			HAL_UART_Receive_IT(&huart6, &rx_byte, 1);
			while(rcv_intpt_flag == (00)){}
      clock_inputs[4] = atoi((char*)rx_buffer);
			HAL_UART_Transmit(&huart6, "\r\n",strlen("\r\n"), 1000);
      
      HAL_UART_Transmit(&huart6, "SECOND ZONE CHOICE WALL CLOCK START TIME:",strlen("SECOND ZONE CHOICE WALL CLOCK START TIME:"), 1000);
			rcv_intpt_flag = 00;
			HAL_UART_Receive_IT(&huart6, &rx_byte, 1);
			while(rcv_intpt_flag == (00)){}
      clock_inputs[5] = atoi((char*)rx_buffer);
			HAL_UART_Transmit(&huart6, "\r\n",strlen("\r\n"), 1000);

      HAL_UART_Transmit(&huart6, "SECOND ZONE CHOICE WALL CLOCK STOP TIME:",strlen("SECOND ZONE CHOICE WALL CLOCK STOP TIME:"), 1000);
			rcv_intpt_flag = 00;
			HAL_UART_Receive_IT(&huart6, &rx_byte, 1);
			while(rcv_intpt_flag == (00)){}
      clock_inputs[6] = atoi((char*)rx_buffer);
			HAL_UART_Transmit(&huart6, "\r\n",strlen("\r\n"), 1000);

      HAL_UART_Transmit(&huart6, "THIRD ZONE CHOICE WALL CLOCK START TIME:",strlen("THIRD ZONE CHOICE WALL CLOCK START TIME:"), 1000);
			rcv_intpt_flag = 00;
			HAL_UART_Receive_IT(&huart6, &rx_byte, 1);
			while(rcv_intpt_flag == (00)){}
      clock_inputs[7] = atoi((char*)rx_buffer);
			HAL_UART_Transmit(&huart6, "\r\n",strlen("\r\n"), 1000);

      HAL_UART_Transmit(&huart6, "THIRD ZONE CHOICE WALL CLOCK STOP TIME:",strlen("THIRD ZONE CHOICE WALL CLOCK STOP TIME:"), 1000);
			rcv_intpt_flag = 00;
			HAL_UART_Receive_IT(&huart6, &rx_byte, 1);
			while(rcv_intpt_flag == (00)){}
      clock_inputs[8] = atoi((char*)rx_buffer);
      /*
			HAL_UART_Transmit(&huart6, "\r\n",strlen("\r\n"), 1000);
			HAL_UART_Transmit(&huart6, "***SETUP SUMMARY*** \r\n",strlen("***SETUP SUMMARY*** \r\n"), 1000);
			sprintf((char*) txd_msg_buffer, "CURRENT WALL CLOCK HOUR: %d \r\n", clock_inputs[0]);
			HAL_UART_Transmit(&huart6, txd_msg_buffer,strlen((char*) txd_msg_buffer), 1000);
			sprintf((char*) txd_msg_buffer, "INLET WALL CLOCK START TIME: %d | INLET WALL CLOCK END TIME: %d \r\n", clock_inputs[1], clock_inputs[2]);
			HAL_UART_Transmit(&huart6, txd_msg_buffer,strlen((char*) txd_msg_buffer), 1000);
			sprintf((char*) txd_msg_buffer, "FIRST ZONE WALL CLOCK START TIME: %d | FIRST ZONE WALL CLOCK END TIME: %d \r\n", clock_inputs[3], clock_inputs[4]);
			HAL_UART_Transmit(&huart6, txd_msg_buffer,strlen((char*) txd_msg_buffer), 1000);
			sprintf((char*) txd_msg_buffer, "SECOND ZONE WALL CLOCK START TIME: %d | SECOND ZONE WALL CLOCK END TIME: %d \r\n", clock_inputs[5], clock_inputs[6]);
			HAL_UART_Transmit(&huart6, txd_msg_buffer,strlen((char*) txd_msg_buffer), 1000);
			sprintf((char*) txd_msg_buffer, "THIRD ZONE WALL CLOCK START TIME: %d | THIRD ZONE WALL CLOCK END TIME: %d \r\n", clock_inputs[7], clock_inputs[8]);
			HAL_UART_Transmit(&huart6, txd_msg_buffer,strlen((char*) txd_msg_buffer), 1000);

			sprintf((char*) txd_msg_buffer, "FIRST PIPELINE LOCATION: %d | FIRST PIPELINE MOTOR PWM: %d \r\n", input_array[0], input_array[1]);
			HAL_UART_Transmit(&huart6, txd_msg_buffer,strlen((char*) txd_msg_buffer), 1000);
			sprintf((char*) txd_msg_buffer, "SECOND PIPELINE LOCATION: %d | SECOND PIPELINE MOTOR PWM: %d \r\n", input_array[2], input_array[3]);
			HAL_UART_Transmit(&huart6, txd_msg_buffer,strlen((char*) txd_msg_buffer), 1000);
			sprintf((char*) txd_msg_buffer, "THIRD PIPELINE LOCATION: %d | THIRD PIPELINE MOTOR PWM: %d \r\n", input_array[4], input_array[5]);
			HAL_UART_Transmit(&huart6, txd_msg_buffer,strlen((char*) txd_msg_buffer), 1000);
			sprintf((char*) txd_msg_buffer, "FOURTH PIPELINE LOCATION: %d | FOURTH PIPELINE MOTOR PWM: %d \r\n", input_array[6], input_array[7]);
			HAL_UART_Transmit(&huart6, txd_msg_buffer,strlen((char*) txd_msg_buffer), 1000);
*/
			HAL_UART_Transmit(&huart6, "\r\n",strlen("\r\n"), 1000);
			HAL_UART_Transmit(&huart6, "SETUP IS DONE. PUSH BLUE BUTTON TO ENTER RUN MODE \r\n",strlen("SETUP IS DONE. PUSH BLUE BUTTON TO ENTER RUN MODE \r\n"), 1000);
			mode = 1;
	  }
	  if(mode == 1){
		uint32_t last_toggle_time = HAL_GetTick();
		while(HAL_GetTick() - last_toggle_time <= 500){
      //stop blinking led if pushbutton is pressed
			if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET) {
				mode = 2;
				break;
			}
		}
    //blink LD2
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		last_toggle_time = HAL_GetTick();
	  }
    //enter and setup run mode
	  if(mode == 2){
			HAL_GPIO_WritePin(GPIOA, LD2_Pin, GPIO_PIN_SET);
			HAL_UART_Transmit(&huart6, "***RUN MODE***\r\n",strlen("***RUN MODE***\r\n"), 1000);
			// start up wall clock
			HAL_TIM_Base_Start_IT(&htim5);
		    clock_hours = clock_inputs[0]; // clock current time
		    clock_mins = 0;
		    clock_secs = 0;
			mode = 3;
    }

	if ((man_mode) && (mode != 4) && (mode != 5)){
		ADC_Select_CH(4);
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 1000);
		uint8_t ADC_CH4 = HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
		if (input_array[0] == 0){
			TIM3->CCR3 = ADC_CH4*7;
		}
		else{
			TIM3->CCR1 = ADC_CH4*7;
		}
		percent_pwm_p = ADC_CH4;
		percent_pwm_p = percent_pwm_p /255;
		percent_pwm = percent_pwm_p * 100;
	}

    //run mode functionality
    if(mode == 3){
      
      //when zone 1 time equal current clock time
      if((done_setting == false) && (clock_inputs[1] == clock_hours)){
        Set_Pipe(input_array[0]);
        if(input_array[1] == 0){
          man_mode = 1;
        }
        else{
        	man_mode = 0;
        	Set_Motor(input_array[0], input_array[1]);
        }
        //Hourly_Report();
        done_setting = true;
      }
      //when zone 1 stop time, stop motor
      if(done_setting == true && clock_inputs[2] == clock_hours){
        TIM3->CCR1 = 0;
        TIM3->CCR3 = 0;
        percent_pwm = 0;
        man_mode = 0;
        done_setting = false;
      }
      //when zone 2 start time
      if(done_setting == false && clock_inputs[3] == clock_hours){
        Set_Pipe(input_array[2]);
        if(input_array[3] == 0){
		  man_mode = 1;
		}
		else{
			man_mode = 0;
			Set_Motor(input_array[2], input_array[3]);
		}
        done_setting = true;
      }
      //when zone 2 stop time
      if(done_setting == true && clock_inputs[4] == clock_hours){
        TIM3->CCR1 = 0;
        TIM3->CCR3 = 0;
        percent_pwm = 0;
        done_setting = false;
        man_mode = 0;
      }
      //when zone 3 start time
      if(done_setting == false && clock_inputs[5] == clock_hours){
        Set_Pipe(input_array[4]);
        if(input_array[5] == 0){
		  man_mode = 1;
		}
		else{
			man_mode = 0;
			Set_Motor(input_array[4], input_array[5]);
		}
        done_setting = true;
      }
      //when zone 3 stop time
      if(done_setting == true && clock_inputs[6] == clock_hours){
        TIM3->CCR1 = 0;
        TIM3->CCR3 = 0;
        percent_pwm = 0;
        done_setting = false;
        man_mode = 0;
      }
      //when zone 4 start time
      if(done_setting == false && clock_inputs[7] == clock_hours){
        Set_Pipe(input_array[6]);
        if(input_array[7] == 0){
		  man_mode = 1;
		}
		else{
			man_mode = 0;
			Set_Motor(input_array[6], input_array[7]);
		}
        done_setting = true;
      }
      //when zone 4 stop time
      if(done_setting == true && clock_inputs[8] == clock_hours){
        TIM3->CCR1 = 0;
        TIM3->CCR3 = 0;
        percent_pwm = 0;
        done_setting = false;
        man_mode = 0;
      }

    }

    if ((halftime) && (mode != 4) && (mode != 5)){
    	us100_Rx_flag = 0;
		HAL_UART_Receive_IT(&huart1, us100_buffer, 2);
		HAL_UART_Transmit(&huart1, &cmd_dist, 1, 500);
		while (us100_Rx_flag == (00)) {};
		distance = us100_buffer[0];
		distance = (distance << 8) + us100_buffer[1];
		float distance_p = distance;
		distance_p = 1-(distance_p / 750);
		distance = distance_p*100;

		if(distance <= 0){ // special event
			TIM3->CCR1 = 0; // motor off
			TIM3->CCR3 = 0;
			percent_pwm = 0;
			HAL_UART_Transmit(&huart6, "RESERVOIR IS EMPTY \r\n",strlen("RESERVOIR IS EMPTY \r\n"), 1000);
			HAL_GPIO_WritePin(GPIOA, LD2_Pin, GPIO_PIN_RESET); //LD2 off
			mode = 4;

		}

		//display depth on timer board
		DIG_A = distance / 10;
		DIG_B = distance % 10;
		DIGITS_Display(DIG_A, DIG_B);
    	uint32_t last_toggle_time2 = HAL_GetTick();
    	rpm_tick_start = rpm_tick_count;
    	while(HAL_GetTick() - last_toggle_time2 <= 1000){}
    	rpm = (rpm_tick_count - rpm_tick_start);
    	rpm = (rpm/20)*(60/1);
    	rpm_p = rpm;
    	halftime = 0;
    	Hourly_Report();
	}

    if (mode == 4){

		  uint32_t last_toggle_time2 = HAL_GetTick();
		  while(HAL_GetTick() - last_toggle_time2 <= 500){}
		  if (on_off == 0){
			  HAL_GPIO_WritePin(GPIOB, BLU_Pin, GPIO_PIN_SET);
			  HAL_GPIO_WritePin(GPIOB, RED_Pin, GPIO_PIN_SET);
			  HAL_GPIO_WritePin(GPIOA, GRN_Pin, GPIO_PIN_SET);
			  on_off = 1;
		  }
		  else if (on_off){
			  HAL_GPIO_WritePin(GPIOB, BLU_Pin, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(GPIOB, RED_Pin, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(GPIOA, GRN_Pin, GPIO_PIN_RESET);
			  on_off = 0;
		  }
		  last_toggle_time2 = HAL_GetTick();
    }
    if(clock_hours == 24){
    	HAL_GPIO_WritePin(GPIOA, LD2_Pin, GPIO_PIN_RESET); //LD2 off
	    mode = 5;
    }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_8B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 16-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 20000-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 500-1;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 16-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 2000-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 1200-1;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.Pulse = 0;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = (16000-1)/300;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 1000-1;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 9600;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, DIGIT_B0_Pin|DIGIT_B2_Pin|DIGIT_B3_Pin|DIGIT_B1_Pin
                          |DIGIT_A3_Pin|DIGIT_A2_Pin|DIGIT_A1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GRN_Pin|LD2_Pin|DIGIT_A0_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, BLU_Pin|RED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PB1_Pin */
  GPIO_InitStruct.Pin = PB1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(PB1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : DIGIT_B0_Pin DIGIT_B2_Pin DIGIT_B3_Pin DIGIT_B1_Pin
                           DIGIT_A3_Pin DIGIT_A2_Pin DIGIT_A1_Pin */
  GPIO_InitStruct.Pin = DIGIT_B0_Pin|DIGIT_B2_Pin|DIGIT_B3_Pin|DIGIT_B1_Pin
                          |DIGIT_A3_Pin|DIGIT_A2_Pin|DIGIT_A1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : GRN_Pin LD2_Pin DIGIT_A0_Pin */
  GPIO_InitStruct.Pin = GRN_Pin|LD2_Pin|DIGIT_A0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : BLU_Pin RED_Pin */
  GPIO_InitStruct.Pin = BLU_Pin|RED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : RPM_TICK_Pin */
  GPIO_InitStruct.Pin = RPM_TICK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(RPM_TICK_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(huart->Instance ==USART1){
		us100_Rx_flag = 1;
	}
	if (huart->Instance == USART6)
	{
		// Store received byte in the buffer
		if (rx_byte != '\n' && rx_byte != '\r' && rx_index < 3 - 1) {
			rx_buffer[rx_index++] = rx_byte; // Add character to buffer
			HAL_UART_Transmit(&huart6, &rx_byte, 1, 100);
			// Re-enable reception for the next byte
			HAL_UART_Receive_IT(&huart6, &rx_byte, 1);
		} else {
			rx_buffer[rx_index] = '\0'; // Null-terminate the string
			rcv_intpt_flag = 1;

			// Reset the buffer for the next input
			rx_index = 0;
		}
	}
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin ==RPM_TICK_Pin){
		rpm_tick_count +=1;
	}
}
void DIGITS_Display(uint8_t DIGIT_A, uint8_t DIGIT_B)
{
	 uint8_t DIGITA_VAL = 0x0F & DIGIT_A; //mask off higher4 bits
	 int Abit0 = (DIGITA_VAL ) & 1;  	// extract Abit0 of the 4-bit value
	 int Abit1 = (DIGITA_VAL >> 1) & 1;  // extract Abit1 of the 4-bit value
	 int Abit2 = (DIGITA_VAL >> 2) & 1;  // extract Abit2 of the 4-bit value
	 int Abit3 = (DIGITA_VAL >> 3) & 1;  // extract Abit3 of the 4-bit value

	 uint8_t DIGITB_VAL = 0x0F & DIGIT_B; //mask off higher4 bits
	 int Bbit0 = (DIGITB_VAL ) & 1;  	// extract Bbit0 of the 4-bit value
	 int Bbit1 = (DIGITB_VAL >> 1) & 1;  // extract Bbit1 of the 4-bit value
	 int Bbit2 = (DIGITB_VAL >> 2) & 1;  // extract Bbit2 of the 4-bit value
	 int Bbit3 = (DIGITB_VAL >> 3) & 1;  // extract Bbit3 of the 4-bit value

	 if (Abit0 == (0))
	 {
		 HAL_GPIO_WritePin(GPIOA, DIGIT_A0_Pin, GPIO_PIN_RESET);
	 }
	 else
	 {
		 HAL_GPIO_WritePin(GPIOA, DIGIT_A0_Pin, GPIO_PIN_SET);

	 }
	 if (Abit1 == (0))
	 {
		 HAL_GPIO_WritePin(GPIOC, DIGIT_A1_Pin, GPIO_PIN_RESET);
	 }
	 else
	 {
		 HAL_GPIO_WritePin(GPIOC, DIGIT_A1_Pin, GPIO_PIN_SET);

	 }
	 if (Abit2 == (0))
	 {
		 HAL_GPIO_WritePin(GPIOC, DIGIT_A2_Pin, GPIO_PIN_RESET);
	 }
	 else
	 {
		 HAL_GPIO_WritePin(GPIOC, DIGIT_A2_Pin, GPIO_PIN_SET);

	 }
	 if (Abit3 == (0))
	 {
		 HAL_GPIO_WritePin(GPIOC, DIGIT_A3_Pin, GPIO_PIN_RESET);
	 }
	 else
	 {
		 HAL_GPIO_WritePin(GPIOC, DIGIT_A3_Pin, GPIO_PIN_SET);

	 }


	 if (Bbit0 == (0))
	 {
		 HAL_GPIO_WritePin(GPIOC, DIGIT_B0_Pin, GPIO_PIN_RESET);
	 }
	 else
	 {
		 HAL_GPIO_WritePin(GPIOC, DIGIT_B0_Pin, GPIO_PIN_SET);

	 }
	 if (Bbit1 == (0))
	 {
		 HAL_GPIO_WritePin(GPIOC, DIGIT_B1_Pin, GPIO_PIN_RESET);
	 }
	 else
	 {
		 HAL_GPIO_WritePin(GPIOC, DIGIT_B1_Pin, GPIO_PIN_SET);

	 }
	 if (Bbit2 == (0))
	 {
		 HAL_GPIO_WritePin(GPIOC, DIGIT_B2_Pin, GPIO_PIN_RESET);
	 }
	 else
	 {
		 HAL_GPIO_WritePin(GPIOC, DIGIT_B2_Pin, GPIO_PIN_SET);

	 }
	 if (Bbit3 == (0))
	 {
		 HAL_GPIO_WritePin(GPIOC, DIGIT_B3_Pin, GPIO_PIN_RESET);
	 }
	 else
	 {
		 HAL_GPIO_WritePin(GPIOC, DIGIT_B3_Pin, GPIO_PIN_SET);

	 }
}
void Set_Pipe(int location){
	//if inlet
	if(location == 0){
		//purple
		HAL_GPIO_WritePin(GPIOB, BLU_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, RED_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GRN_Pin, GPIO_PIN_RESET);
    //default position
		TIM2->CCR1 = 2000;
    zone = 0;
	}
	//if outlet 1
	if(location == 1){
		//red
		HAL_GPIO_WritePin(GPIOB, BLU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, RED_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GRN_Pin, GPIO_PIN_RESET);
    //45 degrees
		TIM2->CCR1 = 500;
    zone = 1;
	}
	//if outlet 2
	if(location == 2){
		//green
		HAL_GPIO_WritePin(GPIOB, BLU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, RED_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GRN_Pin, GPIO_PIN_SET);
    //90 degress
		TIM2->CCR1 = 1000;
    zone = 2;
  }
	//if outlet 3
	if(location == 3){
		//blue
		HAL_GPIO_WritePin(GPIOB, BLU_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, RED_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GRN_Pin, GPIO_PIN_RESET);
    //135 degrees
		TIM2->CCR1 = 1500;
    zone = 3;
	}
}

void Set_Motor(int direction, int speed){

  //*inlet moves motor in reverse
  if(direction == 0){
    if(speed == 1){
      TIM3->CCR3 = 1400; // 70% of 60k
      percent_pwm = 70;
    }
    else if(speed == 2){
      TIM3->CCR3 = 1700; // 85% of 60k
      percent_pwm = 85;
    }
    else if(speed == 3){
      TIM3->CCR3 = 1980; // 99% of 60k
      percent_pwm = 99;
    }
  }
  //for outlet 
  else {
    if(speed == 1){
      TIM3->CCR1 = 1400; // 70% of 60k
      percent_pwm = 70;
    }
    else if(speed == 2){
      TIM3->CCR1 = 1700; // 85% of 60k
      percent_pwm = 85;
    }
    else if(speed == 3){
      TIM3->CCR1 = 1980; // 99% of 60k
      percent_pwm = 99;
    }
  }
}
void Hourly_Report(){
  
  sprintf((char*) txd_msg_buffer, "Time %d:%d | Zone %d | Motor Speed %d%%", clock_hours, clock_mins, zone, percent_pwm);
  HAL_UART_Transmit(&huart6, txd_msg_buffer,strlen((char*) txd_msg_buffer), 1000);
  sprintf((char*) txd_msg_buffer, "| Motor RPM %d | Water Depth %d%% \r\n", rpm_p, distance);
  HAL_UART_Transmit(&huart6, txd_msg_buffer,strlen((char*) txd_msg_buffer), 1000);

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if((htim->Instance == TIM5)){
		wall_clock_hr_update_flag = 0; // screen updates occur hourly on the half-hour
		clock_secs += 1;
		if((clock_secs == 60)){
			clock_mins += 1;
			clock_secs = 0;
			if(clock_mins == 15){
				halftime = 1;
			}
		}
		if((clock_mins == 60 )){
			clock_hours += 1;
			clock_mins = 0;
			//new_hour = 1;
		}
		wall_clock_hr_update_flag = 1; // screen updates occur hourly on the hour
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
