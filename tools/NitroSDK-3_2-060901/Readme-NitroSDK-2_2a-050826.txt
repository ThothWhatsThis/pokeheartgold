**********************************************************************
                                     
  Readme-NitroSDK-2_2a-050826.txt    
                                     
  NITRO-SDK 2.2a                     
                                     
  August 26, 2005                    
                                     
**********************************************************************

==============================================================================
Table of Contents
==============================================================================

  1. Introduction
      1-1. Precautions
      1-2. Version on which Operation were checked

  2. Revision History
      2-1. Note headings
      2-2. Note details


==============================================================================
  1. Introduction
==============================================================================

  1-2. Version on which Operation were checked

    The NITRO-System Library versions for which an operation check has been performed for this SDK are listed below.

      NITRO-System Library (Version 06/06/2005)(*1) to which the NITRO-System Library Patch (Version 06/20/2005)(*2) has been applied.

        (*1) : NITRO-System-Library-050606.zip 
        (*2) : NITRO-System-Library-050606.zip 


==============================================================================
  2. Revision History
==============================================================================
This version is including NitroSDK-2_2-patch-plus-050912

- (WM) Re-revision of internal operations relating to connecting and disconnecting 
The contents disclosed in Note 29-10 and Note 29-11 were not complete, 
so further revisions were made.

- (MATH) Fixed problem in MATH_QSort
With the MATH_QSort() function, sometimes after sorting some values would not be 
correctly placed, both for ascending and descending sorts.  That problem has been fixed.
--------------------------------------------------------------------------


    The revision history since NITRO-SDK 2.1 is listed below.

    Note that the same information can be found in the following HTML file(*3).

        (*3) : NitroSDK\man\ja_JP\changelog.html


  2-1. Note headings

NITRO-SDK 2.2 Official Release (08/23/2005)

Note 01: (CARD) Revised internal processing of FRAM device operations.
Note 02: (FS) Corrections to the FS_EndOverlay function
Note 03: (FS) Corrections to documents
Note 04: (FX) Corrected problem in FX_InvSqrt function
Note 05: (MB) Added the MB_GetMultiBootDownloadParameter function
Note 06: (OS) Added reference for debug output functions on ARM7 side
Note 07: (OS) Added reference for NitroStaticInit function.
Note 08: (SND) Optimized the cache disabling process.
Note 09: (WM) Corrected problem in process that detects disconnection during MP communications.
Note 10: (WM) Fixed internal process related to connecting to child.
Note 11: (WM) Changed internal operations during disconnection process.
SDK 2.2 Known issues in official release

Å° NITRO-SDK Version 2.2 RC (08/08/2005)

Note 01: (CARD) Revised reference for CARD_CheckPulledOut function.
Note 02: (CARD)  Argument restriction for CARD_IdentifyBackup function.
Note 03: (DevEnv) Corrected path for ISD_NITRO_DEBUGGER.
Note 04: (FS) Changed the operations of the overlay functions.
Note 05: (GX) Further corrections to Sub_Double3D sample demo.
Note 06: (GX) Added the const qualifier to the arguments of the functions that get the status of the display list.
Note 07: (MATH) Accelerated the MD5 functions
Note 08: (MB) Made corrections to the sample demo.
Note 09: (PM) Corrected the recovery process after transitioning from LCD OFF state to sleep mode.
Note 10: (WM) Corrected problems with recovery process when ignoreFatalError has been set.
Note 11: (WVR) Corrected problem with the thumb build version of the ichneumon component
SDK 2.2 Known issues in RC

Å° NITRO-SDK Version 2.2 RC (07/15/2005)

