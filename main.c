/**
* @file    main.c
* @author  AMG - RF Application team
* @version 1.0.0
* @date    November, 9 2016
* @brief   SigFox Push Button Demo Application
* @details
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* THIS SOURCE CODE IS PROTECTED BY A LICENSE.
* FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
* IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
*
* <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
*/

/**
 * @file main.c
 * @brief This is a ST-SigFox demo that shows how to use the sigfox protocol to 
 *         a message to the base stations each time the push button is pressed.
 *         The data sent is a number representing the number of times the button 
 *              has been pressed from the boot.

* \section KEIL_project KEIL project
  To use the project with KEIL uVision 5 for ARM, please follow the instructions below:
  -# Open the KEIL uVision 5 for ARM and select Project->Open Project menu. 
  -# Open the KEIL project
     <tt> ...\\Projects\\Projects_Cube\\SigFox_Applications\\SigFox_PushButton_Project\\MDK-ARM\\SigFox_PushButton_Project.uvprojx </tt>
  -# Select desired configuration to build
  -# Select Project->Rebuild all target files. This will recompile and link the entire application
  -# Select Project->Download to download the related binary image.

* \section IAR_project IAR project
  To use the project with IAR Embedded Workbench for ARM, please follow the instructions below:
  -# Open the Embedded Workbench for ARM and select File->Open->Workspace menu. 
  -# Open the IAR project
     <tt> ...\\Projects\\Projects_Cube\\SigFox_Applications\\SigFox_PushButton_Project\\EWARM\\SigFox_PushButton_Project.eww </tt>
  -# Select desired configuration to build
  -# Select Project->Rebuild All. This will recompile and link the entire application
  -# Select Project->Download and Debug to download the related binary image.

* \subsection Project_configurations Project configurations
- \c NUCLEO_L1_ETSI - Configuration to be used for RCZ1
- \c NUCLEO_L1_FCC - Configuration to be used for RCZ2 and RCZ4

     
* \section Board_supported Boards supported
- \c STEVAL-FKI868V1 (for RCZ1)
- \c STEVAL-FKI915V1 (for RCZ2 and RCZ4)

**/

/* Includes ------------------------------------------------------------------*/
#include "assert.h"
#include "stddef.h"
#include "st_sigfox_api.h"
#include "sigfox_retriever.h"
#include "S2LP_SDK_Util.h"
#include "MCU_Interface.h"

//********************************
#include "main.h"		
#include <string.h> /* strlen */
//#include <stdio.h>  /* sprintf */
#include <math.h>   /* trunc */

/* Uncomment one of the lines below to use the sensors as necessary for smaller code size */
/* Note: Enabling all sensors will reach the 64KB flash size limit of the STM32L053R8 */
/* Comment out PRINTF lines to optimize code size */
#define USE_ACC_GYRO //LSM6DSL MEMS 3D accelerometer + 3D gyroscope
//#define USE_MAG_ACC //LSM303AGR MEMS 3D magnetometer + MEMS 3D accelerometer 
//#define USE_ENVI_SENSORS  //LPS22HB MEMS pressure sensor, absolute digital output barometer and HTS221 Capacitive digital relative humidity and temperature

/* Our global variable declaration */
float mytemperature;
float mypressure;
float myhumidity;
SensorAxes_t mymagnetic_field;
SensorAxes_t myangular_velocity;
SensorAxes_t myacceleration;

/* Private variables */
//static char dataOut[256];

#ifdef USE_ACC_GYRO
static void *LSM6DSL_X_0_handle = NULL;
static void *LSM6DSL_G_0_handle = NULL;
#endif
#ifdef USE_MAG_ACC
static void *LSM303AGR_X_0_handle = NULL;
static void *LSM303AGR_M_0_handle = NULL;
#endif
#ifdef USE_ENVI_SENSORS
static void *HTS221_H_0_handle  = NULL;
static void *HTS221_T_0_handle  = NULL;
static void *LPS22HB_P_0_handle  = NULL;
static void *LPS22HB_T_0_handle  = NULL;
#endif
/* Private function prototypes */

static void initializeAllSensors( void );
static void deinitializeAllSensors( void );
static void enableAllSensors( void );
static void disableAllSensors( void );
#ifdef USE_ENVI_SENSORS
static void floatToInt( float in, int32_t *out_int, int32_t *out_dec, int32_t dec_prec );
#endif

