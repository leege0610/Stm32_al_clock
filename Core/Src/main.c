/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include <stdio.h>
#include <string.h>
#include "lcd.h"
#include "key_interval_main.h"
#include "key_value_main.h"
#include "music.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct{
	int8_t hours;
	int8_t minutes;
	int8_t seconds;
}TimeTypeDef;

typedef struct{
  int32_t time;
  GPIO_PinState level;
}ClickInfoDef;



/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
enum CLOCK_MODE{
	NORMAL_STATE,
	TIME_SETTING,
	ALARM_TIME_SETTING,
	MUSIC_SELECT
};

enum CLOCK_BUTTON{
	UP,
	DOWN,
	RIGHT,
	LEFT,
	NO_KEY
};

struct clock_state{
	enum CLOCK_MODE mode;
	enum CLOCK_BUTTON button;
	uint8_t music_num;
};

struct clock_state current_state;

/* Base address of the Flash sectors Bank 1 */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */

/* Base address of the Flash sectors Bank 2 */
#define ADDR_FLASH_SECTOR_12     ((uint32_t)0x08100000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_13     ((uint32_t)0x08104000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_14     ((uint32_t)0x08108000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_15     ((uint32_t)0x0810C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_16     ((uint32_t)0x08110000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_17     ((uint32_t)0x08120000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_18     ((uint32_t)0x08140000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_19     ((uint32_t)0x08160000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_20     ((uint32_t)0x08180000) /* Base @ of Sector 8, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_21     ((uint32_t)0x081A0000) /* Base @ of Sector 9, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_22     ((uint32_t)0x081C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_23     ((uint32_t)0x081E0000) /* Base @ of Sector 11, 128 Kbytes */

#define MAGIC_NUM 0xdeadbeef

typedef struct {
  uint32_t magic_num;
  TimeTypeDef setting_time;
  TimeTypeDef alarm_time;
  int8_t alarm_music_num;
}NVitemTypeDef;

#define nv_items  ((NVitemTypeDef *) ADDR_FLASH_SECTOR_23)

NVitemTypeDef default_nvitem =
{
  MAGIC_NUM,
  {0,0,0},
  {0,0,0},
  0
};

MusicTypeDef alarm_music[] =
{
  {0,"School bell  "},
  {1,"Beep         "},
  {2,"Airplane  "},
};

#define MEL_NUM	24

uint8_t musicOn = 0;

_BUZZER schoolBell[MEL_NUM] = {
		{G, 1}, {G, 1}, {A, 1}, {A, 1}, // 4
		{G, 1}, {G, 1}, {E, 2}, // 3
		{G, 1}, {G, 1}, {E, 1}, {E, 1}, {D, 3}, // 5
		{G, 1}, {G, 1}, {A, 1}, {A, 1}, // 4
		{G, 1}, {G, 1}, {E, 2}, // 3
		{G, 1}, {E, 1}, {D, 1}, {E, 1}, {C, 3} // 5
};

_BUZZER beep[MEL_NUM] = {
		{A, 3}, {A, 3}, {A, 3}, {A, 3}, // 4
		{A, 3}, {A, 3}, {A, 3}, // 3
		{A, 3}, {A, 3}, {A, 3}, {A, 3}, {A, 3}, // 5
		{A, 3}, {A, 3}, {A, 3}, {A, 3}, // 4
		{A, 3}, {A, 3}, {A, 3}, // 3
		{A, 3}, {A, 3}, {A, 3}, {A, 3}, {A, 3} // 5
};

