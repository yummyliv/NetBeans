
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