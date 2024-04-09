******************************************************************************
**                                                                          **
**  Readme-NitroSDK-3_2-060901.txt                                          **
**                                                                          **
**  NITRO-SDK 3.2                                                           **
**                                                                          **
**  September 1, 2006                                                       **
**                                                                          **
******************************************************************************


[Versions with Verified Operation]

The operation of the SDK with the NITRO-System Library versions listed below were verified.

NITRO-System Library (05/29/2006 version)

Caution:

IS-NITRO-DEBUGGER version 1.65 or later must be installed to build programs under this SDK. For details, see Note 36-03.

[Revision History]

The revision history since the official release of NITRO-SDK 3.1 is listed below.

Note that the same information can be found in the following HTML file.

NitroSDK\man\en_US\changelog.htm

Contents:

NITRO-SDK 3.2 Official Release (09/01/2006)
Note 42-01: (CARD) Corrected function reference to CARD_GetRomHeader.
Note 42-02: (GX) Corrected AntiAlias sample demos.
Note 42-03: (OS) Added buffer overflow check for function cost calculation.
Note 42-04: (PM) Corrected issues associated with card removal during wireless communication.
Note 42-05: (WM) Corrected sample demos
Known issues in SDK 3.2 release version

NITRO-SDK 3.2 RC2 (08/17/2006)
Note 41-01: (CARD) Corrected the issue of console not powering down when a card is removed during wireless communication.
Note 41-02: (CP) Added wait processes inside the CP_RestoreContext function.
Note 41-03: (CP) Added a note regarding the use of dividers inside interrupts.
Note 41-04: (CTRDG) Corrected a problem related to hot-swapping cartridges.
Note 41-05: (OS) Added the OS_SleepThreadDirect function.
Note 41-06: (OS) Revised the internal thread switching process.
Note 41-07: (OS) Adjusted V count at time of startup.
Note 41-08: (PRC) Made processing related to dividers thread safe.
Note 41-09: (STD) Changed the specifications of character code conversion functions.
Note 41-10: (WBT) Fixed the WBT_CMD_REQ_GET_BLOCK_DONE notification bug.
Note 41-11: (WBT) Corrected sample demos
Note 41-12: (WM) Revised the WM_SetWEPKey/Ex function.
Note 41-13: (WM) Revised the reset processing of wepMode.
Note 41-14: (WM) Corrected a problem related to wepMode settings.
Note 41-15: (WM) Corrected sample demos
Known problems with SDK 3.2 RC2

 NITRO-SDK 3.2 RC (07/25/2006)
Note 40-01: (CARD) Corrected the issue of the console not powering down when a card is removed.
Note 40-02: (DevEnv) Flaw in how the ITCM/DTCM determines the size used with template files.
Note 40-03: (MATH) Revised the MATHCompareFunc reference.
Note 40-04: (MATH) Changed the MATH_QSortStackSize function.
Note 40-05: (MB) Revised the multiboot-PowerSave sample.
Note 40-06: (MB) Changed the default lifetime values.
Note 40-07: (STD) Revised the character code conversion functions.
Note 40-08: (VIB) Revised the VIB library
Note 40-09: (WM) Corrected the sample demos
Note 40-10: (WM) Prohibited new connections when communications have ended.
Known issues in SDK 3.2 RC

