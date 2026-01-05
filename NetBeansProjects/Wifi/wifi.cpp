/** 
 **	A Template for developing new terminal application
 **/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
/** These two files are necessary for calling CTOS API **/
#include <ctosapi.h>
//#include "Common.h"
#include "inc/libMisc/DebugInfoDefine.h"
#include "inc/libMisc/DebugInfo.h"
#include "inc/libMisc/ByteStream.h"
#include "inc/libMisc/DbgMacro.h"

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <unistd.h>    // for usleep

CDebugInfo cDebug;

#define d_BUFF_SIZE 128
#define DEST_IP "192.168.31.209"
#define DEST_PORT 8080
//define const wifi Data
BYTE byIpAddr[] = "192.168.137.100";
BYTE byMask[] = "255.255.255.0";
BYTE byGateIp[] = "192.168.137.1";
BYTE bySsid[] = "JIM 1511";
BYTE byPwd[] = "12345678";

//Declare Global Variable
USHORT usRet = 0; // Return Value
BYTE byszBaBuff[d_BUFF_SIZE] = {0}; // Global Buffer
BYTE byKey = 0;

enum emKeyMode //Key Mode
{
    eMain,
    ePing,
    eScan,
    eConfig,
    eConnected,
    eTxData,
    eRxData,
    eTcpServer
};

enum emKeyMode emKM;

void show_screen(int tag)
{
    CTOS_LCDTClearDisplay(); //Clear LCD Display

    switch (tag)
    {
        case 0:
            CTOS_LCDTSetReverse(TRUE);
            CTOS_LCDTPrintXY(1, 1, (PBYTE)"WIFI TEST");
            CTOS_LCDTSetReverse(FALSE);
//            CTOS_LCDTPrintXY(1, 2, (PBYTE)"1.Config");
            CTOS_LCDTPrintXY(1, 2, (PBYTE)"1.Connect");
//            CTOS_LCDTPrintXY(1, 4, (PBYTE)"3.Scan");
//            CTOS_LCDTPrintXY(1, 5, (PBYTE)"4.Send Data");
//            CTOS_LCDTPrintXY(1, 6, (PBYTE)"5.Rec Data");
            CTOS_LCDTPrintXY(1, 3, (PBYTE)"2.Ping");
//            CTOS_LCDTPrintXY(1, 8, (PBYTE)"7.TCP Server");
            CTOS_LCDTPrintXY(1, 4, (PBYTE)"3.Exit");
            break;
        case 1:
            CTOS_LCDTSetReverse(TRUE);
            CTOS_LCDTPrintXY(1, 1, (PBYTE)" Config Setting ");
            CTOS_LCDTSetReverse(FALSE);
            CTOS_LCDTPrintXY(1, 2, (PBYTE)"1. Set");
            CTOS_LCDTPrintXY(1, 3, (PBYTE)"2. Get");
            break;
        case 2:
            CTOS_LCDTSetReverse(TRUE);
            CTOS_LCDTPrintXY(1, 1, (PBYTE)" Connect ");
            CTOS_LCDTSetReverse(FALSE);
            CTOS_LCDTPrintXY(1, 2, (PBYTE)"1. Connect");
            CTOS_LCDTPrintXY(1, 3, (PBYTE)"2. DisConnect");
            break;
    }
}

// Config Wifi

void WifiSetConfig(void)
{
    BYTE byszBuff[16 + 1] = {0};
    CTOS_LCDTClearDisplay();
    CTOS_LCDTPrintXY(1, 1, (PBYTE)"--Wifi Set--");

    /*Set up the config of Wifi*/
    memset(byszBuff, 0, sizeof (byszBuff));
    memcpy(byszBuff, (BYTE*)byIpAddr, strlen((const char*)byIpAddr));

    usRet = CTOS_WifiConfigSet(d_WIFI_CONFIG_IP, byszBuff, strlen((const char*)byszBuff));
    if (usRet != d_OK)
    {
        CTOS_LCDTPrintXY(1, 8, (PBYTE)"Set IP Not OK");
        CTOS_Delay(1000);
        return;
    }
    CTOS_LCDTPrintXY(1, 2, byszBuff);
    /*Set Mask*/
    memset(byszBuff, 0, sizeof (byszBuff));
    memcpy(byszBuff, (BYTE*)byMask, strlen((const char*)byMask));
    usRet = CTOS_WifiConfigSet(d_WIFI_CONFIG_MASK, byszBuff, strlen((const char*)byszBuff));
    if (usRet != d_OK)
    {
        CTOS_LCDTPrintXY(1, 8, (PBYTE)"Set Mask Not OK");
        CTOS_Delay(1000);
        return;
    }
    CTOS_LCDTPrintXY(1, 3, byszBuff);

    //Set Gateway IP
    memset(byszBuff, 0, sizeof (byszBuff));
    memcpy(byszBuff, (BYTE*)byGateIp, strlen((const char*)byGateIp));
    usRet = CTOS_WifiConfigSet(d_WIFI_CONFIG_GATEWAY, byszBuff, strlen((const char*)byszBuff));
    if (usRet != d_OK)
    {
        CTOS_LCDTPrintXY(1, 8, (PBYTE)"Set Gate Not OK");
        DEBUG_INFO(DEBUGINFO_NORMAL, "usRet = %04x", usRet);
        return;
    }
    CTOS_LCDTPrintXY(1, 4, byszBuff);
    CTOS_KBDGet(&byKey);
    show_screen(0);
    emKM = eMain;
    return;
}

