// MSTermin4_1u2.cpp
// Praktikum Robotersimulation Kalibrierung
// MicroScribe3D Datenaufzeichnung fuer Termin 4, Aufgabe 1 und 2

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cmath>
#include <windows.h>
#include <mscribe.h>
#include <vectmath.h>

#define DATA_FNAME "System%d.txt"

int main(int argc, const char* argv[])
{
    FILE* fp;
    int nPoints = 0, nSystem = 1;
    char szFile[_MAX_PATH], szInfo[128];
    CMicroScribe ms;
    CMicroScribe::status_t result;
    int nButtons = 0;
    errno_t err;

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
    fprintf(stdout, "MicroScribe3D Datenaufzeichnung fuer Termin 4, Aufgabe 1 und 2.\n");
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
            _snprintf_s(szFile, _TRUNCATE, DATA_FNAME, nSystem);
            fprintf(stdout, "%d@%s: X%+07.2f Y%+07.2f Z%+07.2f  \r", nPoints + 1, szFile, ms.m_stylusPosition.x, ms.m_stylusPosition.y, ms.m_stylusPosition.z);

            if (ms.hci.m_nButtons != nButtons)
            {
                nButtons = ms.hci.m_nButtons;
                if (nButtons)
                {
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
                    fprintf(fp, "%.3f;%.3f;%.3f\n", ms.m_stylusPosition.x, ms.m_stylusPosition.y, ms.m_stylusPosition.z);

                    if (nPoints == 3)
                    {
                        fprintf(stdout, "Der Datensatz %s wurde abgespeichert.\n", szFile);
                        fclose(fp);
                        nPoints = 0;
                        nSystem += 1;
                    }
                }
            }
        }

        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
            break;
    }

    ms.MotionEnd();
    ms.Disconnect();
    fputs("\n\n", stdout);

    if (nPoints)
    {
        fclose(fp);
        DeleteFile(szFile);
    }

    return EXIT_SUCCESS;
}