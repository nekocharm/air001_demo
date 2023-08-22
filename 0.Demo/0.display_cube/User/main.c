#include "main.h"
#include <math.h>

SPI_HandleTypeDef hspi1;

void SystemClock_Config(void);
static void GPIO_Init(void);
static void SPI1_Init(void);

float cube[8][3]={
	{-15,-15,-15},
	{-15, 15,-15},
	{ 15, 15,-15},
	{ 15,-15,-15},
	{-15,-15, 15},
	{-15, 15, 15},
	{ 15, 15, 15},
	{ 15,-15, 15}
};

float tmp[]={0,0,0};

int lineid[] = {1,2,2,3,3,4,4,1,5,6,6,7,7,8,8,5,8,4,7,3,6,2,5,1};

float ax=0,ay=0;

	
void revolve(float x,float y)
{
	for (int i =0;i<8;i++)
		{
			tmp[0]=cos(x)*cube[i][0]-sin(x)*cube[i][2];
			tmp[1]=cube[i][1];
			tmp[2]=sin(x)*cube[i][0]+cos(x)*cube[i][2];
			for(int j=0;j<3;j++)
			{
				cube[i][j]=tmp[j];
			}
			tmp[0]=cube[i][0];
			tmp[1]=cos(y)*cube[i][1]+sin(y)*cube[i][2];
			tmp[2]=-sin(y)*cube[i][1]+cos(y)*cube[i][2];
			for(int j=0;j<3;j++)
			{
				cube[i][j]=tmp[j];
			}  
		}
}


int main(void)
{
  HAL_Init();

  SystemClock_Config();

  GPIO_Init();
  SPI1_Init();
	OLED_Init();
	OLED_DisPlay_On();
  while (1)
  {
		//OLED_Refresh();
		revolve(ax,ay);
		ax=0;
		ay=0;		
		OLED_Clear();
		for(int i=0;i<24;i=i+2)
			OLED_DrawLine(64+(int)(cube[lineid[i]-1][0]),32+(int)(cube[lineid[i]-1][1]),64+(int)(cube[lineid[i+1]-1][0]),32+(int)(cube[lineid[i+1]-1][1]),1);
		OLED_Refresh();
		//HAL_Delay(1000);
  }
	
}


void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /*配置时钟源HSE/HSI/LSE/LSI*/
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;                                                      /* 开启HSI */
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_16MHz;                           /* 配置HSI输出时钟为16MHz */
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;                                                      /* HSI不分频 */
  RCC_OscInitStruct.HSEState = RCC_HSE_OFF;                                                     /* 关闭HSE */
  RCC_OscInitStruct.HSEFreq = RCC_HSE_16_32MHz;                                                 /* HSE工作频率范围16M~32M */
  RCC_OscInitStruct.LSIState = RCC_LSI_OFF;                                                     /* 关闭LSI */
  RCC_OscInitStruct.LSEState = RCC_LSE_OFF;                                                     /* 关闭LSE */
  RCC_OscInitStruct.LSEDriver = RCC_LSEDRIVE_MEDIUM;                                            /* LSE默认驱动能力 */
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF;                                                  /* 关闭PLL */
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /* 初始化CPU,AHB,APB总线时钟 */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1; /* RCC系统时钟类型 */
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;                                        /* SYSCLK的源选择为HSI */
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;                                            /* APH时钟不分频 */
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;                                             /* APB时钟不分频 */
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}


static void SPI1_Init(void)
{
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
}

static void GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);

  /*GPIO pins : PA1 PA2 PA3 PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	/*DC PB0 CS PB1 RES PB3*/
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	HAL_NVIC_SetPriority(EXTI0_1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch (GPIO_Pin)
	{
		case GPIO_PIN_1:
			ax=0.26;
			__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_1);
		break;
		case GPIO_PIN_2:
			ax=-0.26;
			__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);
		break;
		case GPIO_PIN_3:
			ay=0.26;
			__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_3);
		break;
		case GPIO_PIN_4:
			ay=-0.26;
			__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);
		break;
	}
}



void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