void WifiGetConfig(void)
{
    BYTE byLength = 0;
    CTOS_LCDTClearDisplay();
    CTOS_LCDTPrintXY(1, 1, (PBYTE)"--Wifi Get--");

    //Get IP
    memset(byszBaBuff, 0x00, sizeof (byszBaBuff));
    byLength = sizeof (byszBaBuff);
    usRet = CTOS_WifiConfigGet(d_WIFI_CONFIG_IP, byszBaBuff, &byLength);
    if (usRet == d_OK)
    {
        CTOS_LCDTPrintXY(1, 2, byszBaBuff);
    }
    else
    {
        CTOS_LCDTPrintXY(1, 2, (PBYTE)"Get IP Err");
        DEBUG_INFO(DEBUGINFO_NORMAL, "IP usRet=%04x", usRet);
        DEBUG_INFO(DEBUGINFO_NORMAL, "buff =%s", byszBaBuff);
    }

    //Get Mask
    memset(byszBaBuff, 0x00, sizeof (byszBaBuff));
    byLength = sizeof (byszBaBuff);
    usRet = CTOS_WifiConfigGet(d_WIFI_CONFIG_MASK, byszBaBuff, &byLength);
    if (usRet == d_OK)
    {
        CTOS_LCDTPrintXY(1, 3, byszBaBuff);
    }
    else
    {
        CTOS_LCDTPrintXY(1, 3, (PBYTE)"Get Mask Err");
        DEBUG_INFO(DEBUGINFO_NORMAL, "Mask usRet=%04x", usRet);
        DEBUG_INFO(DEBUGINFO_NORMAL, "buff =%s", byszBaBuff);
    }
    //Get Gateway
    memset(byszBaBuff, 0x00, sizeof (byszBaBuff));
    byLength = sizeof (byszBaBuff);
    usRet = CTOS_WifiConfigGet(d_WIFI_CONFIG_GATEWAY, byszBaBuff, &byLength);
    if (usRet == d_OK)
    {
        CTOS_LCDTPrintXY(1, 4, byszBaBuff);
    }
    else
    {
        CTOS_LCDTPrintXY(1, 4, (PBYTE)"Get Gateway Err");
        DEBUG_INFO(DEBUGINFO_NORMAL, "Gateway usRet=%04x", usRet);
        DEBUG_INFO(DEBUGINFO_NORMAL, "buff =%s", byszBaBuff);
    }

}

void WifiPing(void)
{

    BYTE byPingBuff[15 + 1];
    BYTE bTimeOut = 50;
    sprintf((char*)byPingBuff, "%s", "8.8.8.8");
    CTOS_LCDTClearDisplay();
    CTOS_LCDTPrintXY(1, 1, (PBYTE)"---Ping WIFI Info---");
    sprintf((char*)byszBaBuff, "%d", strlen((const char*)byPingBuff));

    /*Ping the other IP Address*/
    usRet = CTOS_WifiPing(byPingBuff, bTimeOut);
    if (usRet == d_OK)
    {
        CTOS_LCDTPrintXY(1, 8, (PBYTE)"Ping 8.8.8.8 Success");
    }
    else
    {
        memset(byszBaBuff, 0, sizeof (byszBaBuff));
        sprintf((char*)byszBaBuff, "usRet=%04x", usRet);
        CTOS_LCDTPrintXY(1, 8, byszBaBuff);
    }

    CTOS_KBDGet(&byKey);
    show_screen(0);
    emKM = eMain;
    return;
}

void WifiScan(void)
{

    CTOS_stWifiInfo *pstInfo = NULL;
    DWORD dwStatus = 0;
    USHORT usrLen = 0;
    BYTE byWifiNum = 0;
    INT r = 0;

    CTOS_LCDTClearDisplay();
    CTOS_LCDTPrintXY(1, 1, (PBYTE)"---Scan WIFI List---");
    usRet = CTOS_WifiScan();

    while (1)
    {
        usRet = CTOS_WifiStatus(&dwStatus);
        if (dwStatus == d_WIFI_STATE_SCANNING)
        {
            DEBUG_INFO(DEBUGINFO_NORMAL, "d_WIFI_STATE_SCANNING");
            continue;
        }
        else
        {
            DEBUG_INFO(DEBUGINFO_NORMAL, "CTOS_WifiStatus dwStatus 0X%08X, Ret = %04x", dwStatus, usRet);
            break;
        }
    }
    usRet = CTOS_WifiInfoGet(&byWifiNum, &pstInfo);
    if (byWifiNum > 0)
    {
        DEBUG_INFO(DEBUGINFO_NORMAL, "byWifiNum = %d", byWifiNum);
        INT iLoop = 0;
        for (iLoop = 0; iLoop < byWifiNum; iLoop++)
        {
            DEBUG_INFO(DEBUGINFO_NORMAL, " ESSID %s", (pstInfo + iLoop)->ESSID);
            r = memcmp((pstInfo + iLoop)->ESSID, bySsid, strlen((const char *)bySsid));
            if (r == 0)
            {
                usRet = CTOS_WifiConnectAP(pstInfo + iLoop, byPwd, strlen((const char *)byPwd));
                while (1)
                {
                    usRet = CTOS_WifiStatus(&dwStatus);
                    if (dwStatus == d_WIFI_STATE_CONNECTING)
                    {
                        DEBUG_INFO(DEBUGINFO_NORMAL, "status = D_WIFI_STATE_CONNECTING");
                        continue;
                    }
                    else if (dwStatus == d_WIFI_STATE_AP_CONNECTING)
                    {
                        DEBUG_INFO(DEBUGINFO_NORMAL, "status = D_WIFI_STATE_AP_CONNECTING");
                        continue;
                    }
                    else
                    {
                        DEBUG_INFO(DEBUGINFO_NORMAL, "status =  0X%08X, Ret = %04x", dwStatus, usRet);
                        break;
                    }
                }
            }

        }

    }

    CTOS_KBDGet(&byKey);
    show_screen(0);
    emKM = eMain;
    return;
}

