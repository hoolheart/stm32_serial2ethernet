#include "lwip/debug.h"
#include "httpd.h"
#include "lwip/tcp.h"
#include "fs.h"
#include "lwip_comm.h"

#include <string.h>
#include <stdlib.h>

#include "config.h"

#define NUM_CONFIG_CGI_URIS	(sizeof(ppcURLs) / sizeof(tCGI))
#define NUM_CONFIG_SSI_TAGS	(sizeof(ppcTAGs) / sizeof(char *))

const char *status_info[] = {//status info
	"",//0
	"本地IP配置错误",//1
	"子网掩码配置错误",//2
	"监听端口配置错误",//3
	"上报IP配置错误",//4
	"上报端口配置错误",//5
	"波特率配置错误",//6
	"校验配置错误",//7
	"轮询周期配置错误",//8
	"配置成功，重启后生效",//9
	"正在重启...",//10
};

int status_index = 0;
 
//config handler
const char* Config_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
//reboot handler
const char* Reboot_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);

static const char *ppcTAGs[]=  //SSI tags
{
	"lip",//0
	"lmask",//1
	"lgate",//2
	"lport",//3
	"rip",//4
	"rport",//5
	"bt01",//6
	"bt02",
	"bt03",
	"bt04",
	"bt05",
	"bt06",
	"bt07",
	"bt08",
	"bt09",
	"bt10",
	"bt11",
	"bt12",
	"bt13",
	"bt14",
	"bt15",
	"bt16",
	"p01",//22
	"p02",
	"p03",
	"p04",
	"p05",
	"p06",
	"p07",
	"p08",
	"p09",
	"p10",
	"p11",
	"p12",
	"p13",
	"p14",
	"p15",
	"p16",
	"cycle",//38
	"status"//39
};


static const tCGI ppcURLs[]= //cgi urls
{
	{"/config.cgi",Config_Handler},
	{"/reboot.cgi",Reboot_Handler},
};


//get cgi parameter
static int FindCGIParameter(const char *pcToFind,char *pcParam[],int iNumParams) {
	int iLoop;
	for(iLoop = 0;iLoop < iNumParams;iLoop ++ ) {
		if(strcmp(pcToFind,pcParam[iLoop]) == 0) {
			return (iLoop);
		}
	}
	return (-1);
}

//SSI handler
static u16_t SSIHandler(int iIndex,char *pcInsert,int iInsertLen)
{
	SYS_CONFIG *cfg = getConfig();//get config
	
	if(iIndex==0) {
		//local ip
		sprintf(pcInsert,"<input type=\"text\" name=\"local_ip\" style=\"width:200pt;\" value=\"%d.%d.%d.%d\"/>",
			cfg->ip_addr[0],cfg->ip_addr[1],cfg->ip_addr[2],cfg->ip_addr[3]);
	}
	else if(iIndex==1) {
		//mask
		sprintf(pcInsert,"<input type=\"text\" name=\"mask\" value=\"%d.%d.%d.%d\" style=\"width:200pt;\"/>",
			cfg->ip_mask[0],cfg->ip_mask[1],cfg->ip_mask[2],cfg->ip_mask[3]);
	}
	else if(iIndex==2) {
		//gate
		sprintf(pcInsert,"<input type=\"text\" name=\"gate\" value=\"%d.%d.%d.%d\" style=\"width:200pt;\"/>",
			cfg->ip_gate[0],cfg->ip_gate[1],cfg->ip_gate[2],cfg->ip_gate[3]);
	}
	else if(iIndex==3) {
		//listen port
		sprintf(pcInsert,"<input type=\"text\" name=\"lport\" value=\"%d\" style=\"width:200pt;\"/>",
			cfg->listen_port);
	}
	else if(iIndex==4) {
		//remote ip
		sprintf(pcInsert,"<input type=\"text\" name=\"rip\" value=\"%d.%d.%d.%d\" style=\"width:200pt;\"/>",
			cfg->remote_ip[0],cfg->remote_ip[1],cfg->remote_ip[2],cfg->remote_ip[3]);
	}
	else if(iIndex==5) {
		//remote port
		sprintf(pcInsert,"<input type=\"text\" name=\"rport\" value=\"%d\" style=\"width:200pt;\"/>",
			cfg->remote_port);
	}
	else if(iIndex<22) {
		//baud rate
		sprintf(pcInsert,"<input type=\"text\" name=\"%s\" value=\"%d\" style=\"width:200pt;\"/>",
			ppcTAGs[iIndex],cfg->baud_rates[iIndex-6]);
	}
	else if(iIndex<38) {
		//parity
		sprintf(pcInsert,"<input type=\"text\" name=\"%s\" value=\"%d\" style=\"width:200pt;\"/>",
			ppcTAGs[iIndex],cfg->parities[iIndex-22]);
	}
	else if(iIndex==38) {
		//cycle
		sprintf(pcInsert,"<input type=\"text\" name=\"cycle\" value=\"%d\" style=\"width:200pt;\"/>",
			cfg->query_cycle_s);
	}
	else if(iIndex==39) {
		//status
		sprintf(pcInsert,"%s",status_info[status_index]);
	}
	
	return strlen(pcInsert);
}

