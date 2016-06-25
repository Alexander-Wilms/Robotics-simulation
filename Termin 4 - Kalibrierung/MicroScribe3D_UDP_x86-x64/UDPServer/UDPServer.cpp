// UDPServer.cpp
// MicroScribe3D/UDP Broadcast Server

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cmath>
#include <winsock2.h>
#include <mscribe.h>

#pragma comment(lib, "ws2_32.lib")

//#define MSCRIBE_SA_SRV   inet_addr("192.168.0.202")
#define MSCRIBE_SA_SRV   INADDR_ANY
//#define MSCRIBE_SA_BC    inet_addr("192.255.255.255")
#define MSCRIBE_SA_BC    INADDR_BROADCAST
#define MSCRIBE_UDP_PORT 8888

#pragma pack(push, 1)
typedef struct mscribe_payload
{
    WORD wTimestamp, wButtons;
    float T[4][4];
} mscibe_payload_t;

typedef struct mscribe_packet
{
    WORD wChecksum;
    struct mscribe_payload data;
} mscribe_packet_t;
#pragma pack(pop)

void ValidatePayload(mscribe_packet_t* packet);
BOOL VerifyPayload(mscribe_packet_t* packet);

int main(int argc, const char* argv[])
{
    SOCKET sockfd;
    struct sockaddr_in saSrv, saBC;
    CMicroScribe ms;
    CMicroScribe::status_t result;
    WSADATA wsa;
    DWORD dwBroadcast = TRUE;
    mscribe_packet_t packet;

    if (WSAStartup(MAKEWORD(2, 2), &wsa))
    {
        fprintf(stderr, "*** WSAStartup() / %d\n", WSAGetLastError());
        return EXIT_FAILURE;
    }

    ZeroMemory(&saSrv, sizeof(saSrv));
    saSrv.sin_family = AF_INET;
    saSrv.sin_port = 0;
    saSrv.sin_addr.s_addr = MSCRIBE_SA_SRV;

    ZeroMemory(&saBC, sizeof(saBC));
    saBC.sin_family = AF_INET;
    saBC.sin_port = htons(MSCRIBE_UDP_PORT);
    saBC.sin_addr.s_addr = MSCRIBE_SA_BC;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == INVALID_SOCKET)
    {
        fprintf(stderr, "*** socket() / %d\n", WSAGetLastError());
        WSACleanup();
        return EXIT_FAILURE;
    }

    if ((setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (LPCSTR)&dwBroadcast, sizeof(dwBroadcast))) == SOCKET_ERROR)
    {
        fprintf(stderr, "*** setsockopt() / %d\n", WSAGetLastError());
        WSACleanup();
        return EXIT_FAILURE;
    }

    if ((bind(sockfd, (const sockaddr*)&saSrv, sizeof(saSrv))) == SOCKET_ERROR)
    {
        fprintf(stderr, "*** bind() / %d\n", WSAGetLastError());
        WSACleanup();
        return EXIT_FAILURE;
    }

    ms.Init();
    ms.SetupDefaultCallbacks();
    ms.LengthUnits(CMicroScribe::MM);
    ms.ReportTimer();
    fprintf(stdout, "Suche MicroScribe3D...\n");
    //result = ms.Connect(2, 115200);
    result = ms.Connect();

    if (result != CHCI::S_SUCCESS)
    {
        fprintf(stderr, "Es konnte keine Verbindung zum MicroScribe3D Messarm hergstellt werden!\n");
        return EXIT_FAILURE;
    }

    fprintf(stdout, "---------------------------------------------------------------------\n");
    fprintf(stdout, "MicroScribe3D/UDP Broadcast Server.\n");
    fprintf(stdout, "Druecken Sie [END] um das Programm zu beenden.\n");
    fprintf(stdout, "---------------------------------------------------------------------\n");
    fprintf(stdout, "Produkt:          %s\n", ms.hci.m_szProductName);
    fprintf(stdout, "Bezeichnung:      %s\n", ms.hci.m_szProductId);
    fprintf(stdout, "Modell:           %s\n", ms.hci.m_szModelName);
    fprintf(stdout, "Version:          %s\n", ms.hci.m_szVersion);
    fprintf(stdout, "Seriennummer:     %s\n", ms.hci.m_szSerialNumber);
    fprintf(stdout, "Parameter:        %s\n", ms.hci.m_szParamFormat);
    fprintf(stdout, "Sonstiges:        %s\n", ms.hci.m_szComment);
    fprintf(stdout, "---------------------------------------------------------------------\n");

    result = ms.SyncStylus6DOF();
    ms.MotionStylus6DOF(5, 10, 0xF);

    while (result == CHCI::S_SUCCESS || result == CHCI::S_NO_PACKET_YET)
    {
        result = ms.MotionCheck();

        if (result == CHCI::S_SUCCESS)
        {
            fprintf(stdout, "[%05.5d] X%+07.2f Y%+07.2f Z%+07.2f  \r", ms.hci.m_nTimer, ms.m_stylusPosition.x, ms.m_stylusPosition.y, ms.m_stylusPosition.z);

            packet.data.wTimestamp = (WORD)ms.hci.m_nTimer;
            packet.data.wButtons = (WORD)ms.hci.m_nButtons;

            for (int r = 0; r < 4; ++r)
                for (int c = 0; c < 4; ++c)
                    packet.data.T[r][c] = ms.m_T[r][c];

            ValidatePayload(&packet);

            if (sendto(sockfd, (LPCSTR)&packet, sizeof(packet), 0, (struct sockaddr *)&saBC, sizeof(saBC)) == SOCKET_ERROR)
                break;
        }

        if (GetAsyncKeyState(VK_END) & 0x8000)
            break;
    }

    closesocket(sockfd);
    WSACleanup();

    ms.MotionEnd();
    ms.Disconnect();
    fputs("\n\n", stdout);

    return EXIT_SUCCESS;
}

void ValidatePayload(mscribe_packet_t* packet)
{
    BYTE *pPayload = (BYTE*)&packet->data;

    packet->wChecksum = 0xFFFF;

    for (size_t i = 0; i < sizeof(packet->data); ++i)
    {
        WORD x = ((packet->wChecksum >> 8) ^ *pPayload) & 0xFF;
        x ^= x >> 4;
        packet->wChecksum = (packet->wChecksum << 8) ^ (x << 12) ^ (x <<5) ^ x;
    }
}

BOOL VerifyPayload(mscribe_packet_t* packet)
{
    BYTE *pPayload = (BYTE*)&packet->data;
    WORD wChecksum = 0xFFFF;

    for (size_t i = 0; i < sizeof(packet->data); ++i)
    {
        WORD x = ((wChecksum >> 8) ^ *pPayload) & 0xFF;
        x ^= x >> 4;
        wChecksum = (wChecksum << 8) ^ (x << 12) ^ (x <<5) ^ x;
    }

    return wChecksum == packet->wChecksum;
}
