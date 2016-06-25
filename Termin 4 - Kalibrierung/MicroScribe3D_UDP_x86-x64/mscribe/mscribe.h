#ifndef MSCRIBE_H_INCLUDED
#define MSCRIBE_H_INCLUDED

#include <list>
#include <string>
#include <ostream>
#include <windows.h>
#include <setupapi.h>

#pragma comment(lib, "setupapi.lib")

class CCommDevice
{
public:
    CCommDevice();
    bool operator<(const CCommDevice& other) const { return m_nPort < other.m_nPort; }

    int m_nPort;
    std::string m_strPort, m_strFile, m_strDeviceDesc, m_strMfg, m_strInstanceId;
    COMMPROP m_cpPort;

    friend std::ostream& operator<<(std::ostream& os, const CCommDevice& cd);
};

typedef std::list<CCommDevice> CCommDeviceList;

bool CommEnumerate(CCommDeviceList&, bool bCommProperties = true);

class CCommPort
{
private:
    static const DWORD INQUE_MAX, OUTQUE_MAX;

    HANDLE m_hPort;
    DCB m_dcb;
    DWORD m_dwTimeoutTicks, m_dwStopTick;

public:
    CCommPort();
    virtual ~CCommPort();

    bool Open(int nPort, int nBaudRate);
    void Close();
    void Flush();
    bool IsOpen() const;

    void Pause(float rDelay) const;
    float GetTimeout() const;
    void SetTimeout(float rTimeout);
    void StartTimeout();
    bool HasTimedOut() const;

    int ReadChar();
    DWORD ReadString(LPSTR pchBuffer, DWORD cchBuffer, float rTimeout);
    bool WriteChar(int chData);
    bool WriteString(LPCSTR szData);

    DWORD GetInputCount() const;
    bool IsInputFull() const;
};

class CHCI
{
public:
    enum
    {
        CHECK_SYNC   = 1,
        CHECK_ASYNC  = 2,
        CHECK_MOTION = 3
    };

    enum
    {
        NUM_ENCODERS = 7,
        NUM_ANALOGS  = 8,
        NUM_BUTTONS  = 7
    };

    enum
    {
        MAX_PACKET_SIZE = 42,
        MAX_CFG_SIZE    = 40,
        MAX_STRING_SIZE = 32,
        TIMEOUT_CHARS   = 2 * MAX_PACKET_SIZE
    };

    /* Labels for the Special Configuration Commands */
    enum
    {
        CONFIG_MIN      = 0xC0,
        GET_PARAMS      = 0xC0,
        GET_HOME_REF    = 0xC1,
        HOME_POS        = 0xC2,
        SET_HOME        = 0xC3,
        SET_BAUD        = 0xC4,
        END_SESSION     = 0xC5,
        GET_MAXES       = 0xC6,
        SET_PARAMS      = 0xC7,
        GET_PROD_NAME   = 0xC8,
        GET_PROD_ID     = 0xC9,
        GET_MODEL_NAME  = 0xCA,
        GET_SERNUM      = 0xCB,
        GET_COMMENT     = 0xCC,
        GET_PRM_FORMAT  = 0xCD,
        GET_VERSION     = 0xCE,
        REPORT_MOTION   = 0xCF,
        SET_HOME_REF    = 0xD0,
        RESTORE_FACTORY = 0xD1,
        INSERT_MARKER   = 0xD2,
        GET_EXT_PARAMS  = 0xD3,
        PASSWD_OK       = 0xFF
    };

    /* Command bit-field place values */
    enum
    {
        PACKET_MARKER  = 0x80,
        CONFIG_BIT     = 0x40,
        TIMER_BIT      = 0x20,
        FUTURE_BIT     = 0x10,
        ANALOG_BITS    = 0x0C,
        ANALOG_HI_BIT  = 0x08,
        ANALOG_LO_BIT  = 0x04,
        ENCODER_BITS   = 0x03,
        ENCODER_HI_BIT = 0x02,
        ENCODER_LO_BIT = 0x01
    };

    typedef LPCSTR status_t;
    typedef status_t (*CallbackHandler_t)(CHCI& hci, status_t condition);

    typedef struct m_packet
    {
        BOOL m_bParsed;                /* Flag tells whether this packet has been parsed */
        BOOL m_bError;                 /* Flag tells whether there has been com error */
        int m_cbNeeded;
        BYTE m_byCommand;
        BYTE m_rgbyPacket[MAX_PACKET_SIZE];
        BYTE* m_pbyPacket;
    } packet_t;

