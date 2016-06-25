#include "mscribe.h"
#include <regex>

#define MSCRIBE_PI 3.1415926535898f

const DWORD CCommPort::INQUE_MAX  = 8192;
const DWORD CCommPort::OUTQUE_MAX = 8192;

LPCSTR CHCI::S_SUCCESS         = "Success";
LPCSTR CHCI::S_NO_PACKET_YET   = "Current packet not yet complete";
LPCSTR CHCI::S_TRY_AGAIN       = "Try this HCI operation again";

LPCSTR CHCI::S_TIMED_OUT       = "Timed out waiting for packet";
LPCSTR CHCI::S_BAD_PORT_NUM    = "Port number out of range";
LPCSTR CHCI::S_BAD_PACKET      = "Corrupted packet";
LPCSTR CHCI::S_NO_HCI          = "Unable to find HCI";
LPCSTR CHCI::S_CANT_BEGIN      = "Found HCI but can't begin session";
LPCSTR CHCI::S_CANT_OPEN_PORT  = "Unable to open serial port";
LPCSTR CHCI::S_BAD_PASSWORD    = "Password rejected during config command";
LPCSTR CHCI::S_BAD_VERSION     = "Firmware version does not support this feature";
LPCSTR CHCI::S_BAD_FORMAT      = "Unknown firmware parameter format";

LPCSTR CHCI::STR_SIGNON        = "IMMC";
LPCSTR CHCI::STR_BEGIN         = "BEGIN";

const float CHCI::WAIT_SIGNON  = 15e-3f;
const float CHCI::WAIT_END     = 15e-3f;
const float CHCI::WAIT_CFG_ARG = 15e-3f;
const float CHCI::WAIT_RESTORE = 2.0f;
const float CHCI::MIN_TIMEOUT  = 0.1f;

/* Length units */
LPCSTR CMicroScribe::INCHES    = "inches";
LPCSTR CMicroScribe::MM        = "mm";

/* Angle units */
LPCSTR CMicroScribe::RADIANS   = "radians";
LPCSTR CMicroScribe::DEGREES   = "degrees";

/* Angle formats */
LPCSTR CMicroScribe::XYZ_FIXED = "xyz fixed";
LPCSTR CMicroScribe::ZYX_FIXED = "zyx fixed";
LPCSTR CMicroScribe::YXZ_FIXED = "yxz fixed";
LPCSTR CMicroScribe::ZYX_EULER = "zyx Euler";
LPCSTR CMicroScribe::XYZ_EULER = "xyz Euler";
LPCSTR CMicroScribe::ZXY_EULER = "zxy Euler";

