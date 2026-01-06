/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**********************************************************
 * FILE NAME:    Ethernet
 * MODULE NAME:  Ethernet
 * PROGRAMMER:   
 * DESCRIPTION: Test Ethernet Functions，V1M withou Ethernet port and ctosapi.h api fewing
 * REVISION:     01.00
 **********************************************************/

/*========================================*
 * I N C L U D E S                        *
 *=========================================*/
#include <string.h>
#include <stdio.h>
#include <ctosapi.h>
#include <stdlib.h>
#include <stdarg.h>

#include "Util/PlatformRelated/Inc/libMisc/DebugInfoDefine.h"
#include "Util/PlatformRelated/Inc/libMisc/DebugInfo.h"
#include "Util/PlatformRelated/Inc/libMisc/ByteStream.h"
#include "Util/PlatformRelated/Inc/libMisc/DbgMacro.h"
#include "V3AppWifi/Common.h"
#include "Util/PlatformRelated/Inc/libMiscEx/JsonUtil.h"
#include "CPComponent/VEGA1000C/Inc/libComm/libComm.h"
/*==========================================*
 * D E F I N E S                           *
 *==========================================*/
#define d_BUFF_SIZE        128     // Buffer Size
#define TEMP_WIFI_INFO "{\n" \
                        "        \"SSID\": \"CB6Test\",\n"     \
                        "        \"Password\": \"12345678\",\n"    \
                        "        \"AutoConn\": \"FALSE\",\n"       \
                        "        \"DHCP\": \"TRUE\", \n"           \
                        "        \"IP\": \"192.168.31.144\",\n"    \
                        "        \"Mask\": \"255.255.255.0\",\n"   \ 
                        "        \"Gateway\": \"192.168.31.1\",\n" \
                        "        \"DNS\": \"192.168.31.1\",\n"     \
                        "        \"Protocol\": \"WPA2\",\n"        \
                        "        \"PairwiseCipher\": \"CCMP\",\n"  \
                        "        \"GroupCipher\": \"CCMP\"\n"      \
                        "    }"
                                
// Define const Ethernet Data
BYTE ip_addr[] = "192.120.100.220";       // IP Address
BYTE mask[] = "255.255.255.0";            // Mask
BYTE gate_ip[] = "192.120.100.200";       // Gateway IP
BYTE host_ip[] = "192.120.100.22";        // Default Host IP
BYTE host_port[] = "6666";                // Default Host Port
BYTE auto_connect[] = "0";
BYTE manual_connect[] = "2";
BYTE blank_str[] = " ";

// Declare Global Variable
USHORT ret;                                 // Return Value
BYTE babuff[d_BUFF_SIZE];                  // Global Buffer
BYTE key;

enum emKeyMode {                            // Key Mode
    eMain,
    eConfig,
    eConnected,
    eTxData,
    eRxData,
};
enum emKeyMode emKM;


/*
 * InitWifi
 * Function:  init and connect wifi
 * 
 * @param
 *      [IN] szSSID: SSID
 *      [IN] szPasswd: password
 *      [IN] nTimeout: timeout
 * @return
 *      0 means OK, others mean fail
 * 
 * @Author: Gray Hu
 */
int InitWifi(const char* szSSID, const char* szPasswd, int nTimeout)
{
    DEBUG_ENTRY();

    if (szSSID == NULL || szPasswd == NULL || nTimeout == 0)
    {
        DEBUG_RETCODE("invalid params", 1);
        return LINKSTATUS_WIFI_ERROR;
    }

    const char* szWifiConfig = TEMP_WIFI_INFO;
    JsonUtil cJsonUtil;
    cJsonUtil.Parse(szWifiConfig);
    cJsonUtil.SetData("SSID", szSSID);
    cJsonUtil.SetData("Password", szPasswd);
    szWifiConfig = cJsonUtil.Print();
    DEBUG_INFO(DEBUGINFO_NORMAL, "wifi config: [%s]", szWifiConfig);

    BYTE byCommType = LINKTYPE_WIFI;
    int nOpenStatus = 0;

    nOpenStatus = CTCP_OpenComm(byCommType, szWifiConfig, NULL);
    DEBUG_INFO(DEBUGINFO_NORMAL, "Open network status %d\n", nOpenStatus);

    if (nOpenStatus == d_OK)
    {
        nOpenStatus = CTCP_WifiConnectAP(szWifiConfig, nTimeout);
        DEBUG_INFO(DEBUGINFO_NORMAL, "connect WifiConnectAP status %d\n", nOpenStatus);
        if (nOpenStatus != d_OK)
        {
            DEBUG_INFO(DEBUGINFO_NORMAL, "Connect Wifi AP failed");
        }
        else 
        {
            SetLinkType(LINKTYPE_WIFI);
            DEBUG_INFO(DEBUGINFO_NORMAL, "Ping process");
            if (GetPingAddr != NULL)
            {
                std::string strTempPing;
                GetPingAddr(strTempPing);

                if (strTempPing.length() > 0)
                {
                    nOpenStatus = CTOS_WifiPing((PBYTE) strTempPing.c_str(), 20);
                    DEBUG_INFO(DEBUGINFO_NORMAL, "PING [%s] %d\n", strTempPing.c_str(), nOpenStatus);
                }
            }
        }
    }

    return DEBUG_EXIT(nOpenStatus);
}

