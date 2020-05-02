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
 ********************************************************************************
 *
 * Core revisions:
 * 0x000000AA	Fix DevAddr in OnethinxCore01.h and reserved byte amount fixed
 * 0x000000AB	Implemented M0+ reset: failed debugging
 * 0x000000AC	Implemented breakCurrentFunction
 * 0x000000AD	Added configurations pointer validity check
 * 0x000000AE	Transmit power issue solved (device was always sending at TX_MAX)
 * 0x000000B0	Added US sub-bands, enhanced RX1 + RX2 window accuracy, fixed CM0p SRAM memory mapping, 
 * 0x000000B1	Added LP functionality, added FlashWrites, added possibility to read Dev Address
 * 0x000000B2	Cleared RX-timeout flag if no downlink response is expected, added DevEUI & build info, fixed high power consumption glitches at wakeup, Added LowPowerDebug, Improved active-power consumption
 * 0x000000B3	Fix Join accept on SF12 in RX1 window
 * 0x000000B4	Fix WakeUp pin in Sleepmode, FlashRead fix for row !=0
 * 0x000000B5	Fix Flashwrites after sleep
 * 0x000000B7	Restructured stack core, Capsense confguration fix, Add LoRa<>LoRa functionality, Add MAC Cmd LinkADR, Fixed RX window timing, Fix confirmed downlink reply, Fix US join implementation, Add Low Power Join, Stability fixes
 * 0x000000B8	Restructured stack core, added low-power idle/join
 * 0x000000B9	Fix ADR
 * 0x000000BA	Unlock functions to use Port 6 & 7 for Capsense and SDW IOs
 *
 ********************************************************************************/

#ifndef ONETHINXCORE01_H
#define ONETHINXCORE01_H	
 
#include <stdint.h>
#include <stdbool.h>

/* Do not modify. If stack version does not match, implement correct OnethinxCore.h & .c API drivers from the Onethinx Github page */
#define minimumVersion 	0x000000B8
#define maximumVersion 	0x000000BA

typedef struct arr8b_t  { uint8_t bytes[8];  } arr8b_t;
typedef struct arr16b_t { uint8_t bytes[16]; } arr16b_t;

/* Do not modify. If code { 0, 0, 0, 0, 0, 0, 0, 0 } is recognized, it is replaced with manufacturing DevEUI. This manufacturing DevEUI can be requested using LoRaWAN_GetInfo */
#define thisDevEUI	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

typedef enum {
	coreFunction_Init					= 0x01,
	coreFunction_Reset					= 0x02,
	coreFunction_GetInfo				= 0x03,
	coreFunction_LW_join				= 0x20,
	coreFunction_LW_send				= 0x30,
	coreFunction_LW_getRXdata			= 0x40,
	coreFunction_LW_sleep				= 0x50,
	coreFunction_LW_MACsave				= 0x60,
	coreFunction_LW_flashRead			= 0x70,
	coreFunction_LW_flashWrite			= 0x71,
} coreFunctions_e;

typedef struct __attribute__((packed, aligned(4)))
{
	uint8_t							clientId;
	uint8_t							userCode;
	uint16_t						intrMask;
	volatile void *					coreArgumentsPtr;
} ipc_msg_t;

typedef struct __attribute__((packed, aligned(4)))
{
	ipc_msg_t	 forCM0;
	ipc_msg_t	 fromCM0;
} ipc_msgs_t;

 typedef enum  {
	ABP_10x_key		 			= 0x01,
	OTAA_10x_key	 			= 0x02,
	OTAA_11x_key	 			= 0x03,
	PreStored_key				= 0xF0,
	UserStored_key				= 0xF1,
} keyType_e;

typedef struct __attribute__ ((__packed__)) {
	keyType_e					KeyType;
	uint8_t						KeyIndex;
} StoredKeys_t;

typedef struct __attribute__ ((__packed__))  {
	arr8b_t						DevEui;
	arr8b_t						AppEui;
	arr16b_t					AppKey;
} OTAA_10x_t;
	
typedef struct __attribute__ ((__packed__))  {
	arr8b_t						DevEui;
	arr8b_t						JoinEui;
	arr16b_t					AppKey;
	arr16b_t					NwkKey;
} OTAA_11x_t;

typedef struct __attribute__ ((__packed__)) {
	arr8b_t						DevEui;
	uint32_t			   		DevAddr;
	arr16b_t					NwkSkey;
	arr16b_t					AppSkey;
} ABP_10x_t;
		