#ifdef USE_ACC_GYRO
static void Accelero_Sensor_Handler( void *handle );
static void Gyro_Sensor_Handler( void *handle );
#endif
#ifdef USE_MAG_ACC
static void Accelero_Sensor_Handler( void *handle );
static void Magneto_Sensor_Handler( void *handle );
#endif
#ifdef  USE_ENVI_SENSORS
static void Humidity_Sensor_Handler( void *handle );
static void Temperature_Sensor_Handler( void *handle );
static void Pressure_Sensor_Handler( void *handle );
#endif
//***************************

/* a flag to understand if the button has been pressed */
static volatile uint8_t but_pressed=0; 


void Fatal_Error(void)
{
  SdkEvalLedInit(LED1);
  
  while(1)
  {
    ST_LOWLEVEL_Delay(100);
    SdkEvalLedToggle(LED1);
  }
}


void Appli_Exti_CB(uint16_t GPIO_Pin)
{
  /* In this case the application EXTI event is the button pressed */
  if(GPIO_Pin==GPIO_PIN_13)
  {
    /* set the button pressed flag */
    but_pressed=1;
		
		/* Add more button press handling here if necessary by the user application */


		
		
		/******************************************/
		
  }
}

/**
* @brief  Configure all the GPIOs in low power mode.
* @param  None
* @retval None
*/
void enterGpioLowPower(void)
{
  /* Set all the GPIOs in low power mode (input analog) */
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStructure.Pin = GPIO_PIN_All;

  GPIO_InitStructure.Pin = GPIO_PIN_All & (~GPIO_PIN_13); //button
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_InitStructure.Pin = GPIO_PIN_All & (~GPIO_PIN_13) & (~GPIO_PIN_14) 
														& (~GPIO_PIN_5) & (~GPIO_PIN_2) & (~GPIO_PIN_3);//SWD, LED, UART
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.Pin = GPIO_PIN_All  & (~GPIO_PIN_8) & (~GPIO_PIN_9); //I2C1 pins
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  /* de init the SPI */
  SdkEvalSpiDeinit();
  
  /* keep the EXTI on the button pin */
  SdkEvalPushButtonInit(BUTTON_KEY,BUTTON_MODE_EXTI);
  
  /* keep the SDN driven */
  SdkEvalM2SGpioInit(M2S_GPIO_SDN,M2S_MODE_GPIO_OUT);
  
  /* Be sure that it is driving the device to be in shutdown*/
  SdkEvalEnterShutdown();
}


/**
* @brief  Configure all the GPIOs to be used by the application.
* @param  None
* @retval None
*/
void exitGpioLowPower(void)
{ 
  /* Reinit the SDN pin and SPI */
  SdkEvalM2SGpioInit(M2S_GPIO_SDN,M2S_MODE_GPIO_OUT);
  SdkEvalSpiInit();
}




/**
* @brief  System main function.
* @param  None
* @retval None
*/

/* dtw algorithm */
int mymin(int a, int b) { return a < b? a: b; }

int myabs(int a) { return a > 0? a: -a; }

int dtw(int a[], int b[], int s) {
    int d[2][50] = {{0}};
    int cur = 0;
		d[!cur][0] = myabs(a[s] - b[0]);
		for(int j = 1; j < 50; j++) {
			d[!cur][j] = myabs(a[s] - b[j]) + d[!cur][j-1];
		}
    for(int i = s+1; i < s+50; i++) {
        for(int j = 0; j < 50; j++) {
            d[cur][j] = myabs(a[i%50] - b[j]);
            if(j > 0) d[cur][j] += mymin(d[cur][j-1], mymin(d[!cur][j], d[!cur][j-1]));
            else d[cur][j] += d[!cur][j];
        }
        cur = !cur;
    }
    return d[!cur][49];
}

