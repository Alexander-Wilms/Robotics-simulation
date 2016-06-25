// UDPTermin4_1u2.cpp
// Praktikum Robotersimulation Kalibrierung
// MicroScribe3D/UDP Datenaufzeichnung fuer Termin 4, Aufgabe 1 und 2

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cmath>
#include <winsock2.h>

#define DATA_FNAME       "System%d.txt"
//#define MSCRIBE_SA_BC    inet_addr("192.168.0.202")
#define MSCRIBE_SA_BC    INADDR_ANY
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

BOOL VerifyPayload(mscribe_packet_t* packet);

#pragma comment(lib, "ws2_32.lib")

int main(int argc, const char* argv[])
{
    SOCKET sockfd;
    struct sockaddr_in saBC;
    int cbSockAddrBC;
    WSADATA wsa;
    DWORD dwReuseAddr = TRUE;
    mscribe_packet_t packet;
    fd_set fds;
    struct timeval timeout;
    FILE* fp;
    int nPoints = 0, nSystem = 1;
    char szFile[_MAX_PATH], szInfo[128];
    WORD wButtons = 0;
    errno_t err;

    if (WSAStartup(MAKEWORD(2, 2), &wsa))
    {
        fprintf(stderr, "*** WSAStartup() / %d\n", WSAGetLastError());
        return EXIT_FAILURE;
    }

    cbSockAddrBC = sizeof(saBC);
    ZeroMemory(&saBC, cbSockAddrBC);
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

    if ((setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (LPCSTR)&dwReuseAddr, sizeof(dwReuseAddr))) == SOCKET_ERROR)
    {
        fprintf(stderr, "*** setsockopt() / %d\n", WSAGetLastError());
        WSACleanup();
        return EXIT_FAILURE;
    }

    if ((bind(sockfd, (const sockaddr*)&saBC, cbSockAddrBC)) == SOCKET_ERROR)
    {
        fprintf(stderr, "*** bind() / %d\n", WSAGetLastError());
        WSACleanup();
        return EXIT_FAILURE;
    }

    fprintf(stdout, "---------------------------------------------------------------------\n");
    fprintf(stdout, "MicroScribe3D/UDP Datenaufzeichnung fuer Termin 4, Aufgabe 1 und 2.\n");
    fprintf(stdout, "Betaetigen Sie den Fusstaster um die aktuelle Position aufzuzeichnen.\n");
    fprintf(stdout, "Druecken Sie [ESC] um das Programm zu beenden.\n");
    fprintf(stdout, "---------------------------------------------------------------------\n");
    fprintf(stdout, "Warte an Port %u auf UDP Packete...\r", MSCRIBE_UDP_PORT);

    while (true)
    {
        timeout.tv_sec = 0;
        timeout.tv_usec = 100;

        FD_ZERO(&fds);
        FD_SET(sockfd, &fds);

        if (select(sizeof(fds)*8, &fds, NULL, NULL, &timeout) > 0
        &&  recvfrom(sockfd, (LPSTR)&packet, sizeof(packet), 0, (struct sockaddr*)&saBC, &cbSockAddrBC) > 0)
        {
            _snprintf_s(szFile, _TRUNCATE, DATA_FNAME, nSystem);

            if (!VerifyPayload(&packet))
            {
                fprintf(stdout, "%d@%s: X%+07.2f Y%+07.2f Z%+07.2f [ERROR]  \r", nPoints + 1, szFile,
                    packet.data.T[0][3], packet.data.T[1][3], packet.data.T[2][3]);
                continue;
            }

            fprintf(stdout, "%d@%s: X%+07.2f Y%+07.2f Z%+07.2f [%05.5d]  \r", nPoints + 1, szFile,
                packet.data.T[0][3], packet.data.T[1][3], packet.data.T[2][3], packet.data.wTimestamp);

            if (packet.data.wButtons == wButtons)
                continue;

            wButtons = packet.data.wButtons;
            if (!wButtons)
                continue;

            if (!nPoints)
            {
                err = fopen_s(&fp, szFile, "w");
                if (err)
                {
                    strerror_s(szInfo, err);
                    fprintf(stderr, "%s: %s\n", szFile, szInfo);
                    break;
                }
            }

            nPoints += 1;
            fputs("\n", stdout);
            fprintf(fp, "%.3f;%.3f;%.3f\n", packet.data.T[0][3], packet.data.T[1][3], packet.data.T[2][3]);

            if (nPoints == 3)
            {
                fprintf(stdout, "Der Datensatz %s wurde abgespeichert.\n", szFile);
                fclose(fp);
                nPoints = 0;
                nSystem += 1;
            }
        }

        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
            break;
    }

    if (nPoints)
    {
        fclose(fp);
        DeleteFile(szFile);
    }

    closesocket(sockfd);
    WSACleanup();

    fputs("\n\n", stdout);

    return EXIT_SUCCESS;
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