Note 01: (CARD) Added the CARD_CheckPulledOut function.
Note 02: (CARD) Made compatible with new backup devices.
Note 03: (CTRDG) Corrected cache operations when getting cartridge information
Note 04: (DevEnv) Added the bin2obj tool
Note 05: (DevEnv) Provided support for times where the cygdrive prefix of CYGWIN is not  /cygdrive.
Note 06: (DevEnv) Corrected the LCF template.
Note 07: (DevEnv) Corrected problem with the StackSize specification for rsf.
Note 08: (DevEnv) Changed the default value for TS_VERSION.
Note 09: (DevEnv) Changed the #include search path.
Note 10: (FS) Added the functions FS_CreateFileFromMemory and FS_CreateFileFromRom.
Note 11: (FS) Added the FS_End function.
Note 12: (FS) Corrected problem with the FS_WaitAsync function.
Note 13: (FS) Corrected problem during overlay unloading.
Note 14: (FX) Corrected problem with the arctangent function
Note 15: (GX) Corrected the Sub_Double3D sample demo.
Note 16: (GX) Corrected problem with ASSERT inside the functions that manipulate display lists.
Note 17: (MATH) Corrected problem with the MATH_QSort function.
Note 18: (MATH)  Accelerated the SHA-1 functions.
Note 19: (MATH) Added Fast Fourier Transform functions.
Note 20: (MB) Added multiboot-wfs sample.
Note 21: (MB) Freed extended memory region used for debugging
Note 22: (MB) Added user-definable extended parameters
Note 23: (MI) Corrected problems with the MI_CompressHuffman function and Huffman compression of ntrcomo.exe.
Note 24: (OS) Corrected internal operation of OS_Sleep function
Note 25: (OS) Corrected problem with the OS_TryLockMutex function.
Note 26: (OS) Added the OS_YieldThread function.
Note 27: (OS) Made internal corrections to initialization process during startup.
Note 28: (OS) Expanded the startup screen process for Chinese-language version.
Note 29: (OS) Corrected problems with flushing of data cache.
Note 30: (OS) Corrected problem when multiple instances of OSMutex are locked.
Note 31: (OS) Added header file-related references.
Note 32: (RTC) Added functions to convert back and forth between date/time and total number of elapsed seconds.
Note 33: (RTC) Added restrictions to APIs for changing date and time.
Note 34: (WBT) Corrected wbt-fs sample demo
Note 35: (WBT) Corrected WBT sample demos
Note 36: (WM) Clearly stated the specification changes and planned deletion of Key Sharing functions.
Note 37: (WM) Changed the internal operations of the WM_StartScanEx function.
Note 38: (WM) Updated WMTestTool.srl
Note 39: (WM) Inhibited MP communications when a child is not connected
Note 40: (WM) Corrected problem with notification when child disconnects
Note 41: (WM) Added restriction on number of MP communications per frame.
Known issues in SDK 2.2 PR


  2-2. Note details

NITRO-SDK 2.2 Official Release (08/23/2005)
--------------------------------------------------------------------------------
Note 01: (CARD) Revised internal processing of FRAM device operations.
When an FRAM device is being used for backup, there was an occasional problem with inconsistency of the hardware internal status. A process was added to deal with this issue. 

--------------------------------------------------------------------------------
Note 02: (FS) Corrections to the FS_EndOverlay function
After making the operation changes noted in Note 28-04, a new problem was discovered. The destructor for the global object array in the overlay would not start correctly when unloading an overlay. This has been fixed. 

--------------------------------------------------------------------------------
Note 03: (FS) Corrections to documents
Made some corrections to the following document explaining the FS library: 

$NitroSDK/docs/TechnicalNotes/AboutFileSystem.pdf

--------------------------------------------------------------------------------
Note 04: (FX) Corrected problem in FX_InvSqrt function
When the argument passed to the FX_InvSqrt function was above a certain value the calculation would be incorrect. This problem has been fixed. 

--------------------------------------------------------------------------------
Note 05: (MB) Added the MB_GetMultiBootDownloadParameter function
In conjunction with the features added in Note 27-22, added the MB_GetMultiBootDownloadParameter function that references user-defined expanded parameters when downloading. 

--------------------------------------------------------------------------------
Note 06: (OS) Added reference for debug output functions on ARM7 side
Added references for the OS_InitPrintServer and OS_PrintServer functions, which are used for ARM 7 debug output. Also added that it is not necessary to use these functions in the application. 

--------------------------------------------------------------------------------
Note 07: (OS) Added reference for NitroStaticInit function.
Added an explanation of the NitroStaticInit function to the Function Reference, in line with the addition made in Note 27-31. 

--------------------------------------------------------------------------------
Note 08: (SND) Optimized the cache disabling process.
At the place where the DC_InvalidateRange function is called from the Sound library, the specified range was larger than necessary, so corrected this to be an appropriate size.
This correction reduces the load of the sound process. 

