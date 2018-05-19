/* ###################################################################
**     Filename    : main.c
**     Project     : MotoresMovimiento
**     Processor   : MCF51QE128CLK
**     Version     : Driver 01.00
**     Compiler    : CodeWarrior ColdFireV1 C Compiler
**     Date/Time   : 2018-05-09, 15:06, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.00
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "Bit1.h"
#include "Bit2.h"
#include "PWM1.h"
#include "PWM2.h"
#include "Bit3.h"
#include "Bit4.h"
#include "AS1.h"
/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

/* User includes (#include below this line is not maintained by Processor Expert) */
void SetPWM1(unsigned short porc, bool dir);
void SetPWM2(unsigned short porc, bool dir);
unsigned char estado=ESPERAR;
unsigned int enviados=2;
unsigned char duty;
bool dir;

void main(void)
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  	  for(;;){
  		  
  		  switch(estado)
  		  {
  		  case ESPERAR:
  			  break;
  		  case PWM_set:
  			  if(AS1_GetCharsInRxBuf()>0) //Si hay información en el Buffer, entra en la lectura de él
  			  {
  				  AS1_RecvChar(&duty);	//Almacena en la variable duty lo recibido desde el serial
  				  AS1_ClearRxBuf();
  				
  			  }
  			  dir=duty & 0x40 >> 6; //0x40=0100 0000
  			  estado= duty & 0x80 >> 7; // 0x80=1000 0000 Decido a que motor le cambiaré el duty cycle
  			  break;
  		  case PWM_m1: //Definir duty cycle motor 1
  			  SetPWM1(duty,dir);
  			  estado= ESPERAR;
  			  break;
  		  case PWM_m2: //Definir duty cycle motor 1
  			  SetPWM2(duty,dir);
  			  estado=ESPERAR;
  			  break;
  		  
  		  }
  	  }
  /* For example: for(;;) { } */

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/
void SetPWM1(unsigned short porc, bool dir)
  {	
  	unsigned short param;
  	porc=porc & 0x3f; //0x3f=0011 1111
  	param = 65535*(porc)/100;
  	PWM1_SetRatio16(param);
  	Bit1_PutVal(dir);
  }
  void SetPWM2(unsigned short porc, bool dir)
  {
  	unsigned short param;
  	porc= porc & 0x3f; //0x3f=0011 1111
  	param = 65535*(porc)/100;
  	PWM2_SetRatio16(param);
  	Bit2_PutVal(dir);
  }
/* END main */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.3 [05.09]
**     for the Freescale ColdFireV1 series of microcontrollers.
**
** ###################################################################
*/
