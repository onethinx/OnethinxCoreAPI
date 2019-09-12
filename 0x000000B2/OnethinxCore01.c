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

/****************************************************************************
*			coreArguments structure
* This structure is used for communication with the LoRaWAN core Stack
* Members should not be modified in user code.
*****************************************************************************/


// Initialize coreArguments structure for use with M0+ IPC calls
volatile coreArguments_t coreArguments = {
	.configurationPtr = 0
};

ipc_msgs_t ipcMsgs =
{
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
*			Internal Functions
*****************************************************************************/

volatile uint32_t callBackDone;

void CM4_MessageCallback(uint32_t *msg)
{
}

void CM4_ReleaseCallback(void)
{
	callBackDone = 1; 
}

coreStatus_t coreComm(coreFunctions_e function, bool waitTillFinished)
{
	systemErrors_e systemError;
	cy_en_ipc_pipe_status_t pipeStatus;
	coreArguments.function = function;
	if (coreArguments.status.system.isBusy) systemError = system_BusyError;
	else {
		coreArguments.status.system.isBusy = true;
		callBackDone = 0;
		pipeStatus = Cy_IPC_Pipe_SendMessage(CY_IPC_EP_CYPIPE_CM0_ADDR, CY_IPC_EP_CYPIPE_CM4_ADDR, (void *) &ipcMsgs.forCM0, CM4_ReleaseCallback);
		if (pipeStatus != CY_IPC_PIPE_SUCCESS) systemError = system_IPCError;
		else if (waitTillFinished) while (coreArguments.status.system.isBusy);
	}
	if (systemError != system_OK) coreArguments.status.system.errorStatus = systemError;
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

coreStatus_t LoRaWAN_GetInfo(coreInfo_t * coreInfo)
{
	coreArguments.arg1 = (uint32_t) coreInfo;
	return coreComm(coreFunction_GetInfo, true);
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
{																						// Debugging will halt here as SWD pins are put in High-Z mode
	coreArguments.arg1 = (uint32_t) sleepConfig;
	coreComm(coreFunction_LW_sleep, false);
	if (coreArguments.status.system.errorStatus != system_OK) return coreArguments.status;
	if ((sleepConfig->sleepMode & 7) == modeHibernate)	
		while(1);																		// CM0+ will put system in hibernate and system will restart with a reset
 	while (!callBackDone);
	if ((sleepConfig->sleepMode & 7)  == modeDeepSleep)
		Cy_SysPm_DeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
	else if ((sleepConfig->sleepMode & 7)  == modeSleep)
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
	return coreComm(coreFunction_LW_flashWrite, true);
}

/* [] END OF FILE */
