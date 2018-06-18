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
#include "AS2.h"
#include "Bit5.h"
#include "Bit6.h"
#include "Bit7.h"
#include "Bit8.h"

#include "AD1.h"
#include "TI1.h"
/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "math.h"
/* User includes (#include below this line is not maintained by Processor Expert) */

unsigned char CodError;
unsigned short Reg_len = 20;
unsigned char Reg_code[20] = {'C','R',' ','1','8',' ','3','6',' ','1','7',' ','2',' ','1','9',' ','3','2','\r'}; //CR [ reg1 value1 [reg2 value2 ... reg16 value16] ]\r  
unsigned short Track_len = 23;
unsigned char Track_code[23] = "TC 45 75 32 60 102 126\r";//"TC 145 151 15 17 105 109\r"; //TC [Rmin Rmax Gmin Gmax Bmin Bmax]\r  :TC 130 255 0 0 30 30 
unsigned int i = 0;
unsigned char Mx= 0;
bool estado_cam=0;
unsigned char rChar = 0;

void SetPWM_r_n(unsigned short porc, bool dir);
void SetPWM_v_b(unsigned short porc, bool dir);
void sharp_motores(float c);
unsigned char estado= ESPERAR;
unsigned int enviados=2;
unsigned short sharp = 0;
unsigned short sharp_temp = 0;
float voltaje = 0;
unsigned short duty_r_n = 0;//motor rojo-negro
unsigned short duty_v_b = 0;//motor verde-blanco
bool dir_r_n = 0;//motor rojo-negro
bool dir_v_b = 0;//motor verde-blanco
short error_theta=0;
unsigned short P_der=0.8; //Constante de control proporcional
unsigned short P_izq=5; //Constante de control proporcional

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
float x_prev=0, x_delta=0;
     

void main(void)
 {
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  
  AD1_Start();
  SetPWM_r_n(0, 1);
  SetPWM_v_b(0, 1);
  
  AS1_SendChar('\r');
  
  Bit5_PutVal(0);
  
  //Cpu_Delay100US(20000);
 
  //AS1_SendChar('\r');
    
  while(estado == 0)
  {
    if(estado_cam == 1)
      {
        CodError = AS1_SendChar('\r');
        estado_cam = 0;
        estado = 1;
      }
  }
  
  while(estado == 1)
  {
    if (estado_cam== 1){    //
      for(i = 0; i < Reg_len; i++){
        CodError = AS1_SendChar(Reg_code[i]);
        //Cpu_Delay100US(1);
      }
      estado = 0;
      estado_cam = 0;
    }
  }
  
  while(estado == 0)
  {
    if (estado_cam== 1){    //
      for(i = 0; i < Track_len; i++){
        CodError = AS1_SendChar(Track_code[i]);
        //Cpu_Delay100US(1);
      }
      estado = 1;
      estado_cam = 0;
    }
  }
  
  estado = 0;
  Bit5_PutVal(1);
  Bit7_PutVal(1);
  TI1_Enable();
  
  	  for(;;){

  		  
  		
  		  switch(estado)
  		  {
  		  case ESPERAR:
  			  break;
  		  case UBICAR_PELOTA:
  			  error_theta=Mx-40;
  			  if(error_theta<-10)
				  //Buscar objeto que está a la derecha
			  {
  				  
  				duty_r_n=PWMb_r_n;
  				duty_v_b=PWMf_v_b + error_theta*P_der;
				dir_r_n=0;
				dir_v_b=0;
				SetPWM_r_n(duty_r_n,dir_r_n);
				SetPWM_v_b(duty_v_b,dir_v_b);
			  }
			  else if (error_theta>10)
				  //Buscar objeto que está a la izquierda
			  {
				duty_r_n=PWMf_r_n;
				duty_v_b=PWMb_v_b + error_theta*P_izq;
				dir_r_n=0;
				dir_v_b=0;
				SetPWM_r_n(duty_r_n,dir_r_n);
				SetPWM_v_b(duty_v_b,dir_v_b);
			  }
			  else
				  estado=AVANZA;  				  			  
			  break;
		  
  		  case AVANZA:
			AD1_Measure(TRUE);
			AD1_GetValue16(&sharp); // guardo en sharp lo que midió el ADC del sensor
			sharp = sharp>>4;		//los 4 bits menos significativos no guardan información
			voltaje = sharp*1.92/2440; // guardo la lectura como voltaje; (max medido ADC)1.96V->0xFFF=4095  
			sharp_motores(voltaje);
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
  //La curva que describe mejor el comportamiento del Sharp en términos de distancia vs V, es un polinomio de 6to grado
  //En esta función se determina a que distancia corresponde el voltaje leído (v)
  void sharp_motores(float v)
  {
  	if(v<=1.92)//Pico maximo de la curva
  	{ 
  		/*p2 = pow(v,2); 	// v^2
		p3 = p2*v;	//v^3
		p4 = p3*v;	//v^4
		p5 = p4*v;	//v^5
		p6 = p5*v;	//v^6*/
  		
		//multiplicación por coeficientes del polinomio
		c6 = 56.1*pow(v,6);
		c5 = -441.64*pow(v,5);
		c4 = 1413.3*pow(v,4);
		c3 = -2364.4*pow(v,3);
		c2 = 2207.7*pow(v,2);
		c1 = -1124.3*v;
		
		x = c6+c5+c4+c3+c2+c1+268.28;	
		
		//Se detiene si no hay obstáculo
		if(x>=9 && x<=59)
		{
			x_delta=x-x_prev;
			dir_r_n = 0;	//adelante
			dir_v_b = 0; 	//adelante
			duty_r_n= PWMf_r_n + x_delta;
			duty_v_b= PWMf_v_b + x_delta;
			x_prev=x;
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
