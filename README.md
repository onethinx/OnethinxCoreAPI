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
| 0x000000BB    | 0x000000BB  | [Onethinx GitHub CoreAPI 0x000000BB](https://github.com/onethinx/OnethinxCoreAPI/tree/master/0x000000B8) |

### How to get the stack version of your module:

```
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

coreStatus_t coreStatus;
uint32_t stackVersion;

int main(void)
{
	CyDelay(1000);
  
	/* enable global interrupts */
	__enable_irq();
	
	coreStatus = LoRaWAN_Init(&coreConfigMain);
	stackVersion = coreStatus.system.version;       // your stack version will be here
	CyDelay(1000);
 }
  ```
  