typedef struct  __attribute__ ((__packed__))  {
	keyType_e					KeyType 		: 8;
	uint8_t						reserved		: 7;
	bool						PublicNetwork	: 1;
	union {
		uint8_t					totalbytes[64];
		StoredKeys_t			StoredKeys;
		OTAA_10x_t				OTAA_10x;
		OTAA_11x_t				OTAA_11x;
		ABP_10x_t				ABP_10x;
	};
} LoRaWAN_keys_t;

typedef enum {
    DR_0						= 0x00,		//!< EU: SF12 125KHz, US: SF10 125KHz
    DR_1						= 0x01,		//!< EU: SF11 125KHz, US: SF9 125KHz
    DR_2						= 0x02,		//!< EU: SF10 125KHz, US: SF8 125KHz
    DR_3						= 0x03,		//!< EU: SF9 125KHz, US: SF7 125KHz
    DR_4						= 0x04,		//!< EU: SF8 125KHz, US: SF8 500KHz
    DR_5						= 0x05,		//!< EU: SF7 125KHz
    DR_6						= 0x06,		//!< EU: SF7 250KHz
    DR_7						= 0x07,		//!< EU: FSK 50kbps (unsupported)
    DR_8						= 0x08,		//!< US: SF12 500KHz (downlinks only)
    DR_9						= 0x09,		//!< US: SF11 500KHz (downlinks only)
    DR_10						= 0x0A,		//!< US: SF10 500KHz (downlinks only)
    DR_11						= 0x0B,		//!< US: SF9 500KHz (downlinks only)
    DR_12						= 0x0C,		//!< US: SF8 500KHz (downlinks only)
    DR_13						= 0x0D,		//!< US: SF7 500KHz (downlinks only)
    DR_ADR						= 0xF0,		//!< Adaptive DataRate
	DR_AUTO						= 0xF1,		//!< Automatic DataRate (during joining)
} Radio_DataRate_e;

typedef enum {
	PWR_ADR						= 0xF0,		//!< ADR (Automatic Power setting by LoRaWAN network)
	PWR_MAX						= 0x00,		//!<  15dBm for SX1261,  22dBm for SX1262
	PWR_ATT_2dB					= 0x01,		//!<  14dBm for SX1261,  20dBm for SX1262
	PWR_ATT_4dB					= 0x02,		//!<  12dBm for SX1261,  18dBm for SX1262
	PWR_ATT_6dB					= 0x03,		//!<  10dBm for SX1261,  16dBm for SX1262
	PWR_ATT_8dB					= 0x04,		//!<   8dBm for SX1261,  14dBm for SX1262
	PWR_ATT_10dB				= 0x05,		//!<   6dBm for SX1261,  12dBm for SX1262
	PWR_ATT_12dB				= 0x06,		//!<   4dBm for SX1261,  10dBm for SX1262
	PWR_ATT_14dB				= 0x07,		//!<   2dBm for SX1261,   8dBm for SX1262
	PWR_ATT_16dB				= 0x08,		//!<   0dBm for SX1261,   6dBm for SX1262
	PWR_ATT_18dB				= 0x09,		//!<  -2dBm for SX1261,   4dBm for SX1262
	PWR_ATT_20dB				= 0x0A,		//!<  -4dBm for SX1261,   2dBm for SX1262
	PWR_ATT_22dB				= 0x0B,		//!<  -6dBm for SX1261,   0dBm for SX1262
	PWR_ATT_24dB				= 0x0C,		//!<  -8dBm for SX1261,  -2dBm for SX1262
	PWR_ATT_26dB				= 0x0D,		//!< -10dBm for SX1261,  -4dBm for SX1262
	PWR_ATT_28dB				= 0x0E,		//!< -12dBm for SX1261,  -6dBm for SX1262
	PWR_ATT_30dB				= 0x0F,		//!< -14dBm for SX1261,  -8dBm for SX1262
    PWR_ATT_32dB				= 0x10,		//!< -16dBm for SX1261,  -9dBm for SX1262
	PWR_MIN						= 0x11,		//!< -17dBm for SX1261,  -9dBm for SX1262
} Radio_TXpower_e;

typedef enum {
	US_SUB_BAND_1			    = 0,
	US_SUB_BAND_2			    = 1,
	US_SUB_BAND_3			    = 2,
	US_SUB_BAND_4			    = 3,
	US_SUB_BAND_5			    = 4,
	US_SUB_BAND_6			    = 5,
	US_SUB_BAND_7			    = 6,
	US_SUB_BAND_8			    = 7,
    US_SUB_BAND_NONE		    = 14,
    US_SUB_BANDS_ALL   		    = 15,
    EU_SUB_BANDS_DEFAULT        = 0,
	EU_SUB_BAND1_ON		        = 1,
	EU_SUB_BAND1_OFF		    = 2,
	EU_SUB_BAND2_ON			    = 3,
	EU_SUB_BAND2_OFF		    = 4
} Radio_SubBands_e;