_BUZZER airplane[MEL_NUM + 1] = {
		{E, 1}, {D, 1}, {C, 1}, {D, 1}, // 4
		{E, 1}, {E, 1}, {E, 2}, // 3
		{D, 1}, {D, 1}, {D, 2}, {E, 1}, {E, 1}, // 5
		{E, 2}, {E, 1}, {D, 1}, {C, 1}, // 4
		{D, 1}, {E, 1}, {E, 1}, // 3
		{E, 2}, {D, 1}, {D, 1}, {E, 1}, {D, 1}, {C, 4} // 6
};
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart3;
UART_HandleTypeDef huart6;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_RTC_Init(void);
static void MX_I2C1_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_USART6_UART_Init(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */
void MUSIC_Play(_BUZZER music[]);
void ALARM_Check();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#ifdef __GNUC__
  /* With GCC, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

ClickInfoDef click[3];  //pc13 use button history save
TimeTypeDef ctime;  //current time
TimeTypeDef stime = {0,0,0};  //set time
TimeTypeDef atime = {0,0,0};  //alarm time
char ampm[3] = {'A','M'};  //AM, PM
uint8_t alFlag = 0;  //Alarm on/off
int8_t position = 0;  //Time setting cursor position
uint8_t seq = 0;  //Music play 음표
uint8_t stop = 0;  //음악 재생시 음과 음 사이 구분
int8_t pos = 0;  //music select menu position
uint8_t key_value;  //bluetooth signal receive
volatile int timer_count;
uint32_t ADC_value;  //ADC button value get
uint32_t last_time,current_time,time_interval, last_time_interval;  //pc13 user button separate select, double click, long key

HAL_StatusTypeDef update_nvitems()  //Flash rom save
{
	uint32_t FirstSector,NbOfSectors,SECTORError;
	FLASH_EraseInitTypeDef EraseInitStruct;
	HAL_StatusTypeDef error= HAL_OK;
    uint32_t Address,i;
    uint64_t Data;
    uint8_t *ptr;

	HAL_FLASH_Unlock();
	FirstSector = FLASH_SECTOR_23;
	NbOfSectors = 1;

	EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector        = FirstSector;
	EraseInitStruct.NbSectors     = NbOfSectors;

	error = HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError);
	if(error != HAL_OK)
	{
		return error;
	}

	ptr = (uint8_t*)&default_nvitem;

	for(i=0;i<sizeof(NVitemTypeDef);i++)
	{
		Address = (uint8_t*)nv_items + i;
		Data = *((uint8_t*)ptr+ i);
		error =	HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,Address,Data);
		if(error != HAL_OK)
		{
			return error;
		}
	}

	HAL_FLASH_Lock();
}

void _60_Cvt(TimeTypeDef* time)  //Time converter
{
	if(time->seconds > 59)
	{
		time->minutes++;
		time->seconds = 0;
	}
	else if(time->seconds < 0)
	{
		time->minutes--;
		time->seconds = 59;
	}

	if(time->minutes > 59)
	{
		time->hours++;
		time->minutes = 0;
	}
	else if(time->minutes < 0)
	{
		time->hours--;
		time->minutes = 59;
	}

	if(time->hours > 23)
	{
		time->hours = 0;
	}
	else if(time->hours < 0)
	{
		time->hours = 23;
	}

	if(time->hours < 12)
	{
		ampm[0] = 'A';
	}
	else if(time->hours >= 12)
	{
		ampm[0] = 'P';
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2)  //time ++
	{
		if((timer_count % 100) == 0)
		{
			ctime.seconds++;
			_60_Cvt(&ctime);
		}
		timer_count++;
	}

	ALARM_Check();

	if(htim->Instance == TIM4)  //music play
	{
		if(alFlag == 1 && musicOn == 1)
		{
			switch(default_nvitem.alarm_music_num)
			{
			case 0:
				MUSIC_Play(schoolBell);
				break;
			case 1:
				MUSIC_Play(beep);
				break;
			case 2:
				MUSIC_Play(airplane);
				break;
			default:
				break;
			}
		}
	}

}

void Select_Key()  //Select operate
{
	if(current_state.mode == NORMAL_STATE)
	{
		if(musicOn == 0) current_state.mode = TIME_SETTING;
		else
		{
			musicOn = 0;
			seq = 0;
			stop = 0;
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
		}
	}

	else
	{
		if(current_state.mode == TIME_SETTING)
		{
			memcpy(&default_nvitem.setting_time, &stime, sizeof(TimeTypeDef));
			update_nvitems();
			memcpy(&ctime, &default_nvitem.setting_time, sizeof(TimeTypeDef));
			position = 0;
			current_state.mode = NORMAL_STATE;
		}
		if(current_state.mode == ALARM_TIME_SETTING)
		{
			memcpy(&default_nvitem.alarm_time, &atime, sizeof(TimeTypeDef));
			update_nvitems();
			position = 0;
			current_state.mode = NORMAL_STATE;
		}
		if(current_state.mode == MUSIC_SELECT)
		{

			memcpy(&default_nvitem.alarm_music_num, &alarm_music[pos].music_num, sizeof(uint8_t));
			update_nvitems();
			current_state.mode = NORMAL_STATE;
		}
	}
}
void Long_Key()  //Long key operate
{
	if(current_state.mode == NORMAL_STATE)
	{
		alFlag = 1;
		current_state.mode = ALARM_TIME_SETTING;
	}
}
void Double_Click()  //Double click operate
{
	if(current_state.mode == NORMAL_STATE)
	{
		current_state.mode = MUSIC_SELECT;
	}
	if(current_state.mode == ALARM_TIME_SETTING)
	{
		if(alFlag == 0)	alFlag = 1;
		else if(alFlag == 1) alFlag = 0;
	}
	if(current_state.mode == TIME_SETTING)
	{
		current_state.mode = NORMAL_STATE;
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)  //User key
{
	GPIO_PinState pin;
	int i;

	if(GPIO_Pin == GPIO_PIN_13)
	{
		current_time = HAL_GetTick();
		time_interval = current_time - last_time;
		last_time = current_time;

		pin = HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13);

		//printf("<%d,%d>\r\n",pin,time_interval);

		if(time_interval <= 2) // noise
		{
			//printf("Noise %d,%d\r\n",pin,time_interval);
		}
		else
		{
			click[2].time = click[1].time;
			click[2].level = click[1].level;

			click[1].time = click[0].time;
			click[1].level = click[0].level;

			click[0].time = time_interval;
			click[0].level = pin;

			if(click[2].level == GPIO_PIN_RESET && click[1].level == GPIO_PIN_SET &&  click[0].level ==GPIO_PIN_RESET)
			{
				for(i=0;i<3;i++)
				{
					if(click[i].time>= DOUBLE_CLICK_MIN && click[i].time <= DOUBLE_CLICK_MAX)
					{
						continue;
					}
					else
						break;
				}
				if(i==3)
				{
					Double_Click();
					//printf("Double Click\r\n");
				}
			}

			if(click[0].level == GPIO_PIN_RESET && click[0].time >=LONG_CLICK_MIN) // long click
			{
				Long_Key();
				//printf("Long Key\r\n");
			}
			if(click[0].level == GPIO_PIN_RESET && click[0].time < LONG_CLICK_MIN && click[0].time > DOUBLE_CLICK_MAX)
			{
				Select_Key();
				//printf("Select Key, %d\r\n",click[0].time);
			}
		}
	}
}

void LCD_Blink(TimeTypeDef* time, char str[17])  //Setting mode blink
{
	static int8_t blinkFlag = 0;
	static volatile int old_timer_count;

	if(timer_count - old_timer_count > 40)
	{
		if(blinkFlag == 0) blinkFlag = 1;
		else if(blinkFlag == 1) blinkFlag = 0;
		old_timer_count = timer_count;
	}

	_60_Cvt(time);

	if(current_state.mode == TIME_SETTING || (current_state.mode == ALARM_TIME_SETTING && alFlag == 0))
	{
		if(time->hours <= 12)
		{
			if(blinkFlag == 0) sprintf(str,"%s %02d:%02d:%02d     ", ampm, time->hours, time->minutes, time->seconds);
			else if (blinkFlag == 1)
			{
				if(position == 0) sprintf(str,"__ %02d:%02d:%02d     ",time->hours, time->minutes, time->seconds);
				else if(position == 1) sprintf(str,"%s __:%02d:%02d     ", ampm, time->minutes, time->seconds);
				else if(position == 2) sprintf(str,"%s %02d:__:%02d     ", ampm, time->hours, time->seconds);
				else if(position == 3) sprintf(str,"%s %02d:%02d:__     ", ampm, time->hours, time->minutes);
			}
		}
		else if(time->hours > 12)
		{
			if(blinkFlag == 0) sprintf(str,"%s %02d:%02d:%02d     ", ampm, time->hours-12, time->minutes, time->seconds);
			else if (blinkFlag == 1)
			{
				if(position == 0) sprintf(str,"__ %02d:%02d:%02d     ",time->hours-12, time->minutes, time->seconds);
				else if(position == 1) sprintf(str,"%s __:%02d:%02d     ", ampm, time->minutes, time->seconds);
				else if(position == 2) sprintf(str,"%s %02d:__:%02d     ", ampm, time->hours-12, time->seconds);
				else if(position == 3) sprintf(str,"%s %02d:%02d:__     ", ampm, time->hours-12, time->minutes);
			}
		}
	}

	else if(current_state.mode == ALARM_TIME_SETTING && alFlag == 1)
	{
		if(time->hours <= 12)
		{
			if(blinkFlag == 0) sprintf(str,"%s %02d:%02d:%02d AL", ampm, time->hours, time->minutes, time->seconds);
			else if (blinkFlag == 1)
			{
				if(position == 0) sprintf(str,"__ %02d:%02d:%02d AL",time->hours, time->minutes, time->seconds);
				else if(position == 1) sprintf(str,"%s __:%02d:%02d AL", ampm, time->minutes, time->seconds);
				else if(position == 2) sprintf(str,"%s %02d:__:%02d AL", ampm, time->hours, time->seconds);
				else if(position == 3) sprintf(str,"%s %02d:%02d:__ AL", ampm, time->hours, time->minutes);
			}
		}
		else if(time->hours > 12)
		{
			if(blinkFlag == 0) sprintf(str,"%s %02d:%02d:%02d AL", ampm, time->hours-12, time->minutes, time->seconds);
			else if (blinkFlag == 1)
			{
				if(position == 0) sprintf(str,"__ %02d:%02d:%02d AL",time->hours-12, time->minutes, time->seconds);
				else if(position == 1) sprintf(str,"%s __:%02d:%02d AL", ampm, time->minutes, time->seconds);
				else if(position == 2) sprintf(str,"%s %02d:__:%02d AL", ampm, time->hours-12, time->seconds);
				else if(position == 3) sprintf(str,"%s %02d:%02d:__ AL", ampm, time->hours-12, time->minutes);
			}
		}
	}
}

void TIME_Setting(TimeTypeDef* time, char str[17])  //Settin mode
{
	switch(current_state.button)  //ADC button operate
	{
	case UP:
		if(position == 0)
		{
			if(time->hours < 12) time->hours += 12;
			else time->hours -= 12;
		}
		if(position == 1) time->hours++;
		if(position == 2) time->minutes++;
		if(position == 3) time->seconds++;
		break;
	case DOWN:
		if(position == 0)
		{
			if(time->hours < 12) time->hours += 12;
			else time->hours -= 12;
		}
		if(position == 1) time->hours--;
		if(position == 2) time->minutes--;
		if(position == 3) time->seconds--;
		break;
	case LEFT:
		position--;
		if(position < 0) position = 3;
		break;
	case RIGHT:
		position++;
		if(position > 3) position = 0;
		break;
	default:
		current_state.button = NO_KEY;
		break;
	}

	if(current_state.mode == TIME_SETTING || current_state.mode == ALARM_TIME_SETTING)
	{
		LCD_Blink(time, str);
	}
}

void MUSIC_Select(char line1[17], char line2[17])  //Music select mode
{
	uint8_t count;

	pos = current_state.music_num;
	count = sizeof(alarm_music)/sizeof(alarm_music[0]);  //total music count

	switch(current_state.button)
	{
	case UP:
		pos++;
		if(pos == count)
		{
			pos = 0;
		}
		break;
	case DOWN:
		pos--;
		if(pos < 0)
		{
			pos = count - 1;
		}
		break;
	default:

		break;
	}
	current_state.music_num =  pos;

	if(pos <= count-2)
	{
		sprintf(line1,"%s <<",alarm_music[pos].music_title);  // "<" mark is selected music title
		sprintf(line2,"%s   ",alarm_music[pos+1].music_title);
	}
	else
	{
		sprintf(line1,"%s   ",alarm_music[pos].music_title);
		sprintf(line2,"%s   ",alarm_music[0].music_title);
	}
}

void _12_Cvt(TimeTypeDef* time, char str[17])  //12시제 컨버터
{
	if(alFlag == 0)
	{
		if(time->hours <= 12)
		{
			sprintf(str,"%s %02d:%02d:%02d     ",ampm, time->hours, time->minutes, time->seconds);
		}
		else if(time->hours > 12)
		{
			sprintf(str,"%s %02d:%02d:%02d     ",ampm, time->hours-12, time->minutes, time->seconds);
		}
	}
	else if(alFlag == 1)
	{
		if(time->hours <= 12)
		{
			sprintf(str,"%s %02d:%02d:%02d AL",ampm, time->hours, time->minutes, time->seconds);
		}
		else if(time->hours > 12)
		{
			sprintf(str,"%s %02d:%02d:%02d AL",ampm, time->hours-12, time->minutes, time->seconds);
		}
	}
}

void LCD_Display()  //Display
{
	char time_str[17];
	char mode_str[17];

	switch(current_state.mode)
	{
	case NORMAL_STATE:
		_12_Cvt(&ctime, time_str);
		sprintf(mode_str,"LEE ALARM CLOCK ");
		break;
	case TIME_SETTING:
		sprintf(mode_str,"TIME SET MODE   ");
		TIME_Setting(&stime, time_str);
		break;
	case ALARM_TIME_SETTING:
		sprintf(mode_str,"ALARM SET MODE  ");
		TIME_Setting(&atime, time_str);
		break;
	case MUSIC_SELECT:
		MUSIC_Select(mode_str, time_str);
		break;
	default:
		current_state.mode = NORMAL_STATE;
		break;
	}
	LCD_Print(0, mode_str);
	LCD_Print(1, time_str);
}

void ADC_Button()  //ADC button operate
{
	HAL_ADC_Start(&hadc1);
	if(HAL_ADC_PollForConversion(&hadc1,20) == HAL_OK) ADC_value = HAL_ADC_GetValue(&hadc1);
	if(ADC_value <= UP_KEY_MAX || key_value == 1)
	{
		current_state.button = UP;
		key_value = 0;
		//printf("UP\r\n");
	}
	else if((ADC_value>= DOWN_KEY_MIN && ADC_value<=DOWN_KEY_MAX) || key_value == 2)
	{
		current_state.button = DOWN;
		key_value = 0;
		//printf("DOWN\r\n");
	}
	else if((ADC_value>= LEFT_KEY_MIN && ADC_value<=LEFT_KEY_MAX) || key_value == 3)
    {
    	current_state.button = LEFT;
    	key_value = 0;
    	//printf("LEFT\r\n");
    }
	else if((ADC_value>= RIGHT_KEY_MIN && ADC_value<=RIGHT_KEY_MAX) || key_value == 4)
    {
    	current_state.button = RIGHT;
    	key_value = 0;
    	//printf("RIGHT\r\n");
    }
	else
	{
		current_state.button = NO_KEY;
	}
}

void TIME_Init()  //Time initiate
{
	if(nv_items->magic_num == MAGIC_NUM) //
	{
		memcpy(&default_nvitem,nv_items,sizeof(NVitemTypeDef));
	    ctime.hours = default_nvitem.setting_time.hours;
		ctime.minutes = default_nvitem.setting_time.minutes;
		ctime.seconds = default_nvitem.setting_time.seconds;

	    stime.hours = default_nvitem.setting_time.hours;
		stime.minutes = default_nvitem.setting_time.minutes;
		stime.seconds = default_nvitem.setting_time.seconds;

	    atime.hours = default_nvitem.alarm_time.hours;
		atime.minutes = default_nvitem.alarm_time.minutes;
		atime.seconds = default_nvitem.alarm_time.seconds;

		current_state.mode = NORMAL_STATE;
	}
	else
	{
		current_state.mode = TIME_SETTING;
	}
	_60_Cvt(&ctime);
}

void MUSIC_Play(_BUZZER music[])  //Play music
{
	uint16_t melody = (uint16_t)(1000000 / music[seq].freq);
	uint8_t num = 0;

	if(default_nvitem.alarm_music_num == 0) num = MEL_NUM;
	else if(default_nvitem.alarm_music_num == 1) num = MEL_NUM;
	else if(default_nvitem.alarm_music_num == 2) num = MEL_NUM+1;

	if(stop == 1)
	{
		TIM4->ARR = 2000;
		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
		stop = 0;
	}
	else
	{
		if(seq == num)
		{
			seq = 0;
		}
		else
		{
			TIM3->ARR = melody;
			TIM3->CCR2 = melody / 2;
			TIM4->ARR = music[seq].delay * 2000;
			HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
			stop = 1;

			seq++;
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
		}
	}
}

void ALARM_Check()  //Alarm check
{
	if(ctime.seconds == atime.seconds && ctime.minutes == atime.minutes && ctime.hours == atime.hours)
	{
		if(alFlag == 1) musicOn = 1;
	}
	HAL_UART_Transmit_IT(&huart6,(uint8_t*)&musicOn,sizeof(musicOn));
}

void BT_Button()  //bluetooth button operate (pc13 User button)
{
	if(key_value > 4)
	{
		switch(key_value)
		{
		case 5:
			Select_Key();
			break;
		case 6:
			Long_Key();
			break;
		case 7:
			Double_Click();
			break;
		case 8:
			Select_Key();
			break;
		case 9:
			Double_Click();
			break;
		case 10:
			if(musicOn == 1) Select_Key();
			break;
		default:
			break;
		}
		key_value = 0;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART6)  //bluetooth signal get.(0 = no_key, 1~4 = ADC key, 5~10 = pc13 user button)
	{
		/* Receive one byte in interrupt mode */
		BT_Button();
		HAL_UART_Receive_IT(&huart6, &key_value, 1);
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

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART3_UART_Init();
  MX_RTC_Init();
  MX_I2C1_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USART6_UART_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Init(&htim2);
  HAL_TIM_Base_Start_IT(&htim2);
  HAL_TIM_Base_Start_IT(&htim4);
  //HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
  HAL_UART_Receive_IT(&huart6,&key_value,1);
  init();  //lcd initiate
  TIME_Init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  ADC_Button();
	  LCD_Display();

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* EXTI15_10_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
  /* TIM2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
  /* TIM4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM4_IRQn);
  /* USART6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART6_IRQn);
  /* USART3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART3_IRQn);
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
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
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
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

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

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 900;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

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

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 90-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 200-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
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
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 9000;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 200;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

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
  huart6.Init.BaudRate = 115200;
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

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD3_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : USER_Btn_Pin */
  GPIO_InitStruct.Pin = USER_Btn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_Btn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RMII_MDC_Pin RMII_RXD0_Pin RMII_RXD1_Pin */
  GPIO_InitStruct.Pin = RMII_MDC_Pin|RMII_RXD0_Pin|RMII_RXD1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : RMII_REF_CLK_Pin RMII_MDIO_Pin */
  GPIO_InitStruct.Pin = RMII_REF_CLK_Pin|RMII_MDIO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : RMII_TXD1_Pin */
  GPIO_InitStruct.Pin = RMII_TXD1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(RMII_TXD1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD3_Pin LD2_Pin */
  GPIO_InitStruct.Pin = LD3_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OverCurrent_Pin */
  GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : USB_SOF_Pin USB_ID_Pin USB_DM_Pin USB_DP_Pin */
  GPIO_InitStruct.Pin = USB_SOF_Pin|USB_ID_Pin|USB_DM_Pin|USB_DP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_VBUS_Pin */
  GPIO_InitStruct.Pin = USB_VBUS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_VBUS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RMII_TX_EN_Pin RMII_TXD0_Pin */
  GPIO_InitStruct.Pin = RMII_TX_EN_Pin|RMII_TXD0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
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