USHORT WifiConnect(BOOL isConnected)
{
    //Declare Local Variable
    DWORD dwStatus = 0;
    USHORT usrLen = 0;
    BYTE byWifiNum = 0;
    INT i = 0;

    CTOS_stWifiInfo *stInfo = NULL;


    if (isConnected)
    {
        // Wifi Scan
        usRet = CTOS_WifiScan();
        while (1)
        {
            usRet = CTOS_WifiStatus(&dwStatus);
            if (dwStatus == d_WIFI_STATE_SCANNING)
            {
                DEBUG_INFO(DEBUGINFO_NORMAL, "d_WIFI_STATE_SCANNING");
            }
            else
            {
                DEBUG_INFO(DEBUGINFO_NORMAL, "status =  0X%08X, Ret = %04x", dwStatus, usRet);
                break;
            }
            CTOS_Delay(500);
        }

        usRet = CTOS_WifiInfoGet(&byWifiNum, &stInfo);

        if (byWifiNum > 0)
        {
            DEBUG_INFO(DEBUGINFO_NORMAL, "byWifiNum=%d ", byWifiNum);
            for (i = 0; i < byWifiNum; i++)
            {
                DEBUG_INFO(DEBUGINFO_NORMAL, "SSID name = %s ", (stInfo + i)->ESSID);
                if (memcmp((stInfo + i)->ESSID, bySsid, strlen((const char *)bySsid)) == 0)
                {
                    usRet = CTOS_WifiConnectAP((stInfo + i), byPwd, strlen((const char *)byPwd));
                    while (1)
                    {
                        usRet = CTOS_WifiStatus(&dwStatus);
                        if (dwStatus == d_WIFI_STATE_CONNECTING)
                        {
                            DEBUG_INFO(DEBUGINFO_NORMAL, "status = d_WIFI_STATE_CONNECTING");
                            continue;
                        }
                        else if (dwStatus == d_WIFI_STATE_AP_CONNECTING)
                        {
                            DEBUG_INFO(DEBUGINFO_NORMAL, "status = d_WIFI_STATE_AP_CONNECTING");
                        }
                        else
                        {
                            DEBUG_INFO(DEBUGINFO_NORMAL, "status =  0X%08X, Ret = %04x", dwStatus, usRet);
                            break;
                        }
                        CTOS_Delay(500);
                    }
                }
            }
        }
        sprintf((char *)byszBaBuff, "0x%08X", dwStatus);
        CTOS_LCDTPrintXY(1, 7, byszBaBuff);
        return 1;
    }
    else
    {
        //Disconnect from the host //
        usRet = CTOS_WifiDisconnectAP();
        while (1)
        {
            usRet = CTOS_WifiStatus(&dwStatus);
            if (dwStatus == d_WIFI_STATE_DISCONNECTING)
            {
                DEBUG_INFO(DEBUGINFO_NORMAL, "status = d_WIFI_STATE_DISCONNECTING");
            }
            else if (dwStatus == d_WIFI_STATE_AP_DISCONNECTING)
            {
                DEBUG_INFO(DEBUGINFO_NORMAL, "status = d_WIFI_STATE_AP_DISCONNECTING");
            }
            else
            {
                DEBUG_INFO(DEBUGINFO_NORMAL, "status =  0X%08X, Ret = %04x", dwStatus, usRet);
                break;
            }
            CTOS_Delay(500);
        }
        if (usRet != d_OK)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }

}

INT OpenSocket()
{
    INT iSockfd = 0;
    struct sockaddr_in dest_addr;

    iSockfd = socket(AF_INET, SOCK_STREAM, 0);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(DEST_PORT);
    dest_addr.sin_addr.s_addr = inet_addr(DEST_IP);
    bzero(&(dest_addr.sin_zero), 8);
    usRet = connect(iSockfd, (struct sockaddr *)&dest_addr, sizeof (sockaddr));
    if (usRet != -1)
    {
        DEBUG_INFO(DEBUGINFO_NORMAL, "connect succeed", usRet);
    }
    else
    {
        DEBUG_INFO(DEBUGINFO_NORMAL, "connect failed", usRet);
    }

    return iSockfd;
}