typedef enum  {
	M0_Active		 			= 0x0,			//!< Keep M0+ active during system idle
	M0_Sleep		 			= 0x1,			//!< Put M0+ in Sleep mode during system idle
	M0_DeepSleep	 			= 0x2,			//!< Put M0+ in DeepSleep mode during system idle
} IdleMode_e;

typedef enum  {
	M4_NoWait			 		= 0x0,			//!< Do not wait till stack finished
	M4_WaitActive		 		= 0x1,			//!< Wait while stack busy, M4 stays in Active mode
	M4_WaitSleep		 		= 0x2,			//!< M4 goes into Sleep while is stack busy
	M4_WaitDeepSleep	 		= 0x3,			//!< M4 goes into DeepSleep while is stack busy
} WaitMode_e;

typedef union {
	struct  __attribute__ ((__packed__)) {
		struct  __attribute__ ((__packed__)) {
			bool					Confirmed			: 1;
			uint8_t										: 7;
			Radio_DataRate_e		DataRate			: 8;
			Radio_TXpower_e			Power				: 8;
			uint8_t					FPort				: 8;
		} TX;
		struct  __attribute__ ((__packed__)) {
			LoRaWAN_keys_t *		KeysPtr;
			Radio_DataRate_e		DataRate			: 8;			//!< Not used for US version (defined by LoRaWAN spec)
			Radio_TXpower_e			Power				: 8;
			uint8_t					MAXTries			: 8;
            uint8_t					SubBand_1st    		: 4;
            uint8_t					SubBand_2nd    		: 4;
		} Join;
		struct  __attribute__ ((__packed__)) {
			struct {
				IdleMode_e				Mode			: 2;			/**< Set Idle Mode to idleActive, idleSleep or idleDeepSleep */
				bool 					BleEcoON	  	: 1;			/**< Leaves BLE ECO ON during idle. Consumes additional power, enable only when ECO/BLE functionality is needed */
				bool 					DebugON			: 1;			/**< Leaves Debug Port active during idle. Consumes additional power, enable only for debugging purposes */
		
			} Idle;
		} System;
	};
	uint8_t reserved[32];
} coreConfiguration_t;

typedef enum __attribute__ ((__packed__)) {
	param_OK					= 0x00,
	param_Invalid				= 0x01,
	param_OutOfRange			= 0x02,
	param_UndefinedError		= 0xFA,
} paramErrors_e;

typedef enum __attribute__ ((__packed__)) {
	radio_OK					= 0x00,
	radio_BusyError				= 0x01,		//!< Radio Busy Error
	radio_BuckStartError		= 0x02,		//!< Buck converter failed to start
	radio_XoscStartError		= 0x03,		//!< XOSC failed to start
	radio_Rc13mCalibError		= 0x04,		//!< RC 13MHz oscillator calibration failed
	radio_Rc64kCalibError		= 0x05,		//!< RC 64kHz oscillator calibration failed
	radio_PllCalibError			= 0x06,		//!< PLL calibration failed
	radio_PllLockError			= 0x07,		//!< PLL lock failed
	radio_ImgCalibError			= 0x08,		//!< Image calibration failed
	radio_AdcCalibError			= 0x09,		//!< ADC calibration failed
	radio_PaRampError			= 0x0A,		//!< PA ramp failed
	radio_InvalidFrequency		= 0x0B,		//!< Invalid frequency set
	radio_UndefinedError		= 0xFA
} radioErrors_e;

typedef enum __attribute__ ((__packed__)) {
	mac_OK						= 0x00,
	mac_BusyError				= 0x01,
	mac_NotJoinedError			= 0x02,
	mac_ChannelsOccupiedError	= 0x03,
	mac_UnrecognizedKeyType		= 0x04,
    mac_EmptyPayload_Error 		= 0x05,
	mac_RXheader_Error 			= 0x06,
	mac_RX_MIC_Error 			= 0x07,
	mac_RX_InvalidDevAddr 		= 0x08,
	mac_InvalidPacket_Error 	= 0x09,
	mac_RXtimeout_Error 		= 0x0A,
	mac_CRC_Error       		= 0x0B,
	mac_UndefinedError			= 0xFA,
} macErrors_e;