NITRO-SDK 3.2 PR (07/07/2006)
Note 39-01: (CARD) Revised the card pull-out detection process.
Note 39-02: (CTRDG) Revised the AGB backup access function reference.
Note 39-03: (CTRDG) Revised documents.
Note 39-04: (DevEnv) Ended support for old CodeWarrior version in shared definition files of the make system.
Note 39-05: (MATH) Revised the MATH_QSort  function.
Note 39-06: (MI) Corrected a problem with out-of-bounds access when using LZ77 compression.
Note 39-07: (MI) Added 8-bit versions of Endian conversion functions.
Note 39-08: (OS) Revised the OS_InitException function.
Note 39-09: (OS) Revised function cost calculation.
Note 39-10: (PM) Corrected a problem where the system will not power down if a card is removed while the LCD is turned off.
Note 39-11: (PM) Revised the PM_GoSleepMode function reference.
Note 39-12: (PM) Corrected a problem with the RTC alarm interrupt after recovering from sleep mode.
Note 39-13: (STD) Revised STD_CopyLString function reference.
Note 39-14: (STD) Changed the specifications of STD_CopyLString and added STD_CopyLStringZeroFill.
Note 39-15: (STD) Added conversion functions ShiftJIS <-> Unicode.
Note 39-16: (TOOL) Support for new overlay-related function for CodeWarrior 2.x.
Note 39-17: (TOOL) Revised reference regarding compstatic.
Note 39-18: (TOOL) Support for new specification 1Gbit/2 Gbit ROM for makerom.
Note 39-19: (TOOL) Added processing for determining valid combinations of ROM parameters.
Note 39-20: (WM) Revised the WFS sample demo.
Note 39-21: (WM) Changed the WM_EndDataSharing function.
Note 39-22: (WM) Adjusted the link level value when communicating with the Wii system.
Note 39-23: (WM) Corrected the sample demos
Note 39-24: (WM) Corrected a problem with the initialization and shutdown functions.
Note 39-25: (WM) Corrected a problem with port callback initialization inside the initialization function.
Note 39-26: (WM) Corrected illegal operations during disconnection.
Known issues in SDK 3.2 PR



Details
NITRO-SDK Version 3.2 Release
(09/01/2006)
--------------------------------------------------------------------------------
Note 42-01: (CARD) Corrected function reference to CARD_GetRomHeader.
Added a note that the CARD_GetRomHeader function can obtain the correct information only with the actual production console. 

--------------------------------------------------------------------------------
Note 42-02: (GX) Corrected AntiAlias sample demos.
A bug was corrected where the anti-alias ON/OFF display in $NitroSDK/build/demos/gx/UnitTours/AntiAlias sample demo was incorrectly displayed in the initial state (and only in the initial state). 

--------------------------------------------------------------------------------
Note 42-03: (OS) Added buffer overflow check for function cost calculation.
Added the OS_CheckFunctionCostBuffer and OS_CheckStatisticsBuffer functions to check whether the buffer used for the function cost calculation is full. 

--------------------------------------------------------------------------------
Note 42-04: (PM) Corrected issues associated with card removal during wireless communication.
An issue was corrected where the backlight will occasionally turn off when a card is removed during wireless communication. 

--------------------------------------------------------------------------------
Note 42-05: (WM) Corrected the sample demos
The following revisions have been made to the individual sample library $NitroSDK/build/demo/wireless_shared. 

An issue was corrected where, when only the file system is shared between the parent and child in the sample library WFS, an invalid file table is generated depending on the parent file count and child overlay count. 
An issue was corrected where, in the sample library WH, the error content could not be checked due to implicit reset process occurring internally. This is seen when the child is unexpectedly disconnected during MP communication start processing. 

--------------------------------------------------------------------------------
Known issues in SDK 3.2 release version
None.

--------------------------------------------------------------------------------

NITRO-SDK Version 3.2 RC2
(08/17/2006)
--------------------------------------------------------------------------------
Note 41-01: (CARD) Corrected the issue of the console not powering down when a card is removed during wireless communication.
A bug was corrected where the console would not power down properly when a card was removed during wireless communication with the clamshell closed. 

--------------------------------------------------------------------------------
Note 41-02: (CP) Added wait processes inside the CP_RestoreContext function.
When the status of the divider is restored using the CP_RestoreContext function inside an interrupt process, occasionally the status of the divider was not completely restored by the completion of the interrupt process.

To correct this issue, a necessary wait was added inside the CP_RestoreContext function. 

