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
#include "math.h"
/* User includes (#include below this line is not maintained by Processor Expert) */
void SetPWM1(unsigned short porc, bool dir);
void SetPWM2(unsigned short porc, bool dir);
void sharp_motores(float c);
unsigned char estado= PWM_set;
unsigned int enviados=2;
unsigned short S;
unsigned float SI;
unsigned short duty1 = 10;
unsigned short duty2 = 10;
unsigned short duty;
bool dir1 = 1;
bool dir2 = 1;
float x1,x2;

void main(void)
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  
  AD1_Start();
  	  for(;;){
  		  
  		  switch(estado)
  		  {
  		  case ESPERAR:
  			  break;
  		  case PWM_set:
  			  //if(AS1_GetCharsInRxBuf()>0) //Si hay información en el Buffer, entra en la lectura de él
  			  //{
  				 // AS1_RecvChar(&duty);	//Almacena en la variable duty lo recibido desde el serial
  				  //AS1_ClearRxBuf();
  				
  			  //}
  			  //dir=duty & 0x40 >> 6; //0x40=0100 0000
  			  //estado= duty & 0x80 >> 7; // 0x80=1000 0000 Decido a que motor le cambiaré el duty cycle
  			  AD1_Mueasure(TRUE);
  			  AD1_GetValue16(&S); // tomo lo que me mide el sharp
  			  SI = ((float)S+22.486)/1357.3; // lo paso a float para guardarlo como voltaje y operarlo en la resolvente
  			  sharp_motores(SI);
  			  estado = PWM_m1;
  			  /*Parte donde a partir de condicionales defino duty1 y duty2 dependiendo de lo que se lea en el ADC*/
  			  break;
  		  case PWM_m1: //Definir duty cycle motor 1
  			  SetPWM1(duty,dir1);
  			  estado= PWM_m2;
  			  break;
  		  case PWM_m2: //Definir duty cycle motor 1
  			  SetPWM2(duty,dir2);
  			  //estado=ESPERAR;
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
  	//porc=porc & 0x3f; //0x3f=0011 1111
  	PWM1_SetRatio16(porc);
  	Bit1_PutVal(dir);
  }
  void SetPWM2(unsigned short porc, bool dir)
  {
  	//porc= porc & 0x3f; //0x3f=0011 1111
  	PWM2_SetRatio16(porc);
  	Bit2_PutVal(dir);
  }
  
  void sharp_motores(float c){
  	if(c<=1.34){
  		  			c=1.9191-c;
  		  			disc=pow(0.0775, 2.0)-(4.0*0.001*c);
  		  				if(disc>0.0){
  		  					duty=((0.0775-sqrt(disc))/(2.0*0.001));
  		  					duty=(-983.01*duty)+55704; // entre 70% y 10% //
  		  					//debug=duty;
  		  				}else{
  		  						c=1.9191-c;
  		  						duty=(-69.024*c)+61.973;	
  		  						duty=(-983.01*duty)+55704; // entre 70% y 10% //
  		  						//duty=20783.25; //// 32% ////38.75cm
  		  					}
  		  			}else
  		  			{
  		  			duty=52427.2; //// 80% ////
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