typedef enum __attribute__ ((__packed__)) {
	system_OK					= 0x00,
	system_BusyError			= 0x01,
	system_NotStarted			= 0x02,
	system_IPCError				= 0x03,
	system_FlashWriteError		= 0x04,
	system_VersionMatchError	= 0x05,
	system_UndefinedError	   	= 0xFA
} systemErrors_e ;

typedef union {
	struct
	{
		paramErrors_e 			errorStatus 			: 8;	//!< Parameter Errors
		uint8_t											: 8;	//!< reserved
	};
	uint8_t reserved[16];
} parameterStatus_t;

typedef union {
	struct
	{
		radioErrors_e 			errorStatus 			: 8;	//!< Radio Errors
		uint8_t							 				: 6;	//!< reserved
		bool 					isConfigured			: 1;	//!< The Radio is configured? (implemented since Stack version 0x000000B5)
		bool 					isBusy					: 1;	//!< Radio is busy?
	};
	uint8_t reserved[16];
} radioStatus_t;

typedef union {
	struct
	{
		macErrors_e 			errorStatus  			: 8;	//!< MAC Errors
		uint8_t		 			bytesToRead  			: 8;	//!< Total bytes in Receive buffer
		uint8_t							 				: 2;	//!< reserved
		bool 					confDown     			: 1;	//!< Is a confirmed downlink sent? (implemented since Stack version 0x000000B5)
		bool 					isConfigured			: 1;	//!< The MAC is configured?
		bool 					messageReceived			: 1;	//!< The MAC received a message
		bool 					isJoined 				: 1;	//!< Is Device Joined?
		bool 					isBusy					: 1;	//!< Is the MAC busy?
		bool 					isPublicNetwork			: 1;	//!< True for LoRaWAN Public Network
		uint32_t				devAddr;						//!< The Device address received when joining the network
	};
	uint8_t reserved[16];
}  macStatus_t;

typedef union {
	struct
	{
		uint32_t				version;
		systemErrors_e 			errorStatus  			: 8;  //!< System Errors
		uint8_t							 				: 5;  //!< reserved
		bool 					breakCurrentFunction	: 1;  //!< Break current execution
		bool 					isStarted  				: 1;  //!< System is started?
		bool 					isBusy  				: 1;  //!< System is busy?
		bool 					isSleeping 				: 1;  //!< System is sleeping?
	};
	uint8_t reserved[16];
}  systemStatus_t;

typedef struct {
	parameterStatus_t		parameters;
	radioStatus_t			radio;
	macStatus_t				mac;
	systemStatus_t			system;
} coreStatus_t;

typedef union {
	uint32_t					errorValue;
	struct  __attribute__ ((__packed__)) {
		paramErrors_e			paramErrors			: 8;
		radioErrors_e			radioErrors			: 8;
		macErrors_e				macErrors			: 8;
		systemErrors_e			systemErrors		: 8;
	};
} errorStatus_t;

typedef volatile struct {
	coreFunctions_e					function;
	coreConfiguration_t *			configurationPtr;
	coreStatus_t					status;
	uint32_t  						arg1;
	uint32_t	 					arg2;
	uint32_t 						arg3;
	uint32_t	 					arg4;
} coreArguments_t;

#define errorStatus_NoError					0

typedef struct {
	bool enabled					: 1;
	bool risingEdge					: 1;
	bool internalPullUpDown			: 1;
	uint32_t						: 21;
} wakeUpPin_t;

typedef struct {
	bool enabled				: 1;
	bool isTimestamp			: 1;
	uint8_t						: 6;
	union {
		uint32_t 	timestamp;
		struct {
			uint8_t		days;
			uint8_t		hours;
			uint8_t		minutes;
			uint8_t		seconds;
		};
	};
} wakeUpTime_t;

typedef enum  {
	modeSleep		 			= 0x1,
	modeDeepSleep	 			= 0x2,
	modeHibernate	 			= 0x3,
} sleepMode_e;

typedef enum  {
	coresM0p					= 1,
	coresM4	 					= 2,
	coresBoth	 				= 3
} sleepCores_e;

