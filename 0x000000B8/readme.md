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
	....
	....
	.System.Idle.Mode = 		M0_DeepSleep,
	.System.Idle.BleEcoON = 	false,
	.System.Idle.DebugON =  	true,
};
```
Where '.System.Idle.Mode' has to be one of the following:

```
M0_Active		//!< Keep M0+ active during system idle
M0_Sleep		//!< Put M0+ in Sleep mode during system idle
M0_DeepSleep		//!< Put M0+ in DeepSleep mode during system idle
```

## Changes to the sleep structure of the new 0x000000B8 API

Sleepmode is slightly updated to accomodate the BLE ECO (32MHz External Crystal Oscillator). The following structure members are changed / added:
```
.sleepMode	//!< Set sleepmode to Sleep, DeepSleep or Hibernate
.BleEcoON	//!< Leaves BLE ECO ON during sleep. Consumes additional power, enable only when ECO/BLE functionality is needed during sleep
DebugON		//!< Leaves Debug Port active during idle. Consumes additional power, enable only for debugging purposes
```
*Example: DeepSleep on both cores and wake up after 15 seconds*

```
sleepConfig_t sleepConfig = {
	.sleepMode = modeDeepSleep,	//!< modeSleep, modeDeepSleep or modeHibernate (debug modes are depreciated)
	.BleEcoON = false,		//!< Leaves BLE ECO on in sleep if true
	.DebugON = false,		//!< Leaves Debug port open on in sleep if true (not recommended for field releases)
	.sleepCores = coresBoth,
	.wakeUpPin = wakeUpPinOff,
	.wakeUpTime = wakeUpDelay(0, 0, 0, 15)
};
```

**Caution: All members of the sleepConfig structure have to be assigned!**