/* ========================================================
 * FUNCTION NAME: show_screen
 * DESCRIPTION:  Show on the LCD Display
 * RETURN:       none
 * NOTES:        none
 * ========================================================*/
void show_screen(int tag)
{
    // Clear LCD Display
    CTOS_LCDTClearDisplay();
    
    switch (tag) {
    case 0:
        CTOS_LCDTSetReverse(TRUE);          // The reverse enable
        CTOS_LCDTPrintXY(1, 1, " ETHERNET TEST ");   // Display a string in the 1 line
        CTOS_LCDTSetReverse(FALSE);         // The reverse disable
        CTOS_LCDTPrintXY(1, 2, "1.Config");         // Display a string in the 2 line
        CTOS_LCDTPrintXY(1, 3, "2.Connect");        // Display a string in the 3 line
        CTOS_LCDTPrintXY(1, 4, "3.Tx Data");        // Display a string in the 4 line
        CTOS_LCDTPrintXY(1, 5, "4.Rx Data");        // Display a string in the 5 line
        CTOS_LCDTPrintXY(1, 6, "5.Ge t Status");      // Display a string in the 6 line
        CTOS_LCDTPrintXY(1, 7, "6.Ping");           // Display a string in the 7 line
        CTOS_LCDTPrintXY(1, 8, "7.Exit");           // Display a string in the 8 line
        break;
        
    case 1:
        CTOS_LCDTSetReverse(TRUE);
        CTOS_LCDTPrintXY(1, 1, " Config Setting ");
        CTOS_LCDTSetReverse(FALSE);
        CTOS_LCDTPrintXY(1, 2, "1. Set");
        CTOS_LCDTPrintXY(1, 3, "2. Get");
        break;
        
    case 2:
        CTOS_LCDTSetReverse(TRUE);
        CTOS_LCDTPrintXY(1, 1, " Connected ");
        CTOS_LCDTSetReverse(FALSE);
        CTOS_LCDTPrintXY(1, 2, "1. Connect");
        CTOS_LCDTPrintXY(1, 3, "2. DisConnect");
        break;
    }
}

/* ========================================================
 * FUNCTION NAME: EthernetSetConfig
 * DESCRIPTION:  Set Ethernet configuration
 * RETURN:       none
 * NOTES:        none
 * ========================================================*/