--------------------------------------------------------------------------------
Note 41-03: (CP) Added a note regarding the use of dividers inside interrupts.
Until now, a precaution regarding the use of CP inside interrupts was only included in the reference for the CP_SetDiv function, the CP_SetSqrt function, and the CP_RestoreContext function, but now sample code and notes regarding use inside interrupts have been added to the other CP function references on the CP overview page.

Furthermore, a link to this precaution has been added to all references of functions that use a divider internally. 

--------------------------------------------------------------------------------
Note 41-04: (CTRDG) Corrected a problem related to hot-swapping cartridges.
An issue was corrected where, if a soft reset was performed with a Game Pak inserted after first powering up without one, that Game Pak would actually be recognized. This occurred due to the fact that the Game Pak recognition was performed at the time of the soft reset in addition to the usual console startup. 

--------------------------------------------------------------------------------
Note 41-05: (OS) Added the OS_SleepThreadDirect function.
The function OS_SleepThreadDirect, capable of putting a specified thread directly into sleep mode, has been added. 

--------------------------------------------------------------------------------
Note 41-06: (OS) Revised the internal thread switching process.
Made changes so that the thread switching process of the OS_RescheduleThread function is executed with interrupts disabled. 

--------------------------------------------------------------------------------
Note 41-07: (OS) Adjusted V count at startup time.
Added a wait process so that the default V count value is constant at program startup. 

--------------------------------------------------------------------------------
Note 41-08: (PRC) Made processing related to dividers thread safe.
A problem was corrected where some code in the PRC SuperFine algorithm was using the divider without having interrupts disabled. 

--------------------------------------------------------------------------------
Note 41-09: (STD) Changed the specifications of character code conversion functions.
Function specifications have been changed so that the number of source characters that have actually been converted can also be retrieved in the same manner as the number of characters actually stored after conversion. 

--------------------------------------------------------------------------------
Note 41-10: (WBT) Fixed the WBT_CMD_REQ_GET_BLOCK_DONE notification bug.
A problem has been corrected where the WBT_CMD_REQ_GET_BLOCK_DONE notification was not being issued at the time of completion. This occurred when using the WBT_CMD_PREPARE_SEND_DATA notification with a NULL buffer specified for the WBT_RegisterBlock function. 

--------------------------------------------------------------------------------
Note 41-11: (WBT) Corrected the sample demos
A problem has been corrected where it was not possible to manage the connection status normally with the $NitroSDK/build/demos/wbt/wbt-1 sample demo when disconnection and connection was carried out repeatedly and the same mode was selected two or more times. 

--------------------------------------------------------------------------------
Note 41-12: (WM) Revised the WM_SetWEPKey/Ex function.
A problem was corrected with the WM_SetWEPKey/Ex function where the function would not function normally if NULL was specified for wepkey when the wepMode was WM_WEPMODE_NO. 

--------------------------------------------------------------------------------
Note 41-13: (WM) Revised the reset processing of wepMode.
A problem was corrected where the wepMode would not reset in cases such as when the status changed from parent to child. 

--------------------------------------------------------------------------------
Note 41-14: (WM) Corrected a problem related to wepMode settings.
A bug was found where the wepMode of the parent attempting a connection was being set to the child side. This has been revised to instead reflect the value of WM_SetWEPKey/Ex function called from the child. 

--------------------------------------------------------------------------------
Note 41-15: (WM) Corrected the sample demos
The following changes have been added to the wh sample library located in $NitroSDK/build/demo/wireless_shared. 

A problem has been corrected where the function WH_ChildConnectAuto, which searches for the parent with a given MAC address and automatically establishes a connection, could not be canceled normally. 
The revision in Note 39-23 was incomplete, so this has been corrected. 

--------------------------------------------------------------------------------
Known problems with SDK 3.2 RC2
None.

--------------------------------------------------------------------------------

NITRO-SDK Version 3.2 RC
(07/25/2006)
--------------------------------------------------------------------------------
Note 40-01: (CARD) Corrected the issue of the console not powering down when a card is removed.
In certain instances, the console did not power down when the card was removed with the clamshell closed.
This problem was corrected. 

