/* ###################################################################
**     Filename    : main.c
**     Project     : Motores
**     Processor   : MCF51QE128CLK
**     Version     : Driver 01.00
**     Compiler    : CodeWarrior ColdFireV1 C Compiler
**     Date/Time   : 2018-04-30, 14:52, # CodeGen: 0
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
#include "Bit3.h"
#include "Bit4.h"
#include "AD1.h"
#include "TI1.h"
#include "PWM1.h"
#include "PWM2.h"
#include "AS1.h"
#include "AS2.h"
/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

unsigned char palabra = 0;
unsigned char palabra_recepcion_PC1[];
unsigned char palabra_recepcion_PC2[];
unsigned char palabra_enviado_CAM1[5]={76,49,32,49,13};
unsigned char palabra_enviado_PC[15];
unsigned char palabra_recepcion_CAM1[5];
unsigned char palabra_recepcion_CAM2[15];
unsigned short enviados;
int Mx=0; 
int error = 0;
int num_PC1 = 4;
int num_PC2 = 4;
int num_CAM1 = 3;
int num_CAM2 = 16; 
int estado_cam = 0;
int estado = 0;
int x = 0;		

unsigned char CodError;
unsigned short Reg_len = 20;
unsigned char Reg_code[20] = {'C','R',' ','1','8',' ','3','6',' ','1','7',' ','2',' ','1','9',' ','3','2','\r'}; //CR [ reg1 value1 [reg2 value2 ... reg16 value16] ]\r  
unsigned short Track_len = 23;
unsigned char Track_code[23] = "TC 182 206 21 29 84 94\r"; //"TC 145 151 15 17 105 109\r"; //TC [Rmin Rmax Gmin Gmax Bmin Bmax]\r  :TC 130 255 0 0 30 30 
unsigned char CR_code = '\r';
unsigned int j = 0;
unsigned char Mxy[2] = {0, 0}, Cxy[6] = {0, 0, 0, 0, 0, 0};	// Mxy -> (xmean, ymean), Cxy = (xleft, yleft, xright, yright, pixels, confidence)
unsigned char rChar;
unsigned int nBuff=0;

void main(void)
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  /*M1_SetDutyUS(337);*/
  
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
	  if (estado_cam== 1){		//
		  for(j = 0; j < Reg_len; j++){
				CodError = AS1_SendChar(Reg_code[j]);
				//Cpu_Delay100US(1);
			}
		  estado = 0;
		  estado_cam = 0;
	  }
  }
  
  while(estado == 0)
  {
	  if (estado_cam== 1){		//
		  for(j = 0; j < Track_len; j++){
			  CodError = AS1_SendChar(Track_code[j]);
			  //Cpu_Delay100US(1);
			}
		  estado = 1;
		  estado_cam = 0;
	  }
  }
  
  estado = 0;

  for(;;) { 
	  
	  /*if (estado == 1){       //Get Mean enviar a
		 	AS1_SendChar(71);
		  	AS1_SendChar(77);
		  	AS1_SendChar(13);
		  	//Cpu_Delay100US(10000);
		  	estado =0;
		  	
	  }
	else if (estado==2){		//Apagar LED enviar b
		  	  AS1_SendChar(76);
			  AS1_SendChar(49);
			  AS1_SendChar(32);
			  AS1_SendChar(48);
			  AS1_SendChar(13);
			  //Cpu_Delay100US(10000);
			  estado = 0;
		  
	  }else if (estado==3){		//Encender LED Modo Automatico enviar c
			  AS1_SendChar(76);
			  AS1_SendChar(49);
			  AS1_SendChar(32);
			  AS1_SendChar(50);
			  AS1_SendChar(13);
			  //Cpu_Delay100US(10000);
			  estado = 0;
		  
	  }else if (estado==4){		//Track Window enviar d 
			  AS1_SendChar(84);
			  AS1_SendChar(87);
			  AS1_SendChar(32);
			  AS1_SendChar(13);
			  //Cpu_Delay100US(10000);
			  estado = 0;
		 
	  }else if (estado==5){		//Middle Mass enviar e
		  AS1_SendChar(77);
		  AS1_SendChar(77);
		  AS1_SendChar(32);
		  AS1_SendChar(48);
		  AS1_SendChar(48);
		  AS1_SendChar(48);
		  AS1_SendChar(49);
		  AS1_SendChar(13);
		  //Cpu_Delay100US(10000);
		  estado = 0;
		  
	 } else if (estado==6){		//
		  	for(j = 0; j < Reg_len; j++){
		  		CodError = AS1_SendChar(Reg_code[j]);
		  		//Cpu_Delay100US(1);
		  	}
		  estado = 0;
	  
	  }else if (estado==7){		//
		  	for(j = 0; j < Track_len; j++){
		  	CodError = AS1_SendChar(Track_code[j]);
		 		//Cpu_Delay100US(1);
		  	}
		  estado = 0;
	  }*/
	  
  	  
  }

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

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