void TcpTxData()
{
    INT iSockfd = 0, iLen = 0;
    iSockfd = OpenSocket();
    while (1)
    {
        memset(byszBaBuff, 0, sizeof (byszBaBuff));
        sprintf((char *)byszBaBuff, "Hello World!\n");
        iLen = strlen((const char *)byszBaBuff);
        usRet = send(iSockfd, byszBaBuff, iLen, 0);
        DEBUG_INFO(DEBUGINFO_NORMAL, "send = %d", usRet);
        CTOS_KBDHit(&byKey);
        //return main()
        if (byKey == d_KBD_CANCEL)
        {
            //CTOS_EthernetFlushRxData();
            show_screen(0);
            emKM = eMain;
            return;
        }
        CTOS_Delay(500);
    }
}

void TcpRxData()
{
    INT iSockfd = 0;

    //create socket
    iSockfd = OpenSocket();
    while (1)
    {
        memset(byszBaBuff, 0, sizeof (byszBaBuff));
        usRet = recv(iSockfd, byszBaBuff, sizeof (byszBaBuff), 0);
        DEBUG_INFO(DEBUGINFO_NORMAL, "recive data...");
        DEBUG_INFO(DEBUGINFO_NORMAL, "data = %s", byszBaBuff);
        CTOS_KBDHit(&byKey);
        //return main()
        if (byKey == d_KBD_CANCEL)
        {
            //CTOS_EthernetFlushRxData();
            show_screen(0);
            emKM = eMain;
            return;
        }
    }
}

// void UdpTxData()
// {
//     INT iSockfd = 0, iNumbytes = 0;
//     struct sockaddr_in their_addr;
//     
//     iSockfd = socket(AF_INET, SOCK_DGRAM, 0);
//     their_addr.sin_port = htons(DEST_PORT);
//     their_addr.sin_addr = inet_addr(DEST_IP);
//     bzero(&(their_addr.sin_zero), 8);
//     sprintf((char *)byszBaBuff,"Hello World!\n");
//     usRet = sendto(iSockfd, byszBaBuff, strlen((const char *)byszBaBuff), 0, 
//             (struct sockaddr *)&their_addr, sizeof(struct sockaddr));
//     DEBUG_INFO(DEBUGINFO_NORMAL, "send = %d", usRet);
// }

void TcpServer()
{
    INT iSockfd = 0, iNewfd = 0, iSin_size = 0;
    struct sockaddr_in my_addr;
    struct sockaddr_in their_addr;
    iSockfd = socket(AF_INET, SOCK_STREAM, 0);
    DEBUG_INFO(DEBUGINFO_NORMAL, "socket =%d", iSockfd);

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(DEST_PORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(my_addr.sin_zero), 8);
    usRet = bind(iSockfd, (struct sockaddr *)&my_addr, sizeof (struct sockaddr));
    DEBUG_INFO(DEBUGINFO_NORMAL, "bind =%d", usRet);
    usRet = listen(iSockfd, 5);
    DEBUG_INFO(DEBUGINFO_NORMAL, "listen = %d", usRet);

    //     while (1)
    //     {
    //         iSin_size = sizeof(struct sockaddr_in);
    //        iNewfd = accept(iSockfd, (struct sockaddr *)&their_addr, &iSin_size);
    //     }  
}

/** 
 ** The main entry of the terminal application 
 **/
