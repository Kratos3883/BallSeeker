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
#include "M1.h"
#include "M2.h"
#include "AS1.h"
#include "AS2.h"
/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

unsigned char CodError;
unsigned short Reg_len = 20;
unsigned char Reg_code[20] = {'C','R',' ','1','8',' ','3','6',' ','1','7',' ','2',' ','1','9',' ','3','2','\r'}; //CR [ reg1 value1 [reg2 value2 ... reg16 value16] ]\r  
unsigned short Track_len = 25;
unsigned char Track_code[25] = "TC 145 151 15 17 105 109\r"; //TC [Rmin Rmax Gmin Gmax Bmin Bmax]\r  :TC 130 255 0 0 30 30 
unsigned int i = 0;
unsigned char Mx= 0;

void main(void)
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  /*M1_SetDutyUS(337);*/
  
  
  for(i = 0; i < Reg_len; i++){
  	CodError = AS1_SendChar(Reg_code[i]);
  }
  	  
  for(i = 0; i < Track_len; i++){
  	CodError = AS1_SendChar(Track_code[i]);
  }

  for(;;){}

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