--------------------------------------------------------------------------------
Note 40-02: (DevEnv) Flaw in how the ITCM/DTCM determines the size used with template files.
During the ITCM/DTCM allocated area overflow check using the lcf template file, the allocated area had been calculated as non-overlapped. This was still seen when an overlay area was configured in ITCM/DTCM and the area was actually overlapped through the use of overlays.
In certain cases this resulted in errors when free space was still available.

Because at the present time the ITCM/DTCM size check process with added overlay is not very well implemented in the lcf template, the overlay area will not be part of the used area check.

For this reason, those who create applications must pay careful attention to the management of the size of the overlay region on ITCM/DTCM.
Specifically, attach a dummy variable at the end of the ITCM/DTCM region and a check whether the variable address is out of bounds. 

--------------------------------------------------------------------------------
Note 40-03: (MATH) Revised the MATHCompareFunc reference.
In the reference for MATHCompareFunc, the description given under "Return Values" was the opposite of the actual operation, so this was corrected. 

--------------------------------------------------------------------------------
Note 40-04: (MATH) Changed the MATH_QSortStackSize function.
Following the changes described in Note 39-05, the MATH_QSortStackSize function was changed to not return a value equal to or less than 0. 

--------------------------------------------------------------------------------
Note 40-05: (MB) Revised the multiboot-PowerSave sample.
In the $NitroSDK/build/demos/mb/multiboot-PowerSave sample, the MB_SetPowerSaveMode function was not actually being called, so this has been corrected. 

--------------------------------------------------------------------------------
Note 40-06: (MB) Changed the default lifetime values.
Specifications have changed so that the default lifetime value used for wireless control within the MB library is the same as that set for the WM library. 

--------------------------------------------------------------------------------
Note 40-07: (STD) Revised the character code conversion functions.
The following revisions have been made regarding character code conversion functions. 

An error in the internal character conversion table has been corrected. 
Part of the function specifications have been changed to allow operations such as getting only the size after conversion and converting partial character strings. 

--------------------------------------------------------------------------------
Note 40-08: (VIB) Revised the VIB library
The following changes have been made to the VIB library. 

Added a check process so that invalid pulse parameters are not passed to the VIB_StartPulse function.
Improved pulse control precision following the modification of exclusion control in the library.

--------------------------------------------------------------------------------
Note 40-09: (WM) Corrected the sample demos
The WH_SetSsid function has been added to the WH sample module in $NitroSDK/build/demo/wireless_shared/wh. 

--------------------------------------------------------------------------------
Note 40-10: (WM) Prohibited new connections when communications have ended.
When terminating a parent using the WM_EndParent or WM_Reset functions, sometimes a new child would connect immediately after the parent disconnects children as part of end processing. The specifications have changed so that the child that attempts to connect is automatically disconnected in such cases.
At this time, the WM_DISCONNECT_REASON_NO_ENTRY notification will be sent to the child as the reason for the disconnection.

--------------------------------------------------------------------------------
Known issues in SDK 3.2 RC
None.

--------------------------------------------------------------------------------

NITRO-SDK Version 3.2 PR
(07/07/2006)
--------------------------------------------------------------------------------
Note 39-01: (CARD) Revised the card pull-out detection process.
A bug has been corrected where the automatic card removal detection did not work properly on the ARM7 processor side if a card was removed immediately after the program started. 

--------------------------------------------------------------------------------
Note 39-02: (CTRDG) Revised the AGB backup access function reference.
Added a note to the AGB backup access function reference stating that IS-NITRO-DEBUGGER version 1.66 and earlier cannot properly access the AGB backup device. 

--------------------------------------------------------------------------------
Note 39-03: (CTRDG) Revised documents.
Numerous errors have been corrected in the following document describing AGB cartridge backup access for the DS.

$NitroSDK/docs/TechnicalNotes/AboutAGBBackupforDS.pdf