void main(void)
{  
  /* Instance some variables where the SigFox data of the board are stored */
  uint8_t pac[8];
  uint32_t id;
  uint8_t rcz, retr_err;
  
  /* Some variables to store the application data to transmit */
  uint32_t cust_counter=0;
  uint8_t customer_data[12]={0};
  uint8_t customer_resp[8];

  /* Our local variable declaration*/
	int pattern[50][50] = {{0}};
	int walk_pattern[50] = {1464,1454,1515,1512,1384,1257,1101,997,906,807,750,706,686,704,705,719,708,689,652,652,668,693,740,698,660,665,653,711,749,795,775,852,903,983,983,1013,1028,1001,977,1712,-79,-628,1666,932,1061,1022,1039,1009,984,1011};
  int my_pattern[50] = {0};
	int count = 0;
	uint8_t dwnlink_req = 0;
	
  /* Initialize the hardware */
  HAL_Init();
  ST_LOWLEVEL_SetSysClock();
  SdkEvalIdentification();  
  SdkEvalM2SGpioInit(M2S_GPIO_SDN,M2S_MODE_GPIO_OUT);
  SdkEvalSpiInit();
  EepromSpiInitialization();
	
	//For X_NUCLEO_IKS01A2 ********
  USARTConfig();	/* Initialize UART */
	initializeAllSensors();	/* Initialize sensors */
	enableAllSensors();
	//****************************  
	
  /* Identify the RF board reading some production data */
  S2LPManagementIdentificationRFBoard();
  
  /* Put the radio off */
  SdkEvalEnterShutdown();
      
  /* Give the RF offset to the library */
  ST_MANUF_API_set_freq_offset(S2LPManagementGetOffset());
  
  /* The low level driver uses the internal RTC as a timer while the STM32 is in low power.
     This function calibrates the RTC using an auxiliary general purpose timer in order to 
     increase its precision. */
  ST_LOWLEVEL_TimerCalibration(500);
  
  /* Initialize the blue push button on the board */
  SdkEvalPushButtonInit(BUTTON_KEY,BUTTON_MODE_EXTI);

  /* Retrieve the SigFox data from the board - ID, PAC and RCZ used */
  retr_err=enc_utils_retrieve_data(&id,pac,&rcz);
  
  /* If the retriever returned an error (code different from RETR_OK) the application must not continue */
  if(retr_err!=RETR_OK){
	 /* Print error message */
		PRINTF("Board not ready for SigFox. Missing board ID/PAC.\n\r");
		PRINTF("Please register the board with ST to obtain the ID/PAC.\n\r");
		Fatal_Error();
  }
  
  /* Here it is necessary to understand if we are using the code for ETSI or FCC */
#ifdef FOR_ETSI
  /* In case of ETSI we should stuck if RCZ is not 1 */
  if(rcz!=1)
    Fatal_Error();
  
  /* RCZ1 - open the SigFox library */
  if(ST_SIGFOX_API_open(ST_RCZ1,(uint8_t*)&id)!=0)
  {
    /* Stuck in case of error */
    Fatal_Error();
  }
#elif FOR_FCC
  
  uint8_t send_call_num=0;
  
  if(rcz==2)
  {
    /* RCZ2 - open the SigFox library */
    if(ST_SIGFOX_API_open(ST_RCZ2,(uint8_t*)&id)!=0)
    {
      /* Stuck in case of error */
      Fatal_Error();
    }
     
    /* In FCC we can choose the macro channel to use by a 86 bits bitmask
       In this case we use the first 9 macro channels */
    uint32_t config_words[3]={0x1FF,0,0};
    
    /* Set the standard configuration with default channel to 1 */
    if(ST_SIGFOX_API_set_std_config(config_words,1)!=0)
    {
      /* Stuck in case of error */
      Fatal_Error();
    }
    ST_SIGFOX_API_reset();
  }
  else if(rcz==4)
  {
    /* RCZ4 - open the SigFox library */
    if(ST_SIGFOX_API_open(ST_RCZ4,(uint8_t*)&id)!=0)
    {
      /* Stuck in case of error */
      Fatal_Error();
    }
    
    /* In FCC we can choose the macro channel to use by a 86 bits bitmask
       In this case we use 9 consecutive macro channels starting from 63 (920.8MHz) */
    uint32_t config_words[3]={0,0xF0000000,0x1F};
    
    /* Set the standard configuration with default channel to 63 */
    if(ST_SIGFOX_API_set_std_config(config_words,63)!=0)
    {
      /* Stuck in case of error */
      Fatal_Error();
    }
    ST_SIGFOX_API_reset();

  }
  else
  {
    /* Stuck the application if we are using the FW for FCC and RCZ is not 2 or 4 */
    Fatal_Error();
  }
#endif

	/* Print SigFox sensor node details */
	//PRINTF("\r\nSigFox Sensor Node ID/PAC/Zone\n\r");
	//PRINTF("ID  = %02X\n\r", (int)id);
	//PRINTF("PAC = %02X", (int)pac[0]); for(int i=1; i<8 ; i++) {PRINTF("%02X", pac[i]); }; PRINTF("\n\r");
	//PRINTF("RCZ = %d\n\r",(int)rcz);
	
  /* application main loop */
	for(; count < 50; ++count){
		for(int i = 0; i < 10000; ++i);
		Accelero_Sensor_Handler( LSM6DSL_X_0_handle );
		my_pattern[count] = myacceleration.AXIS_X;
	}
	count %= 50;
	PRINTF("dtm: %d\n\r", dtw(my_pattern, walk_pattern, count));
  while(1)
  {
        Accelero_Sensor_Handler( LSM6DSL_X_0_handle );
		my_pattern[count++] = myacceleration.AXIS_X;
		count %= 50;
		int sim_walk = dtw(my_pattern, walk_pattern, count);
		//PRINTF("dtw: %d", sim_walk);  PRINTF("\n\r");
		for(int i = 0; i < 10000; ++i);
		PRINTF("%d\n", myacceleration.AXIS_X);
		ST_SIGFOX_API_send_frame(customer_data,12,customer_resp,0,dwnlink_req);
		//PRINTF("\r\nMessage sent.\n\r");
#ifdef FOR_FCC
      /* Only for RCZ2 and 4. Since the SigFox base stations are able to receive only on the default ch
      (1 for RCZ2 and 63 for RCZ4), we send an API reset in order to be sure that all the packets will be 
      received by the sigfox network. In this way, the duty cycle will be not ensured by library.
      The user should perfom the TX operation every a minimum of 20s. */
      if(send_call_num==1)
      {
        ST_SIGFOX_API_reset();
        send_call_num=0;
      }
      else
      {
        send_call_num++;
      }
#endif

  }
	
}
#ifdef USE_ENVI_SENSORS
/**
 * @brief  Splits a float into two integer values.
 * @param  in the float value as input
 * @param  out_int the pointer to the integer part as output
 * @param  out_dec the pointer to the decimal part as output
 * @param  dec_prec the decimal precision to be used
 * @retval None
 */