int main(void)
{
    
    DWORD dwStatus = 0;
    while (1)
    {
        
    }
    //DEBUG_ENABLE(1, 0, DEBUGINFO_NORMAL, NULL);
    //DEBUG_INFO(DEBUGINFO_NORMAL, "Begin");

    //DEBUG_ENTRY();
    CTOS_BackLightSet(d_BKLIT_LCD, d_ON); //?
    CTOS_LCDSelectMode(d_LCD_TEXT_MODE);
    
    CTOS_LCDTPrintXY(1, 1, (PBYTE)"---WIFI---");
            
    CTOS_KBDGet(&byKey);
    //Open the Ethernet Cannel
#ifdef debug
    usRet = CTOS_WifiOpen();
    if (usRet != d_OK)
    {
        CTOS_LCDTPrintXY(1, 7, (PBYTE)"WIFI Open Not OK...");
        CTOS_KBDGet(&byKey); //
        return 0;
    }

    CTOS_LCDTPrintXY(1, 2, (PBYTE)"WIFI Open OK");
    char szBuff[20];
    /*Show Main screen*/
    show_screen(0);
    emKM = eMain;

    while (1)
    {
        CTOS_KBDHit(&byKey);
        switch (emKM)
        {
            case eMain:
                switch (byKey)
                {
//                    case d_KBD_1:
//                        show_screen(1);
//                        emKM = eConfig;
//                        break;
                    case d_KBD_1:
                        show_screen(2);
                        emKM = eConnected;
                        break;
//                    case d_KBD_3:
//                        emKM = eScan;
//                        break;
//                    case d_KBD_4:
//                        emKM = eTxData;
//                        break;
//                    case d_KBD_5:
//                        emKM = eRxData;
//                        break;
                    case d_KBD_2:
                        emKM = ePing;
                        break;
//                    case d_KBD_7:
//                        emKM = eTcpServer;
                    case d_KBD_3:
                    case d_KBD_CANCEL:
                        /*Disconnect from the host*/
                        CTOS_WifiDisconnectAP();
                        CTOS_WifiClose();
                        return 1;
                        //                        case d_KBD_8:
                        //                            EthernetConnectEx(TRUE, (BYTE *)byHostIp, strlen((const char*)byHostIp), (BYTE *)byHostPort, strlen((const char*)byHostPort));
                        //                            break;
                }
                break;
            case eConfig:
                switch (byKey)
                {
                    case d_KBD_1:
                        WifiSetConfig();
                        break;
                    case d_KBD_2:
                        WifiGetConfig();
                        break;
                    case d_KBD_CANCEL:
                        show_screen(0);
                        emKM = eMain;
                        break;
                }
                break;
            case eConnected:
                switch (byKey)
                {
                    case d_KBD_1:
                        if (WifiConnect(TRUE) == 1)
                        {
                            CTOS_LCDTPrintXY(1, 8, (PBYTE)"Connect OK");
                        }
                        else
                        {
                            CTOS_LCDTPrintXY(1, 8, (PBYTE)"Connect Failed");
                        }
                        CTOS_Delay(1000);
                        show_screen(0);
                        emKM = eMain;
                        break;
                    case d_KBD_2:
                        if (WifiConnect(FALSE) == 1)
                        {
                            CTOS_LCDTPrintXY(1, 8, (PBYTE)"DisConnect OK");
                        }
                        else
                        {
                            CTOS_LCDTPrintXY(1, 8, (PBYTE)"DisConnect Failed");
                        }
                        CTOS_Delay(1000);
                        show_screen(0);
                        emKM = eMain;
                        break;
                    case d_KBD_8:
                    case d_KBD_CANCEL:
                        show_screen(0);
                        emKM = eMain;
                        break;
                }
                break;
            case eTxData:
                TcpTxData();
                break;
            case eRxData:
                TcpRxData();
                break;
            case ePing:
                WifiPing();
                break;
            case eScan:
                WifiScan();
                break;
            case eTcpServer:
                TcpServer();
                break;
        }

        USHORT usRet = CTOS_WifiStatus(&dwStatus);
        CTOS_LCDTPrintXY(1, 10, (PBYTE)"     WIFI status    ");
        memset(szBuff, 0, sizeof (szBuff));
        sprintf(szBuff, "Ret code %04X", usRet);
        CTOS_LCDTPrintXY(1, 11, (PBYTE)szBuff);
        memset(szBuff, 0, sizeof (szBuff));
        sprintf(szBuff, "Status %08X", dwStatus);
        CTOS_LCDTPrintXY(1, 12, (PBYTE)szBuff);
        DEBUG_INFO(DEBUGINFO_NORMAL, "usRet = %04x", usRet);
        DEBUG_INFO(DEBUGINFO_NORMAL, "Status = %08x", dwStatus);
        usleep(500*100);
    }

    CTOS_BackLightSet(d_BKLIT_LCD, d_OFF);
    return 0;

#endif
}

#ifdef Ethernet