--------------------------------------------------------------------------------
Note 39-04: (DevEnv) Ended support for old CodeWarrior versions in the shared definition files of the make system.
Support has ended for CodeWarrior for NINTENDO DS Version 1.0.2 and earlier versions in the common definition file commondefs.cctype.CW used by the make system.
Due to this change, the supported compiler will be version 1.2 and later. 

--------------------------------------------------------------------------------
Note 39-05: (MATH) Revised the MATH_QSort  function.
A problem was corrected where the function would not function normally if the number of pieces of data to be sorted was 0 or 1. 

--------------------------------------------------------------------------------
Note 39-06: (MI) Corrected a problem with out-of-bounds access when using LZ77 compression.
When compressing data using MI_CompressLZ or MI_CompressLZFast functions, the compression failed if the data after compression was larger than the data before compression. When this occurred, an out-of-bounds access of 1byte was seen in the compression target buffer.
This problem has been corrected. 

--------------------------------------------------------------------------------
Note 39-07: (MI) Added 8-bit versions of Endian conversion functions.
8-bit versions of functions for Endian conversion have been added.
These have been created to standardize the look of the source code; they will not perform an actual conversion. 

--------------------------------------------------------------------------------
Note 39-08: (OS) Revised the OS_InitException function.
Due to an error in the conditions set by the exception handler, there was a problem where the user-specified exception handler could not be called when a program was executed by loadrun.
This has been corrected so that the correct conditions are used. 

--------------------------------------------------------------------------------
Note 39-09: (OS) Revised function cost calculation.
During function cost calculation, an interrupt occasionally recorded a new cost calculation record during the _PROFILE_ENTRY and _PROFILE_EXIT function input/output record save operation. This prevented accurate cost calculations. The issue was circumvented by banning interrupts at the appropriate section of os_functioncost.c.  

--------------------------------------------------------------------------------
Note 39-10: (PM) Corrected a problem where the system will not power down if a card is removed while the LCD is turned off.
As a result of the revisions described in "Note 36-13: (PM), Revisions to the PM_ForceToPowerOffAsync function," when a card was removed while the LCD was turned off, the device would not power down even if it was closed because PM_ForceToPowerOffAsync could not be called while interrupts were disabled.
This has been fixed. 

--------------------------------------------------------------------------------
Note 39-11: (PM) Revised the PM_GoSleepMode function reference.
Parts of the description given in the reference for the PM_GoSleepMode function did not follow the guidelines. This has been corrected. 

--------------------------------------------------------------------------------
Note 39-12: (PM) Corrected a problem with the RTC alarm interrupt after recovering from sleep mode.
There have been certain cases where the RTC alarm interrupt stopped working after recovering from sleep mode if the RTC alarm was not included in the recovery parameter.
This problem was corrected. 

--------------------------------------------------------------------------------
Note 39-13: (STD) Revised the STD_CopyLString function reference.
An error in the STD_CopyLString function reference has been corrected. 

--------------------------------------------------------------------------------
Note 39-14: (STD) Changed the specifications of STD_CopyLString and added STD_CopyLStringZeroFill.
The STD_CopyLString function has been changed to operate in the same manner as the strlcpy function, as it was implemented under a different specification from the standard strlcpy.
With this change, the function return value has been changed from a 'char' type pointer, which indicates the copy target pointer, to an 'int' type value, which returns the character length of the copy source.

In addition, the old STD_CopyLString function, implemented under different specifications from strlcpy, has been added as the function STD_CopyLStringZeroFill. The return value has been changed to an int type value that returns the character string length of the copy source just like the function STD_CopyLString.

The difference between these two functions lies in the fact that, if the copy size for STD_CopyLStringZerofFill is larger than the copy source, the difference is padded with NULL. 

--------------------------------------------------------------------------------
Note 39-15: (STD) Added conversion functions ShiftJIS <-> Unicode.
Functions for performing conversion of ShiftJIS and Unicode character strings have been added to the STD library.
We have also added a demo showing how to use these functions to $NitroSDK/build/demos/std/unicode-1.

