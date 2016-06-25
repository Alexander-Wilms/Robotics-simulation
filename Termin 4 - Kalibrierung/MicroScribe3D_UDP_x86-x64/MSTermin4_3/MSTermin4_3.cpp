// MSTermin4_3.cpp
// Praktikum Robotersimulation Kalibrierung
// MicroScribe3D Datenaufzeichnung fuer Termin 4, Aufgabe 3

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

#define DATA_FNAME "Kreisel.txt"

int main(int argc, const char* argv[])
{
    FILE* fp;
    int nPoints = 0;
    char szInfo[128];
    CMicroScribe ms;
    CMicroScribe::status_t result;
    Transform T;
    Euler_angle_xyz ea;
    int nButtons = 0;
    errno_t err;

    err = fopen_s(&fp, DATA_FNAME, "w");
    if (err)
    {
        strerror_s(szInfo, err);
        fprintf(stderr, "%s: %s\n", DATA_FNAME, szInfo);
        return EXIT_FAILURE;
    }

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
    fprintf(stdout, "MicroScribe3D Datenaufzeichnung fuer Termin 4, Aufgabe 3.\n");
    fprintf(stdout, "Betaetigen Sie den Fusstaster um die aktuelle Position aufzuzeichnen.\n");
    fprintf(stdout, "Druecken Sie [ESC] um das Programm zu beenden.\n");
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

            ea = Euler_angle_xyz(T);

            fprintf(stdout, "%d@%s: X%+07.2f Y%+07.2f Z%+07.2f A%+07.2f B%+07.2f C%+07.2f  \r", nPoints + 1, DATA_FNAME,
                ea.x(), ea.y(), ea.z(), ea.rx() * RAD2DEG, ea.ry() * RAD2DEG, ea.rz() * RAD2DEG);

            if (ms.hci.m_nButtons != nButtons)
            {
                nButtons = ms.hci.m_nButtons;
                if (nButtons)
                {
                    nPoints += 1;
                    fprintf(fp, "%.3f;%.3f;%.3f;%.6f;%.6f;%.6f\n", ea.x(), ea.y(), ea.z(), ea.rx(), ea.ry(), ea.rz());
                    fputs("\n", stdout);
                }
            }
        }

        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
            break;
    }

    ms.MotionEnd();
    ms.Disconnect();
    fputs("\n\n", stdout);

    fclose(fp);

    return EXIT_SUCCESS;
}