--------------------------------------------------------------------------------
Note 09: (WM) Corrected problem in process that detects disconnection during MP communications.
During MP communications, if the communications partner disappears (such as by powering off), there is a timeout and notification is made of the disconnection. However, due to a bug in the timeout process, sometimes notification of the disconnection was not made. This bug was fixed. 

--------------------------------------------------------------------------------
Note 10: (WM) Fixed internal process related to connecting to child.
In environments where communications status is extremely bad and the child repeatedly tries to connect to the parent, there was a bug that put parent into an abnormal internal state. That bug has been fixed. 

--------------------------------------------------------------------------------
Note 11: (WM) Changed internal operations during disconnection process.
As mentioned in Note 25-22, internal operation was changed to wait for the signal strength that can send the disconnection notification when disconnecting. But this has been changed to timeout after a set period of time. 
This corrects those cases where the process did not end when WMTestTool.srl was used to set the Noise rate to 100%. 

--------------------------------------------------------------------------------
SDK 2.2 Known issues in official release
Å@None.

--------------------------------------------------------------------------------
Å° NITRO-SDK Version 2.2 RC (08/08/2005)
--------------------------------------------------------------------------------
Note 01: (CARD) Revised reference for CARD_CheckPulledOut function.
Added a statement to the CARD_CheckPulledOut function reference indicating that the CARD_LockRom function must be called and CARD-ROM bus needs to be locked before calling this function .  

--------------------------------------------------------------------------------
Note 02: (CARD) Set restrictions on arguments for the CARD_IdentifyBackup function
Added a restriction so that CARD_BACKUP_TYPE_NOT_USE cannot be specified as the device type passed to the CARD_IdentifyBackup function.
This change was added in 2.2 PR, but it was left out of the changelog.

--------------------------------------------------------------------------------
Note 03: (DevEnv) Corrected path for ISD_NITRO_DEBUGGER.
Corrected the path location that ISD_NITRO_DEBUGGER indicates, which is defined in $NitroSDK/build/buildtools/commondefs.emtype.ISD.

--------------------------------------------------------------------------------
Note 04: (FS) Changed the operations of the overlay functions.
When there was a global object in an overlay to be unloaded that required a destructor, the destructor was called automatically. The operation has been changed so that the destructor is not called.

--------------------------------------------------------------------------------
Note 05: (GX) Further corrections to Sub_Double3D sample demo.
The following correction was made to the $NitroSDK/build/demos/gx/UnitTours/Sub_Double3D sample demo:

The correction described in Note27-15 was not complete and the same symptoms still appeared occasionally. The problem was corrected again.

--------------------------------------------------------------------------------
Note 06: (GX) Added the const qualifier to the arguments of the functions that get the status of the display list.
The contents of the pointer passed to the arguments of the following functions have not been rewritten; however, the const qualifier had not been added to the arguments, so const has been added to them. 

G3_GetDLStart function
G3_GetDLLength function
G3_GetDLSize function

--------------------------------------------------------------------------------
Note 07: (MATH) Accelerated the MD5 functions
Replaced the implementations of the MD5 functions.
The code sizes are about half of what they were before, and the functions operate approximately 20% faster compared to the previous implementation.
The interface has not been changed.
This change was added in 2.2 PR, but it was left out of the changelog. 

--------------------------------------------------------------------------------
Note 08: (MB) Made corrections to the sample demo.
The following correction was made to the $NitroSDK/build/demos/mb/cloneboot sample:

When the ConnectMain function on the parent failed, a reattempt was made without ending the WH module. This bug has been fixed. 

--------------------------------------------------------------------------------
Note 09: (PM) Changed restore operation after transitioning from LCD Off to sleep mode
When moved from the LCD OFF state to sleep mode and then recovered a discrepancy would arise between the actual state and the internal state of the GX library.
This has been fixed.
(See Note22-23)

--------------------------------------------------------------------------------
Note 10: (WM) Corrected problems with recovery process when ignoreFatalError has been set.
When the ignoreFatalError argument of the WM_StartMPEx function is set to TRUE, there should be no auto-disconnect even when a fatal error occurs with the child. However, due to a problem in this process, communications become unavailable after this point.
This has been fixed. 