void EthernetSetConfig(void)
{
    BYTE buff[16 + 1];
    
    CTOS_LCDTClearDisplay();
    CTOS_LCDTPrintXY(1, 1, "--Ethernet Set--");
    
    /* Set up the configuration of Ethernet */
    // Set IP Address
    memset(buff, 0, sizeof(buff));
    memcpy(buff, (BYTE*)ip_addr, strlen(ip_addr));
    ret = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_IP, buff, strlen(buff));
    if (ret != d_OK) {
        CTOS_LCDTPrintXY(1, 8, "Set IP Not OK");
        return;
    }
    CTOS_LCDTPrintXY(1, 2, buff);
    
    // Set Mask
    memset(buff, 0, sizeof(buff));
    memcpy(buff, (BYTE*)mask, strlen(mask));
    ret = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_MASK, buff, strlen(buff));
    if (ret != d_OK) {
        CTOS_LCDTPrintXY(1, 8, "Set Mask Not OK");
        CTOS_Delay(1000);
        return;
    }
    CTOS_LCDTPrintXY(1, 3, buff);
    
    // Set Gateway IP
    memset(buff, 0, sizeof(buff));
    memcpy(buff, (BYTE*)gate_ip, strlen(gate_ip));
    ret = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_GATEWAY, buff, strlen(buff));
    if (ret != d_OK) {
        CTOS_LCDTPrintXY(1, 8, "Set GATE Not OK");
        return;
    }
    CTOS_LCDTPrintXY(1, 4, buff);
    
    // Set Host IP
    memset(buff, 0, sizeof(buff));
    memcpy(buff, (BYTE*)host_ip, strlen(host_ip));
    ret = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_HOSTIP, buff, strlen(buff));
    if (ret != d_OK) {
        CTOS_LCDTPrintXY(1, 8, "Set H_IP Not OK");
        return;
    }
    CTOS_LCDTPrintXY(1, 5, buff);
    
    // Set Host Port
    memset(buff, 0, sizeof(buff));
    memcpy(buff, (BYTE*)host_port, strlen(host_port));
    ret = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_HOSTPORT, buff, strlen(buff));
    if (ret != d_OK) {
        CTOS_LCDTPrintXY(1, 8, "Set H_Port Not OK");
        return;
    }
    CTOS_LCDTPrintXY(1, 6, buff);
    
    // Set other configurations
    memset(buff, 0, sizeof(buff));
    buff[0] = 0x32;
    ret = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_AUTOCON, buff, 1);
    if (ret != d_OK) {
        CTOS_LCDTPrintXY(1, 8, "Set M_Mod Not OK");
        return;
    }
    
    buff[0] = 0x30;
    ret = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_DHCP, buff, 1);
    if (ret != d_OK) {
        CTOS_LCDTPrintXY(1, 8, "Set DHCP Not OK");
        return;
    }
    
    ret = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_UPDATE_EXIT, babuff, 0);
    if (ret != d_OK) {
        CTOS_LCDTPrintXY(1, 8, "Save Exit Not OK");
        return;
    }
    
    CTOS_LCDTPrintXY(1, 8, "Save Exit OK");
}

/* ========================================================
 * FUNCTION NAME: EthernetGetConfig
 * DESCRIPTION:  Get the Ethernet Configuration
 * RETURN:       none
 * NOTES:        none
 * ========================================================*/
void EthernetGetConfig(void)
{
    BYTE bLength;
    
    CTOS_LCDTClearDisplay();
    CTOS_LCDTPrintXY(1, 1, "--Ethernet Get--");
    
    // Get the configuration value of Ethernet
    memset(babuff, 0x00, sizeof(babuff));
    bLength = sizeof(babuff);
    
    // Get the IP Address
    ret = CTOS_EthernetConfigGet(d_ETHERNET_CONFIG_IP, babuff, &bLength);
    if (ret == d_OK) {
        CTOS_LCDTPrintXY(1, 2, babuff);
    } else {
        CTOS_LCDTPrintXY(1, 2, "IP Error");
    }
    
    memset(babuff, 0x00, sizeof(babuff));
    bLength = sizeof(babuff);
    
    // Get the mask
    ret = CTOS_EthernetConfigGet(d_ETHERNET_CONFIG_MASK, babuff, &bLength);
    if (ret == d_OK) {
        CTOS_LCDTPrintXY(1, 3, babuff);
    } else {
        CTOS_LCDTPrintXY(1, 3, "Mask Error");
    }
    
    memset(babuff, 0x00, sizeof(babuff));
    bLength = sizeof(babuff);
    
    // Get the Gateway IP
    ret = CTOS_EthernetConfigGet(d_ETHERNET_CONFIG_GATEWAY, babuff, &bLength);
    if (ret == d_OK) {
        CTOS_LCDTPrintXY(1, 4, babuff);
    } else {
        CTOS_LCDTPrintXY(1, 4, "GateIP Error");
    }
    
    memset(babuff, 0x00, sizeof(babuff));
    bLength = sizeof(babuff);
    
    // Get the Host IP
    ret = CTOS_EthernetConfigGet(d_ETHERNET_CONFIG_HOSTIP, babuff, &bLength);
    if (ret == d_OK) {
        CTOS_LCDTPrintXY(1, 5, babuff);
    } else {
        CTOS_LCDTPrintXY(1, 5, "HostIP Error");
    }
    
    memset(babuff, 0x00, sizeof(babuff));
    bLength = sizeof(babuff);
    
    // Get the Host Port
    ret = CTOS_EthernetConfigGet(d_ETHERNET_CONFIG_HOSTPORT, babuff, &bLength);
    if (ret == d_OK) {
        CTOS_LCDTPrintXY(1, 6, babuff);
    } else {
        CTOS_LCDTPrintXY(1, 6, "HostPort Error");
    }
}

