#include "msg_handlers.h"
#include "console.h"
#include "delay.h"
#include "debug_led.h"
#include "lan8720.h"
#include "protocol_layer.h"
#include "msg_code_user.h"

#include <string.h>
#include <stdio.h>

u8 link_status = 1;

/** on command received from console */
void onConsoleCommand(int para) {
	if(getCommandNum()>0) {
		CONSOLE_CMD * cmd = getCommand();
		//if(strcmp((char*)cmd->cmd,"cmd_template")==0) {
		//}
	}
}

/** on work tick */
void onWorkTick(int para) {
	if((para<8) && (para>=0)) {
		//check uart
		checkUartFsmc(((u8)para)*2);
		checkUartFsmc(((u8)para)*2+1);
	}
	else if(para==8) {
		closeLED(2);
		closeLED(3);
	}
	else if(para==9) {
		if(LAN8720_Get_link()) {
			closeLED(4);
			if(link_status) {
				link_status = 0;
				publishMsg(MSG_LINK_DOWN,0);
			}
		}
		else {
			openLED(4);
			link_status = 1;
		}
	}
	
	//flash cycle
	if(para<5) {
		openLED(1);
	}
	else {
		closeLED(1);
	}
}
