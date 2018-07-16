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
#include "Bit9.h"
#include "Bit10.h"
#include "Bit11.h"
#include "Bit12.h"
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
unsigned char Track_code[23] = "TC 35 75 30 60 100 127\r";//"TC 145 151 15 17 105 109\r"; //TC [Rmin Rmax Gmin Gmax Bmin Bmax]\r  :TC 130 255 0 0 30 30 
unsigned int i = 0;
unsigned char Mx= 0;
unsigned char SRmin = 0, SRmax = 0, SGmin = 0, SGmax = 0, SBmin = 0, SBmax = 0;
bool margarita=0;
bool estado_cam=0;
unsigned char rChar = 0;

void SetPWM_r_n(unsigned short porc, bool dir);
void SetPWM_v_b(unsigned short porc, bool dir);
void Reconfiguracion_Facial();
void sharp_motores(unsigned short v);
unsigned char estado= ESPERAR;
unsigned int enviados=2;
unsigned short sharp = 0;
unsigned short sharp_av = 0;
unsigned short sharp_temp = 0;
unsigned short duty_r_n = 0;//motor rojo-negro
unsigned short duty_v_b = 0;//motor verde-blanco
bool dir_r_n = 0;//motor rojo-negro
bool dir_v_b = 0;//motor verde-blanco
unsigned char r_n = 0;//motor rojo-negro
unsigned char v_b = 0;//motor verde-blanco
short error_theta=0,error_theta_prev=0;
float Kp_der=1; //Constante de control proporcional
float Kp_izq=0; //Constante de control proporcional
float Kd_der=0; //Constante de control derivativo
float Kd_izq=0; //Constante de control derivativo

bool error_flag=0, parate_flag=0, retro_flag=0;		//Se ha recibido centroide, pasa a UBICAR_PELOTA


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
  
  Cpu_Delay100US(20000);

  Bit5_PutVal(1);
  Bit7_PutVal(1);
  
  TI1_Enable();
  
  	  for(;;){

  		  	
  		  switch(estado)
  		  {
  		  case ESPERAR:
  			  break;
  			  
  		  	  
  		  case PWM:
  			  if (r_n>127)	//atras
  			  {  
  				  dir_r_n=1;
  				  duty_r_n=(r_n-0x80)*10;  			  
  			  }
  			  else
  			  {
  				  dir_r_n=0;
  				  duty_r_n=1000 - 10*r_n;
  			  }
  			  if (v_b>127)	//atras
  			  {  
  				  dir_v_b=1;
  				  duty_v_b=(v_b-0x80)*10;	  
  			  }
  			  else
  			  {
  				dir_v_b=0;
  				duty_v_b=1000 - 10*v_b;
  			  }
  			SetPWM_r_n(duty_r_n,dir_r_n);
  			SetPWM_v_b(duty_v_b,dir_v_b);
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