/* ========================================================
 * FUNCTION NAME: EthernetRxData
 * DESCRIPTION:  Receive data from the host
 * RETURN:       none
 * NOTES:        none
 * ========================================================*/
void EthernetRxData(void)
{
    USHORT usr_Len = 0, usR_Len = 0;
    
    CTOS_LCDTClearDisplay();
    CTOS_LCDTPrintXY(1, 1, "----Rx Data----");
    CTOS_LCDTPrintXY(1, 8, "Exit->'X'");
    
    memset(babuff, 0x00, sizeof(babuff));
    usr_Len = sizeof(babuff);
    CTOS_LCDTPrintXY(1, 2, "Receive Data....");
    CTOS_LCDTPrintXY(1, 3, "Length=");
    
    while (1) {
        // Check if the data is currently available in Ethernet
        ret = CTOS_EthernetRxReady(&usr_Len);
        if (ret == d_OK) {
            // Receive data via Ethernet channel
            CTOS_EthernetRxData(&babuff[usR_Len], &usr_Len);
            usR_Len += usr_Len;
            sprintf(&babuff[100], "%dbytes", usR_Len);
            CTOS_LCDTPrintXY(8, 3, &babuff[100]);
            CTOS_Delay(1000);
        } else {
            CTOS_LCDTPrintXY(1, 8, "RxReady Failed");
        }
        
        CTOS_KBDHit(&key);
        // return main()
        if (key == d_KBD_CANCEL) {
            // Clear the receive buffer of Ethernet
            CTOS_EthernetFlushRxData();
            show_screen(0);
            emKM = eMain;
            return;
        }
    }
}

/* ========================================================
 * FUNCTION NAME: EthernetConnect
 * DESCRIPTION:  Connect or Disconnect the host with IP Address
 *               and Port number which are stored in the Ethernet module
 * RETURN:       1-->Connect or Disconnect Success
 *               0-->Connect or Disconnect Fail
 * NOTES:        none
 * ========================================================*/
USHORT EthernetConnect(BOOL IsConnected)
{
    DWORD dwStatus;
    USHORT usr_Len = 0;
    
    if (IsConnected) {
        // Use default Host IP address & Port number
        ret = CTOS_EthernetConnect();
        if (ret != d_OK) {
            memset(babuff, 0, sizeof(babuff));
            sprintf(babuff, "ret=%04X", ret);
            CTOS_LCDTPrintXY(1, 7, babuff);
            CTOS_KBDGet(&key);
            return 0;
        } else {
            ret = CTOS_EthernetStatus(&dwStatus);
            if (dwStatus & d_STATUS_ETHERNET_RX_READY) {
                ret = CTOS_EthernetRxReady(&usr_Len);
                CTOS_EthernetRxData(&babuff[0], &usr_Len);
            }
        }
        sprintf(babuff, "0x%08X", dwStatus);
        CTOS_LCDTPrintXY(1, 7, babuff);
        return 1;
    } else {
        // Disconnect from the host
        ret = CTOS_EthernetDisconnect();
        if (ret != d_OK) {
            return 0;
        } else {
            return 1;
        }
    }
}

/* ========================================================
 * FUNCTION NAME: EthernetConnectEx
 * DESCRIPTION:  Use the specific IP address & Port number
 * RETURN:       1-->Connect or Disconnect Success
 *               0-->Connect or Disconnect Fail
 * NOTES:        none
 * ========================================================*/
USHORT EthernetConnectEx(BOOL IsConnected, BYTE* baDestIP, BYTE bIPLen, 
                         BYTE* baPort, BYTE bPortLen)
{
    DWORD dwStatus;
    USHORT usr_Len = 0;
    
    CTOS_LCDTPrintXY(1, 7, baDestIP);
    CTOS_LCDTPrintXY(1, 8, baPort);
    
    if (IsConnected) {
        ret = CTOS_EthernetConnectEx(baDestIP, bIPLen, baPort, bPortLen);
        if (ret != d_OK) {
            memset(babuff, 0, sizeof(babuff));
            sprintf(babuff, "ret=%04X", ret);
            CTOS_LCDTPrintXY(1, 7, babuff);
            CTOS_KBDGet(&key);
            return 0;
        } else {
            ret = CTOS_EthernetStatus(&dwStatus);
            if (dwStatus & d_STATUS_ETHERNET_RX_READY) {
                ret = CTOS_EthernetRxReady(&usr_Len);
                CTOS_EthernetRxData(&babuff[0], &usr_Len);
            }
        }
        return 1;
    } else {
        // Disconnect from the host
        ret = CTOS_EthernetDisconnect();
        if (ret != d_OK) {
            return 0;
        } else {
            return 1;
        }
    }
}