--------------------------------------------------------------------------------
Note 11: (WVR) Corrected problem with the thumb build version of the ichneumon component
It was confirmed that programs do not operate when the thumb build version of the ichneumon component is used. This has been corrected. 

--------------------------------------------------------------------------------
SDK 2.2 Known issues in RC
Å@None.

--------------------------------------------------------------------------------

Å° NITRO-SDK Version 2.2 RC (07/15/2005)

--------------------------------------------------------------------------------
Note 01: (CARD) Added the CARD_CheckPulledOut function.

Added the CARD_CheckPulledOut function, which determines directly from ARM9 without waiting for the notification of the result of the card removal detection process that the ARM7 performs periodically. 
This function is not needed for normal scenes, where the Card is not being accessed, but in scenes where you need to know without delay that the Card has been removed, you can use this function instead of CARD_IsPulledOut. 

--------------------------------------------------------------------------------
Note 02: (CARD) Made compatible with new backup devices.

The CARD library was made to support 512KB EEPROM and 256KB FRAM as backup devices. Added CARD_BACKUP_TYPE_EEPROM_512KBITS and CARD_BACKUP_TYPE_FRAM_256KBITS to the enumerated type CARDBackupType.  

--------------------------------------------------------------------------------
Note 03: (CTRDG) Corrected cache operations when getting cartridge information

Corrected the cache operations involved in the reading of the cartridge information region during the execution of the CTRDG_Init function.
Up until now, when the cartridge information such as the maker code and game code was obtained, sometimes incorrect values were returned. This problem has been corrected. 

--------------------------------------------------------------------------------
Note 04: (DevEnv) Added the bin2obj tool

Added the bin2obj tool for converting binary files into object files.
This tool operates the same way as the BinToElf tool in the CodeWarrior for NINTENDO DS package, so you can use either one. 

--------------------------------------------------------------------------------
Note 05: (DevEnv) Provided support for times where the cygdrive prefix of CYGWIN is not  /cygdrive.

When a build was performed from the CYGWIN command line, building process failed if the cygdrive prefix was not /cygdrive which is the default setting .
This has been corrected so that builds are possible when the cygdrive prefix has been changed. 

--------------------------------------------------------------------------------
Note 06: (DevEnv) Corrected the LCF template.

The .sinit section is not linked in the case of programs which use the LCF template $NitroSDK/include/nitro/specfiles/ARM9-TS-cloneboot-C.lcf.template
The problem where the static initializer feature could not be used has been corrected. 

--------------------------------------------------------------------------------
Note 07: (DevEnv) Corrected problem with the StackSize specification for rsf.

With StackSize specified in rsf (linker spec file), the specification that subtracts the absolute value from the maximum possible size for specification when a negative number was specified for the first parameter was not valid. This has been fixed. 

--------------------------------------------------------------------------------
Note 08: (DevEnv) Changed the default value for TS_VERSION.

Changed the default value of the SDK_TS_VERSION constant that indicates the PMIC version. The value was 100 but has been changed to 200.
SDK_TS_VERSION = 200 is the version E and later versions of the IS-NITRO-EMULATOR hardware and corresponds to the PMIC version embedded in the Nintendo DS retail product.
This change does not alter the internal operations of the NITRO-SDK features, but you should be aware that this could affect applications (such as sample demos that use the microphone) that use this constant as a build switch for changing the behavior.
When NITRO_TS_VERSION is specified as the environment variable, SDK_TS_VERSION takes the specified value and not the default value.

NITRO-SDK has an os and an spi library prepared for every TS_VERSION.
This has relevance when the development environment used  the TS breadboard, etc. and has owner information in an old save format. But when the save format is the same as that used for the retail console, the library is exactly the same.
When the make system is used, the library for linking is selected automatically based on the environment variable. But in a development environment using IDE, you need to manually select which library to link.
Actually, except in the development environment that uses the TS breadboard, etc., there is no meaning in selecting a library and there is no problem using the library that gets linked by default. 

--------------------------------------------------------------------------------
Note 09: (DevEnv) Changed the #include search path.

Changed the #include search path when conducting a build with Makefile to include the directory of the file that called #include.
If the previous search path is required, enter a specification as MACRO_FLAGS += -cwd proj inside Makefile. 