void EthernetSetConfig(void)
{
    BYTE byszBuff[16 + 1] = {0};
    CTOS_LCDTClearDisplay();
    CTOS_LCDTPrintXY(1, 1, (PBYTE)"--Ethernet Set--");

    /*Set up the config of Ethernet*/
    memset(byszBuff, 0, sizeof (byszBuff));
    memcpy(byszBuff, (BYTE*)byIpAddr, strlen((const char*)byIpAddr));
    //    DEBUG_INFO(DEBUGINFO_NORMAL, "usRet = %s", byszBuff);
    //    DEBUG_INFO(DEBUGINFO_NORMAL, "usRet = %d", strlen((const char*)byszBuff));
    usRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_DHCP, (BYTE *)"1", 14);
    //    usRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_IP, byszBuff, strlen((const char*)byszBuff));
    //    usRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_IP, byszBuff, strlen((const char*)byszBuff));
    //    DEBUG_INFO(DEBUGINFO_NORMAL, "usRet = %04x", usRet);
    if (usRet != d_OK)
    {
        CTOS_LCDTPrintXY(1, 8, (PBYTE)"Set IP Not OK");
        CTOS_Delay(1000);
        return;
    }
    CTOS_LCDTPrintXY(1, 2, byszBuff);

    /*Set Mask*/
    memset(byszBuff, 0, sizeof (byszBuff));
    memcpy(byszBuff, (BYTE*)byMask, strlen((const char*)byMask));
    usRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_MASK, byszBuff, strlen((const char*)byszBuff));
    if (usRet != d_OK)
    {
        CTOS_LCDTPrintXY(1, 8, (PBYTE)"Set Mask Not OK");
        CTOS_Delay(1000);
        return;
    }

    //Set Gateway IP
    memset(byszBuff, 0, sizeof (byszBuff));
    memcpy(byszBuff, (BYTE*)byGateIp, strlen((const char*)byGateIp));
    //    DEBUG_INFO(DEBUGINFO_NORMAL, "byszBuff=%s", byszBuff);
    //    DEBUG_INFO(DEBUGINFO_NORMAL, "byszBuff length=%d", strlen((const char*)byszBuff));
    usRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_GATEWAY, byszBuff, strlen((const char*)byszBuff));
    //    usRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_GATEWAY, (BYTE *)"192.168.31.1", 12);
    if (usRet != d_OK)
    {
        CTOS_LCDTPrintXY(1, 8, (PBYTE)"Set Gate Not OK");
        DEBUG_INFO(DEBUGINFO_NORMAL, "usRet = %04x", usRet);
        return;
    }
    CTOS_LCDTPrintXY(1, 4, byszBuff);

    /*Set Host IP*/
    memset(byszBuff, 0, sizeof (byszBuff));
    //    memcpy(byszBuff, byHostIp, strlen((const char*)byHostIp));
    usRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_HOSTIP, byszBuff, strlen((const char*)byszBuff));
    if (usRet != d_OK)
    {
        CTOS_LCDTPrintXY(1, 8, (PBYTE)"Set HOST IP Not OK");
        return;
    }

    CTOS_LCDTPrintXY(1, 5, byszBuff);
    // Set Host Port //
    memset(byszBuff, 0, sizeof (byszBuff));
    //    memcpy(byszBuff, (BYTE*)byHostPort,strlen((const char*)byHostPort));
    usRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_HOSTPORT, byszBuff, strlen((const char*)byszBuff));
    if (usRet != d_OK)
    {
        CTOS_LCDTPrintXY(1, 8, (PBYTE)"Set H_IP Not OK");
        return;
    }
    CTOS_LCDTPrintXY(1, 6, byszBuff);
    memset(byszBuff, 0, sizeof (byszBuff));
    byszBuff[0] = 0x32;
    usRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_AUTOCON, byszBuff, 1);
    if (usRet != d_OK)
    {
        CTOS_LCDTPrintXY(1, 8, (PBYTE)"Set M_Mod Not OK");
        return;
    }
    CTOS_LCDTPrintXY(1, 7, byszBuff);
    byszBuff[0] = 0x30;
    usRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_DHCP, byszBuff, 1);
    if (usRet != d_OK)
    {
        CTOS_LCDTPrintXY(1, 8, (PBYTE)"Set M_Mod Not OK");
        return;
    }
    CTOS_LCDTPrintXY(1, 7, byszBuff);
    usRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_UPDATE_EXIT, byszBaBuff, 0);
    if (usRet != d_OK)
    {
        CTOS_LCDTPrintXY(1, 8, (PBYTE)"Save Exit Not OK");
        return;
    }
    CTOS_LCDTPrintXY(1, 8, (PBYTE)"Save Exit OK");
}

void EthernetGetConfig(void)
{
    //Declare Local Variable
    BYTE byLength = 0;
    CTOS_LCDTClearDisplay();
    CTOS_LCDTPrintXY(1, 1, (PBYTE)"--Ethernet Get--");

    // Get the configuration value of Ethernet
    memset(byszBaBuff, 0x00, sizeof (byszBaBuff));
    byLength = sizeof (byszBaBuff);
    //Get the IP Address
    usRet = CTOS_EthernetConfigGet(d_ETHERNET_CONFIG_IP, byszBaBuff, &byLength);
    if (usRet == d_OK)
    {
        CTOS_LCDTPrintXY(1, 2, byszBaBuff);
    }
    else
    {
        CTOS_LCDTPrintXY(1, 2, (PBYTE)"IP Error");
    }

    memset(byszBaBuff, 0x00, sizeof (byszBaBuff));
    byLength = sizeof (byszBaBuff);
    // Get the Mask
    usRet = CTOS_EthernetConfigGet(d_ETHERNET_CONFIG_MASK, byszBaBuff, &byLength);
    if (usRet == d_OK)
    {
        CTOS_LCDTPrintXY(1, 3, byszBaBuff);
    }
    else
    {
        CTOS_LCDTPrintXY(1, 3, (PBYTE)"Mask Error");
    }

    memset(byszBaBuff, 0x00, sizeof (byszBaBuff));
    byLength = sizeof (byszBaBuff);

    //Get the Gateway IP
    usRet = CTOS_EthernetConfigGet(d_ETHERNET_CONFIG_GATEWAY, byszBaBuff, &byLength);
    if (usRet == d_OK)
    {
        CTOS_LCDTPrintXY(1, 4, byszBaBuff);
    }
    else
    {
        CTOS_LCDTPrintXY(1, 4, (PBYTE)"GateIP Error");
    }

    memset(byszBaBuff, 0x00, sizeof (byszBaBuff));
    byLength = sizeof (byszBaBuff);

    //Get the Host IP
    usRet = CTOS_EthernetConfigGet(d_ETHERNET_CONFIG_HOSTIP, byszBaBuff, &byLength);
    if (usRet == d_OK)
    {
        CTOS_LCDTPrintXY(1, 5, byszBaBuff);
    }
    else
    {
        CTOS_LCDTPrintXY(1, 5, (PBYTE)"Host IP Error");
    }

    memset(byszBaBuff, 0x00, sizeof (byszBaBuff));
    byLength = sizeof (byszBaBuff);
    //Get the Host Port
    usRet = CTOS_EthernetConfigGet(d_ETHERNET_CONFIG_HOSTPORT, byszBaBuff, &byLength);
    if (usRet == d_OK)
    {
        CTOS_LCDTPrintXY(1, 6, byszBaBuff);
    }
    else
    {
        CTOS_LCDTPrintXY(1, 6, (PBYTE)"Host Port Error");
    }


}