/* ========================================================
 * FUNCTION NAME: EthernetTxData
 * DESCRIPTION:  Send data to the destination HOST using TCP/IP protocols
 * RETURN:       none
 * NOTES:        none
 * ========================================================*/
void EthernetTxData(void)
{
    DWORD dwStatus;
    int i, iLength = 0;
    
    CTOS_LCDTClearDisplay();
    CTOS_LCDTPrintXY(1, 1, "----Tx Data----");
    
    // Get the status of Ethernet
    ret = CTOS_EthernetStatus(&dwStatus);
    if (ret != d_OK) {
        CTOS_LCDTPrintXY(1, 7, "Status Fail");
    } else {
        sprintf(babuff, "0x%08X", dwStatus);
        CTOS_LCDTPrintXY(1, 7, babuff);
    }
    
    if (dwStatus & d_STATUS_ETHERNET_CONNECTED) {
        sprintf(babuff, "0123456789ABCDEF");
        CTOS_LCDTPrintXY(1, 2, "Send Data....");
        CTOS_LCDTPrintXY(1, 3, "Length=");
        
        for (i = 0; i <= 10; i++) {
            // Check Ethernet whether ready to transmit data
            if (CTOS_EthernetTxReady() == d_OK) {
                // Transmit data via Ethernet channel
                CTOS_EthernetTxData(babuff, strlen(babuff));
                iLength += strlen(babuff);
                sprintf(&babuff[100], "%dbyte", iLength);
                CTOS_LCDTPrintXY(8, 3, &babuff[100]);
                CTOS_Delay(1000);
            } else {
                CTOS_LCDTPrintXY(1, 8, "Tx Data Fail");
            }
        }
        CTOS_LCDTPrintXY(1, 8, "Exit->'X'");
    }
    
    // Get a key from keyboard
    CTOS_KBDGet(&key);
    // return main()
    if (key == d_KBD_CANCEL) {
        show_screen(0);
        emKM = eMain;
    }
}

/* ========================================================
 * FUNCTION NAME: EthernetGetStatus
 * DESCRIPTION:  Get the status of Ethernet
 * RETURN:       none
 * NOTES:        none
 * ========================================================*/
void EthernetGetStatus(void)
{
    DWORD dwStatus;
    int i = 1;
    
    CTOS_LCDTClearDisplay();
    CTOS_LCDTPrintXY(1, i, "---Get Status---");
    i++;
    
    // Get the status
    CTOS_EthernetStatus(&dwStatus);
    
    if (dwStatus & d_STATUS_ETHERNET_CONNECTED) {
        CTOS_LCDTPrintXY(1, i, "Connected"); // if Ethernet is connected
        i++;
    }
    if (dwStatus & d_STATUS_ETHERNET_COMMAND_MODE) {
        CTOS_LCDTPrintXY(1, i, "Command Mode"); // if Ethernet is command mode
        i++;
    }
    if (dwStatus & d_STATUS_ETHERNET_PHYICAL_ONLINE) {
        CTOS_LCDTPrintXY(1, i, "Phyical Online"); // if Ethernet is physical online
        i++;
    }
    if (dwStatus & d_STATUS_ETHERNET_RX_READY) {
        CTOS_LCDTPrintXY(1, i, "Rx Ready"); // if Ethernet is Rx ready
        i++;
    }
    if (dwStatus & d_STATUS_ETHERNET_TX_BUSY) {
        CTOS_LCDTPrintXY(1, i, "Tx Busy"); // if Ethernet is Tx busy
        i++;
    }
    
    CTOS_KBDGet(&key);
    if (key == d_KBD_CANCEL) {
        emKM = eMain;
        show_screen(0);
    }
}

/* ========================================================
 * FUNCTION NAME: EthernetPing
 * DESCRIPTION:  Used to test whether a particular host is reachable
 * RETURN:       none
 * NOTES:        none
 * ========================================================*/