--------------------------------------------------------------------------------
Note 10: (FS) Added the functions FS_CreateFileFromMemory and FS_CreateFileFromRom.

Added the FS_CreateFileFromMemory function and FS_CreateFileFromRom function to treat a specific region in memory region or Card as a temporary file to open it. 
Also added the sample demo $NitroSDK/build/demos/fs/file-3 to demonstrate the behavior of these functions. 

--------------------------------------------------------------------------------
Note 11: (FS) Added the FS_End function.

Added the function FS_End  to return the state to what it was before the FS library was initialized. 

--------------------------------------------------------------------------------
Note 12: (FS) Corrected problem with the FS_WaitAsync function.

When the FS_WaitAsync function was called for a file handle when an asynchronous command was not yet processing in the archive, the archive processing would stop. That problem has been fixed. 

--------------------------------------------------------------------------------
Note 13: (FS) Corrected problem during overlay unloading.

When the FS_UnloadOverlay or FS_EndOverlay function unloaded an overlay, the destructor was not called correctly. The problem has been fixed. This applies to objects where all of the following conditions are met at the same time:

The destructor entity located in the overlay .data section is not located on the overlay

--------------------------------------------------------------------------------
Note 14: (FX) Corrected problem with the arctangent function

The arctangent functions noted below returned incorrect values when the (x,y) slope is extremely close to 1. This has been fixed. 

FX_Atan2
FX_Atan2Idx
This applies to (x,y) when the following conditions are met:
((x != y) && (FX_Div(y, x) == FX32_ONE))

--------------------------------------------------------------------------------
Note 15: (GX) Corrected the Sub_Double3D sample demo.

The following correction was made to the $NitroSDK/build/demos/gx/UnitTours/Sub_Double3D sample demo:

When two-screen 3D and a heavy-load process were put together, the things in the upper screen would occasionally wrap around to the bottom screen briefly (or vice versa). This problem has been fixed. 

--------------------------------------------------------------------------------
Note 16: (GX) Corrected problem with ASSERT inside the functions that manipulate display lists.

When the G3C_UpdateGXDLInfo and G3B_UpdateGXDLInfo functions were called from inside functions that manipulate display lists, the ASSERT determination did not go through when a display list was being created that was the same size as the buffer. 
This bug was fixed. 

--------------------------------------------------------------------------------
Note 17: (MATH) Corrected problem with the MATH_QSort function.

When sorting with the MATH_QSort function, in some cases the target array would face over-access. This bug was fixed. 

--------------------------------------------------------------------------------
Note 18: (MATH)  Accelerated the SHA-1 functions.

Replaced the implementations of the SHA-1 functions.
The code size has hardly changed, but they can operate approximately twice as fast as before.
The interface has not been changed. 

--------------------------------------------------------------------------------
Note 19: (MATH) Added Fast Fourier Transform functions.

Added functions that perform fast Fourier transforms.
Also added the demos $NitroSDK/build/demos/math/fft-1 and fft-2 to confirm the operation of these functions. 

--------------------------------------------------------------------------------
Note 20: (MB) Added multiboot-wfs sample.

Added a sample of the child in Single-Card Play using the file system via wireless communications in $NitroSDK/build/demos/mb/multiboot-wfs. 

--------------------------------------------------------------------------------
Note 21: (MB) Freed extended memory region used for debugging

For a child booted with the MB protocol, there was a restriction that prevented the use of expanded memory and forcibly set main memory to 4M. That restriction has been removed, enabling use of expanded memory by IS-NITRO-EMULATOR and other hardware that have 8M of main memory. 

--------------------------------------------------------------------------------
Note 22: (MB) Added user-definable extended parameters

Added the userParam member to the MBGameRegistry structure.
Set as much as 32 bytes of any kind of data in this buffer and use the MB_RegisterFile function to allow a Single-Card Play child to reference this data after starting up. 

--------------------------------------------------------------------------------
Note 23: (MI) Corrected problems with the MI_CompressHuffman function and Huffman compression of ntrcomo.exe.
 

The MI_CompressHuffman function and ntrcomp.exe had a bug that sometimes caused abnormal compression of data filled with a single value. This bug was fixed. 

--------------------------------------------------------------------------------
Note 24: (OS) Corrected internal operation of OS_Sleep function

