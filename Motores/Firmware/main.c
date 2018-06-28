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
unsigned int enviados=2;
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
  			  //if(AS1_GetCharsInRxBuf()>0) //Si hay informaci�n en el Buffer, entra en la lectura de �l
  			  //{
  				 // AS1_RecvChar(&duty);	//Almacena en la variable duty lo recibido desde el serial
  				  //AS1_ClearRxBuf();
  				
  			  //}
  			  //dir=duty & 0x40 >> 6; //0x40=0100 0000
  			  //estado= duty & 0x80 >> 7; // 0x80=1000 0000 Decido a que motor le cambiar� el duty cycle
  			  AD1_Measure(TRUE);
  			  AD1_GetValue16(&sharp); // guardo en sharp lo que midi� el ADC del sensor
  			  sharp = sharp>>4;		//los 4 bits menos significativos no guardan informaci�n
  			  sharp_motores(sharp);
  			  SetPWM_r_n(duty_r_n,dir_r_n);
  			  SetPWM_v_b(duty_v_b,dir_v_b);
  			  estado = ESPERAR;
  			  /*Parte donde a partir de condicionales defino duty1 y duty2 dependiendo de lo que se lea en el ADC*/
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
  	//porc=porc & 0x3f; //0x3f=0011 1111
	PWM1_SetDutyUS(porc);
  	//PWM1_SetRatio16(porc);
  	Bit1_PutVal(dir);
  	Bit3_PutVal(dir);
  }
void SetPWM_v_b(unsigned short porc, bool dir)
  {
  	//porc= porc & 0x3f; //0x3f=0011 1111
	PWM2_SetDutyUS(porc);
	//PWM2_SetRatio16(porc);
  	Bit2_PutVal(dir);
  	Bit4_PutVal(dir);
  }
  //La curva que describe mejor el comportamiento del Sharp en t�rminos de distancia vs V, es un polinomio de 6to grado
  //En esta funci�n se determina a que distancia corresponde el voltaje le�do (v)
  void sharp_motores(unsigned short v)
  {
    	if(v<=2000)//Pico maximo de la curva
    	{ 
    		//multiplicaci�n por coeficientes del polinomio
  		
  		//Se detiene si no hay obst�culo
  		if(v>=300 && v<=1300)
  		{
  			
  			dir_r_n = 0;	//adelante
  			dir_v_b = 0; 	//adelante
  			duty_r_n= PWMf_r_n;//+(Vref-v)*Kp_r_n;
  			duty_v_b= PWMf_v_b;//+(Vref-v)*Kp_v_b;
  			//duty_r_n=65535*0.35; //65535(0xFFFF)*0.8=52427.2 (80% duty cycle) conforme se acerca al obstaculo disminuye rapidez
  			//duty_v_b=65535*0.3; //65535(0xFFFF)*0.8=52427.2 (80% duty cycle) conforme se acerca al obstaculo disminuye rapidez
  			
  		}
  		else
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
  		//duty_r_n=65535*0.35; //65535(0xFFFF)*0.8=52427.2 (80% duty cycle) conforme se acerca al obstaculo disminuye rapidez
  		//duty_v_b=65535*0.3; //65535(0xFFFF)*0.8=52427.2 (80% duty cycle) conforme se acerca al obstaculo disminuye rapidez		
    	}
  }
  /*{
  	if(v<=1.92)//Pico maximo de la curva
  	{ 
  		p2 = v*v; 	// v^2
		p3 = p2*v;	//v^3
		p4 = p3*v;	//v^4
		p5 = p4*v;	//v^5
		p6 = p5*v;	//v^6
		//multiplicaci�n por coeficientes del polinomio
		c6 = 56.1*p6;
		c5 = -441.64*p5;
		c4 = 1413.3*p4;
		c3 = -2364.4*p3;
		c2 = 2207.7*p2;
		c1 = -1124.3*v;
		
		x = c6+c5+c4+c3+c2+c1+268.28;	
		
		//Se detiene si no hay obst�culo
		if(x>=9 && x<=59)
		{
			dir_r_n = 0;	//adelante
			dir_v_b = 0; 	//adelante
			duty_r_n= PWMf_r_n;
			duty_v_b= PWMf_v_b;
			//duty_r_n=65535*0.35; //65535(0xFFFF)*0.8=52427.2 (80% duty cycle) conforme se acerca al obstaculo disminuye rapidez
			//duty_v_b=65535*0.3; //65535(0xFFFF)*0.8=52427.2 (80% duty cycle) conforme se acerca al obstaculo disminuye rapidez
		}
		else if(x>59 || (7<x && x<9))
		{
			duty_r_n = 0;
			duty_v_b = 0;
			dir_r_n = 1;	//adelante
			dir_v_b = 1; 	//adelante
		}
  	}
  	else
  	{
  		dir_r_n = 1;	//atras
		dir_v_b = 1; 	//atras
		duty_r_n= PWMb_r_n;
		duty_v_b= PWMb_v_b;
		//duty_r_n=65535*0.35; //65535(0xFFFF)*0.8=52427.2 (80% duty cycle) conforme se acerca al obstaculo disminuye rapidez
		//duty_v_b=65535*0.3; //65535(0xFFFF)*0.8=52427.2 (80% duty cycle) conforme se acerca al obstaculo disminuye rapidez		
  	}
  }*/
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