void EthernetPing(void)
{
    BYTE PingBuff[15 + 1];
    
    sprintf(PingBuff, "192.120.100.22");
    CTOS_LCDTClearDisplay();
    CTOS_LCDTPrintXY(1, 1, "-----Ping-----");
    CTOS_LCDTPrintXY(1, 2, PingBuff);
    
    sprintf(babuff, "%d", strlen(PingBuff));
    CTOS_LCDTPrintXY(1, 3, babuff);
    
    // Ping the other IP Address
    ret = CTOS_EthernetPing(PingBuff, strlen(PingBuff));
    if (ret == d_OK) {
        CTOS_LCDTPrintXY(1, 8, "Ping Success");
    } else {
        memset(babuff, 0, sizeof(babuff));
        sprintf(babuff, "ret=%04X", ret);
        CTOS_LCDTPrintXY(1, 8, babuff);
    }
    
    CTOS_KBDGet(&key);
    if (key == d_KBD_CANCEL) {
        emKM = eMain;
        show_screen(0);
    }
}

/* ========================================================
 * FUNCTION NAME: main
 * DESCRIPTION:  Main function
 * RETURN:       0 on success
 * NOTES:        none
 * ========================================================*/
int main(int argc, char *argv[])
{
    // Open the Back Light in the LCD Display
    CTOS_BackLightSet(d_BKLIT_LCD, d_ON);
    CTOS_LCDTPrintXY(1, 1, "----Ethernet----");
    
    // Open the Ethernet channel
    ret = CTOS_EthernetOpen();
    if (ret != d_OK) {
        CTOS_LCDTPrintXY(1, 7, "Open Not OK ");
        CTOS_KBDGet(&key);
        return 0;
    }
    
    CTOS_LCDTPrintXY(1, 2, "Open OK ");
    
    // Show Main screen
    show_screen(0);
    emKM = eMain;
    
    while (1) {
        CTOS_KBDGet(&key);
        
        switch (emKM) {
        case eMain:
            switch (key) {
            case d_KBD_1:
                show_screen(1);      // Show sub screen
                emKM = eConfig;      // Key mode change
                break;
                
            case d_KBD_2:
                show_screen(2);
                emKM = eConnected;
                break;
                
            case d_KBD_3:
                emKM = eTxData;
                break;
                
            case d_KBD_4:
                emKM = eRxData;
                break;
                
            case d_KBD_5:
                EthernetGetStatus();
                break;
                
            case d_KBD_6:
                EthernetPing();
                break;
                
            case d_KBD_7:
            case d_KBD_CANCEL:
                // Disconnect from the host
                CTOS_EthernetDisconnect();
                // Close an opened Ethernet channel and release its working buffer
                CTOS_EthernetClose();
                return 1;
                
            case d_KBD_8:
                EthernetConnectEx(TRUE, (BYTE *)host_ip, strlen(host_ip), 
                                 (BYTE *)host_port, strlen(host_port));
                break;
            }
            break;
            
        case eConfig:
            switch (key) {
            case d_KBD_1:
                EthernetSetConfig();
                break;
                
            case d_KBD_2:
                EthernetGetConfig();
                break;
                
            case d_KBD_CANCEL:
                show_screen(0);
                emKM = eMain;
                break;
            }
            break;
            
        case eConnected:
            switch (key) {
            case d_KBD_1:
                if (EthernetConnect(TRUE) == 1) {
                    CTOS_LCDTPrintXY(1, 8, "Connect OK");
                } else {
                    CTOS_LCDTPrintXY(1, 8, "Connect Failed");
                }
                CTOS_Delay(1000);
                show_screen(0);
                emKM = eMain;
                break;
                
            case d_KBD_2:
                if (EthernetConnect(FALSE) == 1) {
                    CTOS_LCDTPrintXY(1, 8, "DisConnect OK");
                } else {
                    CTOS_LCDTPrintXY(1, 8, "DisConnect Failed");
                }
                CTOS_Delay(1000);
                show_screen(0);
                emKM = eMain;
                break;
                
            case d_KBD_7:
            case d_KBD_CANCEL:
                show_screen(0);
                emKM = eMain;
                break;
            }
            break;
            
        case eTxData:
            EthernetTxData();
            break;
            
        case eRxData:
            EthernetRxData();
            break;
        }
    }
    
    // Close the Back Light in the LCD Display
    CTOS_BackLightSet(d_BKLIT_LCD, d_OFF);
    return 0;
}

// End of file