/* ========================================================
 * FUNCTION NAME: Connected
 * DESCRIPTION: Connect or Disconnect the host with IP Address
 * and Port number which are stored in the Ethernet module.
 * RETURN: 1-->Connect or Disconnect Success,
 * 0-->Connect or Disconnect Fail.
 * NOTES: none.
 * ========================================================*/

USHORT EthernetConnect(BOOL isConnected)
{
    //Declare Local Variable
    DWORD dwStatus = 0;
    USHORT usrLen = 0;
    if (isConnected)
    {
        //use default Host IP address & Port number
        usRet = CTOS_EthernetConnect();
        if (usRet != d_OK)
        {
            memset(byszBaBuff, 0, sizeof (byszBaBuff));
            sprintf((char*)byszBaBuff, "usRet=%04x", usRet);
            CTOS_LCDTPrintXY(1, 7, byszBaBuff);
            CTOS_KBDGet(&byKey);
            return 0;
        }
        else
        {
            usRet = CTOS_EthernetStatus(&dwStatus);
        }
        DEBUG_INFO(DEBUGINFO_NORMAL, "status=%08x", dwStatus);
        DEBUG_INFO(DEBUGINFO_NORMAL, "d_STATUS_ETHERNET_RX_READY=%08x", d_STATUS_ETHERNET_RX_READY);
        DEBUG_INFO(DEBUGINFO_NORMAL, "d_STATUS_ETHERNET_CONNECTED=%08x", d_STATUS_ETHERNET_CONNECTED);
        if (dwStatus & d_STATUS_ETHERNET_RX_READY)
        {
            usRet = CTOS_EthernetRxReady(&usrLen);
            CTOS_EthernetRxData(&byszBaBuff[0], &usrLen);
        }
        sprintf((char *)byszBaBuff, "0x%08X", dwStatus);
        CTOS_LCDTPrintXY(1, 7, byszBaBuff);
        return 1;

    }
    else
    {
        //Disconnect from the host //
        usRet = CTOS_EthernetDisconnect();
        if (usRet != d_OK)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
}

void EthernetRxData(void)
{
    //Declare  Local Variable
    USHORT usrLen = 0, usRLen = 0;
    CTOS_LCDTClearDisplay();
    CTOS_LCDTPrintXY(1, 1, (PBYTE)"---Rx Data---");
    CTOS_LCDTPrintXY(1, 8, (PBYTE)"Exit->'X'");
    usrLen = sizeof (byszBaBuff);
    CTOS_LCDTPrintXY(1, 2, (PBYTE)"Receive Data....");
    CTOS_LCDTPrintXY(1, 3, (PBYTE)"Length=");

    while (1)
    {
        //Check if the data is currently available in Ethernet
        memset(byszBaBuff, 0, sizeof (byszBaBuff));
        usRet = CTOS_EthernetRxReady(&usrLen);
        if (usRet == d_OK)
        {
            //Receive data via Ethernet channel
            CTOS_EthernetRxData(&byszBaBuff[usRLen], &usrLen);
            usRLen += usrLen;
            sprintf((char*)&byszBaBuff[100], "%dbytes", usRLen);
            CTOS_LCDTPrintXY(8, 3, &byszBaBuff[100]);
            CTOS_LCDTPrintXY(1, 4, (PBYTE)"                  ");
            CTOS_LCDTPrintXY(1, 4, byszBaBuff);
            CTOS_Delay(1000);
        }
        else
        {
            CTOS_LCDTPrintXY(1, 8, (PBYTE)"RxReady Failed");
        }
        CTOS_KBDHit(&byKey);
        //return main()
        if (byKey == d_KBD_CANCEL)
        {
            CTOS_EthernetFlushRxData();
            show_screen(0);
            emKM = eMain;
            return;
        }
    }
}

void EthernetTxData(void)
{
    //Declare Local Variable
    DWORD dwStatus;
    int i, iLength = 0;
    CTOS_LCDTClearDisplay();
    CTOS_LCDTPrintXY(1, 1, (PBYTE)"----Tx Data----");
    //Get the status of Ethernet //
    usRet = CTOS_EthernetStatus(&dwStatus);
    if (usRet != d_OK)
    {
        CTOS_LCDTPrintXY(1, 7, (PBYTE)"Status Fail");
    }
    else
    {
        sprintf((char*)byszBaBuff, "0x%08X", dwStatus);
        CTOS_LCDTPrintXY(1, 7, byszBaBuff);
    }
    if (dwStatus & d_STATUS_ETHERNET_CONNECTED)
    {
        sprintf((char *)byszBaBuff, "0123456789ABCDEF");
        CTOS_LCDTPrintXY(1, 2, (PBYTE)"Send Data....");
        CTOS_LCDTPrintXY(1, 3, (PBYTE)"Length=");
        for (i = 0; i <= 10; i++)
        {
            //Check Ethernet hether ready to transmit data.
            if (CTOS_EthernetTxReady() == d_OK)
            {
                //Transmit data via Ethernet channel //
                CTOS_EthernetTxData(byszBaBuff, strlen((const char*)byszBaBuff));
                iLength += strlen((const char*)byszBaBuff);
                sprintf((char *)&byszBaBuff[100], "%dbyte", iLength);
                CTOS_LCDTPrintXY(8, 3, &byszBaBuff[100]);
                CTOS_Delay(1000);
            }
            else
            {
                CTOS_LCDTPrintXY(1, 8, (PBYTE)"Tx Data Fail");
            }
        }
        CTOS_LCDTPrintXY(1, 8, (PBYTE)"Exit->'X'");
    }
    //Get a key from keyboard //
    CTOS_KBDGet(&byKey);
    //retrun main() //
    if (byKey == d_KBD_CANCEL)
    {
        show_screen(0);
        emKM = eMain;
    }
}

void EthernetGetStatus(void)
{
    //Declare Local Variable //
    DWORD dwStatus;
    int i = 1;
    CTOS_LCDTClearDisplay();
    CTOS_LCDTPrintXY(1, i, (PBYTE)"---Get Status---");
    i++;
    //Get the status //
    CTOS_EthernetStatus(&dwStatus);
    if (dwStatus & d_STATUS_ETHERNET_CONNECTED)
    {
        CTOS_LCDTPrintXY(1, i, (PBYTE)"Connected"); //if Ethernet is connected //
        i++;
    }
    if (dwStatus & d_STATUS_ETHERNET_COMMAND_MODE)
    {
        CTOS_LCDTPrintXY(1, i, (PBYTE)"Command Mode"); //if Ethernet is command mode //
        i++;
    }
    if (dwStatus & d_STATUS_ETHERNET_PHYICAL_ONLINE)
    {
        CTOS_LCDTPrintXY(1, i, (PBYTE)"Phyical Online"); //if Ethernet is phyical online //
        i++;
    }
    if (dwStatus & d_STATUS_ETHERNET_RX_READY)
    {
        CTOS_LCDTPrintXY(1, i, (PBYTE)"Rx Ready"); //if Ethernet is Rx readey //
        i++;
    }
    if (dwStatus & d_STATUS_ETHERNET_TX_BUSY)
    {
        CTOS_LCDTPrintXY(1, i, (PBYTE)"Tx Busy"); //if Ethernet is Tx busy //
        i++;
    }
    CTOS_KBDGet(&byKey);
    if (byKey == d_KBD_CANCEL)
    {
        emKM = eMain;
        show_screen(0);
    }
}

/*
 *FUNCTION Ethernet Ping
 *
 */

void EthernetPing(void)
{
    BYTE byPingBuff[15 + 1];
    sprintf((char*)byPingBuff, "%s", "192.168.31.13");
    CTOS_LCDTClearDisplay();
    CTOS_LCDTPrintXY(1, 1, (PBYTE)"----Ping----");
    CTOS_LCDTPrintXY(1, 2, (PBYTE)byPingBuff);
    sprintf((char*)byszBaBuff, "%d", strlen((const char*)byPingBuff));
    CTOS_LCDTPrintXY(1, 3, byszBaBuff);

    /*Ping the other IP Address*/
    usRet = CTOS_EthernetPing(byPingBuff, strlen((const char*)byPingBuff));
    if (usRet == d_OK)
    {
        CTOS_LCDTPrintXY(1, 8, (PBYTE)"Ping Success");
    }
    else
    {
        memset(byszBaBuff, 0, sizeof (byszBaBuff));
        sprintf((char*)byszBaBuff, "usRet=%04x", usRet);
        CTOS_LCDTPrintXY(1, 8, byszBaBuff);
    }
    CTOS_KBDGet(&byKey);
    if (byKey == d_KBD_CANCEL)
    {
        emKM = eMain;
        show_screen(0);
    }
}

//Use the specific IP adrress & Port number //

USHORT EthernetConnectEx(BOOL IsConnected, BYTE* baDestIP, BYTE bIPLen, BYTE* baPort, BYTE bPortLen)
{
    //Declare Local Variable //
    DWORD dwStatus;
    USHORT usrLen = 0;
    CTOS_LCDTPrintXY(1, 7, baDestIP);
    CTOS_LCDTPrintXY(1, 8, baPort);
    if (IsConnected)
    {
        usRet = CTOS_EthernetConnectEx(baDestIP, bIPLen, baPort, bPortLen);
        if (usRet != d_OK)
        {
            memset(byszBaBuff, 0, sizeof (byszBaBuff));
            sprintf((char *)byszBaBuff, "ret=%04X", usRet);
            CTOS_LCDTPrintXY(1, 7, byszBaBuff);
            CTOS_KBDGet(&byKey);
            return 0;
        }
        else
        {
            usRet = CTOS_EthernetStatus(&dwStatus);
        }
        if (dwStatus & d_STATUS_ETHERNET_RX_READY)
        {
            usRet = CTOS_EthernetRxReady(&usrLen);
            CTOS_EthernetRxData(&byszBaBuff[0], &usrLen);
        }
        return 1;
    }
    else
    {
        //Disconnect from the host //
        usRet = CTOS_EthernetDisconnect();
    }
    if (usRet != d_OK)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

#endif