--------------------------------------------------------------------------------
Note 39-16: (TOOL) Support for new overlay-related function for CodeWarrior 2.x.
Beginning with CodeWarrior 2.x, a symbol search target overlay group may be specified when performing a link process.
The lsf file format was extended and the SearchSymbol command supported in order to support this feature.

The makelcf command and lcf template file were also revised in line with this change.
For details, see the makelcf manual.

With this change, CodeWarrior 2.0 is now officially supported. 

--------------------------------------------------------------------------------
Note 39-17: (TOOL) Revised reference regarding compstatic.
A description of the -f option has been added to the reference for the compstatic tool. 

--------------------------------------------------------------------------------
Note 39-18: (TOOL) Support for new specification 1Gbit/2 Gbit ROM for makerom.
Part of the ROM format has been changed to support the new 1Gbit/2Gbit ROMs. For a ROM image of 1Gbit or larger, a fixed size area of the top level became system reserved.
Beginning from NITRO-SDK 3.2 PR, a ROM format output following this new specification will be the default setting for makerom.

Be sure to add -V1 to the makerom options when you want to output a ROM image in the old format used prior to NITRO-SDK 3.2 PR. 

--------------------------------------------------------------------------------
Note 39-19: (TOOL) Added processing for determining valid combinations of ROM parameters.
We have added a process in the rsf file, which specifies makerom tool operation, to evaluate whether the given RomSize and RomSpeedType setting combination can actually be made into a ROM.

If the RomSpeedType is MROM, RomSize must be 512 Mbit or less.
Furthermore, if the RomSpeedType is 1TROM, RomSize must be 128 Mbit or more. 

--------------------------------------------------------------------------------
Note 39-20: (WM) Revised the WFS sample demo.
Added the following revisions to the WM sample demo. 

A problem has been corrected where, in the WFS sample demo in $NitroSDK/build/demos/wireless_shared/wfs, the internal status of WFS would become abnormal if the WFS_Start function was called before the initialization process of the WFS_Init function had completed.
This problem does not occur when the initialization function is called while the wireless state of both the parent and the child is IDLE and if the connection starts after the return from this function is made. 

--------------------------------------------------------------------------------
Note 39-21: (WM) Changed the WM_EndDataSharing function.
The state check inside the WM_EndDataSharing function has been deleted in line with the change described in Note 39-23. 

--------------------------------------------------------------------------------
Note 39-22: (WM) Adjusted the link level value when communicating with the Wii system.
The link level value that can be retrieved using the WM_GetLinkLevel function has been adjusted for when a Wii Console is the MP parent. 

--------------------------------------------------------------------------------
Note 39-23: (WM) Corrected the sample demos
A process for calling the WM_EndDataSharing function has been added inside both of the wc and wh Reset functions because the WM_EndDataSharing function was not called by the wh or wc sample libraries located under $NitroSDK/build/demo/wireless_shared. 

--------------------------------------------------------------------------------
Note 39-24: (WM) Corrected a problem with the initialization and shutdown functions.
If a WM library function was called from a different thread or an interrupt handler during execution of the WM_Init function, initialization was judged as complete even if it was not. This raised a chance for unexpected and illegal operations.
A similar state management problem was seen in WM_Finish function as well.
These problems have been fixed. 

--------------------------------------------------------------------------------
Note 39-25: (WM) Corrected a problem with port callback initialization inside the initialization function.
An issue with the port callback initialization in WM initialization function resulted in unnecessary warnings. This has been corrected. 

--------------------------------------------------------------------------------
Note 39-26: (WM) Corrected illegal operations during disconnection.
A problem has been corrected where the internal state of WM would occasionally become invalid if repeated connections and disconnections were made frequently. 

--------------------------------------------------------------------------------
Known issues in SDK 3.2 PR
None.

--------------------------------------------------------------------------------

End