static void floatToInt( float in, int32_t *out_int, int32_t *out_dec, int32_t dec_prec )
{

  *out_int = (int32_t)in;
  if(in >= 0.0f)
  {
    in = in - (float)(*out_int);
  }
  else
  {
    in = (float)(*out_int) - in;
  }
  *out_dec = (int32_t)trunc(in * pow(10, dec_prec));
}
#endif

#if defined(USE_ACC_GYRO) || defined (USE_MAG_ACC)
/**
 * @brief  Handles the accelerometer axes data getting/sending
 * @param  handle the device handle
 * @retval None
 */
static void Accelero_Sensor_Handler( void *handle )
{

  uint8_t id;
  SensorAxes_t acceleration;
  uint8_t status;

  BSP_ACCELERO_Get_Instance( handle, &id );

  BSP_ACCELERO_IsInitialized( handle, &status );

  if ( status == 1 )
  {
    if ( BSP_ACCELERO_Get_Axes( handle, &acceleration ) == COMPONENT_ERROR )
    {
      acceleration.AXIS_X = 0;
      acceleration.AXIS_Y = 0;
      acceleration.AXIS_Z = 0;
    }
		
		/* Perform post processing of sensor data */
		
		
		
		
		
		/* ************************************** */
		
		
		
#if 1
		/* Print sensor data for debuging purposes */
//    sprintf( dataOut, "\r\nACC_X[%d]: %d, ACC_Y[%d]: %d, ACC_Z[%d]: %d\r\n", (int)id, (int)acceleration.AXIS_X, (int)id,
//             (int)acceleration.AXIS_Y, (int)id, (int)acceleration.AXIS_Z );

//    HAL_UART_Transmit( &UartHandle, ( uint8_t * )dataOut, strlen( dataOut ), 5000 );
//		
		
		//PRINTF("\r\nAcc[%d](mg)   -> ACC_X: %d, ACC_Y: %d, ACC_Z: %d\n\r",(int)id, (int)acceleration.AXIS_X,
		//					(int)acceleration.AXIS_Y, (int)acceleration.AXIS_Z );
		myacceleration = acceleration;
#endif

  }
}
#endif