//handler to config
const char* Config_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
	int i,chl;
	char *value;
	SYS_CONFIG n_cfg;
	//local ip
	i = FindCGIParameter("local_ip",pcParam,iNumParams);
	if(i>=0) {
		value = pcValue[i];//get value
		if(sscanf(value,"%d.%d.%d.%d",&n_cfg.ip_addr[0],&n_cfg.ip_addr[1],&n_cfg.ip_addr[2],&n_cfg.ip_addr[3])!=4) {
			status_index = 1;
			return "/CONFIG.shtml";
		}
	}
	//mask
	i = FindCGIParameter("mask",pcParam,iNumParams);
	if(i>=0) {
		value = pcValue[i];//get value
		if(sscanf(value,"%d.%d.%d.%d",&n_cfg.ip_mask[0],&n_cfg.ip_mask[1],&n_cfg.ip_mask[2],&n_cfg.ip_mask[3])!=4) {
			status_index = 2;
			return "/CONFIG.shtml";
		}
	}
	//gate
	i = FindCGIParameter("gate",pcParam,iNumParams);
	if(i>=0) {
		value = pcValue[i];//get value
		if(sscanf(value,"%d.%d.%d.%d",&n_cfg.ip_gate[0],&n_cfg.ip_gate[1],&n_cfg.ip_gate[2],&n_cfg.ip_gate[3])!=4) {
			n_cfg.ip_gate[0] = n_cfg.ip_addr[0];
			n_cfg.ip_gate[1] = n_cfg.ip_addr[1];
			n_cfg.ip_gate[2] = n_cfg.ip_addr[2];
			n_cfg.ip_gate[3] = 1;
		}
	}
	//lport
	i = FindCGIParameter("lport",pcParam,iNumParams);
	if(i>=0) {
		value = pcValue[i];//get value
		if(sscanf(value,"%d",&n_cfg.listen_port)!=1) {
			status_index = 3;
			return "/CONFIG.shtml";
		}
	}
	//rip
	i = FindCGIParameter("rip",pcParam,iNumParams);
	if(i>=0) {
		value = pcValue[i];//get value
		if(sscanf(value,"%d.%d.%d.%d",&n_cfg.remote_ip[0],&n_cfg.remote_ip[1],&n_cfg.remote_ip[2],&n_cfg.remote_ip[3])!=4) {
			status_index = 4;
			return "/CONFIG.shtml";
		}
	}
	//rport
	i = FindCGIParameter("rport",pcParam,iNumParams);
	if(i>=0) {
		value = pcValue[i];//get value
		if(sscanf(value,"%d",&n_cfg.remote_port)!=1) {
			status_index = 5;
			return "/CONFIG.shtml";
		}
	}
	//baud rate
	for(chl=0;chl<16;chl++) {
		i = FindCGIParameter(ppcTAGs[6+chl],pcParam,iNumParams);
		if(i>=0) {
			value = pcValue[i];//get value
			if(sscanf(value,"%u",&n_cfg.baud_rates[chl])!=1) {
				status_index = 6;
				return "/CONFIG.shtml";
			}
		}
	}
	//parities
	for(chl=0;chl<16;chl++) {
		i = FindCGIParameter(ppcTAGs[22+chl],pcParam,iNumParams);
		if(i>=0) {
			value = pcValue[i];//get value
			if(sscanf(value,"%u",&n_cfg.parities[chl])!=1) {
				status_index = 7;
				return "/CONFIG.shtml";
			}
		}
	}
	//cycle
	i = FindCGIParameter("cycle",pcParam,iNumParams);
	if(i>=0) {
		value = pcValue[i];//get value
		if(sscanf(value,"%d",&n_cfg.query_cycle_s)!=1) {
			status_index = 8;
			return "/CONFIG.shtml";
		}
	}
	
	//success
	status_index = 9;
	setConfig(n_cfg);
	return "/CONFIG.shtml";
}

const char* Reboot_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
	applyConfig();
	status_index = 10;
	return "/CONFIG.shtml";
}

//initialize ssi handler
void httpd_ssi_init(void) {
	http_set_ssi_handler(SSIHandler,ppcTAGs,NUM_CONFIG_SSI_TAGS);
}

//initialize cgi handlers
void httpd_cgi_init(void) { 
  http_set_cgi_handlers(ppcURLs, NUM_CONFIG_CGI_URIS);
}


