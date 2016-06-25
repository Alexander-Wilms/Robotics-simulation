// MSTermin4_3_Test.cpp
// Praktikum Robotersimulation Kalibrierung
// MicroScribe3D Testprogramm fuer Termin 4, Aufgabe 3

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cmath>
#include <windows.h>
#include <mscribe.h>
#include <vectmath.h>

#define PI 3.14159265359
#define DEG2RAD (PI/180.0)
#define RAD2DEG (180.0/PI)

#define DATA_FNAME "Kalibrierung.txt"

bool LoadCal(const char* szFileName, Position& Pt, Position& Ppivot);

int main(int argc, const char* argv[])
{
    CMicroScribe ms;
    CMicroScribe::status_t result;
    Transform T;
    Euler_angle_xyz ea;
    Position Pt, Ppivot, p;

    if (!LoadCal(DATA_FNAME, Pt, Ppivot))
        return EXIT_FAILURE;

    ms.Init();
    ms.SetupDefaultCallbacks();
    ms.LengthUnits(CMicroScribe::MM);
    fprintf(stdout, "Suche MicroScribe3D...\n");
    //result = ms.Connect(2, 115200);
    result = ms.Connect();

    if (result != CHCI::S_SUCCESS)
    {
        fprintf(stderr, "Es konnte keine Verbindung zum MicroScribe3D Messarm hergstellt werden!\n");
        return EXIT_FAILURE;
    }

    fprintf(stdout, "---------------------------------------------------------------------\n");
    fprintf(stdout, "MicroScribe3D Testprogramm fuer Termin 4, Aufgabe 3.\n");
    fprintf(stdout, "Druecken Sie [ESC] bzw. den Fusstaster um das Programm zu beenden.\n");
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
            for (int r = 0; r < 4; ++r)
                for (int c = 0; c < 4; ++c)
                    T[r][c] = ms.m_T[r][c];

            p = Position(T * Pt - Ppivot);
            ea = Euler_angle_xyz(T);

            fprintf(stdout, "X%+07.2f Y%+07.2f Z%+07.2f A%+07.2f B%+07.2f C%+07.2f  \r",
                p.x(), p.y(), p.z(), ea.rx() * RAD2DEG, ea.ry() * RAD2DEG, ea.rz() * RAD2DEG);

            if (ms.hci.m_nButtons)
                break;
        }

        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
            break;
    }

    ms.MotionEnd();
    ms.Disconnect();
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