bool CommEnumerate(CCommDeviceList& lst, bool bCommProperties)
{
    const std::regex rxPortName("com([1-9][0-9]{0,2})", std::regex_constants::icase);
    HDEVINFO hDevInfoSet;
    SP_DEVINFO_DATA devInfo;

    lst.clear();

    hDevInfoSet = SetupDiGetClassDevs(&GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR, 0, 0, DIGCF_PRESENT);
    if (hDevInfoSet == INVALID_HANDLE_VALUE)
        return false;

    for (int nIndex = 0, nNext = 1; nNext; ++nIndex)
    {
        devInfo.cbSize = sizeof(SP_DEVINFO_DATA);
        nNext = SetupDiEnumDeviceInfo(hDevInfoSet, nIndex, &devInfo);

        if (nNext)
        {
            HKEY hKey = SetupDiOpenDevRegKey(hDevInfoSet, &devInfo, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE);
            if (hKey != INVALID_HANDLE_VALUE)
            {
                CCommDevice cd;
                BYTE rgbyBuffer[128];
                DWORD cbBuffer = sizeof(rgbyBuffer);
                std::cmatch rxMatch;

                if (SUCCEEDED(RegQueryValueExA(hKey, "PortName", 0, 0, rgbyBuffer, &cbBuffer)) && std::regex_match((LPCSTR)rgbyBuffer, rxMatch, rxPortName))
                {
                    cd.m_nPort = atoi(rxMatch[1].first);
                    cd.m_strPort = (LPCSTR)rgbyBuffer;
                    cd.m_strFile = "\\\\.\\";
                    cd.m_strFile.append((LPCSTR)rgbyBuffer);

                    cbBuffer = sizeof(rgbyBuffer);
                    if (SUCCEEDED(SetupDiGetDeviceRegistryPropertyA(hDevInfoSet, &devInfo, SPDRP_DEVICEDESC, 0, rgbyBuffer, cbBuffer, &cbBuffer)))
                        cd.m_strDeviceDesc = (LPCSTR)rgbyBuffer;

                    cbBuffer = sizeof(rgbyBuffer);
                    if (SUCCEEDED(SetupDiGetDeviceRegistryPropertyA(hDevInfoSet, &devInfo, SPDRP_MFG, 0, rgbyBuffer, cbBuffer, &cbBuffer)))
                        cd.m_strMfg = (LPCSTR)rgbyBuffer;

                    cbBuffer = sizeof(rgbyBuffer);
                    if (SUCCEEDED(SetupDiGetDeviceInstanceIdA(hDevInfoSet, &devInfo, (LPSTR)rgbyBuffer, cbBuffer, &cbBuffer)))
                        cd.m_strInstanceId = (LPCSTR)rgbyBuffer;

                    if (bCommProperties)
                    {
                        HANDLE hFile = CreateFileA(cd.m_strFile.c_str(), 0, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
                        if (hFile)
                        {
                            GetCommProperties(hFile, &cd.m_cpPort);
                            CloseHandle(hFile);
                        }
                    }

                    lst.push_back(cd);
                }

                RegCloseKey(hKey);
            }
        }
    }

    SetupDiDestroyDeviceInfoList(hDevInfoSet);

    lst.sort();

    return true;
}

CCommDevice::CCommDevice()
    : m_nPort(0)
{
    ZeroMemory(&m_cpPort, sizeof(m_cpPort));
}

std::ostream& operator<<(std::ostream& os, const CCommDevice& cd)
{
    return os << cd.m_strPort << ": " << cd.m_strDeviceDesc << " / " << cd.m_strMfg << std::endl;
}

CCommPort::CCommPort()
    : m_hPort(INVALID_HANDLE_VALUE)
    , m_dwTimeoutTicks(0)
    , m_dwStopTick(0)
{
    ZeroMemory(&m_dcb, sizeof(m_dcb));
}

CCommPort::~CCommPort()
{
    if (IsOpen())
        Close();
}

bool CCommPort::Open(int nPort, int nBaudRate)
{
    char szBuffer[64];
    COMMTIMEOUTS cto;

    if (IsOpen())
        Close();

    if (!nBaudRate)
        return false;

    _snprintf_s(szBuffer, _TRUNCATE, "\\\\.\\COM%d", nPort);

    m_hPort = CreateFileA(szBuffer, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
    if (m_hPort == INVALID_HANDLE_VALUE)
        return false;

    if (SetupComm(m_hPort, INQUE_MAX, OUTQUE_MAX))
    {
        GetCommTimeouts(m_hPort, &cto);
        cto.ReadTotalTimeoutConstant = 10;
        cto.ReadTotalTimeoutMultiplier = 5;
        SetCommTimeouts(m_hPort, &cto);

        wsprintfA(szBuffer, "baud=%ld parity=N data=8 stop=1", nBaudRate);
        m_dcb.DCBlength = sizeof(DCB);
        if (BuildCommDCBA(szBuffer, &m_dcb) && SetCommState(m_hPort, &m_dcb))
            return true;
    }

    Close();

    return false;
}

void CCommPort::Close()
{
    if (IsOpen())
    {
        PurgeComm(m_hPort, PURGE_TXCLEAR | PURGE_RXCLEAR);
        CloseHandle(m_hPort);
        m_hPort = INVALID_HANDLE_VALUE;
    }
}

void CCommPort::Flush()
{
    if (IsOpen())
        PurgeComm(m_hPort, PURGE_TXCLEAR | PURGE_RXCLEAR);
}

bool CCommPort::IsOpen() const
{
    return m_hPort != INVALID_HANDLE_VALUE;
}

void CCommPort::Pause(float rDelay) const
{
    DWORD dwStart, dwStop;

    dwStart = GetTickCount();
    dwStop = dwStart + (DWORD)(rDelay * 1000.0f);

    if (dwStop == dwStart)
        ++dwStop;

    while (GetTickCount() < dwStop);
}

float CCommPort::GetTimeout() const
{
    return (float)m_dwTimeoutTicks / 1000.0f;
}

void CCommPort::SetTimeout(float rTimeout)
{
    m_dwTimeoutTicks = (DWORD)(rTimeout * 1000.0f);
    if (m_dwTimeoutTicks == 0)
        m_dwTimeoutTicks = 1;
}

void CCommPort::StartTimeout()
{
    m_dwStopTick = GetTickCount() + m_dwTimeoutTicks;
}

bool CCommPort::HasTimedOut() const
{
    return m_dwStopTick < GetTickCount();
}

int CCommPort::ReadChar()
{
    COMSTAT cs;
    DWORD dwStatus, cbData;
    BYTE byData;

    if (!IsOpen())
        return EOF;

    ClearCommError(m_hPort, &dwStatus, &cs);

    if (cs.cbInQue > 0 && ReadFile(m_hPort, &byData, 1, &cbData, 0) && cbData > 0)
        return (int)byData;
    return EOF;
}

DWORD CCommPort::ReadString(LPSTR pchBuffer, DWORD cchBuffer, float rTimeout)
{
    DWORD cchData;
    COMMTIMEOUTS octo, cto;

    if (!IsOpen())
        return 0;

    GetCommTimeouts(m_hPort, &octo);
    cto.ReadTotalTimeoutConstant = (int)(rTimeout * 1000.0f);
    cto.ReadTotalTimeoutMultiplier = 0;
    SetCommTimeouts(m_hPort, &cto);
    ReadFile(m_hPort, pchBuffer, cchBuffer, &cchData, 0);
    SetCommTimeouts(m_hPort, &octo);

    return cchData;
}

bool CCommPort::WriteChar(int chData)
{
    BYTE byData = (BYTE)chData;
    DWORD cbData;

    if (IsOpen() && WriteFile(m_hPort, &byData, 1, &cbData, 0))
        return true;
    return false;
}

bool CCommPort::WriteString(LPCSTR szData)
{
    COMSTAT cs;
    DWORD dwStatus, cchData = (DWORD)strlen(szData);

    if (!IsOpen())
        return false;

    ClearCommError(m_hPort, &dwStatus, &cs);

    if (cchData >= OUTQUE_MAX - cs.cbOutQue || !WriteFile(m_hPort, szData, cchData, &cchData, 0))
        return false;
    return true;
}

DWORD CCommPort::GetInputCount() const
{
    COMSTAT cs;
    DWORD dwStatus;

    if (!IsOpen())
        return 0;

    ClearCommError(m_hPort, &dwStatus, &cs);
    return cs.cbInQue;
}

bool CCommPort::IsInputFull() const
{
    return INQUE_MAX == GetInputCount();
}

void CHCI::ClearPacket()
{
    m_packet.m_cbNeeded = 0;
    m_packet.m_byCommand = 0;
    m_packet.m_bParsed = 1;
    m_packet.m_bError = 0;
    m_packet.m_pbyPacket = m_packet.m_rgbyPacket;
    m_nPacketsExpected = 0;
}

void CHCI::ComParams(int nPort, int nBaudRate)
{
    m_nPort = nPort;

    AdjustBaudRate(&nBaudRate);
    m_nBaudRate = nBaudRate;

    m_rSlowTimeout = 0.5f; // Default: 3.0f
    m_rFastTimeout = TIMEOUT_CHARS * 8.0f / (float)nBaudRate;
    if (m_rFastTimeout < MIN_TIMEOUT)
        m_rFastTimeout = MIN_TIMEOUT;

    ClearPacket();
}

void CHCI::Init(int nPort, int nBaudRate)
{
    ComParams(nPort, nBaudRate);
    ClearPacket();

    m_szSerialNumber[0] = 0;
    m_szProductName[0] = 0;
    m_szProductId[0] = 0;
    m_szModelName[0] = 0;
    m_szComment[0] = 0;
    m_szParamFormat[0] = 0;
    m_szVersion[0] = 0;

    m_cbDEFAULT = NULL;
    m_cbBADPORT = m_cbDEFAULT;
    m_cbCANTOPEN = m_cbDEFAULT;
    m_cbNOHCI = m_cbDEFAULT;
    m_cbCANTBEGIN = m_cbDEFAULT;
    m_cbTIMEDOUT = m_cbDEFAULT;
    m_cbBADPACKET = m_cbDEFAULT;
    m_cbBADPASSWORD = m_cbDEFAULT;
    m_cbBADVERSION = m_cbDEFAULT;
    m_cbBADFORMAT = m_cbDEFAULT;

    m_pvUserData = 0;
}

void CHCI::FastTimeout()
{
    commport.SetTimeout(m_rFastTimeout);
}

void CHCI::SlowTimeout()
{
    commport.SetTimeout(m_rSlowTimeout);
}

CHCI::status_t CHCI::Connect()
{
    status_t result;
    int port = m_nPort;

    if (commport.Open(port, m_nBaudRate))
    {
        SlowTimeout();

        result = Autosynch();
        if (result == S_SUCCESS)
            result = Begin();
    }
    else
        result = S_CANT_OPEN_PORT;

    return Error(result);
}

CHCI::status_t CHCI::Autosynch()
{
    int ch;
    LPCSTR pchSignOn = STR_SIGNON;
    BOOL bSignedOn = FALSE;

    commport.StartTimeout();

    while (!bSignedOn && !commport.HasTimedOut())
    {
        End();

        commport.WriteString(STR_SIGNON);
        commport.Pause(WAIT_SIGNON);

        while (((ch = commport.ReadChar()) != EOF) && !bSignedOn)
        {
            if (ch == *pchSignOn)
            {
                if (!*++pchSignOn)
                    bSignedOn = TRUE;
            }
            else
                pchSignOn = STR_SIGNON;
        }
    }

    commport.Flush(); /* Get rid of excess SIGNON strings in buffer */

    if (bSignedOn)
        return S_SUCCESS;
    return S_NO_HCI;
}

CHCI::status_t CHCI::Begin()
{
    commport.WriteString(STR_BEGIN);
    if (ReadString(m_szProductId) == S_SUCCESS)
        return S_SUCCESS;
    return S_CANT_BEGIN;
}

void CHCI::End()
{
    commport.WriteChar(END_SESSION);
}

void CHCI::Disconnect()
{
    commport.Flush();
    End();
    commport.Pause(WAIT_END);
    commport.Close();
}

void CHCI::ResetCom()
{
    commport.Pause(5e-2f); /* Wait 50 ms for HCI to settle */
    commport.Flush();
    ClearPacket();
}

void CHCI::ChangeBaud(int nNewBaudRate)
{
    BYTE byBaudCode;

    AdjustBaudRate(&nNewBaudRate);
    byBaudCode = Baud2Code(nNewBaudRate);
    nNewBaudRate = Code2Baud(byBaudCode);
    commport.WriteChar(SET_BAUD);
    commport.WriteChar(byBaudCode);
    commport.Pause(WAIT_END);
    commport.Close();
    commport.Open(m_nPort, nNewBaudRate);
}

CHCI::status_t CHCI::GetStrings()
{
    status_t result;

    result = StringCmd(GET_PROD_NAME);
    if (result == S_SUCCESS) result = StringCmd(GET_PROD_ID);
    if (result == S_SUCCESS) result = StringCmd(GET_MODEL_NAME);
    if (result == S_SUCCESS) result = StringCmd(GET_SERNUM);
    if (result == S_SUCCESS) result = StringCmd(GET_COMMENT);
    if (result == S_SUCCESS) result = StringCmd(GET_PRM_FORMAT);
    if (result == S_SUCCESS) result = StringCmd(GET_VERSION);

    return result;
}

BYTE CHCI::CmdByte(int nTimerFlag, int nAnalogReports, int nEncoderReports) const
{
    return ( (nTimerFlag ? TIMER_BIT : 0)                    \
             | (nAnalogReports > 4 ? ANALOG_BITS :           \
                 (nAnalogReports > 2 ? ANALOG_HI_BIT :       \
                 (nAnalogReports ? ANALOG_LO_BIT : 0)))      \
             | (nEncoderReports > 6 ? ENCODER_HI_BIT :       \
                 (nEncoderReports > 5 ? ENCODER_BITS :       \
                 (nEncoderReports ? ENCODER_LO_BIT : 0))) );
}

void CHCI::StdCmd(int nTimerFlag, int nAnalogReports, int nEncoderReports)
{
    BYTE byCommand = CmdByte(nTimerFlag, nAnalogReports, nEncoderReports);
    commport.WriteChar(byCommand);

    if (!m_nPacketsExpected++)
    {
        FastTimeout();
        commport.StartTimeout();
    }
}

void CHCI::SimpleCfgCmd(BYTE byCommand)
{
    commport.WriteChar(byCommand);

    if (!m_nPacketsExpected++)
    {
        FastTimeout();
        commport.StartTimeout();
    }
}

CHCI::status_t CHCI::GetParams(BYTE* pbyBlock, int* pcbBlockSize)
{
    int ch;

    commport.WriteChar(GET_PARAMS);
    FastTimeout();
    commport.StartTimeout();
    while ((ch = commport.ReadChar()) != GET_PARAMS)
    {
        if (ch != EOF)
            commport.StartTimeout();

        if (commport.HasTimedOut())
            break;
    }

    if (ch != GET_PARAMS)
        return Error(S_TIMED_OUT);

    *pcbBlockSize = -1; /* Means take 1st byte as the block length */

    return ReadBlock(pbyBlock, pcbBlockSize);
}

CHCI::status_t CHCI::GetExtParams(BYTE* pbyBlock, int* pcbBlockSize)
{
    int ch;

    commport.WriteChar(GET_EXT_PARAMS);
    FastTimeout();
    commport.StartTimeout();
    while ((ch = commport.ReadChar()) != GET_EXT_PARAMS)
    {
        if (ch != EOF)
            commport.StartTimeout();

        if (commport.HasTimedOut())
            break;
    }

    if (ch != GET_EXT_PARAMS)
        return Error(S_TIMED_OUT);

    *pcbBlockSize = -1; /* Means take 1st byte as the block length */

    return ReadBlock(pbyBlock, pcbBlockSize);
}

CHCI::status_t CHCI::SetParams(BYTE* pbyBlock, int cbBlockSize)
{
    m_nConfigArguments = cbBlockSize;

    for (int i = 0; i < cbBlockSize; ++i)
        m_rgbyConfigArguments[i] = pbyBlock[i];

    return PasswdCmd(SET_PARAMS);
}

CHCI::status_t CHCI::GetHomeRef()
{
    SimpleCfgCmd(GET_HOME_REF);
    return WaitPacket();
}

CHCI::status_t CHCI::GoHomePos()
{
    SimpleCfgCmd(HOME_POS);
    return WaitPacket();
}

CHCI::status_t CHCI::GetMaxes()
{
    SimpleCfgCmd(GET_MAXES);
    return WaitPacket();
}

void CHCI::InsertMarker(BYTE byMarker)
{
    SimpleCfgCmd(INSERT_MARKER);
    commport.WriteChar(byMarker);
}

CHCI::status_t CHCI::StringCmd(BYTE byCommand)
{
    status_t result;
    int ch;

    commport.WriteChar(byCommand);
    FastTimeout();
    commport.StartTimeout();
    while ((ch = commport.ReadChar()) != byCommand)
    {
        if (ch != EOF)
            commport.StartTimeout();

        if (commport.HasTimedOut())
            break;
    }

    if (ch != byCommand)
        return Error(S_TIMED_OUT);

    switch (byCommand)
    {
        case GET_PROD_NAME:
            result = ReadString(m_szProductName);
            break;
        case GET_PROD_ID:
            result = ReadString(m_szProductId);
            break;
        case GET_MODEL_NAME:
            result = ReadString(m_szModelName);
            break;
        case GET_SERNUM:
            result = ReadString(m_szSerialNumber);
            break;
        case GET_COMMENT:
            result = ReadString(m_szComment);
            break;
        case GET_PRM_FORMAT:
            result = ReadString(m_szParamFormat);
            break;
        case GET_VERSION:
            result = ReadString(m_szVersion);
            break;
        default:
            result = S_SUCCESS;
            break;
    }

    return result;
}

CHCI::status_t CHCI::PasswdCmd(BYTE byCommand)
{
    int ch;

    commport.WriteChar(byCommand);
    FastTimeout();
    commport.StartTimeout();
    while ((ch = commport.ReadChar()) != byCommand)
    {
        if (ch != EOF)
            commport.StartTimeout();

        if (commport.HasTimedOut())
            break;
    }

    if (ch == byCommand)
    {
        commport.WriteString(m_szSerialNumber);
        commport.WriteChar(0); /* Write the null to terminate */
        commport.StartTimeout();
        while ((ch = commport.ReadChar()) == EOF)
        {
            if (commport.HasTimedOut())
                break;
        }

        if (ch == EOF)
            return Error(S_TIMED_OUT);
        else if (ch == PASSWD_OK)
        {
            for (int i = 0; i < m_nConfigArguments; ++i)
                commport.WriteChar(m_rgbyConfigArguments[i]);

            for (int i = 0; i < m_nConfigArguments; ++i)
                commport.Pause(WAIT_CFG_ARG);

            return S_SUCCESS;
        }
        else
            return S_BAD_PASSWORD;
    }

    return Error(S_TIMED_OUT);
}

CHCI::status_t CHCI::SetHomePos(int* pnHomePos)
{
    m_nConfigArguments = 2 * NUM_ENCODERS;
    m_rgbyConfigArguments[0] = pnHomePos[0] >> 8;
    m_rgbyConfigArguments[1] = pnHomePos[0] & 0x00FF;
    m_rgbyConfigArguments[2] = pnHomePos[1] >> 8;
    m_rgbyConfigArguments[3] = pnHomePos[1] & 0x00FF;
    m_rgbyConfigArguments[4] = pnHomePos[2] >> 8;
    m_rgbyConfigArguments[5] = pnHomePos[2] & 0x00FF;
    m_rgbyConfigArguments[6] = pnHomePos[3] >> 8;
    m_rgbyConfigArguments[7] = pnHomePos[3] & 0x00FF;
    m_rgbyConfigArguments[8] = pnHomePos[4] >> 8;
    m_rgbyConfigArguments[9] = pnHomePos[4] & 0x00FF;
    m_rgbyConfigArguments[10] = pnHomePos[5] >> 8;
    m_rgbyConfigArguments[11] = pnHomePos[5] & 0x00FF;

    return PasswdCmd(SET_HOME);
}

CHCI::status_t CHCI::SetHomeRef( int* pnHomeRef )
{
    m_nConfigArguments = 2 * NUM_ENCODERS;
    m_rgbyConfigArguments[0] = pnHomeRef[0] >> 8;
    m_rgbyConfigArguments[1] = pnHomeRef[0] & 0x00FF;
    m_rgbyConfigArguments[2] = pnHomeRef[1] >> 8;
    m_rgbyConfigArguments[3] = pnHomeRef[1] & 0x00FF;
    m_rgbyConfigArguments[4] = pnHomeRef[2] >> 8;
    m_rgbyConfigArguments[5] = pnHomeRef[2] & 0x00FF;
    m_rgbyConfigArguments[6] = pnHomeRef[3] >> 8;
    m_rgbyConfigArguments[7] = pnHomeRef[3] & 0x00FF;
    m_rgbyConfigArguments[8] = pnHomeRef[4] >> 8;
    m_rgbyConfigArguments[9] = pnHomeRef[4] & 0x00FF;
    m_rgbyConfigArguments[10] = pnHomeRef[5] >> 8;
    m_rgbyConfigArguments[11] = pnHomeRef[5] & 0x00FF;

    return PasswdCmd(SET_HOME_REF);
}

CHCI::status_t CHCI::FactorySettings()
{
    status_t result;

    m_nConfigArguments = 0;
    result = PasswdCmd(RESTORE_FACTORY);
    commport.Pause(WAIT_RESTORE);

    return result;
}

void CHCI::ReportMotion(int nTimerFlag, int nAnalogReports, int nEncoderReports, int nDelay, BYTE byActiveButtons, int* pnAnalogDeltas, int* pnEncoderDeltas)
{
    BYTE byCommand = CmdByte(nTimerFlag, nAnalogReports, nEncoderReports);

    commport.WriteChar(REPORT_MOTION);
    commport.WriteChar(nDelay >> 8);
    commport.WriteChar(nDelay & 0x00FF);
    commport.WriteChar(byCommand);
    commport.WriteChar(byActiveButtons);

    for (int i = 0; i < NUM_ANALOGS; ++i)
        commport.WriteChar(pnAnalogDeltas[i]);

    if (nEncoderReports < NUM_ENCODERS)
    {
        for (int i = 0; i < (NUM_ENCODERS-1); ++i)
        {
            commport.WriteChar(pnEncoderDeltas[i] >> 8);
            commport.WriteChar(pnEncoderDeltas[i] & 0x00FF);
        }
    }
    else
    {
        for (int i = 0; i < NUM_ENCODERS; ++i)
        {
            commport.WriteChar(pnEncoderDeltas[i] >> 8);
            commport.WriteChar(pnEncoderDeltas[i] & 0x00FF);
        }
    }
}

void CHCI::EndMotion()
{
    commport.WriteChar(0);
    commport.Pause(5e-2f);
    commport.Flush();
}

float CHCI::VersionNum()
{
    float rVersionNum = 0.0f;

    sscanf_s(m_szVersion, "HCI %f", &rVersionNum);

    return rVersionNum;
}

CHCI::status_t CHCI::WaitPacket()
{
    status_t result;

    FastTimeout();
    commport.StartTimeout();
    while ((result = CheckPacket(CHECK_SYNC)) == S_NO_PACKET_YET);

    return result;
}

CHCI::status_t CHCI::CheckPacket(int nCheckType)
{
    status_t result;

    if ((result = BuildPacket(nCheckType)) == S_SUCCESS)
    {
        if ((result = ParsePacket()) == S_SUCCESS)
            return result;
        return Error(result);
    }

    if (result == S_TIMED_OUT || commport.HasTimedOut())
        return Error(S_TIMED_OUT);
    return S_NO_PACKET_YET;
}

CHCI::status_t CHCI::CheckMotion()
{
    status_t result;

    if (BuildPacket(CHECK_MOTION) == S_SUCCESS)
    {
        if ((result = ParsePacket()) == S_SUCCESS)
            return result;
        return Error(result);
    }

    return S_NO_PACKET_YET;
}

CHCI::status_t CHCI::BuildPacket(int nCheckType)
{
    int ch, cbRead;

    if (m_packet.m_bParsed)
    {
        if ((ch = commport.ReadChar()) == EOF)
            return S_NO_PACKET_YET;

        m_packet.m_byCommand = (BYTE)ch;
        if (ch < 0x80)
        {
            m_packet.m_bError = TRUE;
            return S_SUCCESS;
        }

        m_packet.m_bParsed = FALSE;
        m_packet.m_bError = FALSE;
        m_packet.m_pbyPacket = m_packet.m_rgbyPacket;
        m_packet.m_cbNeeded = PacketSize(ch);
        --m_nPacketsExpected;

        if (nCheckType == CHECK_ASYNC)
        {
            FastTimeout();
            commport.StartTimeout();
        }
    }

    if (!m_packet.m_bParsed && m_packet.m_cbNeeded > 0)
    {
        if (nCheckType != CHECK_SYNC)
        {
            while ((ch = commport.ReadChar()) != EOF)
            {
                *(m_packet.m_pbyPacket)++ = ch;
                if (--m_packet.m_cbNeeded == 0)
                    break;
            }

            if (m_packet.m_cbNeeded <= 0)
                return S_SUCCESS;
            if (nCheckType == CHECK_ASYNC && commport.HasTimedOut())
                return S_TIMED_OUT;
            return S_NO_PACKET_YET;
        }
        else
        {
            cbRead = commport.ReadString((LPSTR)m_packet.m_pbyPacket, m_packet.m_cbNeeded, m_rFastTimeout);

            m_packet.m_cbNeeded -= cbRead;
            m_packet.m_pbyPacket += cbRead;

            if (m_packet.m_cbNeeded == 0)
                return S_SUCCESS;
            return S_TIMED_OUT;
        }
    }

   return S_SUCCESS;
}

int CHCI::PacketSize(int nCommand)
{
    int cbPacket = 1; /* Regular commands always include buttons byte */
    int nBitMask;

    if (nCommand < CONFIG_MIN)
    {
        if (nCommand & TIMER_BIT)
            cbPacket += 2;
        nBitMask = nCommand & ANALOG_BITS;
        if (nBitMask == ANALOG_LO_BIT)
            cbPacket += 2 + 1;
        else if (nBitMask == ANALOG_HI_BIT)
            cbPacket += 4 + 1;
        else if (nBitMask == ANALOG_BITS)
            cbPacket += 8 + 1;
        nBitMask = nCommand & ENCODER_BITS;
        if (nBitMask == ENCODER_LO_BIT)
            cbPacket += 2 * 5;
        else if (nBitMask == ENCODER_HI_BIT)
            cbPacket += 2 * 7;
        else if (nBitMask == ENCODER_BITS)
            cbPacket += 2 * 6;
    }
    else
    {
        switch (nCommand)
        {
        case GET_HOME_REF:
            cbPacket = 12;
            break;
        case SET_BAUD:
        case INSERT_MARKER:
            cbPacket = 1;
            break;
        case GET_MAXES:
            cbPacket = 24;
            break;
        case HOME_POS:
        case END_SESSION:
        case REPORT_MOTION:
            cbPacket = 0;
            break;
        case GET_PARAMS:
        case GET_EXT_PARAMS:
        case GET_PROD_NAME:
        case GET_PROD_ID:
        case GET_MODEL_NAME:
        case GET_SERNUM:
        case GET_COMMENT:
        case GET_PRM_FORMAT:
        case GET_VERSION:
        case SET_PARAMS:
        case SET_HOME:
        case SET_HOME_REF:
        case RESTORE_FACTORY:
            cbPacket = -1;
            break;
        }
    }

    return cbPacket;
}

void CHCI::InvalidateFields()
{
    m_nTimerUpdated = 0;
    m_rgnAnalogUpdated[0] = 0;
    m_rgnAnalogUpdated[1] = 0;
    m_rgnAnalogUpdated[2] = 0;
    m_rgnAnalogUpdated[3] = 0;
    m_rgnAnalogUpdated[4] = 0;
    m_rgnAnalogUpdated[5] = 0;
    m_rgnAnalogUpdated[6] = 0;
    m_rgnAnalogUpdated[7] = 0;
    m_rgnEncoderUpdated[0] = 0;
    m_rgnEncoderUpdated[1] = 0;
    m_rgnEncoderUpdated[2] = 0;
    m_rgnEncoderUpdated[3] = 0;
    m_rgnEncoderUpdated[4] = 0;
    m_rgnEncoderUpdated[5] = 0;
    m_rgnEncoderUpdated[6] = 0;
    m_bMarkerUpdated = 0;
}

CHCI::status_t CHCI::ParsePacket()
{
    int nCommand = m_packet.m_byCommand, nBitMask, temp;
    status_t result = S_SUCCESS;
    BYTE *pbyPacket;
    int *p, *q;

    if (m_packet.m_cbNeeded)
    {
        if (m_packet.m_cbNeeded < 0)
            result = S_BAD_PACKET;
        return S_NO_PACKET_YET;
    }

    if (m_packet.m_bError)
        result = Error(S_BAD_PACKET);

    if (result == S_SUCCESS)
    {
        InvalidateFields();
        pbyPacket = m_packet.m_rgbyPacket;

        if (nCommand < CONFIG_MIN)
        {
            nBitMask = *pbyPacket++;
            m_nButtons = nBitMask;
            p = m_rgnButton;
            *p++ = nBitMask & 0x01;
            *p++ = nBitMask & 0x02;
            *p++ = nBitMask & 0x04;
            *p++ = nBitMask & 0x08;
            *p++ = nBitMask & 0x10;
            *p++ = nBitMask & 0x20;
            *p++ = nBitMask & 0x40;

            if (nCommand & TIMER_BIT)
            {
                temp = *pbyPacket++ << 7;
                temp += *pbyPacket++;
                m_nTimer = temp;
                m_nTimerUpdated = 1;
            }

            nBitMask = (nCommand & ANALOG_BITS) >> 2;
            if (nBitMask--)
            {
                p = m_rgnAnalog;
                q = m_rgnAnalogUpdated;
                *p++ = *pbyPacket++ << 1;
                *p++ = *pbyPacket++ << 1;
                *q++ = 1, *q++ = 1;

                if (nBitMask--)
                {
                    *p++ = *pbyPacket++ << 1;
                    *p++ = *pbyPacket++ << 1;
                    *q++ = 1, *q++ = 1;

                    if (nBitMask--)
                    {
                        *p++ = *pbyPacket++ << 1;
                        *p++ = *pbyPacket++ << 1;
                        *p++ = *pbyPacket++ << 1;
                        *p++ = *pbyPacket++ << 1;
                        *q++ = 1, *q++ = 1;
                        *q++ = 1, *q++ = 1;
                    }
                }

                p = m_rgnAnalog;
                *p++ |= (*pbyPacket & 0x40 ? 1 : 0);
                *p++ |= (*pbyPacket & 0x20 ? 1 : 0);
                *p++ |= (*pbyPacket & 0x10 ? 1 : 0);
                *p++ |= (*pbyPacket & 0x08 ? 1 : 0);
                *p++ |= (*pbyPacket & 0x04 ? 1 : 0);
                *p++ |= (*pbyPacket & 0x02 ? 1 : 0);
                *p++ |= (*pbyPacket++ & 0x01 ? 1 : 0);
            }

            nBitMask = nCommand & ENCODER_BITS;
            if (nBitMask)
            {
                p = m_rgnEncoder;
                q = m_rgnEncoderUpdated;
                *p = *pbyPacket++ << 7;
                *p++ += *pbyPacket++;
                *p = *pbyPacket++ << 7;
                *p++ += *pbyPacket++;
                *p = *pbyPacket++ << 7;
                *p++ += *pbyPacket++;
                *p = *pbyPacket++ << 7;
                *p++ += *pbyPacket++;
                *p = *pbyPacket++ << 7;
                *p++ += *pbyPacket++;
                *q++ = 1,*q++ = 1;*q++ = 1,*q++ = 1,*q++ = 1;

                if (nBitMask & ENCODER_HI_BIT)
                {
                    *p = *pbyPacket++ << 7;
                    *p++ += *pbyPacket++;
                    *q++ = 1;

                    if (nBitMask == ENCODER_HI_BIT)
                    {
                        *p = *pbyPacket++ << 7;
                        *p++ += *pbyPacket++;
                        *q++ = 1;
                    }
                }
            }
        }
        else
            result = ParseCfgPacket();

        m_packet.m_bParsed = TRUE;
    }

    return result;
}

CHCI::status_t CHCI::ParseCfgPacket()
{
    status_t result = S_SUCCESS;
    BYTE *pbyPacket = m_packet.m_rgbyPacket;
    int *p;

    switch (m_packet.m_byCommand)
    {
    case GET_HOME_REF:
        p = m_rgnHomeRef;
        *p = *pbyPacket++ << 8; *p++ += *pbyPacket++;
        *p = *pbyPacket++ << 8; *p++ += *pbyPacket++;
        *p = *pbyPacket++ << 8; *p++ += *pbyPacket++;
        *p = *pbyPacket++ << 8; *p++ += *pbyPacket++;
        *p = *pbyPacket++ << 8; *p++ += *pbyPacket++;
        *p = *pbyPacket++ << 8; *p++ += *pbyPacket++;
        *p = *pbyPacket++ << 8; *p++ += *pbyPacket++;
        break;

    case GET_MAXES:
        p = m_rgnButtonSupported;
        *p++ = *pbyPacket & 0x01; *p++ = *pbyPacket & 0x02;
        *p++ = *pbyPacket & 0x04; *p++ = *pbyPacket & 0x08;
        *p++ = *pbyPacket & 0x10; *p++ = *pbyPacket & 0x20;
        *p++ = *pbyPacket++ & 0x40;

        m_nTimerMax = *pbyPacket++ << 8;
        m_nTimerMax |= *pbyPacket++;

        p = m_rgnAnalogMax;
        *p++ = *pbyPacket++; *p++ = *pbyPacket++; *p++ = *pbyPacket++;
        *p++ = *pbyPacket++; *p++ = *pbyPacket++; *p++ = *pbyPacket++;
        *p++ = *pbyPacket++; *p++ = *pbyPacket++;
        p = m_rgnAnalogMax;
        *p++ |= (0x40 & *pbyPacket ? 0x01 : 0);
        *p++ |= (0x20 & *pbyPacket ? 0x01 : 0);
        *p++ |= (0x10 & *pbyPacket ? 0x01 : 0);
        *p++ |= (0x08 & *pbyPacket ? 0x01 : 0);
        *p++ |= (0x04 & *pbyPacket ? 0x01 : 0);
        *p++ |= (0x02 & *pbyPacket ? 0x01 : 0);
        *p++ |= (0x01 & *pbyPacket++ ? 0x01 : 0);

        p = m_rgnEncoderMax;
        *p = *pbyPacket++ << 8; *p++ += *pbyPacket++;
        *p = *pbyPacket++ << 8; *p++ += *pbyPacket++;
        *p = *pbyPacket++ << 8; *p++ += *pbyPacket++;
        *p = *pbyPacket++ << 8; *p++ += *pbyPacket++;
        *p = *pbyPacket++ << 8; *p++ += *pbyPacket++;
        *p = *pbyPacket++ << 8; *p++ += *pbyPacket++;
        *p = *pbyPacket++ << 8; *p++ += *pbyPacket++;
        break;

    case INSERT_MARKER:
        m_nMarker = *pbyPacket;
        m_bMarkerUpdated = TRUE;
        break;

    case HOME_POS: /* No action needed */
    case REPORT_MOTION:
        break;
    default:
        result = S_BAD_PACKET;
    }

    return result;
}

CHCI::status_t CHCI::ReadString(LPSTR pchData)
{
    int ch;

    commport.StartTimeout();
    while (!commport.HasTimedOut())
    {
        ch = commport.ReadChar();
        if (ch != EOF)
        {
            *pchData++ = (BYTE)ch;
            if (ch == 0)
                return S_SUCCESS;
        }
    }

    return S_TIMED_OUT;
}


CHCI::status_t CHCI::ReadBlock(BYTE* pbyBlock, int* pcbBlock)
{
    int ch, cbBlock;

    cbBlock = *pcbBlock;
    *pcbBlock = 0;
    SlowTimeout();
    commport.StartTimeout();
    while (!commport.HasTimedOut())
    {
        if (cbBlock < 0)
            cbBlock = commport.ReadChar();
        else
        {
            ch = commport.ReadChar();
            if (ch != EOF)
            {
                *pbyBlock++ = (BYTE)ch;
                (*pcbBlock)++;
                if (--cbBlock == 0)
                    return S_SUCCESS;
            }
        }
    }

    return S_TIMED_OUT;
}

CHCI::status_t CHCI::SimpleString(status_t condition)
{
    fprintf(stdout, "\n**HCI Error: %s\n", condition);
    return condition;
}

CHCI::status_t CHCI::Error(status_t condition)
{
    CallbackHandler_t pfn;

    /* These two are not really errors */
    if (condition == S_SUCCESS)
        return S_SUCCESS;

    if (condition == S_NO_PACKET_YET)
        return S_NO_PACKET_YET;

    /* These errors have handler pointers */
    if (condition == S_BAD_PORT_NUM)
        pfn = m_cbBADPORT;
    else if (condition == S_CANT_OPEN_PORT)
        pfn = m_cbCANTOPEN;
    else if (condition == S_NO_HCI)
        pfn = m_cbNOHCI;
    else if (condition == S_CANT_BEGIN)
        pfn = m_cbCANTBEGIN;
    else if (condition == S_TIMED_OUT)
        pfn = m_cbTIMEDOUT;
    else if (condition == S_BAD_PACKET)
        pfn = m_cbBADPACKET;
    else if (condition == S_BAD_PASSWORD)
        pfn = m_cbBADPASSWORD;
    else if (condition == S_BAD_VERSION)
        pfn = m_cbBADVERSION;
    else if (condition == S_BAD_FORMAT)
        pfn = m_cbBADFORMAT;
    else pfn = NULL;

    if (pfn == NULL)
        pfn = m_cbDEFAULT;

    if (pfn == NULL)
        return condition;

    return (*pfn)(*this, condition);
}

BYTE CHCI::Baud2Code(int nBaudRate)
{
    BYTE byCode;

    switch(nBaudRate)
    {
    case 115200l:
        byCode = 0x00;
        break;
    case 57600l:
        byCode = 0x01;
        break;
    case 28800:
        byCode = 0x02;
        break;
    case 14400:
        byCode = 0x03;
        break;
    case 38400l:
        byCode = 0x10;
        break;
    case 19200:
        byCode = 0x11;
        break;
    case 9600:
        byCode = 0x12;
        break;
    default: /* When in doubt, use 9600 */
        byCode = 0x12;
    }

    return byCode;
}

int CHCI::Code2Baud(BYTE byCode)
{
    int nBaudRate;

    switch(byCode)
    {
    case 0x00:
        nBaudRate = 115200;
        break;
    case 0x01:
        nBaudRate = 57600;
        break;
    case 0x02:
        nBaudRate = 28800;
        break;
    case 0x03:
        nBaudRate = 14400;
        break;
    case 0x10:
        nBaudRate = 38400;
        break;
    case 0x11:
        nBaudRate = 19200;
        break;
    case 0x12:
        nBaudRate = 9600;
        break;
    default: /* When in doubt, use 9600 */
        nBaudRate = 9600;
    }

    return nBaudRate;
}

void CHCI::AdjustBaudRate(int* pnBaudRate)
{
    switch(*pnBaudRate)
    {
    case 115200:
    case 1152:
    case 115:
        *pnBaudRate = 115200;
        break;
    case 57600:
    case 576:
    case 58:
    case 57:
        *pnBaudRate = 57600;
        break;
    case 38400:
    case 384:
    case 38:
        *pnBaudRate = 38400;
        break;
    case 19200:
    case 192:
    case 19:
        *pnBaudRate = 19200;
        break;
    case 14400:
    case 144:
    case 14:
        *pnBaudRate = 14400;
        break;
    case 9600:
    case 96:
        *pnBaudRate = 9600;
        break;
    default:
        if (*pnBaudRate < 1000)
            *pnBaudRate *= 1000;

        if (*pnBaudRate > 86400)
            *pnBaudRate = 115200;
        else if (*pnBaudRate > 48000)
            *pnBaudRate = 57600;
        else if (*pnBaudRate > 28800)
            *pnBaudRate = 38400;
        else if (*pnBaudRate > 16800)
            *pnBaudRate = 19200;
        else if (*pnBaudRate > 12000)
            *pnBaudRate = 14400;
        else
            *pnBaudRate = 9600;
    }
}

void CMicroScribe::Init()
{
    /* Temporarily use default port & baud rate (we're not connecting yet) */
    hci.Init(1, 9600);

    m_lengthUnits = MM;
    m_angleUnits = DEGREES;
    m_angleFormat = YXZ_FIXED;
    m_nTimerReport = 0;
    m_nAnalogReports = 0;
    m_nNumPoints = -1;
    m_pfnPacketCalculation = &CMicroScribe::CalcNothing;
    m_rBETA = 0.0f;
}

CMicroScribe::status_t CMicroScribe::Connect()
{
    CCommDeviceList lst;
    int pnValidBaudRates[] = { 115200, 57600, 38400, 19200, 14400, 9600 };

    if (CommEnumerate(lst))
    {
        CHCI::CallbackHandler_t cbBADPORT, cbCANTOPEN, cbNOHCI, cbCANTBEGIN, cbTIMEDOUT;
        CHCI::CallbackHandler_t cbBADPACKET, cbBADPASSWORD, cbBADVERSION, cbBADFORMAT, cbDEFAULT;

        cbBADPORT = hci.m_cbBADPORT;
        cbCANTOPEN = hci.m_cbCANTOPEN;
        cbNOHCI = hci.m_cbNOHCI;
        cbCANTBEGIN = hci.m_cbCANTBEGIN;
        cbTIMEDOUT = hci.m_cbTIMEDOUT;
        cbBADPACKET = hci.m_cbBADPACKET;
        cbBADPASSWORD = hci.m_cbBADPASSWORD;
        cbBADVERSION = hci.m_cbBADVERSION;
        cbBADFORMAT = hci.m_cbBADFORMAT;
        cbDEFAULT = hci.m_cbDEFAULT;

        hci.m_cbBADPORT = hci.m_cbCANTOPEN = hci.m_cbNOHCI = hci.m_cbCANTBEGIN = hci.m_cbTIMEDOUT = 0;
        hci.m_cbBADPACKET = hci.m_cbBADPASSWORD = hci.m_cbBADVERSION = hci.m_cbBADFORMAT = hci.m_cbDEFAULT = 0;

        for (int i = 0; i < sizeof(pnValidBaudRates) / sizeof(*pnValidBaudRates); ++i)
        {
            for (CCommDeviceList::const_reverse_iterator it = lst.crbegin(); it != lst.crend(); ++it)
            {
                DWORD dwCanSetBaud = 0;

                if (!it->m_cpPort.wPacketLength)
                    continue;

                switch (pnValidBaudRates[i])
                {
                case 115200:
                    dwCanSetBaud = it->m_cpPort.dwSettableBaud & BAUD_115200;
                    break;
                case 57600:
                    dwCanSetBaud = it->m_cpPort.dwSettableBaud & BAUD_57600;
                    break;
                case 38400:
                    dwCanSetBaud = it->m_cpPort.dwSettableBaud & BAUD_38400;
                    break;
                case 19200:
                    dwCanSetBaud = it->m_cpPort.dwSettableBaud & BAUD_19200;
                    break;
                case 14400:
                    dwCanSetBaud = it->m_cpPort.dwSettableBaud & BAUD_14400;
                    break;
                case 9600:
                    dwCanSetBaud = it->m_cpPort.dwSettableBaud & BAUD_9600;
                    break;
                }

                fprintf(stdout, "%s(%u) %s / %s\n", it->m_strPort.c_str(), pnValidBaudRates[i], it->m_strDeviceDesc.c_str(), it->m_strMfg.c_str());

                hci.ComParams(it->m_nPort, pnValidBaudRates[i]);
                hci.ClearPacket();
                if (hci.Connect() == CHCI::S_SUCCESS)
                {
                    hci.m_cbBADPORT = cbBADPORT;
                    hci.m_cbCANTOPEN = cbCANTOPEN;
                    hci.m_cbNOHCI = cbNOHCI;
                    hci.m_cbCANTBEGIN = cbCANTBEGIN;
                    hci.m_cbTIMEDOUT = cbTIMEDOUT;
                    hci.m_cbBADPACKET = cbBADPACKET;
                    hci.m_cbBADPASSWORD = cbBADPASSWORD;
                    hci.m_cbBADVERSION = cbBADVERSION;
                    hci.m_cbBADFORMAT = cbBADFORMAT;
                    hci.m_cbDEFAULT = cbDEFAULT;

                    return GetConstants();
                }
            }
        }

        hci.m_cbBADPORT = cbBADPORT;
        hci.m_cbCANTOPEN = cbCANTOPEN;
        hci.m_cbNOHCI = cbNOHCI;
        hci.m_cbCANTBEGIN = cbCANTBEGIN;
        hci.m_cbTIMEDOUT = cbTIMEDOUT;
        hci.m_cbBADPACKET = cbBADPACKET;
        hci.m_cbBADPASSWORD = cbBADPASSWORD;
        hci.m_cbBADVERSION = cbBADVERSION;
        hci.m_cbBADFORMAT = cbBADFORMAT;
        hci.m_cbDEFAULT = cbDEFAULT;
    }

    return CHCI::S_CANT_OPEN_PORT;
}

CMicroScribe::status_t CMicroScribe::Connect(int nPort, int nBaudRate)
{
    status_t result = CHCI::S_TRY_AGAIN;

    while (result == CHCI::S_TRY_AGAIN)
    {
        hci.ComParams(nPort, nBaudRate);
        hci.ClearPacket();
        result = hci.Connect();
    }

    if (result == CHCI::S_SUCCESS)
        result = GetConstants();

    return result;
}

void CMicroScribe::Disconnect()
{
    hci.Disconnect();
}

void CMicroScribe::ChangeBaud(int nNewBaudRate)
{
    hci.ChangeBaud(nNewBaudRate);
}

int CMicroScribe::GetPoint()
{
    static BOOL bPedalReset = TRUE;
    status_t result;

    result = SyncStylus3DOF();
    if (result != CHCI::S_SUCCESS)
        return 0;

    if (hci.m_nButtons == LEFT_PEDAL || hci.m_nButtons == RIGHT_PEDAL)
    {
        if (bPedalReset)
        {
            bPedalReset = FALSE;
            return hci.m_nButtons;
        }

        return 0;
    }

    bPedalReset = TRUE;

    return 0;
}

int CMicroScribe::AutoPlotPoint(float rDistanceSetting)
{
    static float rPrevX, rPrevY, rPrevZ;
    static BOOL bLeftPedalReset = TRUE, bRightPedalReset = TRUE;
    float rX, rY, rZ;
    status_t result;

    m_rPt2PtDistance = 0.0f;

    result = SyncStylus3DOF();
    if (result != CHCI::S_SUCCESS)
        return 0;

    if (hci.m_nButtons == LEFT_PEDAL)
    {
        if (bLeftPedalReset)
        {
            rPrevX = m_stylusPosition.x;
            rPrevY = m_stylusPosition.y;
            rPrevZ = m_stylusPosition.z;
            bLeftPedalReset = FALSE;

            return hci.m_nButtons;
        }
        else
        {
            rX = m_stylusPosition.x - rPrevX;
            rY = m_stylusPosition.y - rPrevY;
            rZ = m_stylusPosition.z - rPrevZ;
            m_rPt2PtDistance = sqrt(rX*rX+rY*rY+rZ*rZ);
            if (m_rPt2PtDistance >= rDistanceSetting)
            {
                rPrevX = m_stylusPosition.x;
                rPrevY = m_stylusPosition.y;
                rPrevZ = m_stylusPosition.z;

                return hci.m_nButtons;
            }

            return 0;
        }
    }

    if (hci.m_nButtons == RIGHT_PEDAL)
    {
        bLeftPedalReset = TRUE;
        if (bRightPedalReset)
        {
            bRightPedalReset = FALSE;
            return hci.m_nButtons;
        }

        return 0;
    }

    bRightPedalReset = TRUE;

    return 0;
}

CMicroScribe::status_t CMicroScribe::SyncStylus6DOF()
{
    status_t result;

    hci.StdCmd(m_nTimerReport, m_nAnalogReports, 6);
    if ((result = hci.WaitPacket()) == CHCI::S_SUCCESS)
    {
        CalcJoints();
        CalcStylus6DOF();
    }

    return result;
}

CMicroScribe::status_t CMicroScribe::SyncStylus3DOF()
{
    status_t result;

    hci.StdCmd(m_nTimerReport, m_nAnalogReports, 6);
    if ((result = hci.WaitPacket()) == CHCI::S_SUCCESS)
    {
        CalcJoints();
        CalcStylus3DOF();
    }

    return result;
}

CMicroScribe::status_t CMicroScribe::Sync3Joint()
{
    status_t result;

    hci.StdCmd(m_nTimerReport, m_nAnalogReports, 3);
    if ((result = hci.WaitPacket()) == CHCI::S_SUCCESS)
        CalcJoints();

    return result;
}

CMicroScribe::status_t CMicroScribe::Sync6Joint()
{
    status_t result;

    hci.StdCmd(m_nTimerReport, m_nAnalogReports, 6);
    if ((result = hci.WaitPacket()) == CHCI::S_SUCCESS)
        CalcJoints();

    return result;
}

CMicroScribe::status_t CMicroScribe::SyncAll()
{
    status_t result;

    hci.StdCmd(m_nTimerReport, m_nAnalogReports, 6);
    if ((result = hci.WaitPacket()) == CHCI::S_SUCCESS)
    {
        CalcJoints();
        CalcAll();
    }

    return result;
}

CMicroScribe::status_t CMicroScribe::AsyncCheck()
{
    status_t result;

    if ((result = hci.CheckPacket(CHCI::CHECK_ASYNC)) == CHCI::S_SUCCESS)
    {
        CalcJoints();
        (this->*m_pfnPacketCalculation)();
    }

    return result;
}

void CMicroScribe::AsyncStylus6DOF()
{
    hci.StdCmd(m_nTimerReport, m_nAnalogReports, 6);
    m_pfnPacketCalculation = &CMicroScribe::CalcStylus6DOF;
}

void CMicroScribe::AsyncStylus3DOF()
{
    hci.StdCmd(m_nTimerReport, m_nAnalogReports, 6);
    m_pfnPacketCalculation = &CMicroScribe::CalcStylus3DOF;
}

void CMicroScribe::Async3Joint()
{
    hci.StdCmd(m_nTimerReport, m_nAnalogReports, 3);
    m_pfnPacketCalculation = &CMicroScribe::CalcNothing;
}

void CMicroScribe::Async6Joint()
{
    hci.StdCmd(m_nTimerReport, m_nAnalogReports, 6);
    m_pfnPacketCalculation = &CMicroScribe::CalcNothing;
}

void CMicroScribe::AsyncAll()
{
    hci.StdCmd(m_nTimerReport, m_nAnalogReports, 6);
    m_pfnPacketCalculation = &CMicroScribe::CalcAll;
}

CMicroScribe::status_t CMicroScribe::MotionCheck()
{
    status_t result;

    if ((result = hci.CheckMotion()) == CHCI::S_SUCCESS)
    {
        CalcJoints();
        (this->*m_pfnPacketCalculation)();
    }

    return result;
}

void CMicroScribe::MotionStylus6DOF(int nMotionThreshold, int nPacketDelay, int nActiveButtons)
{
    MotionStart(6, nMotionThreshold, nPacketDelay, nActiveButtons);
    m_pfnPacketCalculation = &CMicroScribe::CalcStylus6DOF;
}

void CMicroScribe::MotionStylus3DOF(int nMotionThreshold, int nPacketDelay, int nActiveButtons)
{
    MotionStart(6, nMotionThreshold, nPacketDelay, nActiveButtons);
    m_pfnPacketCalculation = &CMicroScribe::CalcStylus3DOF;
}

void CMicroScribe::Motion6Joint(int nMotionThreshold, int nPacketDelay, int nActiveButtons)
{
    MotionStart(6, nMotionThreshold, nPacketDelay, nActiveButtons);
    m_pfnPacketCalculation = &CMicroScribe::CalcNothing;
}

void CMicroScribe::Motion3Joint(int nMotionThreshold, int nPacketDelay, int nActiveButtons)
{
    MotionStart(3, nMotionThreshold, nPacketDelay, nActiveButtons);
    m_pfnPacketCalculation = &CMicroScribe::CalcNothing;
}

void CMicroScribe::MotionAll(int nMotionThreshold, int nPacketDelay, int nActiveButtons)
{
    MotionStart(3, nMotionThreshold, nPacketDelay, nActiveButtons);
    m_pfnPacketCalculation = &CMicroScribe::CalcAll;
}

void CMicroScribe::MotionEnd()
{
    hci.EndMotion();
}

void CMicroScribe::LengthUnits(length_units_t units)
{
    m_lengthUnits = units;
    ConvertParams();
}

void CMicroScribe::AngleUnits(angle_units_t units)
{
    m_angleUnits = units;
}

void CMicroScribe::AngleFormat(angle_format_t format)
{
    m_angleFormat = format;
}

void CMicroScribe::ReportTimer()
{
    m_nTimerReport = 1;
}

void CMicroScribe::SkipTimer()
{
    m_nTimerReport = 0;
}

void CMicroScribe::ReportAnalog(int nAnalogReports)
{
    m_nAnalogReports = nAnalogReports;
}

void CMicroScribe::SkipAnalog()
{
    m_nAnalogReports = 0;
}

void CMicroScribe::CalcStylus6DOF()
{
    CalcStylus3DOF();
    CalcStylusDir();
}

void CMicroScribe::CalcStylus3DOF()
{
    CalcTrig();
    CalcM();
    CalcT();

    m_stylusPosition.x = m_T[0][3];
    m_stylusPosition.y = m_T[1][3];
    m_stylusPosition.z = m_T[2][3];
}

void CMicroScribe::CalcTrig()
{
    angle_t *prJointRad = m_rgrJointRad;
    ratio_t *prSin = m_rgrSin, *prCos = m_rgrCos;

    *prSin++ = sin(*prJointRad);
    *prCos++ = cos(*prJointRad++);
    *prSin++ = sin(*prJointRad);
    *prCos++ = cos(*prJointRad++);
    *prSin++ = sin(*prJointRad);
    *prCos++ = cos(*prJointRad++);
    if (hci.m_rgnEncoderUpdated[5])
    {
        *prSin++ = sin(*prJointRad);
        *prCos++ = cos(*prJointRad++);
        *prSin++ = sin(*prJointRad);
        *prCos++ = cos(*prJointRad++);
        *prSin++ = sin(*prJointRad);
        *prCos++ = cos(*prJointRad++);
    }
}

void CMicroScribe::CalcJoints()
{
    ratio_t *prDegFactor = m_rgrJointDegreesFactor;
    ratio_t *prRadFactor = m_rgrJointRadiansFactor;
    angle_t *prJointDeg = m_rgrJointDeg;
    angle_t *prJointRad = m_rgrJointRad;
    LPDWORD pdwEncoder = (LPDWORD)hci.m_rgnEncoder;
    LPDWORD pdwEncoderMax = (LPDWORD)hci.m_rgnEncoderMax;
    DWORD dwHex;

    /* Update joints 0-2 if their encoders were reported last time */
    if (hci.m_rgnEncoderUpdated[2])
    {
        dwHex = *pdwEncoder++ & *pdwEncoderMax++;
        *prJointDeg++ = *prDegFactor++ * dwHex;
        *prJointRad++ = *prRadFactor++ * dwHex;
        dwHex = *pdwEncoder++ & *pdwEncoderMax++;
        *prJointDeg++ = *prDegFactor++ * dwHex;
        *prJointRad++ = *prRadFactor++ * dwHex;
        dwHex = *pdwEncoder++ & *pdwEncoderMax++;
        *prJointDeg++ = *prDegFactor++ * dwHex;
        *prJointRad++ = *prRadFactor++ * dwHex;
    }

    /* Update joints 3-5 if their encoders were reported last time */
    if (hci.m_rgnEncoderUpdated[5])
    {
        dwHex = *pdwEncoder++ & *pdwEncoderMax++;
        *prJointDeg++ = *prDegFactor++ * dwHex;
        *prJointRad++ = *prRadFactor++ * dwHex;
        dwHex = *pdwEncoder++ & *pdwEncoderMax++;
        *prJointDeg++ = *prDegFactor++ * dwHex;
        *prJointRad++ = *prRadFactor++ * dwHex;
        dwHex = *pdwEncoder++ & *pdwEncoderMax++;
        *prJointDeg++ = *prDegFactor++ * dwHex;
        *prJointRad++ = *prRadFactor++ * dwHex;
    }
}

void CMicroScribe::CalcAll()
{
    CalcStylus6DOF();
}

void CMicroScribe::CalcNothing()
{
    /* empty */
}

void CMicroScribe::Identity4x4(mat4x4_t M)
{
    M[0][3] = M[0][1] = M[0][2] = 0.0f; M[0][0] = 1.0f;
    M[1][0] = M[1][3] = M[1][2] = 0.0f; M[1][1] = 1.0f;
    M[2][0] = M[2][1] = M[2][3] = 0.0f; M[2][2] = 1.0f;
    M[3][0] = M[3][1] = M[3][2] = 0.0f; M[3][3] = 1.0f;
}

void CMicroScribe::Mul4x4(mat4x4_t M1, mat4x4_t M2, mat4x4_t X)
{
    X[0][0] = M1[0][0]*M2[0][0] + M1[0][1]*M2[1][0] + M1[0][2]*M2[2][0];
    X[0][1] = M1[0][0]*M2[0][1] + M1[0][1]*M2[1][1] + M1[0][2]*M2[2][1];
    X[0][2] = M1[0][0]*M2[0][2] + M1[0][1]*M2[1][2] + M1[0][2]*M2[2][2];
    X[0][3] = M1[0][0]*M2[0][3] + M1[0][1]*M2[1][3] + M1[0][2]*M2[2][3] + M1[0][3];
    X[1][0] = M1[1][0]*M2[0][0] + M1[1][1]*M2[1][0] + M1[1][2]*M2[2][0];
    X[1][1] = M1[1][0]*M2[0][1] + M1[1][1]*M2[1][1] + M1[1][2]*M2[2][1];
    X[1][2] = M1[1][0]*M2[0][2] + M1[1][1]*M2[1][2] + M1[1][2]*M2[2][2];
    X[1][3] = M1[1][0]*M2[0][3] + M1[1][1]*M2[1][3] + M1[1][2]*M2[2][3] + M1[1][3];
    X[2][0] = M1[2][0]*M2[0][0] + M1[2][1]*M2[1][0] + M1[2][2]*M2[2][0];
    X[2][1] = M1[2][0]*M2[0][1] + M1[2][1]*M2[1][1] + M1[2][2]*M2[2][1];
    X[2][2] = M1[2][0]*M2[0][2] + M1[2][1]*M2[1][2] + M1[2][2]*M2[2][2];
    X[2][3] = M1[2][0]*M2[0][3] + M1[2][1]*M2[1][3] + M1[2][2]*M2[2][3] + M1[2][3];
    X[3][0] = X[3][1] = X[3][2] = 0.0f;  X[3][3] = 1.0f;
}

void CMicroScribe::Assign4x4(mat4x4_t to, mat4x4_t from)
{
    to[0][0] = from[0][0]; to[0][1] = from[0][1]; to[0][2] = from[0][2]; to[0][3] = from[0][3];
    to[1][0] = from[1][0]; to[1][1] = from[1][1]; to[1][2] = from[1][2]; to[1][3] = from[1][3];
    to[2][0] = from[2][0]; to[2][1] = from[2][1]; to[2][2] = from[2][2]; to[2][3] = from[2][3];
    to[3][0] = from[3][0]; to[3][1] = from[3][1]; to[3][2] = from[3][2]; to[3][3] = from[3][3];
}

void CMicroScribe::CalcT()
{
    mat4x4_t M;

    Assign4x4(M, m_rgM[0]);

    m_rgEndpoint->x = M[0][3];
    m_rgEndpoint->y = M[1][3];
    m_rgEndpoint->z = M[2][3];

    for(int i = 1; i < NUM_DOF; ++i)
    {
        Mul4x4(M, m_rgM[i], m_T);
        Assign4x4(M, m_T);

        m_rgEndpoint[i].x = M[0][3];
        m_rgEndpoint[i].y = M[1][3];
        m_rgEndpoint[i].z = M[2][3];
    }
}

void CMicroScribe::CalcM()
{
    ratio_t rCos, rSin, rCosALPHA, rSinALPHA;
    mat4x4_t *pMi;

    for (int i = 0; i < NUM_DOF; ++i)
    {
        rCos = m_rgrCos[i];
        rSin = m_rgrSin[i];
        rCosALPHA = m_rgrCosALPHA[i];
        rSinALPHA = m_rgrSinALPHA[i];
        pMi = m_rgM + i;

        if (i != 2)
        {
            (*pMi)[0][0] = rCos;
            (*pMi)[0][1] = -rSin;
            (*pMi)[0][2] = 0.0f;
            (*pMi)[0][3] = m_rgrA[i];

            (*pMi)[1][0] = rSin * rCosALPHA;
            (*pMi)[1][1] = rCos * rCosALPHA;
            (*pMi)[1][2] = -rSinALPHA;
            (*pMi)[1][3] = -rSinALPHA * m_rgrD[i];

            (*pMi)[2][0] = rSin * rSinALPHA;
            (*pMi)[2][1] = rCos * rSinALPHA;
            (*pMi)[2][2] = rCosALPHA;
            (*pMi)[2][3] = rCosALPHA * m_rgrD[i];
        }
        else
        {
            ratio_t rCosBETA, rSinBETA;

            rCosBETA = cos(m_rBETA);
            rSinBETA = sin(m_rBETA);

            (*pMi)[0][0] = rCos * rCosBETA;
            (*pMi)[0][1] = -rSin * rCosBETA;
            (*pMi)[0][2] = rSinBETA;
            (*pMi)[0][3] = (rSinBETA * m_rgrD[i]) + m_rgrA[i];

            (*pMi)[1][0] = (rSin * rCosALPHA) + (rSinALPHA *rSinBETA * rCos);
            (*pMi)[1][1] = (rCos * rCosALPHA) - (rSinALPHA *rSinBETA * rSin);
            (*pMi)[1][2] = -rSinALPHA * rCosBETA;
            (*pMi)[1][3] = -rSinALPHA * rCosBETA * m_rgrD[i];

            (*pMi)[2][0] = (rSin * rSinALPHA) - (rCosALPHA * rSinBETA * rCos);
            (*pMi)[2][1] = (rCos * rSinALPHA) + (rSin * rSinBETA * rCosALPHA);
            (*pMi)[2][2] = rCosALPHA * rCosBETA;
            (*pMi)[2][3] = rCosBETA * rCosALPHA * m_rgrD[i];
        }

        (*pMi)[3][0] = 0.0f;
        (*pMi)[3][1] = 0.0f;
        (*pMi)[3][2] = 0.0f;
        (*pMi)[3][3] = 1.0f;
    }
}

void CMicroScribe::CalcStylusDir()
{
    if (m_angleFormat == XYZ_FIXED || m_angleFormat == ZYX_EULER)
    {
        m_stylusOrientation.x = atan2(m_T[2][1], m_T[2][2]);
        m_stylusOrientation.y = atan2(-m_T[2][0], sqrt(m_T[0][0]*m_T[0][0] + m_T[1][0]*m_T[1][0]));
        m_stylusOrientation.z = atan2(m_T[1][0], m_T[0][0]);
    }
    else if (m_angleFormat == YXZ_FIXED || m_angleFormat == ZXY_EULER)
    {
        m_stylusOrientation.x = atan2(-m_T[1][2], sqrt(m_T[0][2]*m_T[0][2] + m_T[2][2]*m_T[2][2]));
        m_stylusOrientation.y = atan2(m_T[0][2], m_T[2][2]);
        m_stylusOrientation.z = atan2(m_T[1][0], m_T[1][1]);
    }
    else if (m_angleFormat == ZYX_FIXED || m_angleFormat == XYZ_EULER)
    {
        m_stylusOrientation.x = atan2(-m_T[1][2], m_T[2][2]);
        m_stylusOrientation.y = atan2(m_T[0][2], sqrt(m_T[1][2]*m_T[1][2] + m_T[2][2]*m_T[2][2]));
        m_stylusOrientation.z = atan2(-m_T[0][1], m_T[0][0]);
    }

    if (m_angleUnits == DEGREES)
    {
        m_stylusOrientation.x *= 180.0f / MSCRIBE_PI;
        m_stylusOrientation.y *= 180.0f / MSCRIBE_PI;
        m_stylusOrientation.z *= 180.0f / MSCRIBE_PI;
    }
}

CMicroScribe::status_t CMicroScribe::HomePos()
{
    return hci.GoHomePos();
}

CMicroScribe::status_t CMicroScribe::ConvertParams()
{
    int converted = 0;

    if (!_stricmp(hci.m_szParamFormat, "Format DH0.5"))
        converted = ParamsDH05();

    if (converted)
        CalcParams();

    return converted ? CHCI::S_SUCCESS : CHCI::S_BAD_FORMAT;
}

CMicroScribe::status_t CMicroScribe::ConvertExtParams()
{
    BYTE *pbyExtParamBlock = m_rgbyExtParamBlock;

    if (strstr(hci.m_szComment, "Beta") && m_cbExtParamBlock >= 2)
    {
        int temp = (char)pbyExtParamBlock[0] * 256 + pbyExtParamBlock[1];
        m_rBETA = (temp / 32768.0f) * MSCRIBE_PI;
    }

    return CHCI::S_SUCCESS;
}

int CMicroScribe::ParamsDH05()
{
    BYTE *pbyParamBlock = m_rgbyParamBlock;
    float rUnitFactor = (m_lengthUnits == INCHES ? 1.0f : 25.4f);
    int temp;

    if (m_cbParamBlock != 36)
        return 0;

    temp = (char)pbyParamBlock[0] * 256 + pbyParamBlock[1];
    m_rgrALPHA[0] = (temp / 32768.0f) * MSCRIBE_PI;
    temp = (char)pbyParamBlock[2] * 256 + pbyParamBlock[3];
    m_rgrALPHA[1] = (temp / 32768.0f) * MSCRIBE_PI;
    temp = (char)pbyParamBlock[4] * 256 + pbyParamBlock[5];
    m_rgrALPHA[2] = (temp / 32768.0f) * MSCRIBE_PI;
    temp = (char)pbyParamBlock[6] * 256 + pbyParamBlock[7];
    m_rgrALPHA[3] = (temp / 32768.0f) * MSCRIBE_PI;
    temp = (char)pbyParamBlock[8] * 256 + pbyParamBlock[9];
    m_rgrALPHA[4] = (temp / 32768.0f) * MSCRIBE_PI;
    temp = (char)pbyParamBlock[10] * 256 + pbyParamBlock[11];
    m_rgrALPHA[5] = (temp / 32768.0f) * MSCRIBE_PI;

    temp = (char)pbyParamBlock[12] * 256 + pbyParamBlock[13];
    m_rgrA[0] = (temp / 1000.0f) * rUnitFactor;
    temp = (char)pbyParamBlock[14] * 256 + pbyParamBlock[15];
    m_rgrA[1] = (temp / 1000.0f) * rUnitFactor;
    temp = (char)pbyParamBlock[16] * 256 + pbyParamBlock[17];
    m_rgrA[2] = (temp / 1000.0f) * rUnitFactor;
    temp = (char)pbyParamBlock[18] * 256 + pbyParamBlock[19];
    m_rgrA[3] = (temp / 1000.0f) * rUnitFactor;
    temp = (char)pbyParamBlock[20] * 256 + pbyParamBlock[21];
    m_rgrA[4] = (temp / 1000.0f) * rUnitFactor;
    temp = (char)pbyParamBlock[22] * 256 + pbyParamBlock[23];
    m_rgrA[5] = (temp / 1000.0f) * rUnitFactor;

    temp = (char)pbyParamBlock[24] * 256 + pbyParamBlock[25];
    m_rgrD[0] = (temp / 1000.0f) * rUnitFactor;
    temp = (char)pbyParamBlock[26] * 256 + pbyParamBlock[27];
    m_rgrD[1] = (temp / 1000.0f) * rUnitFactor;
    temp = (char)pbyParamBlock[28] * 256 + pbyParamBlock[29];
    m_rgrD[2] = (temp / 1000.0f) * rUnitFactor;
    temp = (char)pbyParamBlock[30] * 256 + pbyParamBlock[31];
    m_rgrD[3] = (temp / 1000.0f) * rUnitFactor;
    temp = (char)pbyParamBlock[32] * 256 + pbyParamBlock[33];
    m_rgrD[4] = (temp / 1000.0f) * rUnitFactor;
    temp = (char)pbyParamBlock[34] * 256 + pbyParamBlock[35];
    m_rgrD[5] = (temp / 1000.0f) * rUnitFactor;

    return 1;
}

void CMicroScribe::CalcParams()
{
    for(int i = 0; i < NUM_DOF; ++i)
    {
        m_rgrCosALPHA[i] = cos(m_rgrALPHA[i]);
        m_rgrSinALPHA[i] = sin(m_rgrALPHA[i]);
    }
}

void CMicroScribe::SetupDefaultCallbacks()
{
    hci.m_cbTIMEDOUT = cbTIMEDOUT;
    hci.m_cbBADPORT = cbBADPORT;
    hci.m_cbBADPACKET = cbBADPACKET;
    hci.m_cbNOHCI = cbNOHCI;
    hci.m_cbCANTBEGIN = cbCANTBEGIN;
    hci.m_cbCANTOPEN = cbCANTOPENPORT;
}

CMicroScribe::status_t CMicroScribe::cbTIMEDOUT(CHCI& hci, status_t condition)
{
    fprintf(stdout, "\n%s: port COM%d, %d baud\n", condition, hci.m_nPort, hci.m_nBaudRate);
    hci.ResetCom();
    return condition;
}

CMicroScribe::status_t CMicroScribe::cbBADPORT(CHCI& hci, status_t condition)
{
    char szBuffer[4] = "";

    fprintf(stdout, "\n%s: port COM%d, %d baud\n", condition, hci.m_nPort, hci.m_nBaudRate);
    fprintf(stdout, "   Type 'p' to try a different PORT,\n");
    fprintf(stdout, "   Type any other key to ABORT.\n");
    fscanf_s(stdin, "%s", szBuffer, sizeof(szBuffer));

    if (*szBuffer == 'p' || *szBuffer == 'P')
    {
        fprintf(stdout, "Type a new port to try -> ");
        fscanf_s(stdin, "%d", &hci.m_nPort);
        condition = CHCI::S_TRY_AGAIN;
    }

    return condition;
}

CMicroScribe::status_t CMicroScribe::cbBADPACKET(CHCI& hci, status_t condition)
{
    char szBuffer[4] = "";

    fprintf(stdout, "\n%s: port COM%d, %d baud\n", condition, hci.m_nPort, hci.m_nBaudRate);
    fprintf(stdout, "   Type 'f' to FLUSH host serial buffer.\n");
    fprintf(stdout, "   Type any other key to ABORT.\n");
    fscanf_s(stdin, "%s", szBuffer, sizeof(szBuffer));

    if (*szBuffer == 'f' || *szBuffer == 'F')
        hci.EndMotion();

    return condition;
}

CMicroScribe::status_t CMicroScribe::cbNOHCI(CHCI& hci, status_t condition)
{
    char szBuffer[4] = "";
    status_t result;

    fprintf(stdout, "\n%s: port COM%d, %d baud\n", condition, hci.m_nPort, hci.m_nBaudRate);
    fprintf(stdout, "Check that the electronics module is plugged in and turned on.\n");
    fprintf(stdout, "   Type 'c' to change baud rate or port number and RETRY,\n");
    fprintf(stdout, "   Type 'a' to ABORT,\n");
    fprintf(stdout, "   Type any other key to retry at same baud rate on same port.\n");
    fscanf_s(stdin, "%s", szBuffer, sizeof(szBuffer));

    switch (*szBuffer)
    {
    case 'a':
    case 'A':
        result = CHCI::S_NO_HCI;
        break;
    case 'c':
    case 'C':
        hci.Disconnect();
        fprintf(stdout, "Type the new port -> ");
        fscanf_s(stdin, "%d", &hci.m_nPort);
        fprintf(stdout, "Type the new baud rate -> ");
        fscanf_s(stdin, "%d", &hci.m_nBaudRate);
    default:
        result = CHCI::S_TRY_AGAIN;
        fprintf(stdout, "Retrying...\n");
    }

    return result;
}

CMicroScribe::status_t CMicroScribe::cbCANTBEGIN(CHCI& hci, status_t condition)
{
    char szBuffer[4] = "";
    status_t result;

    fprintf(stdout, "\n%s: port COM%d, %d baud\n", condition, hci.m_nPort, hci.m_nBaudRate);
    fprintf(stdout, "You must reset the electronics module and check all connections.\n");
    fprintf(stdout, "   Type 'a' to ABORT,\n");
    fprintf(stdout, "   Type any other key to restart.\n");
    fscanf_s(stdin, "%s", szBuffer, sizeof(szBuffer));

    switch (*szBuffer)
    {
    case 'a':
    case 'A':
        result = CHCI::S_CANT_BEGIN;
        break;
    default:
        result = CHCI::S_TRY_AGAIN;
        hci.Disconnect();
        fprintf(stdout, "Retrying...\n");
    }

    return result;
}

CMicroScribe::status_t CMicroScribe::cbCANTOPENPORT(CHCI& hci, status_t condition)
{
    char szBuffer[4] = "";
    status_t result;

    fprintf(stdout, "\n%s: port COM%d, %d baud\n", condition, hci.m_nPort, hci.m_nBaudRate);
    fprintf(stdout, "Check communications parameters and host hardware.\n");
    fprintf(stdout, "   Type 'c' to change baud rate or port number and RETRY,\n");
    fprintf(stdout, "   Type 'a' to ABORT,\n");
    fprintf(stdout, "   Type any other key to restart.\n");
    fscanf_s(stdin, "%s", szBuffer, sizeof(szBuffer));

    switch (*szBuffer)
    {
    case 'a':
    case 'A':
        result = CHCI::S_CANT_OPEN_PORT;
        break;
    case 'c':
    case 'C':
        hci.Disconnect();
        fprintf(stdout, "Type the new port -> ");
        fscanf_s(stdin, "%d", &hci.m_nPort);
        fprintf(stdout, "Type the new baud rate -> ");
        fscanf_s(stdin, "%d", &hci.m_nBaudRate);
    default:
        result = CHCI::S_TRY_AGAIN;
        fprintf(stdout, "Retrying...\n");
    }

    return result;
}

void CMicroScribe::MotionStart(int nNumEncoder, int nMotionThreshold, int nPacketDelay, int nActiveButtons)
{
    int rgnAnalogDeltas[CHCI::NUM_ANALOGS], rgnEncoderDeltas[CHCI::NUM_ENCODERS];

    for (int i = 0; i < sizeof(rgnAnalogDeltas) / sizeof(*rgnAnalogDeltas); ++i)
        rgnAnalogDeltas[i] = 0;

    for (int i = 0; i < sizeof(rgnEncoderDeltas) / sizeof(*rgnEncoderDeltas); ++i)
        rgnEncoderDeltas[i] = nMotionThreshold;

    hci.ReportMotion(m_nTimerReport, m_nAnalogReports, nNumEncoder, nPacketDelay, nActiveButtons, rgnAnalogDeltas, rgnEncoderDeltas);
}

CMicroScribe::status_t CMicroScribe::GetConstants()
{
    status_t result;

    result = hci.GetStrings();
    if (result == CHCI::S_SUCCESS) result = hci.GetMaxes();
    if (result == CHCI::S_SUCCESS) result = hci.GetParams(m_rgbyParamBlock, &m_cbParamBlock);
    if (result == CHCI::S_SUCCESS && strstr(hci.m_szComment, "Beta") != NULL)
        result = hci.GetExtParams(m_rgbyExtParamBlock, &m_cbExtParamBlock);
    if (result == CHCI::S_SUCCESS) result = ConvertParams();
    if (result == CHCI::S_SUCCESS && strstr(hci.m_szComment, "Beta") != NULL)
        result = ConvertExtParams();
    if (result == CHCI::S_SUCCESS)
    {
        m_rgrJointRadiansFactor[0] = 2.0f * MSCRIBE_PI / (hci.m_rgnEncoderMax[0] + 1);
        m_rgrJointRadiansFactor[1] = 2.0f * MSCRIBE_PI / (hci.m_rgnEncoderMax[1] + 1);
        m_rgrJointRadiansFactor[2] = 2.0f * MSCRIBE_PI / (hci.m_rgnEncoderMax[2] + 1);
        m_rgrJointRadiansFactor[3] = 2.0f * MSCRIBE_PI / (hci.m_rgnEncoderMax[3] + 1);
        m_rgrJointRadiansFactor[4] = 2.0f * MSCRIBE_PI / (hci.m_rgnEncoderMax[4] + 1);
        m_rgrJointRadiansFactor[5] = 2.0f * MSCRIBE_PI / (hci.m_rgnEncoderMax[5] + 1);

        m_rgrJointDegreesFactor[0] = 360.0f / (hci.m_rgnEncoderMax[0] + 1);
        m_rgrJointDegreesFactor[1] = 360.0f / (hci.m_rgnEncoderMax[1] + 1);
        m_rgrJointDegreesFactor[2] = 360.0f / (hci.m_rgnEncoderMax[2] + 1);
        m_rgrJointDegreesFactor[3] = 360.0f / (hci.m_rgnEncoderMax[3] + 1);
        m_rgrJointDegreesFactor[4] = 360.0f / (hci.m_rgnEncoderMax[4] + 1);
        m_rgrJointDegreesFactor[5] = 360.0f / (hci.m_rgnEncoderMax[5] + 1);
    }

    return result;
}
