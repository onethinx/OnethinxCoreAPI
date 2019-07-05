/* ==========================================================
 *    ___             _   _     _			
 *   / _ \ _ __   ___| |_| |__ (_)_ __ __  __
 *  | | | | '_ \ / _ \ __| '_ \| | '_ \\ \/ /
 *  | |_| | | | |  __/ |_| | | | | | | |>  < 
 *   \___/|_| |_|\___|\__|_| |_|_|_| |_/_/\_\
 *									   
 * Copyright Onethinx, 2018
 * All Rights Reserved
 *
 * UNPUBLISHED, LICENSED SOFTWARE.
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF Onethinx BV
 *
 * ==========================================================
*/

#include "OnethinxCore01.h"
#include "cy_ipc_pipe.h"
#include "cy_syspm.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/****************************************************************************
*			Shared Variables
*****************************************************************************/
/* CyPipe defines */

#define CY_IPC_CYPIPE_CHAN_MASK_EP0     (uint32_t)(0x0001ul << CY_IPC_CHAN_CYPIPE_EP0)
#define CY_IPC_CYPIPE_CHAN_MASK_EP1     (uint32_t)(0x0001ul << CY_IPC_CHAN_CYPIPE_EP1)
#define CY_IPC_CYPIPE_INTR_MASK 		(uint32_t)( CY_IPC_CYPIPE_CHAN_MASK_EP0 | CY_IPC_CYPIPE_CHAN_MASK_EP1 )

coreArguments_t coreArguments = {
	.configurationPtr = 0
};

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

cy_en_ipc_pipe_status_t pipeStatus;

/****************************************************************************
*			Internal Functions
*****************************************************************************/

volatile uint32_t callBackDone;

coreStatus_t coreComm(coreFunctions_e function, bool waitTillFinished)
{
	coreArguments.function = function;
	if (coreArguments.status.system.isBusy)
		coreArguments.status.system.errorStatus = system_BusyError;
	else {
		coreArguments.status.system.isBusy = true;
		callBackDone = 0;
		pipeStatus = Cy_IPC_Pipe_SendMessage(CY_IPC_EP_CYPIPE_CM0_ADDR, CY_IPC_EP_CYPIPE_CM4_ADDR, (void *) &ipcMsgs.forCM0, CM4_ReleaseCallback);
		if (pipeStatus != CY_IPC_PIPE_SUCCESS) coreArguments.status.system.errorStatus = system_IPCError;
		else if (waitTillFinished) while (coreArguments.status.system.isBusy);
	}
	return coreArguments.status;
}

/****************************************************************************
*			Public Functions
*****************************************************************************/

coreStatus_t LoRaWAN_Init(coreConfiguration_t * coreConfigurationPtr)
{
	/* Register callback to handle response from CM4 */
    Cy_IPC_Pipe_RegisterCallback(CY_IPC_EP_CYPIPE_ADDR, CM4_MessageCallback, CY_IPC_EP_CYPIPE_CM0_ADDR); 
	/* Initialize pointers to shared variables */
	ipcMsgs.forCM0.coreArgumentsPtr = &coreArguments;
	ipcMsgs.fromCM0.coreArgumentsPtr = &coreArguments;
	coreArguments.configurationPtr = coreConfigurationPtr;
	/* Force current function to quit */
	coreArguments.status.system.breakCurrentFunction = true;
	coreArguments.status.system.isBusy = false;
	coreComm(coreFunction_Init, true);
	if (coreArguments.status.system.version < minimumVersion || coreArguments.status.system.version > maximumVersion)
		coreArguments.status.system.errorStatus = system_VersionMatchError;
	return coreArguments.status;
}

coreStatus_t LoRaWAN_Reset(void)
{
	return coreComm(coreFunction_Reset, true);
}

coreStatus_t LoRaWAN_Join(bool waitTillFinished)
{
	return coreComm(coreFunction_LW_join, waitTillFinished);
}

coreStatus_t LoRaWAN_Send(uint8_t * bufferPtr, uint8_t length, bool waitTillFinished)
{
	coreArguments.arg1 = (uint32_t) bufferPtr;
	coreArguments.arg2 = length;
	return coreComm(coreFunction_LW_send, waitTillFinished);
}

coreStatus_t LoRaWAN_GetRXdata(uint8_t * RXdata, uint8_t length)
{
	coreArguments.arg1 = (uint32_t) RXdata;
	coreArguments.arg2 = length;
	return coreComm(coreFunction_LW_getRXdata, true);
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

coreStatus_t LoRaWAN_Sleep(sleepConfig_t * sleepConfig)
{
	coreArguments.arg1 = (uint32_t) sleepConfig;
	coreComm(coreFunction_LW_sleep, false);
	if (sleepConfig->sleepMode == modeHibernate)	
		while(1);																		// CM0+ will put system in hibernate and system will restart with a reset
	while (!callBackDone); 
	if (sleepConfig->sleepMode == modeDeepSleep)
		Cy_SysPm_DeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
	else if (sleepConfig->sleepMode == modeSleep)
		Cy_SysPm_Sleep(CY_SYSPM_WAIT_FOR_INTERRUPT);	
	return coreArguments.status;
}

coreStatus_t LoRaWAN_MacSave()
{
	return coreComm(coreFunction_LW_MACsave, true);
}

coreStatus_t LoRaWAN_FlashRead(uint8_t* buffer, uint8_t block, uint8_t length)
{
	coreArguments.arg1 = (uint32_t) buffer;
	coreArguments.arg2 = block;
	coreArguments.arg3 = length;
	return coreComm(coreFunction_LW_flashRead, true);
}

coreStatus_t LoRaWAN_FlashWrite(uint8_t* buffer, uint8_t block, uint8_t length)
{
	coreArguments.arg1 = (uint32_t) buffer;
	coreArguments.arg2 = block;
	coreArguments.arg3 = length;
	coreComm(coreFunction_LW_flashWrite, true);
	return coreArguments.status;
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
	callBackDone = 1; 
}

/* [] END OF FILE */