    static LPCSTR S_BAD_PORT_NUM;      /* Port number not valid */
    static LPCSTR S_CANT_OPEN_PORT;    /* Can't open the port during start-up */
    static LPCSTR S_NO_HCI;            /* No response from HCI during start-up */
    static LPCSTR S_CANT_BEGIN;        /* No response to "BEGIN" at start of session */
    static LPCSTR S_NO_PACKET_YET;     /* Complete packet not yet recv'd */
    static LPCSTR S_TIMED_OUT;         /* Didn't get complete packet in time */
    static LPCSTR S_BAD_PACKET;        /* Received packet that did not make sense */
    static LPCSTR S_BAD_PASSWORD;      /* Cfg cmd requiring passwd was denied */
    static LPCSTR S_SUCCESS;           /* Successful operation */
    static LPCSTR S_TRY_AGAIN;         /* Try the operation again */
    static LPCSTR S_BAD_VERSION;      /* Feature isn't supported by firmware */
    static LPCSTR S_BAD_FORMAT;       /* Parameter block is in unreadable format */

    static LPCSTR STR_SIGNON;
    static LPCSTR STR_BEGIN;

    static const float WAIT_SIGNON;
    static const float WAIT_END;
    static const float WAIT_CFG_ARG;
    static const float WAIT_RESTORE;
    static const float MIN_TIMEOUT;

    CCommPort commport;
    int       m_nPort;            /* This HCI's serial port number */
    int       m_nBaudRate;        /* This Probe's baud rate */
    float     m_rSlowTimeout;     /* Timeout period for slow process */
    float     m_rFastTimeout;     /* Timeout period for fast process */
    packet_t  m_packet;           /* The current packet */
    int       m_nPacketsExpected; /* Determines whether timeout is important */
    int       m_nMarker;
    BOOL      m_bMarkerUpdated;

    /* Primary quantities */
    int  m_nButtons;                 /* button bits all together */
    int  m_rgnButton[NUM_BUTTONS];   /* ON/OFF flags for buttons */
    int  m_nTimer;                   /* Running counter */
    int  m_rgnAnalog[NUM_ANALOGS];   /* A/D channels */
    int  m_rgnEncoder[NUM_ENCODERS]; /* Encoder counts */

    /* Normalization values for primary quantities */
    int  m_rgnButtonSupported[NUM_BUTTONS]; /* zero = button not supported */
    int  m_nTimerMax;                       /* Max count reached before wrapping */
    int  m_rgnAnalogMax[NUM_ANALOGS];       /* Full-scale A/D reading */
    int  m_rgnEncoderMax[NUM_ENCODERS];     /* Max value each encoder reaches INCLUDING quadrature */

    /* Status of primary fields */
    int  m_nTimerUpdated;
    int  m_rgnAnalogUpdated[NUM_ANALOGS];
    int  m_rgnEncoderUpdated[NUM_ENCODERS];

    /* Encoder "home" position & references */
    int  m_rgnHomePos[NUM_ENCODERS];
    int  m_rgnHomeRef[NUM_ENCODERS];

    /* Arguments to config commands */
    BYTE m_rgbyConfigArguments[MAX_CFG_SIZE];
    int  m_nConfigArguments;

    /* Descriptor strings */
    char m_szSerialNumber[MAX_STRING_SIZE];
    char m_szProductName[MAX_STRING_SIZE];
    char m_szProductId[MAX_STRING_SIZE];
    char m_szModelName[MAX_STRING_SIZE];
    char m_szComment[MAX_STRING_SIZE];
    char m_szParamFormat[MAX_STRING_SIZE];
    char m_szVersion[MAX_STRING_SIZE];

    /* Handlers for errors */
    CallbackHandler_t m_cbBADPORT;
    CallbackHandler_t m_cbCANTOPEN;
    CallbackHandler_t m_cbNOHCI;
    CallbackHandler_t m_cbCANTBEGIN;
    CallbackHandler_t m_cbTIMEDOUT;
    CallbackHandler_t m_cbBADPACKET;
    CallbackHandler_t m_cbBADPASSWORD;
    CallbackHandler_t m_cbBADVERSION;
    CallbackHandler_t m_cbBADFORMAT;
    CallbackHandler_t m_cbDEFAULT;

    /* Extra field available for user's application-specific purpose */
    LPVOID m_pvUserData;

