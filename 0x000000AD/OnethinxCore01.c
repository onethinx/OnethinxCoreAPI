/********************************************************************************
 *    ___             _   _     _			
 *   / _ \ _ __   ___| |_| |__ (_)_ __ __  __
 *  | | | | '_ \ / _ \ __| '_ \| | '_ \\ \/ /
 *  | |_| | | | |  __/ |_| | | | | | | |>  < 
 *   \___/|_| |_|\___|\__|_| |_|_|_| |_/_/\_\
 *
 ********************************************************************************
 *
 * Copyright (c) 2019 Onethinx BV <info@onethinx.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 ********************************************************************************
 *
 * Created by: Rolf Nooteboom
 *
 * Library to use with the Onethinx Core LoRaWAN module.
 * 
 * For a description please see:
 * https://github.com/onethinx/OnethinxCoreAPI
 *
 ********************************************************************************/

#include "OnethinxCore01.h"
#include "cy_ipc_pipe.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/****************************************************************************
*			Shared Variables
*****************************************************************************/
/* CyPipe defines */

#define CY_IPC_CYPIPE_CHAN_MASK_EP0     (uint32_t)(0x0001ul << CY_IPC_CHAN_CYPIPE_EP0)
#define CY_IPC_CYPIPE_CHAN_MASK_EP1     (uint32_t)(0x0001ul << CY_IPC_CHAN_CYPIPE_EP1)
#define CY_IPC_CYPIPE_INTR_MASK   (uint32_t)( CY_IPC_CYPIPE_CHAN_MASK_EP0 | CY_IPC_CYPIPE_CHAN_MASK_EP1 )

volatile bool rdyToProcess = false;	  /* Ready to process flag		   */

coreArguments_t coreArguments = {
	.configurationPtr = 0
};
coreStatus_t status;

ipc_msgs_t ipcMsgs = {
	{
		/* IPC structure to be sent to CM0  */
		.clientId = 1,  // IPC_CM4_TO_CM0_CLIENT_ID = 1
		.userCode = 0,
		.intrMask = CY_IPC_CYPIPE_INTR_MASK,
		.coreArgumentsPtr = &coreArguments
	},
	{
		/* IPC structure to be received from CM0  */
		.clientId = 0,  // IPC_CM0_TO_CM4_CLIENT_ID = 0
		.userCode = 0,
		.intrMask = CY_IPC_CYPIPE_INTR_MASK,
		.coreArgumentsPtr = &coreArguments
	}
};

/****************************************************************************
*			Prototype Functions
*****************************************************************************/
void CM4_MessageCallback(uint32_t *msg);
void CM4_ReleaseCallback(void);

//void Cy_IPC_Pipe_RegisterCallback(CY_IPC_EP_CYPIPE_ADDR, CM4_MessageCallback, IPC_CM0_TO_CM4_CLIENT_ID);	

coreStatus_t LoRaWAN_Init(coreConfiguration_t * coreConfigurationPtr)
{
	coreArguments.configurationPtr = coreConfigurationPtr;
	coreArguments.status.system.isBusy = true;
	coreArguments.status.system.breakCurrentFunction = true;
	coreArguments.function = coreFunction_Init;
	Cy_IPC_Pipe_SendMessage(CY_IPC_EP_CYPIPE_CM0_ADDR, CY_IPC_EP_CYPIPE_CM4_ADDR, (void *) &ipcMsgs.forCM0, CM4_ReleaseCallback);
	while (coreArguments.status.system.isBusy);

	return coreArguments.status;
}

coreStatus_t LoRaWAN_Join(bool waitTillFinished)
{
	if (coreArguments.status.system.isBusy)
		coreArguments.status.system.errorStatus = system_BusyError;
	else {
		coreArguments.status.system.isBusy = true;
		coreArguments.function = coreFunction_LW_join;
		Cy_IPC_Pipe_SendMessage(CY_IPC_EP_CYPIPE_CM0_ADDR, CY_IPC_EP_CYPIPE_CM4_ADDR, (void *) &ipcMsgs.forCM0, CM4_ReleaseCallback);
		if (waitTillFinished) while (coreArguments.status.system.isBusy);
	}
	return coreArguments.status;
}

coreStatus_t LoRaWAN_Send(uint8_t * bufferPtr, uint8_t length, bool waitTillFinished)
{
	if (coreArguments.status.system.isBusy)
		coreArguments.status.system.errorStatus = system_BusyError;
	else {
		coreArguments.status.system.isBusy = true;
		coreArguments.function = coreFunction_LW_send;
		coreArguments.coreTXbufPtr = bufferPtr;
		coreArguments.coreTXbufLength = length;
		Cy_IPC_Pipe_SendMessage(CY_IPC_EP_CYPIPE_CM0_ADDR, CY_IPC_EP_CYPIPE_CM4_ADDR, (void *) &ipcMsgs.forCM0, CM4_ReleaseCallback);		
		if (waitTillFinished) while (coreArguments.status.system.isBusy);
	}
	return coreArguments.status;
}

coreStatus_t LoRaWAN_GetRXdata(uint8_t * RXdata, uint8_t length)
{
	if (coreArguments.status.system.isBusy)
		coreArguments.status.system.errorStatus = system_BusyError;
	else {
		coreArguments.status.system.isBusy = true;
		coreArguments.function = coreFunction_LW_getRXdata;
		coreArguments.coreRXbufPtr = RXdata;
		coreArguments.coreRXbufLength = length;
		Cy_IPC_Pipe_SendMessage(CY_IPC_EP_CYPIPE_CM0_ADDR, CY_IPC_EP_CYPIPE_CM4_ADDR, (void *) &ipcMsgs.forCM0, CM4_ReleaseCallback);		
		while (coreArguments.status.system.isBusy);
	}
	return coreArguments.status;
}

coreStatus_t LoRaWAN_GetStatus()
{
	return coreArguments.status;
}

errorStatus_t LoRaWAN_GetError()
{
	errorStatus_t errorStatus;
	errorStatus.paramErrors = coreArguments.status.parameters.errorStatus;
	errorStatus.radioErrors = coreArguments.status.radio.errorStatus;
	errorStatus.macErrors = coreArguments.status.mac.errorStatus;
	errorStatus.systemErrors = coreArguments.status.system.errorStatus;
	return errorStatus;
}

/*******************************************************************************
* Function Name: CM4_MessageCallback()
********************************************************************************
* Summary:
*   Callback function that is executed when a string message is received from 
*   CM0. Copy the string message to a local array, which is processed in the
*   main loop.
*
* Parameters:
*   msg: IPC message received
*
*******************************************************************************/
void CM4_MessageCallback(uint32_t *msg)
{

}

void CM4_ReleaseCallback(void)
{
		
}

/* [] END OF FILE */
