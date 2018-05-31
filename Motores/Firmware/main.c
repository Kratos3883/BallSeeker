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
#include "AD1.h"
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
unsigned short sharp = 0;
unsigned short sharp_temp = 0;
float voltaje = 0;
unsigned short duty1 = 10;
unsigned short duty2 = 10;
unsigned short duty;
bool dir1 = 0;
bool dir2 = 0;

double p1=0;
double p2=0;
double p3=0;
double p4=0;
double p5=0;
double p6=0;
double c6= 0;
double c5= 0;
double c4= 0;
double c3= 0;
double c2= 0;
double c1= 0;
double x = 0;

     

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
  			  AD1_Measure(TRUE);
  			  AD1_GetValue16(&sharp); // guardo en sharp lo que midió el ADC del sensor
  			  sharp = sharp>>4;		//los 4 bits menos significativos no guardan información
  			  voltaje = sharp*1.92/2440; // guardo la lectura como voltaje; (max medido ADC)1.96V->0xFFF=4095  
  			  sharp_motores(voltaje);
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
  			  estado= PWM_set;
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
  //La curva que describe mejor el comportamiento del Sharp en términos de distancia vs V, es un polinomio de 6to grado
  //En esta función se determina a que distancia corresponde el voltaje leído (v)
  void sharp_motores(float v)
  {
  	if(v<=0.92)//A partir del primer mínimo después del máximo de voltaje
  	{ 
  		p2 = v*v; 	// v^2
		p3 = p2*v;	//v^3
		p4 = p3*v;	//v^4
		p5 = p4*v;	//v^5
		p6 = p5*v;	//v^6
		//multiplicación por coeficientes del polinomio
		c6 = 56.1*p6;
		c5 = -441.64*p5;
		c4 = 1413.3*p4;
		c3 = -2364.4*p3;
		c2 = 2207.7*p2;
		c1 = -1124.3*v;
		
		x = c6+c5+c4+c3+c2+c1+268.28;	
		
		//Se detiene si no hay obstáculo
		if(x>=15 && x<=20)
		{
			duty = 0;
		}
		//En este rango de distancias busco la pelota, avanzo hacia adelante
		if(x>20 && x<=59) 
		{
			dir1 = 0;	//adelante
			dir2 = 0; 	//adelante
			Bit3_PutVal(FALSE);	//enciende los leds PTC2
			duty=65535*0.4; //65535(0xFFFF)*0.8=52427.2 (80% duty cycle) conforme se acerca al obstáculo disminuye rapidez
			//duty=(-0.4909*c)+81.899;//duty=(-69.024*c)+61.973;	
			//duty=(-983.01*duty)+55704; // entre 70% y 10% //
		}
		if(x>59)
		{
			duty = 0;
		}
  	
  	}
  	else
  	{
  		
  		dir1 = 1;	//atras
		dir2 = 1; 	//atras
		Bit3_PutVal(FALSE);	//enciende los leds PTC2
		duty=65535*0.35;	
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
