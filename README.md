# OnethinxCoreAPI

## Library to use the Onethinx Core LoRaWAN module

The following table indicates which API version to use:

| Stack version | API version | Where to find                                                                                    |
|---------------|-------------|--------------------------------------------------------------------------------------------------|
| 0x000000AA    | 0x000000AA  | not public                                                                                       |
| 0x000000AB    | 0x000000AA  | not public                                                                                       |
| 0x000000AC    | 0x000000AA  | not public                                                                                       |
| 0x000000AD    | 0x000000AD  | [Onethinx GitHub CoreAPI 0x000000AD](https://github.com/onethinx/OnethinxCoreAPI/tree/master/0x000000AD) |
| 0x000000AE    | 0x000000AD  | [Onethinx GitHub CoreAPI 0x000000AD](https://github.com/onethinx/OnethinxCoreAPI/tree/master/0x000000AD) |
| 0x000000B0    | 0x000000B0  | [Onethinx GitHub CoreAPI 0x000000B0](https://github.com/onethinx/OnethinxCoreAPI/tree/master/0x000000B0) |
| 0x000000B1    | 0x000000B1  | [Onethinx GitHub CoreAPI 0x000000B1](https://github.com/onethinx/OnethinxCoreAPI/tree/master/0x000000B1) |
| 0x000000B2    | 0x000000B2  | [Onethinx GitHub CoreAPI 0x000000B1](https://github.com/onethinx/OnethinxCoreAPI/tree/master/0x000000B2) |
| 0x000000B3    | 0x000000B2  | [Onethinx GitHub CoreAPI 0x000000B2](https://github.com/onethinx/OnethinxCoreAPI/tree/master/0x000000B2) |
| 0x000000B4    | 0x000000B2  | [Onethinx GitHub CoreAPI 0x000000B2](https://github.com/onethinx/OnethinxCoreAPI/tree/master/0x000000B2) |
| 0x000000B5    | 0x000000B2  | [Onethinx GitHub CoreAPI 0x000000B2](https://github.com/onethinx/OnethinxCoreAPI/tree/master/0x000000B2) |
| 0x000000B6    | 0x000000B2  | not public                                                                                       |
| 0x000000B7    | 0x000000B7  | not public                                                                                       |
| 0x000000B8    | 0x000000B8  | [Onethinx GitHub CoreAPI 0x000000B8](https://github.com/onethinx/OnethinxCoreAPI/tree/master/0x000000B8) |
| 0x000000B9    | 0x000000B8  | [Onethinx GitHub CoreAPI 0x000000B8](https://github.com/onethinx/OnethinxCoreAPI/tree/master/0x000000B8) |
| 0x000000BA    | 0x000000B8  | [Onethinx GitHub CoreAPI 0x000000B8](https://github.com/onethinx/OnethinxCoreAPI/tree/master/0x000000B8) |
| 0x000000BB    | 0x000000BB  | [Onethinx GitHub CoreAPI 0x000000BB](https://github.com/onethinx/OnethinxCoreAPI/tree/master/0x000000BB) |
| 0x000000BC    | 0x000000BB  | [Onethinx GitHub CoreAPI 0x000000BB](https://github.com/onethinx/OnethinxCoreAPI/tree/master/0x000000BB) |
| 0x000000BD    | 0x000000BD  | not public
| 0x000000BE    | 0x000000BE  | [Onethinx GitHub CoreAPI 0x000000BE](https://github.com/onethinx/OnethinxCoreAPI/tree/master/0x000000BE) |
| 0x000000BF    | 0x000000BE  | [Onethinx GitHub CoreAPI 0x000000BE](https://github.com/onethinx/OnethinxCoreAPI/tree/master/0x000000BE) |
| 0x000000C0    | 0x000000C0  | not public                                                                                       |
| 0x000000C1    | 0x000000C1  | [Onethinx GitHub CoreAPI 0x000000C1](https://github.com/onethinx/OnethinxCoreAPI/tree/master/0x000000C1) |

### How to get the stack version of your module.

Put the following code in your main.c:

```

#include "project.h"
#include "OnethinxCore01.h"

LoRaWAN_keys_t SomeOTAAkeys = {
	.KeyType					= OTAA_10x_key,
	.PublicNetwork					= true,
	.OTAA_10x.DevEui				= {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }},
	.OTAA_10x.AppEui				= {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }},
	.OTAA_10x.AppKey				= {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }}
};

coreConfiguration_t	coreConfigMain = {
	.Join.KeysPtr = 		&SomeOTAAkeys,
	.Join.DataRate =		DR_AUTO,
	.Join.Power =			PWR_MAX,
	.Join.MAXTries = 		100,
	.TX.Confirmed = 		false,
	.TX.DataRate = 			DR_0,
	.TX.Power = 			PWR_MAX,
	.TX.FPort = 			1,
};

uint32_t stackVersion;
extern coreStatus_t coreComm(coreFunctions_e function, WaitMode_e waitMode);
extern ipc_msgs_t ipcMsgs;
extern void CM4_MessageCallback(uint32_t *msg __attribute__((unused)));

int main(void)
{
  	CyDelay(1000);
	__enable_irq();
	Cy_IPC_Pipe_RegisterCallback(CY_IPC_EP_CYPIPE_ADDR, CM4_MessageCallback, CY_IPC_EP_CYPIPE_CM0_ADDR); 
	ipcMsgs.forCM0.coreArgumentsPtr = &coreArguments;
	ipcMsgs.fromCM0.coreArgumentsPtr = &coreArguments;
	coreArguments.configurationPtr = &coreConfigMain;
	coreArguments.status.system.breakCurrentFunction = true;
	coreArguments.status.system.isBusy = false;
	coreComm(coreFunction_Init, M4_NoWait);
	CyDelay(100);
	stackVersion = coreArguments.status.system.version;       // your stack version will be here
	while(1){}
 }
  ```
  