If  the OS_WakeupThreadDirect function was called for a thread that was in the wait state after a call to the OS_Sleep function, after that there would be problems with the alarm process. That has been fixed. 

--------------------------------------------------------------------------------
Note 25: (OS) Corrected problem with the OS_TryLockMutex function.

There were errors in the list operations on mutex kept in the OS when mutex was locked by the OS_TryLockMutex function.
As a result, sometimes the OS_UnlockMutex function would cause exceptions, and problems would arise when a thread ended while mutex was still locked.
This has been fixed.
The OS_LockMutex function (which is different from the OS_TryLockMutex function mentioned here) operates normally and has no problems. 

--------------------------------------------------------------------------------
Note 26: (OS) Added the OS_YieldThread function.

Added the OS_YieldThread function, which performs thread rescheduling and gives executable threads with the same priority level as the current thread the opportunity to execute.

--------------------------------------------------------------------------------
Note 27: (OS) Made internal corrections to initialization process during startup.

Corrected the internal processes to make sure that cache has been cleaned/invalidated when initializing autoload and each segment during the startup of the program.  

--------------------------------------------------------------------------------
Note 28: (OS) Expanded the startup screen process for Chinese-language version.

The startup screen display process for Chinese-language applications was modified with SDK_WEAK_SYMBOL and made public with the OS_ShowAttentionChina function. This implementation can be replaced by external definitions.

In conjunction with this, added the sample $NitroSDK/build/demos/os/forChina-fs. This shows how to load the data for images that are only used at startup from the file system without a static link. 

--------------------------------------------------------------------------------
Note 29: (OS) Corrected problems with flushing of data cache.

The cause of the bugs handled in the Nitro-SDK2.1 PR and Nitro-SDK2.1 RC releases (covered in Note 24-10 and Note 25-16) has been clarified.  Along with this, the internal implementation of the functions DC_FlushRange and DC_FlushAll have been changed so that operations are handled by the CPU flush command.
Specifically, a command to wait for the write buffer to be empty had been inserted immediately before the flush command and when returning from the interrupt handler. This was done in order to avoid the symptoms of the problem, which was that a clean cache line would be invalidated when the flush command was executed when the write buffer was full. 
The function specifications have not changed. 

--------------------------------------------------------------------------------
Note 30: (OS) Corrected problem when multiple instances of OSMutex are locked.

There was a problem with the list process of mutex stored in OSMutex.
Specifically, a problem arose when one thread used the OS_LockMutex or OS_TryLockMutex function to lock more than one OSMutex at the same time. 
This has been fixed.

The problem does not occur when one thread locks only one OSMutex at a time. 

--------------------------------------------------------------------------------
Note 31: (OS) Added header file-related references.

Added explanations about the main items and the special features of the NITRO-SDK header files, placing the explanations inside the OS category of the Function Reference.
Explanations were added about the following files: 

nitro.h
nitro_win32.h
nitro/sinit.h
nitro/code32.h, nitro/code16.h, nitro/codereset.h
nitro/dtcm_begin.h, nitro/dtcm_end.h
nitro/itcm_begin.h, nitro/itcm_end.h
nitro/parent_begin.h, nitro/parent_end.h
nitro/version_begin.h, nitro/version_end.h
nitro/wram_begin.h, nitro/wram_end.h
nitro/version.h

--------------------------------------------------------------------------------
Note 32: (RTC) Added functions to convert back and forth between date/time and total passage of seconds.

Added functions to the RTC category that convert between type RTCDate and RTCTime date/time data and the total number of seconds that have passed since January 1 of the year 2000. Also added functions that convert between type RTCDate data and the total number of days that have passed.
The demo that can be used to confirm these operations is $NitroSDK/build/demos/rtc/convert-1. 

--------------------------------------------------------------------------------
Note 33: (RTC) Added restrictions to APIs for changing date and time.

APIs have been prepared for debug purposes that can change the RTC's date and time. To avoid incorrect use of these APIs in non-debug applications, a restriction has been introduced that causes any request to write to the RTC to fail if NITRO_FINALROM has been specified.
Following are the APIs whose operations have been changed: 

RTC_SetDateTimeAsync
RTC_SetDateTime
RTC_SetTimeAsync
RTC_SetTime
RTC_SetDateAsync
RTC_SetDate