    void Init(int nPort, int nBaudRate);
    status_t Connect();
    void Disconnect();
    status_t GetStrings();
    void ChangeBaud(int nNewBaudRate);
    void ResetCom();
    status_t Autosynch();
    status_t Begin();
    void End();
    void ClearPacket();
    void ComParams(int nPort, int nBaudRate);
    void FastTimeout();
    void SlowTimeout();

    BYTE CmdByte(int nTimerFlag, int nAnalogReports, int nEncoderReports) const;
    void StdCmd(int nTimerFlag, int nAnalogReports, int nEncoderReports);
    void SimpleCfgCmd(BYTE byCommand);
    status_t StringCmd(BYTE byCommand);
    status_t PasswdCmd(BYTE byCommand);
    void InsertMarker(BYTE byMarker);
    status_t GetParams(BYTE* pbyBlock, int* pcbBlockSize);
    status_t GetExtParams(BYTE* pbyBlock, int* pcbBlockSize);
    status_t SetParams(BYTE* pbyBlock, int cbBlockSize);
    status_t GetHomeRef();
    status_t SetHomeRef(int* pnHomeRef);
    status_t GoHomePos();
    status_t SetHomePos(int* pnHomePos);
    status_t GetMaxes();
    status_t FactorySettings();
    void ReportMotion(int nTimerFlag, int nAnalogReports, int nEncoderReports, int nDelay, BYTE byActiveButtons, int* pnAnalogDeltas, int* pnEncoderDeltas);
    void EndMotion();

    float VersionNum();

    status_t WaitPacket();
    status_t CheckPacket(int nCheckType);
    status_t CheckMotion();
    status_t BuildPacket(int nCheckType);

    status_t ParsePacket();
    status_t ParseCfgPacket();
    int PacketSize(int nCommand);

    status_t ReadString(LPSTR pchData);
    status_t ReadBlock(BYTE* pbyBlock, int* pcbBlock);
    void InvalidateFields();

    status_t Error(status_t condition);
    status_t SimpleString(status_t condition);

    BYTE Baud2Code(int nBaudRate);
    int Code2Baud(BYTE byCode);
    void AdjustBaudRate(int* pnBaudRate);
};

class CMicroScribe
{
public:
    enum
    {
        NUM_DOF              = 6,
        QUICK_3DOF_POINTS    = 4,
        STD_3DOF_POINTS      = 4,
        NUM_TABLE_POS        = 4,
        PARAM_BLOCK_SIZE     = 40,
        EXT_PARAM_BLOCK_SIZE = 10
    };

    enum
    {
        RIGHT_PEDAL = 1,
        LEFT_PEDAL  = 2,
        BOTH_PEDALS = 3
    };

    enum
    {
        BASE     = 0,
        SHOULDER = 1,
        ELBOW    = 2,
        FOREARM  = 3,
        WRIST    = 4,
        STYLUS   = 5
    };

    typedef CHCI::status_t status_t;

    typedef float length_t;
    typedef float angle_t;
    typedef float ratio_t;
    typedef float mat4x4_t[4][4];

    typedef LPCSTR length_units_t;
    typedef LPCSTR angle_units_t;
    typedef LPCSTR angle_format_t;

    typedef struct position { length_t x, y, z; } position_t;
    typedef struct axis_angle { angle_t x, y, z; } axis_angle_t;

    /* Constants for length_units variables */
    static LPCSTR INCHES;
    static LPCSTR MM;

    /* Constants for angle_units variables */
    static LPCSTR DEGREES;
    static LPCSTR RADIANS;

    /* Constants for angle_format variables */
    static LPCSTR XYZ_FIXED;
    static LPCSTR ZYX_FIXED;
    static LPCSTR YXZ_FIXED;
    static LPCSTR ZYX_EULER;
    static LPCSTR XYZ_EULER;
    static LPCSTR ZXY_EULER;

    CHCI hci;

    /* Fundamental 6DOF quantities */
    position_t   m_stylusPosition;
    axis_angle_t m_stylusOrientation;

    /* Transformation matrix representing stylus 6DOF coordinates */
    mat4x4_t m_T;

    /* Series of linkage endpoints */
    position_t m_rgEndpoint[NUM_DOF];

    /* Joint angles */
    angle_t m_rgrJointRad[NUM_DOF];
    angle_t m_rgrJointDeg[NUM_DOF];

