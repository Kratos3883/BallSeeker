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
#include "AS1.h"
#include "AS2.h"
#include "Bit5.h"
#include "Bit6.h"
#include "Bit7.h"
#include "Bit8.h"
#include "Bit9.h"
#include "Bit10.h"
#include "Bit11.h"
#include "Bit12.h"
#include "Bit3.h"
#include "Bit4.h"
#include "AD1.h"
#include "TI1.h"
/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "math.h"
/* User includes (#include below this line is not maintained by Processor Expert) */
void SetPWM_r_n(unsigned short porc, bool dir);
void SetPWM_v_b(unsigned short porc, bool dir);
void sharp_motores(unsigned short v);
unsigned char estado= PWM_set;
unsigned short sharp = 0;
unsigned short sharp_temp = 0;
unsigned short duty_r_n = 0;//motor rojo-negro
unsigned short duty_v_b = 0;//motor verde-blanco
unsigned short duty;
bool dir_r_n = 1;//motor rojo-negro
bool dir_v_b = 1;//motor verde-blanco



void main(void)
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  
  AD1_Start();
  TI1_Enable();
  	  for(;;){
  		
  		  switch(estado)
  		  {
  		  case ESPERAR:
  			  break;
  		  case PWM_set:
  			  AD1_GetValue16(&sharp); // se guarda en sharp lo que midió el ADC del sensor
  			  sharp = sharp>>4;		//los 4 bits menos significativos no guardan informacion
  			  sharp_motores(sharp);
  			  SetPWM_r_n(duty_r_n,dir_r_n);
  			  SetPWM_v_b(duty_v_b,dir_v_b);
  			  estado = ESPERAR;
  			  break;
  		   		  
  		  }
  		  
  		  
  	  }
  /* For example: for(;;) { } */

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/
void SetPWM_r_n(unsigned short porc, bool dir)
  {	
	PWM1_SetDutyUS(porc);
  	Bit1_PutVal(dir);
  	Bit3_PutVal(dir);
  }
void SetPWM_v_b(unsigned short porc, bool dir)
  {
	PWM2_SetDutyUS(porc);
  	Bit2_PutVal(dir);
  	Bit4_PutVal(dir);
  }
  void sharp_motores(unsigned short v)
  // En esta función se toman los datos crudos que vienen del ADC los valores significan los siguiente:
  // 2000-> 10cm 
  // 1300-> 15cm
  // 300-> distancia mayores a 60cm

  {
    	if(v<=2000)//Pico maximo de la curva
    	{ 
    	
  		
  		if(v>=300 && v<=1300)	//Avanza si hay objeto detectado (entre 15 y 60cm)
  		{
  			dir_r_n = 0;	//adelante
  			dir_v_b = 0; 	//adelante
  			duty_r_n= PWMf_r_n;
  			duty_v_b= PWMf_v_b;
  			
  		}
  		else	//Mantente detenido si no hay objeto cercano
  		{
  			duty_r_n = 0;
  			duty_v_b = 0;
  			dir_r_n = 1;	
  			dir_v_b = 1; 	
  		}
    	}
    	else
    	{
    	dir_r_n = 1;	//atras
  		dir_v_b = 1; 	//atras
  		duty_r_n= PWMb_r_n;
  		duty_v_b= PWMb_v_b;

    	}
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