#ifdef USE_ACC_GYRO
/**
 * @brief  Handles the gyroscope axes data getting/sending
 * @param  handle the device handle
 * @retval None
 */
static void Gyro_Sensor_Handler( void *handle )
{

  uint8_t id;
  SensorAxes_t angular_velocity;
  uint8_t status;

  BSP_GYRO_Get_Instance( handle, &id );

  BSP_GYRO_IsInitialized( handle, &status );

  if ( status == 1 )
  {
    if ( BSP_GYRO_Get_Axes( handle, &angular_velocity ) == COMPONENT_ERROR )
    {
      angular_velocity.AXIS_X = 0;
      angular_velocity.AXIS_Y = 0;
      angular_velocity.AXIS_Z = 0;
    }
		
		/* Perform post processing of sensor data */
		
		
		
		
		
		/* ************************************** */
		
		
		
#if 1
		/* Print sensor data for debuging purposes */
//    sprintf( dataOut, "\r\nGYR_X[%d]: %d, GYR_Y[%d]: %d, GYR_Z[%d]: %d\r\n", (int)id, (int)angular_velocity.AXIS_X, (int)id,
//             (int)angular_velocity.AXIS_Y, (int)id, (int)angular_velocity.AXIS_Z );

//    HAL_UART_Transmit( &UartHandle, ( uint8_t * )dataOut, strlen( dataOut ), 5000 );
		
		//PRINTF("\r\nGyr[%d](mdps) -> GYR_X: %d, GYR_Y: %d, GYR_Z: %d\n\r",(int)id, (int)angular_velocity.AXIS_X,
    //         (int)angular_velocity.AXIS_Y, (int)angular_velocity.AXIS_Z );
		myangular_velocity = angular_velocity;
#endif
		
  }
}
#endif

#ifdef USE_MAG_ACC
/**
 * @brief  Handles the magneto axes data getting/sending
 * @param  handle the device handle
 * @retval None
 */
static void Magneto_Sensor_Handler( void *handle )
{

  uint8_t id;
  SensorAxes_t magnetic_field;
  uint8_t status;

  BSP_MAGNETO_Get_Instance( handle, &id );

  BSP_MAGNETO_IsInitialized( handle, &status );

  if ( status == 1 )
  {
    if ( BSP_MAGNETO_Get_Axes( handle, &magnetic_field ) == COMPONENT_ERROR )
    {
      magnetic_field.AXIS_X = 0;
      magnetic_field.AXIS_Y = 0;
      magnetic_field.AXIS_Z = 0;
    }
		
		/* Perform post processing of sensor data */
		
		
		
		
		
		/* ************************************** */
		
		
		
#if 1
		/* Print sensor data for debuging purposes */
//    sprintf( dataOut, "\r\nMAG_X[%d]: %d, MAG_Y[%d]: %d, MAG_Z[%d]: %d\r\n", (int)id, (int)magnetic_field.AXIS_X, (int)id,
//             (int)magnetic_field.AXIS_Y, (int)id, (int)magnetic_field.AXIS_Z );
//    HAL_UART_Transmit( &UartHandle, ( uint8_t * )dataOut, strlen( dataOut ), 5000 );

		//PRINTF("\r\nMag[%d](mgaus)-> MAG_X: %d, MAG_Y: %d, MAG_Z: %d\n\r", (int)id, (int)magnetic_field.AXIS_X,
    //         (int)magnetic_field.AXIS_Y, (int)magnetic_field.AXIS_Z );
		mymagnetic_field = magnetic_field;
#endif
		
  }
}
#endif

#ifdef USE_ENVI_SENSORS
/**
 * @brief  Handles the humidity data getting/sending
 * @param  handle the device handle
 * @retval None
 */
