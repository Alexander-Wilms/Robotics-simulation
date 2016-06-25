// UDPTermin4_3_Test.cpp
// Praktikum Robotersimulation Kalibrierung
// MicroScribe3D/UDP Testprogramm fuer Termin 4, Aufgabe 3

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cmath>
#include <winsock2.h>
#include <vectmath.h>

#define PI 3.14159265359
#define DEG2RAD (PI/180.0)
#define RAD2DEG (180.0/PI)

#define DATA_FNAME       "Kalibrierung.txt"
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

bool LoadCal(const char* szFileName, Position& Pt, Position& Ppivot);
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
    WORD wButtons = 0;
    Transform T;
    Euler_angle_xyz ea;
    Position Pt, Ppivot, p;

    if (!LoadCal(DATA_FNAME, Pt, Ppivot))
        return EXIT_FAILURE;

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
    fprintf(stdout, "MicroScribe3D/UDP Testprogramm fuer Termin 4, Aufgabe 3.\n");
    fprintf(stdout, "Druecken Sie [ESC] bzw. den Fusstaster um das Programm zu beenden.\n");
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
            for (int r = 0; r < 4; ++r)
                for (int c = 0; c < 4; ++c)
                    T[r][c] = packet.data.T[r][c];

            p = Position(T * Pt - Ppivot);
            ea = Euler_angle_xyz(T);

            if (!VerifyPayload(&packet))
            {
                fprintf(stdout, "X%+07.2f Y%+07.2f Z%+07.2f A%+07.2f B%+07.2f C%+07.2f [ERROR]  \r",
                    p.x(), p.y(), p.z(), ea.rx() * RAD2DEG, ea.ry() * RAD2DEG, ea.rz() * RAD2DEG);
            }

            fprintf(stdout, "X%+07.2f Y%+07.2f Z%+07.2f A%+07.2f B%+07.2f C%+07.2f [%05.5d]  \r",
                p.x(), p.y(), p.z(), ea.rx() * RAD2DEG, ea.ry() * RAD2DEG, ea.rz() * RAD2DEG, packet.data.wTimestamp);

            if (packet.data.wButtons)
                break;
        }

        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
            break;
    }

    closesocket(sockfd);
    WSACleanup();

    fputs("\n\n", stdout);

    return EXIT_SUCCESS;
}

bool LoadCal(const char* szFileName, Position& Pt, Position& Ppivot)
{
    FILE* fp;
    errno_t err;
    char szBuffer[_MAX_PATH];

    err = fopen_s(&fp, szFileName, "r");
    if (err)
    {
        strerror_s(szBuffer, err);
        fprintf(stderr, "%s: %s\n", szFileName, szBuffer);
        return false;
    }

    fprintf(stdout, "Lese Datensatz aus %s\n", szFileName);

    if (!fgets(szBuffer, _countof(szBuffer) - 1, fp)
    ||  sscanf_s(szBuffer, "%lf;%lf;%lf", &Pt.x(), &Pt.y(), &Pt.z()) != 3
    ||  !fgets(szBuffer, _countof(szBuffer) - 1, fp)
    ||  sscanf_s(szBuffer, "%lf;%lf;%lf", &Ppivot.x(), &Ppivot.y(), &Ppivot.z()) != 3)
    {
        fprintf(stderr, "%s: Ungueltiges Format!\n", szFileName);
        return false;
    }

    fclose(fp);

    return true;
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