#define 	wakeUpPinHigh(pullDown)							((wakeUpPin_t) { .enabled = true, .risingEdge = true, .internalPullUpDown =  pullDown})
#define 	wakeUpPinLow(pullUp) 							((wakeUpPin_t) { .enabled = true, .risingEdge = false, .internalPullUpDown =  pullUp})
#define 	wakeUpPinOff      								((wakeUpPin_t) { .enabled = false })
#define 	wakeUpTimestamp(_timestamp)     				((wakeUpTime_t) { .enabled = true, .isTimestamp = true, .timestamp = _timestamp })
#define 	wakeUpDelay(_days, _hours, _minutes, _seconds)  ((wakeUpTime_t) { .enabled = true, .isTimestamp = false, .days = _days, .hours = _hours, .minutes = _minutes, .seconds = _seconds })
#define 	wakeUpTimeOff				     				((wakeUpTime_t) { .enabled = false })

/** Sleep configuration  */
/** Make sure to initialize all members, best practise is to use examples */
typedef struct  __attribute__ ((__packed__))
{
	wakeUpPin_t		wakeUpPin;							/**< S24 bits  */
	wakeUpTime_t	wakeUpTime;							/**< S40 bits  */
	sleepMode_e		sleepMode			: 2;			/**< Set sleepmode to Sleep, DeepSleep or Hibernate*/
	bool 			BleEcoON	 	 	: 1;			/**< Leaves BLE ECO ON during sleep. Consumes additional power, enable only when ECO/BLE functionality is needed during sleep*/
	bool 			DebugON				: 1;			/**< Leaves Debug Port active during idle. Consumes additional power, enable only for debugging purposes */
	sleepCores_e	sleepCores			: 4;
	bool			saveMAC				: 1;
	uint32_t							: 31;
} sleepConfig_t;

typedef enum  {
	stack_AS								= 1,			/**< Region Australia 923 MHz*/
	stack_AU 								= 2,			/**< Region Japan 915-928 MHz */
	stack_CN_L	 							= 3,			/**< Region China 470-510 MHz */
	stack_CN_H	 							= 4,			/**< Region China 779-787 MHz */
	stack_EU_L								= 5,			/**< Region Europe 433 MHz */
	stack_EU_H								= 6,			/**< Region Europe 863-870 MHz*/
	stack_IN								= 7,			/**< Region India 865-867 MHz */
	stack_KR								= 8,			/**< Region Korea 920-923 MHz*/
	stack_US								= 9,			/**< Region North America 902-928 MHz */
	stack_RU								= 10			/**< Region Russia 864-870 MHz*/
} stackRegion_e;

typedef struct  __attribute__ ((__packed__))
{
	uint32_t		buildYear				: 6;			/**< core firmware Year of build */
	uint32_t		buildMonth				: 4;			/**< core firmware Month of build */
	uint32_t		buildDayOfMonth			: 5;			/**< core firmware Day of build */
	uint32_t		buildHour				: 5;			/**< core firmware Hour of build (24h mode) */
	uint32_t		buildMinute				: 6;			/**< core firmware Minute of build */
	uint32_t		buildSecond				: 6;			/**< core firmware Second of build */
	uint32_t 		buildNumber;							/**< core firmware incremental build number */
	uint8_t			devEUI[8];								/**< core devEUI */
	char			buildType;								/**< core firmware build type */
	stackRegion_e 	stackRegion				:8;				/**< core firmware stack region */
	char		 	stackOption;							/**< core firmware stack option: 'S'ecure, 'P'SA, 'C'onfigurable */
	char			stackStage;								/**< core firmware lifecycle stage: 'a' pre-alpha, 'A' Alpha, 'b' perpetual beta, 'B' Beta, 'r' release candidate, 'R' Release */
	char			codeName[16];							/**< core firmware code name */
} coreInfo_t;
	
coreStatus_t				LoRaWAN_Reset(void);
coreStatus_t		 		LoRaWAN_Init(coreConfiguration_t * coreConfigurationPtr);
coreStatus_t				LoRaWAN_GetInfo(coreInfo_t * coreInfo);
coreStatus_t				LoRaWAN_Join(WaitMode_e waitMode);
coreStatus_t				LoRaWAN_MacSave();
coreStatus_t				LoRaWAN_FlashRead(uint8_t* buffer, uint8_t block, uint8_t length);
coreStatus_t				LoRaWAN_FlashWrite(uint8_t* buffer, uint8_t block, uint8_t length);
coreStatus_t 				LoRaWAN_GetRXdata(uint8_t * RXdata, uint8_t length);
coreStatus_t 				LoRaWAN_Send(uint8_t* buffer, uint8_t length, WaitMode_e waitMode);
coreStatus_t 				LoRaWAN_Sleep(sleepConfig_t * sleepConfig);
coreStatus_t 				LoRaWAN_GetStatus();
errorStatus_t 				LoRaWAN_GetError();

#endif /* ONETHINXCORE01_H */