--------------------------------------------------------------------------------
Note 34: (WBT) Corrected wbt-fs sample demo

The following correction was made to the sample demo $NitroSDK/build/demos/wbt/wbt-fs:

Table memory processed with the FS_LoadTable function was not released with every connection and disconnection but instead was maintained, causing memory leaks. The problem has been fixed. 

--------------------------------------------------------------------------------
Note 35: (WBT) Corrected WBT sample demos

The following revisions were made to the $NitroSDK/build/demos/wireless_shared/wc sample module and the $NitroSDK/build/demos/wbt/wbt-1 demo:

During continuous MP communications, the wireless parent would unconditionally overwrite the Send buffer when a new child was connected. Depending on the timing, data would be sent to wireless hardware while data was being loaded to the Send buffer, and this would result in WBT packet mismatches. This problem has been fixed.

The following processing has been added by waiting for information on children that are currently connected to the WBT parent inside $NitroSDK/build/demos/wbt/wbt-1/src/main.c.
  1. If it is the first child, start continuous MP communications.  
  2. If it is the second or higher child, do nothing. (As there is nothing to do during serial MP communications.)
  3. If a child ceases to exist, stop serial MP communications.
	 

Since the above process requires information about the disconnection of child units, $NitroSDK/build/demos/wireless_shared/wc/wc.c has been revised so that a callback is called on the sample side when a child is disconnected. 

--------------------------------------------------------------------------------
Note 36: (WM) Clarified the specification changes and planned abolition of Key Sharing functions.

In order to decrease the memory size used by WM, the specifications have been changed for each of the infrequently used functions WM_StartKeySharing, WM_EndKeySharing, and WM_GetKeySet.
Dedicated buffer memory for Key Sharing is no longer secured during WM initialization.
In addition, since the Key Sharing features are scheduled to be abolished at a later date, a section was added to the document asking that programmers instead use the functions WM_StartDataSharing, WM_EndDataSharing, and WM_StepDataSharing. 

--------------------------------------------------------------------------------
Note 37: (WM) Changed the internal operations of the WM_StartScanEx function.

It has been determined that when an SSID filter is specified by the WM_StartScanEx function, sometimes the SSID inside the discovered parent information has invalid data that differs from the specified SSID.

To deal with this problem, a process has been added inside the WM library that, when necessary, overwrites the filter-specified SSID with the SSID inside the discovered parent information. 

Note that this only happens when the parent is using infrastructure mode communications and is hiding its SSID. 

--------------------------------------------------------------------------------
Note 38: (WM) Updated WMTestTool.srl

Updated the tool that is used for testing wireless communications: NitroSDK/bin/ARM9-TS/Rom/WMTestTool.srl. 
Up until now, when an access point was discovered during scanning, the message "WLAN AP" was displayed.  That has been changed so that now the SSID is displayed.
This was implemented for both Scan Test mode and ScanEx Test mode. 

--------------------------------------------------------------------------------
Note 39: (WM) Inhibited MP communications when a child is not connected

Up until now, when the wireless communications parent called the WM_StartMP function, it would send an MP frame even if no child was connected.  This has been changed so the parent does not do this.
The basic behavior of the port's send/receive callbacks has not changed. 

--------------------------------------------------------------------------------
Note 40: (WM) Corrected problem with notification when child disconnects

During MP communications, when the child responded with nothing but NULL and the communications timed out, the notification WM_STATECODE_DISCONNECTED would get posted multiple times to the parent's callback WMStartParentCallback. This has been fixed.

This problem mainly occurred when a child was started by IS-NITRO-DEBUGGER and was paused for a long time during MP communications. 

--------------------------------------------------------------------------------
Note 41: (WM) Added restriction on number of MP communications per frame.

Up until now, during continuous communications mode and after communications failed repeatedly, transmissions would just keep on happening.  But in a situation where only empty data is being sent and received, the number of MP communications expands rapidly and the child's data-receiving process cannot keep up. This can trigger problems.
To counter this problem, a restriction has been set that limits the number of MP communications that the parent can send in one frame to 6 (including resends).
This restriction is not placed on normal communications. 

--------------------------------------------------------------------------------
Known issues in SDK 2.2 PR

None.

--------------------------------------------------------------------------------
