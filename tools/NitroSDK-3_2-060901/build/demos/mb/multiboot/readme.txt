MB Library Multiboot Parent Demo &ndash; Operation Summary -

When a program starts, the parent device automatically starts and displays the following list:
AID USERNAME STATE
1  name1    NONE
2  name2    NONE
3  name3    NONE
4  name4    NONE
 

Select the child device that will be the operation target using the + control pad and press the A button to run the operation associated with STATE. You also can perform the same operation with the A button for every connected child device by using the Start button. 

The SELECT button cancels and restarts wireless communications.

() Explanation of each STATE

NONE          ... Nothing is connected
CONNECTED     ... Child device is connected
DISCONNECTED  ... Child device ended a connection 
ENTRY REQUEST ... Entry request received from a child device [A Button: Permits entry; B Button: Denies entry]
KICKED        ... Child device was kicked 
ENTRY OK      ... A download request from a child device was allowed [A Button: Starts sending binary]
SENDING       ... Sending binary to a child device
SEND COMPLETE ... Binary send to the child device completed [A Button: Sends boot request]
BOOT REQUEST  ... Sends a boot request to a child device.
BOOT READY    ... Download is complete and the downloaded application is ready to run
MEMBER FULL   ... Game reached its maximum number of players; child devices cannot connect

Also, the operations can be automated by changing the following #define entries to 1.

#define AUTO_ACCEPT_ENTRY	(0)
#define AUTO_SENDFILE		(0)
#define AUTO_BOOTREQ		(0)

brvbar; 
This program only supports ASCII encoding, so some information, such as the user name in the current child device binary mb_child.srl, will not display correctly. Please be aware beforehand.


