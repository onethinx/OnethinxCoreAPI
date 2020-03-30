## Updates for the 0x000000B8 Stack

Restructured stack core,
Added extended configurations:
  LoRa<>LoRa functionality
  Stack debug options
Capsense confguration fix
Add LoRa<>LoRa functionality
Add MAC Cmd LinkADR
Fixed RX window timing
Fix confirmed downlink reply (ACK)
Fix US join implementation
Add Low Power Idle/Join
Stability fixes

## Changes to the configuration structure of the new 0x000000B8 API

The Onethinx 0x000000B8 stack has implemented M0+ idle/join-backoff sleepmode. The user has to update the coreConfiguration to accomodate the idle sleepmode.
```
coreConfiguration_t coreConfig = {
	.....
	.System.Idle.Mode = 	M0_DeepSleep,
	.System.Idle.BleEcoON = false,
	.System.Idle.DebugON =  true,
};
```
Where '.System.Idle.Mode' has to be one of the following:

```
M0_Active				//!< Keep M0+ active during system idle
M0_Sleep				//!< Put M0+ in Sleep mode during system idle
M0_DeepSleep		//!< Put M0+ in DeepSleep mode during system idle
```

## Changes to the sleep structure of the new 0x000000B8 API



#### LoRaWAN_Sleep

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
	.sleepCores = coresM0p,
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
