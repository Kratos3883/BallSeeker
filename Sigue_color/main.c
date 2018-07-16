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
unsigned short sharp = 0;
unsigned short sharp_av = 0;
unsigned short duty_r_n = 0;//motor rojo-negro
unsigned short duty_v_b = 0;//motor verde-blanco
bool dir_r_n = 0;//motor rojo-negro
bool dir_v_b = 0;//motor verde-blanco
short error_theta=0;
unsigned short P_der=2; //Constante de control proporcional
unsigned short P_izq=5; //Constante de control proporcional
bool Rx_Mx=0;		//Se ha recibido centroide, pasa a UBICAR_PELOTA

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

  Reconfiguracion_Facial(); // Reconfigurar la cámara para definir el color a seguir

  Bit5_PutVal(1);
  Bit7_PutVal(1);
  
  TI1_Enable();
  
  	  for(;;){

  		  
  		
  		  switch(estado)
  		  {
  		  case ESPERAR:
  			  break;
  			  
  		  case MEDIR_DISTANCIA:
  			  AD1_GetValue16(&sharp); // guardo en sharp lo que midió el ADC del sensor
  			  sharp = sharp>>4;		//los 4 bits menos significativos no guardan información
  			  sharp_av=(sharp_av+sharp)/2;
  			  estado=ESPERAR;
  			  
  			  break;
  		  case UBICAR_PELOTA:
  			  error_theta=Mx-40;
  			  Rx_Mx=0;
  			  if(error_theta<-17)
				  //Buscar objeto que está a la derecha
			  {
  				  
  				duty_r_n=PWMf_r_n;
  				duty_v_b=PWMf_v_b + error_theta*P_der;	
				dir_r_n=0;
				dir_v_b=0;
				SetPWM_r_n(duty_r_n,dir_r_n);
				SetPWM_v_b(duty_v_b,dir_v_b);
			  }
			  else if (error_theta>17)
				  //Buscar objeto que está a la izquierda
			  {
				duty_r_n=PWMf_r_n;
				duty_v_b=PWMf_v_b + error_theta*P_izq;
				dir_r_n=0;
				dir_v_b=0;
				SetPWM_r_n(duty_r_n,dir_r_n);
				SetPWM_v_b(duty_v_b,dir_v_b);
			  }
			  else
				  estado=AVANZA;  				  			  
			  break;
		  
  		  case AVANZA: 
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
	if(sharp>1500 && sharp<2000)
	{	
		PWM1_SetDutyUS(0);
		Bit1_PutVal(1);
		Bit3_PutVal(1);
	}
	else
	{
		PWM1_SetDutyUS(porc);
		Bit1_PutVal(dir);
		Bit3_PutVal(dir);
	}	
}
void SetPWM_v_b(unsigned short porc, bool dir)
{
  	
	if(sharp>1500 && sharp<2000)
	{	
		PWM2_SetDutyUS(0);
		Bit2_PutVal(1);
		Bit4_PutVal(1);
	}
	else
	{
		PWM2_SetDutyUS(porc);
		Bit2_PutVal(dir);
		Bit4_PutVal(dir);
	}
}

  void sharp_motores(unsigned short v)
  // En esta función se toman los datos crudos que vienen del ADC los valores significan los siguiente:
  // 2000-> 10cm 
  // 1300-> 15cm
  // 300-> distancia mayores a 60cm

  {
  	if(v<=2000)
  	{ 
		if(v>=300 && v<=1300)	//Avanza si hay objeto detectado
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
  
  void Reconfiguracion_Facial()
  {
	 	  
	  AS1_SendChar('\r'); //limpiar info en cámara
	  
	  margarita = 0;
	  
	  while(margarita == 0)
	  {
	    if (estado_cam== 1){    //
	      for(i = 0; i < Reg_len; i++){
	        CodError = AS1_SendChar(Reg_code[i]);	//Configuración de registros
	      }
	      margarita = 1;
	      estado_cam = 0;
	    }
	  }	    

	  while(margarita == 1)	
	  {
	    if(estado_cam == 1)
	      {
		 	AS1_SendChar('G');
		  	AS1_SendChar('M');
		  	AS1_SendChar('\r');
		  	estado_cam = 0;
	        margarita = 0;
	      }
	  }
	  
	  while(margarita == 0)	
	  {
	    if(estado_cam == 1)
	      {
	        CodError = AS1_SendChar('\r');	//limpiar info en cámara
	        estado_cam = 0;
	        margarita = 1;
	      }
	  }
	  	  
	  while(margarita == 1)	
	  {
	    if (estado_cam== 1){    //recibe dos puntos, espera una orden
	    	CodError = AS1_SendChar('T');
	    	CodError = AS1_SendChar('C');
	    	CodError = AS1_SendChar(' ');
	    	
			CodError = AS1_SendChar(((SRmin/100) % 10) + 0x30);
			CodError = AS1_SendChar(((SRmin/10) % 10) + 0x30);
			CodError = AS1_SendChar(((SRmin) % 10) + 0x30);
			
			CodError = AS1_SendChar(' ');
			
			CodError = AS1_SendChar(((SRmax/100) % 10) + 0x30);
			CodError = AS1_SendChar(((SRmax/10) % 10) + 0x30);
			CodError = AS1_SendChar(((SRmax) % 10) + 0x30);
			
			CodError = AS1_SendChar(' ');
			
			CodError = AS1_SendChar(((SGmin/100) % 10) + 0x30);
			CodError = AS1_SendChar(((SGmin/10) % 10) + 0x30);
			CodError = AS1_SendChar(((SGmin) % 10) + 0x30);
			
			CodError = AS1_SendChar(' ');
			
			CodError = AS1_SendChar(((SGmax/100) % 10) + 0x30);
			CodError = AS1_SendChar(((SGmax/10) % 10) + 0x30);
			CodError = AS1_SendChar(((SGmax) % 10) + 0x30);
			
			CodError = AS1_SendChar(' ');
			
			CodError = AS1_SendChar(((SBmin/100) % 10) + 0x30);
			CodError = AS1_SendChar(((SBmin/10) % 10) + 0x30);
			CodError = AS1_SendChar(((SBmin) % 10) + 0x30);
			
			CodError = AS1_SendChar(' ');
			
			CodError = AS1_SendChar(((SBmax/100) % 10) + 0x30);
			CodError = AS1_SendChar(((SBmax/10) % 10) + 0x30);
			CodError = AS1_SendChar(((SBmax) % 10) + 0x30);
			
			CodError = AS1_SendChar('\r');

		      
		    margarita = 0;
		    estado_cam = 0;
	    
	    }

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