static void Humidity_Sensor_Handler( void *handle )
{

  int32_t d1, d2;
  uint8_t id;
  float humidity;
  uint8_t status;

  BSP_HUMIDITY_Get_Instance( handle, &id );

  BSP_HUMIDITY_IsInitialized( handle, &status );

  if ( status == 1 )
  {
    if ( BSP_HUMIDITY_Get_Hum( handle, &humidity ) == COMPONENT_ERROR )
    {
      humidity = 0.0f;
    }
		
		/* Perform post processing of sensor data */
		
		
		
		
		
		/* ************************************** */
		
		
		
#if 1
		/* Print sensor data for debuging purposes */
    floatToInt( humidity, &d1, &d2, 2 );
//    sprintf( dataOut, "\r\nHUM[%d]: %d.%02d\r\n", (int)id, (int)d1, (int)d2 );
//    HAL_UART_Transmit( &UartHandle, ( uint8_t * )dataOut, strlen( dataOut ), 5000 );

		//PRINTF("\r\nHUM[%d](prcnt): %d.%02d %\r\n", (int)id, (int)d1, (int)d2 );
		myhumidity = humidity;
		
#endif
		
  }
}



/**
 * @brief  Handles the temperature data getting/sending
 * @param  handle the device handle
 * @retval None
 */
static void Temperature_Sensor_Handler( void *handle )
{

  int32_t d1, d2;
  uint8_t id;
  float temperature;
  uint8_t status;

  BSP_TEMPERATURE_Get_Instance( handle, &id );

  BSP_TEMPERATURE_IsInitialized( handle, &status );

  if ( status == 1 )
  {
    if ( BSP_TEMPERATURE_Get_Temp( handle, &temperature ) == COMPONENT_ERROR )
    {
      temperature = 0.0f;
    }
		
		/* Perform post processing of sensor data */
		
		
		
		
		
		/* ************************************** */
		
		
		
#if 1
		/* Print sensor data for debuging purposes */
    floatToInt( temperature, &d1, &d2, 2 );
//    sprintf( dataOut, "\r\nTEMP[%d]: %d.%02d\r\n", (int)id, (int)d1, (int)d2 );
//    HAL_UART_Transmit( &UartHandle, ( uint8_t * )dataOut, strlen( dataOut ), 5000 );

		//PRINTF("\r\nTEMP[%d](degC): %d.%02dC\r\n", (int)id, (int)d1, (int)d2 );
		mytemperature = temperature;
#endif
		
  }
}



/**
 * @brief  Handles the pressure sensor data getting/sending
 * @param  handle the device handle
 * @retval None
 */
static void Pressure_Sensor_Handler( void *handle )
{

  int32_t d1, d2;
  uint8_t id;
  float pressure;
  uint8_t status;

  BSP_PRESSURE_Get_Instance( handle, &id );

  BSP_PRESSURE_IsInitialized( handle, &status );

  if( status == 1 )
  {
    if ( BSP_PRESSURE_Get_Press( handle, &pressure ) == COMPONENT_ERROR )
    {
      pressure = 0.0f;
    }
		
		/* Perform post processing of sensor data */
		
		
		
		
		
		/* ************************************** */
		
#if 1
		/* Print sensor data for debuging purposes */
    floatToInt( pressure, &d1, &d2, 2 );
//    sprintf(dataOut, "\r\nPRESS[%d]: %d.%02d\r\n", (int)id, (int)d1, (int)d2);
//    HAL_UART_Transmit( &UartHandle, ( uint8_t * )dataOut, strlen( dataOut ), 5000 );

		//PRINTF("\r\nPRESS[%d](mbar): %d.%02d\r\n", (int)id, (int)d1, (int)d2);
		mypressure = pressure;
#endif
		
  }
}
#endif


/**
 * @brief  Initialize all sensors
 * @param  None
 * @retval None
 */
