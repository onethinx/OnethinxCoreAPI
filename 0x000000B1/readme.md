## Usage of the new 0x000000B1 API

The following API functionality has changed since version 0x000000B0:

The addition of the configuration of two sub-bands ( `.Join.SubBand_1st` and `.Join.SubBand_2nd` ). 
This configuration is to accommodate the LoRaWAN US specifications and reduce joining time, and NEEDS to be configured for all versions since 0x000000B0.

### EU configuration
For LoRaWAN EU region, a total of 16 channels are possible where the sub-band 1 represents channels 0-7 and sub-band 2 represents channels 8-15. 

The default configuration is:
- sub-band 1 enabled
- sub-band 2 disabled

EU configuration example:
```
coreConfiguration_t coreConfig = {
	.Join.KeysPtr =			&SomeOTAAkeys,
	.Join.DataRate =		DR_AUTO,
	.Join.Power =			PWR_MAX,
	.Join.MAXTries =		100,
	.Join.SubBand_1st =		EU_SUB_BANDS_DEFAULT,
	.Join.SubBand_2nd =		EU_SUB_BANDS_DEFAULT,
	.TX.Confirmed =			false,
	.TX.DataRate =			DR_0,
	.TX.Power = 			PWR_MAX,
	.TX.FPort =			1,
};
```

### US configuration
For LoRaWAN US region, a total of 64 uplink channels plus 8 downlink channels are possible where:
Sub-band 1 represents uplink channels 0-7 + downlink channel 0
Sub-band 2 represents uplink channels 8-15  + downlink channel 1 and so on...

US configuration example:

```
coreConfiguration_t coreConfig = {
	.Join.KeysPtr =			&SomeOTAAkeys,
	.Join.DataRate =		DR_AUTO,
	.Join.Power =			PWR_MAX,
	.Join.MAXTries =		100,
	.Join.SubBand_1st =		US_SUB_BAND_2,
	.Join.SubBand_2nd =		US_SUB_BAND_NONE,
	.TX.Confirmed =			false,
	.TX.DataRate =			DR_0,
	.TX.Power = 			PWR_MAX,
	.TX.FPort =			1,
};
```

## API updates

The following API calls are added since version 0x000000B1:
```
coreStatus_t	LoRaWAN_Reset();
coreStatus_t	LoRaWAN_MacSave();
coreStatus_t	LoRaWAN_FlashRead(uint8_t* buffer, uint8_t block, uint8_t length);
coreStatus_t	LoRaWAN_FlashWrite(uint8_t* buffer, uint8_t block, uint8_t length);
coreStatus_t 	LoRaWAN_Sleep(sleepConfig_t * sleepConfig);
```

#### LoRaWAN_Reset
```
coreStatus_t	LoRaWAN_Reset();
```
Resets the MAC configuration (rejoin needed).

#### LoRaWAN_MacSave
```
coreStatus_t	LoRaWAN_MacSave();
```
Saves the MAC configuration.

#### LoRaWAN_FlashRead
```
coreStatus_t	LoRaWAN_FlashRead(uint8_t* buffer, uint8_t block, uint8_t length);
```
Reads data of a maximum of 256 bytes per block from EEPROM memory. A total of 8 blocks are available.

#### LoRaWAN_FlashWrite
```
coreStatus_t	LoRaWAN_FlashWrite(uint8_t* buffer, uint8_t block, uint8_t length);
```
Writes data of a maximum of 256 bytes per block in EEPROM memory. A total of 8 blocks are available.

#### LoRaWAN_Sleep
```
coreStatus_t	LoRaWAN_Sleep();
```
Puts the device to sleep

The following API call is used for hibernate, deepsleep and sleep:
```
coreStatus_t status = LoRaWAN_Sleep(&sleepConfig);
```
Where sleepConfig is configured:
```
sleepConfig_t sleepConfig = {
	.sleepMode = modeDeepSleep,
	.sleepCores = coresBoth,
	.wakeUpPin = wakeUpPinOff,
	.wakeUpTime = wakeUpDelay(0, 0, 0, 15)
};
```

**Caution: All members of the sleepConfig structure have to be assigned!**

The following members have to be assigned with one of the parameters:
```
.sleepmode:
	modeSleep
	modeDeepSleep
	modeHibernate
```
```
.sleepCores:
	coresM0p
	coresM4
	coresBoth
```
```
.wakeUpPin:
	wakeUpPinHigh(bool pullDown)
	wakeUpPinDown(bool pullUp)
	wakeUpPinOff
```
```
.wakeUpTime:
	wakeUpDelay(uint8 days, uint8 hours, uint8 minutes, uint8 seconds)
	wakeUpTimestamp(uint32 timestamp)
	wakeUpTimeOff
```

**Examples:**


*DeepSleep on both cores and wake up on pin P0_4 falling edge with internal pull-up*
```
sleepConfig_t sleepConfig = {
	.sleepMode = modeDeepSleep,
	.sleepCores = coresBoth,
	.wakeUpPin = wakeUpPinDown(true),
	.wakeUpTime = wakeUpTimeOff
};
```
*Sleep on CM0+ and wake up on pin P0_4 rising edge without internal pull down*
```
sleepConfig_t sleepConfig = {
	.sleepMode = modeSleep,
	.sleepCores = coresCM0p,
	.wakeUpPin = wakeUpPinUp(false),
	.wakeUpTime = wakeUpTimeOff
};
```
*Hibernate and wakeup after 180 seconds*

Note: Hibernate always shuts down most parts of the system including both cores. Member `.sleepCores` will be ignored when using hibernate sleepmode.
The device will wake up which includes a device reset. SRAM will not be preserved.
```
sleepConfig_t sleepConfig = {
	.sleepMode = modeHibernate,
	.sleepCores = coresBoth,
	.wakeUpPin = wakeUpPinOff,
	.wakeUpTime = wakeUpDelay(0, 0, 0, 180)
};
```
`wakeUpDelay(days, hours, minutes, seconds)` adds all four parameters

eg. a delay of 12 days, 4 hours, 3 minutes and 0 seconds can be created in multiple ways:
`wakeUpDelay(12, 4, 3, 0)`, or
`wakeUpDelay(12, 4, 0, 180)`, or
`wakeUpDelay(11, 26, 120, 180)`,
as long as each parameter fits in 8 bits (< 256)
