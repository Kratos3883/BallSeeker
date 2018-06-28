/* ###################################################################
**     Filename    : Events.c
**     Project     : MotoresMovimiento
**     Processor   : MCF51QE128CLK
**     Component   : Events
**     Version     : Driver 01.02
**     Compiler    : CodeWarrior ColdFireV1 C Compiler
**     Date/Time   : 2018-05-09, 15:06, # CodeGen: 0
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file Events.c
** @version 01.02
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"
#include "math.h"

extern unsigned char Mx;
extern bool estado_cam;
bool bypass1 = 0; 	//SE HA RECIBIDO UNA 'M' o una 'S'
bool bypass2 = 0;	//Se ha recibido el espacio despues de 'M'
bool Rx_Mx=0;		//Se ha recibido centroide, pasa a UBICAR_PELOTA

unsigned char j = 0;
unsigned char charEdo = 0;

extern unsigned char rChar;
extern unsigned char SRmin, SRmax, SGmin, SGmax, SBmin, SBmax;
unsigned char SRm, SGm, SBm, SRv, SGv, SBv;
//extern unsigned char S[6];
//unsigned char S[6] = {0,0,0,0,0,0};
unsigned char S;

/* User includes (#include below this line is not maintained by Processor Expert) */

/*
** ===================================================================
**     Event       :  AS1_OnError (module Events)
**
**     Component   :  AS1 [AsynchroSerial]
**     Description :
**         This event is called when a channel error (not the error
**         returned by a given method) occurs. The errors can be read
**         using <GetError> method.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void  AS1_OnError(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AS1_OnRxChar (module Events)
**
**     Component   :  AS1 [AsynchroSerial]
**     Description :
**         This event is called after a correct character is received.
**         The event is available only when the <Interrupt
**         service/event> property is enabled and either the <Receiver>
**         property is enabled or the <SCI output mode> property (if
**         supported) is set to Single-wire mode.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void  AS1_OnRxChar(void)
{
	/*Interrupci�n de puerto serial*/
	
	rChar = 0;
		
	//estado_cam = 1; 
	AS1_RecvChar(&rChar);
	AS2_SendChar(rChar);

		
	
	
	switch(charEdo)
	{
		case TRACK:
			if(bypass2)
			{
				//AS2_SendChar(rChar);
				if(rChar == ' ')
				{
					bypass2 = 0;
					Rx_Mx=1;
				}
				else
					Mx = Mx*10 + (rChar-0x30);
			}
			if(bypass1)
			{
				bypass1 = 0;
				//AS2_SendChar(rChar);
				bypass2 = 1;	//Inmediatamente despues de 'M' viene un espacio, activo bypass2
			}


			break;
		
		case IDENTIFICA:
			if(bypass2)
			{
				//AS2_SendChar(rChar);
				if(rChar == ' ')
				{
					j++;
					S = 0;
				}
				else if(rChar == '\r')
				{
					j = 0;
					
					S = 0;
					bypass2 = 0;
					estado_cam = 1;
					charEdo = 0;
					SRmin = SRm - SRv;
					SRmax = SRm + SRv;
					SGmin = SGm - SGv;
					SGmax = SGm + SGv;
					SBmin = SBm - SBv;
					SBmax = SBm + SBv;
					//AS1_ClearRxBuf();
					AS1_SendChar('\r');
				}
				else if(rChar >= 0x30 && rChar <= 0x39)
				{
					S = S*10 + (rChar-0x30);
					switch(j)
					{
						case 0:
							SRm = S;
							break;
						case 1:
							SGm = S;
							break;
						case 2:
							SBm = S;
							break;
						case 3:
							SRv = S;
							break;
						case 4:
							SGv = S;
							break;
						case 5:
							SBv = S;
							break;
					}
				}

			}
			if(bypass1)
			{
				bypass1 = 0;
				//AS2_SendChar(rChar);
				bypass2 = 1;	//Inmediatamente despues de 'M' viene un espacio, activo bypass2
			}
			break;
		
	}
	
	if(rChar == ':')
	{
		estado_cam = 1;	//Est� esperando una orden
	}
	
	if(rChar == 'M')
	{		
		charEdo=TRACK;
		bypass1=1;
		Mx = 0;
	}
	if(rChar == 'S')
	{		
		charEdo=IDENTIFICA;
		bypass1=1;
		S = 0;
	}
	
}

/*
** ===================================================================
**     Event       :  AS1_OnFullRxBuf (module Events)
**
**     Component   :  AS1 [AsynchroSerial]
**     Description :
**         This event is called when the input buffer is full;
**         i.e. after reception of the last character 
**         that was successfully placed into input buffer.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void  AS1_OnFullRxBuf(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AS1_OnTxChar (module Events)
**
**     Component   :  AS1 [AsynchroSerial]
**     Description :
**         This event is called after a character is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void  AS1_OnTxChar(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AS1_OnFreeTxBuf (module Events)
**
**     Component   :  AS1 [AsynchroSerial]
**     Description :
**         This event is called after the last character in output
**         buffer is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void  AS1_OnFreeTxBuf(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AD1_OnEnd (module Events)
**
**     Component   :  AD1 [ADC]
**     Description :
**         This event is called after the measurement (which consists
**         of <1 or more conversions>) is/are finished.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AD1_OnEnd(void)
{
  /* Write your code here ... */
}


/*
** ===================================================================
**     Event       :  TI1_OnInterrupt (module Events)
**
**     Component   :  TI1 [TimerInt]
**     Description :
**         When a timer interrupt occurs this event is called (only
**         when the component is enabled - <Enable> and the events are
**         enabled - <EnableEvent>). This event is enabled only if a
**         <interrupt service/event> is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void TI1_OnInterrupt(void)
{
	Bit8_NegVal();
	if(estado==ESPERAR && Rx_Mx)
		estado=UBICAR_PELOTA;
	else if(estado==ESPERAR && !Rx_Mx)
		estado=MEDIR_DISTANCIA;

}

/*
** ===================================================================
**     Event       :  AS2_OnError (module Events)
**
**     Component   :  AS2 [AsynchroSerial]
**     Description :
**         This event is called when a channel error (not the error
**         returned by a given method) occurs. The errors can be read
**         using <GetError> method.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void  AS2_OnError(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AS2_OnRxChar (module Events)
**
**     Component   :  AS2 [AsynchroSerial]
**     Description :
**         This event is called after a correct character is received.
**         The event is available only when the <Interrupt
**         service/event> property is enabled and either the <Receiver>
**         property is enabled or the <SCI output mode> property (if
**         supported) is set to Single-wire mode.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void  AS2_OnRxChar(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AS2_OnTxChar (module Events)
**
**     Component   :  AS2 [AsynchroSerial]
**     Description :
**         This event is called after a character is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void  AS2_OnTxChar(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AS2_OnFullRxBuf (module Events)
**
**     Component   :  AS2 [AsynchroSerial]
**     Description :
**         This event is called when the input buffer is full;
**         i.e. after reception of the last character 
**         that was successfully placed into input buffer.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void  AS2_OnFullRxBuf(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AS2_OnFreeTxBuf (module Events)
**
**     Component   :  AS2 [AsynchroSerial]
**     Description :
**         This event is called after the last character in output
**         buffer is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void  AS2_OnFreeTxBuf(void)
{
  /* Write your code here ... */
}

/* END Events */

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