static void initializeAllSensors( void )
{
	deinitializeAllSensors();
	
#ifdef USE_ACC_GYRO	
  BSP_ACCELERO_Init( LSM6DSL_X_0, &LSM6DSL_X_0_handle );
  BSP_GYRO_Init( LSM6DSL_G_0, &LSM6DSL_G_0_handle );
#endif
#ifdef USE_MAG_ACC
  BSP_ACCELERO_Init( LSM303AGR_X_0, &LSM303AGR_X_0_handle );
  BSP_MAGNETO_Init( LSM303AGR_M_0, &LSM303AGR_M_0_handle );
#endif
#ifdef USE_ENVI_SENSORS
	BSP_HUMIDITY_Init( HTS221_H_0, &HTS221_H_0_handle );
  BSP_TEMPERATURE_Init( HTS221_T_0, &HTS221_T_0_handle );
//  BSP_TEMPERATURE_Init( LPS22HB_T_0, &LPS22HB_T_0_handle );	//optional
  BSP_PRESSURE_Init( LPS22HB_P_0, &LPS22HB_P_0_handle );
#endif
}

/**
 * @brief  Initialize all sensors
 * @param  None
 * @retval None
 */
static void deinitializeAllSensors( void )
{

#ifdef USE_ACC_GYRO	
  BSP_ACCELERO_DeInit(&LSM6DSL_X_0_handle );
  BSP_GYRO_DeInit(&LSM6DSL_G_0_handle );
#endif
#ifdef USE_MAG_ACC
  BSP_ACCELERO_DeInit(&LSM303AGR_X_0_handle );
  BSP_MAGNETO_DeInit(&LSM303AGR_M_0_handle );
#endif
#ifdef USE_ENVI_SENSORS
  BSP_HUMIDITY_DeInit(&HTS221_H_0_handle );
  BSP_TEMPERATURE_DeInit(&HTS221_T_0_handle );
//  BSP_TEMPERATURE_DeInit(&LPS22HB_T_0_handle );	//optional
  BSP_PRESSURE_DeInit(&LPS22HB_P_0_handle );
#endif
}

/**
 * @brief  Enable all sensors
 * @param  None
 * @retval None
 */
static void enableAllSensors( void )
{

#ifdef USE_ACC_GYRO	
  BSP_ACCELERO_Sensor_Enable( LSM6DSL_X_0_handle );
  BSP_GYRO_Sensor_Enable( LSM6DSL_G_0_handle );
#endif
#ifdef USE_MAG_ACC
  BSP_ACCELERO_Sensor_Enable( LSM303AGR_X_0_handle );
  BSP_MAGNETO_Sensor_Enable( LSM303AGR_M_0_handle );
#endif
#ifdef USE_ENVI_SENSORS
  BSP_HUMIDITY_Sensor_Enable( HTS221_H_0_handle );
  BSP_TEMPERATURE_Sensor_Enable( HTS221_T_0_handle );
//  BSP_TEMPERATURE_Sensor_Enable( LPS22HB_T_0_handle );
  BSP_PRESSURE_Sensor_Enable( LPS22HB_P_0_handle );
#endif
	
	SdkDelayMs(10);
}



/**
 * @brief  Disable all sensors
 * @param  None
 * @retval None
 */
static void disableAllSensors( void )
{

#ifdef USE_ACC_GYRO		
  BSP_ACCELERO_Sensor_Disable( LSM6DSL_X_0_handle );
  BSP_GYRO_Sensor_Disable( LSM6DSL_G_0_handle );
#endif
#ifdef USE_MAG_ACC
  BSP_ACCELERO_Sensor_Disable( LSM303AGR_X_0_handle );
  BSP_MAGNETO_Sensor_Disable( LSM303AGR_M_0_handle );
#endif
#ifdef USE_ENVI_SENSORS
  BSP_HUMIDITY_Sensor_Disable( HTS221_H_0_handle );
  BSP_TEMPERATURE_Sensor_Disable( HTS221_T_0_handle );
//  BSP_TEMPERATURE_Sensor_Disable( LPS22HB_T_0_handle );	//optional
  BSP_PRESSURE_Sensor_Disable( LPS22HB_P_0_handle );
#endif
}



#ifdef  USE_FULL_ASSERT
/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param file: pointer to the source file name
* @param line: assert_param error line source number
* @retval : None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number */
  printf("Wrong parameters value: file %s on line %d\r\n", file, line);
  
  /* Infinite loop */
  while (1)
  {
  }
}
#endif


/******************* (C) COPYRIGHT 2016 STMicroelectronics *****END OF FILE****/