    /* Unit conversion and data format specifiers (READ-ONLY) */
    length_units_t m_lengthUnits; /* inches/mm for xyz coordinates */
    angle_units_t  m_angleUnits;  /* radians/degrees for stylus angles */
    angle_format_t m_angleFormat; /* xyz_fixed/zyx_fixed ... for stylus angles */

    /* Fields describing internal physical structure */
    length_t m_rgrD[NUM_DOF];     /* Offsets between joint axes */
    length_t m_rgrA[NUM_DOF];     /* Offsets between joint axes */
    angle_t  m_rgrALPHA[NUM_DOF]; /* Skew angles between joint axes */
    angle_t  m_rBETA;             /* Beta angle in T23 */
    float    m_rPt2PtDistance;    /* Distance between points in AutoPlotPoint() */

    /* Pre-computed conversion factors */
    ratio_t  m_rgrJointRadiansFactor[NUM_DOF];
    ratio_t  m_rgrJointDegreesFactor[NUM_DOF];

    /* Intermediate matrix products */
    mat4x4_t m_rgM[NUM_DOF];

    /* Trigonometric quantities: */
    ratio_t  m_rgrCos[NUM_DOF];
    ratio_t  m_rgrSin[NUM_DOF];
    ratio_t  m_rgrCosALPHA[NUM_DOF];
    ratio_t  m_rgrSinALPHA[NUM_DOF];

    /* Fields requested in subsequent reports */
    int m_nTimerReport;
    int m_nAnalogReports;

    /* Number of points needed in next endpoint calculation */
    int m_nNumPoints;

    /* Calculation function to execute after getting next packet */
    void (CMicroScribe::*m_pfnPacketCalculation)();

    BYTE m_rgbyParamBlock[PARAM_BLOCK_SIZE];
    BYTE m_rgbyExtParamBlock[EXT_PARAM_BLOCK_SIZE];
    int m_cbParamBlock;
    int m_cbExtParamBlock;

    void Init();

    status_t Connect();
    status_t Connect(int nPort, int nBaudRate);
    void Disconnect();
    void ChangeBaud(int nNewBaudRate);

    int GetPoint();
    int AutoPlotPoint(float rDistanceSetting);

    status_t SyncStylus6DOF();
    status_t SyncStylus3DOF();
    status_t Sync3Joint();
    status_t Sync6Joint();
    status_t SyncAll();

    status_t AsyncCheck();
    void AsyncStylus6DOF();
    void AsyncStylus3DOF();
    void Async3Joint();
    void Async6Joint();
    void AsyncAll();

    status_t MotionCheck();
    void MotionEnd();
    void MotionStylus6DOF(int nMotionThreshold, int nPacketDelay, int nActiveButtons);
    void MotionStylus3DOF(int nMotionThreshold, int nPacketDelay, int nActiveButtons);
    void Motion3Joint(int nMotionThreshold, int nPacketDelay, int nActiveButtons);
    void Motion6Joint(int nMotionThreshold, int nPacketDelay, int nActiveButtons);
    void MotionAll(int nMotionThreshold, int nPacketDelay, int nActiveButtons);

    void LengthUnits(length_units_t units);
    void AngleUnits(angle_units_t units);
    void AngleFormat(angle_format_t format);

    void ReportTimer();
    void SkipTimer();
    void ReportAnalog(int nAnalogReports);
    void SkipAnalog();

    void CalcStylus6DOF();
    void CalcStylus3DOF();
    void CalcTrig();
    void CalcJoints();
    void CalcAll();
    void CalcNothing();

    void CalcT();
    void CalcM();
    void CalcStylusDir();
    void Mul4x4(mat4x4_t M1, mat4x4_t M2, mat4x4_t X);
    void Identity4x4(mat4x4_t M);
    void Assign4x4(mat4x4_t to, mat4x4_t from);

    status_t HomePos();
    status_t ConvertParams();
    status_t ConvertExtParams();
    int ParamsDH05();
    void CalcParams();

    static status_t cbNOHCI(CHCI& hci, status_t condition);
    static status_t cbCANTBEGIN(CHCI& hci, status_t condition);
    static status_t cbCANTOPENPORT(CHCI& hci, status_t condition);
    static status_t cbTIMEDOUT(CHCI& hci, status_t condition);
    static status_t cbBADPORT(CHCI& hci, status_t condition);
    static status_t cbBADPACKET(CHCI& hci, status_t condition);
    void SetupDefaultCallbacks();

    status_t GetConstants();
    void MotionStart(int nNumEncoder, int nMotionThreshold, int nPacketDelay, int nActiveButtons);
};

#endif
