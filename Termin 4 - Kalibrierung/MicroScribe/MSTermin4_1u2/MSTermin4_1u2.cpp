// MSTermin4_1u2.cpp
// Praktikum Robotersimulation Kalibrierung
// MicroScribe-Datenaufzeichnung fuer Termin 4, Aufgabe 1 und 2

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cmath>
#include <windows.h>
#include <armdll32.h>
#include <vectmath.h>

#pragma comment(lib, "armdll32.lib")

#define DATA_FNAME "System%d.txt"

bool CheckButton(void);

int main(int argc, const char* argv[])
{
    FILE* fp;
    int nResult, nPoints = 0, nSystem = 0;
    char szFile[_MAX_PATH], szInfo[2][128];
    length_3D l3TipPosition;
    errno_t err;

    // The ArmStart function prepares ArmDll32 for operation.
    // Internally, it spawns a read thread used to retrieve
    // data from the device.
    nResult = ArmStart(NULL);
    if (ARM_SUCCESS != nResult)
    {
        fprintf(stderr, "ArmDll32 konnte nicht gestartet werden!\n");
        return EXIT_FAILURE;
    }

    fprintf(stdout, "MicroScribe-Datenaufzeichnung fuer Termin 4, Aufgabe 1 und 2.\n");
    fprintf(stdout, "Betaetigen Sie den Fusstaster um die aktuelle Position aufzuzeichnen.\n");
    fprintf(stdout, "Druecken Sie [ESC] um das Programm zu beenden.\n");
    fprintf(stdout, "---------------------------------------------------------------------\n");

    /* Time to connect to the hardware */
    // The USB port is always checked first.  The port and baud
    // values are only used if the device is not found on a USB
    // port.  Passing 0,0 will check all serial ports.
    fprintf(stdout, "Suche MicroScribe...\n");
    nResult = ArmConnect(0, 0);
    if (ARM_SUCCESS != nResult)
    {
        fprintf(stderr, "Es wurde kein kompatibles Geraet gefunden!\n");
        ArmEnd();
        return EXIT_FAILURE;
    }

    nResult = ArmGetProductName(szInfo[0], _countof(szInfo[0]));
    if (nResult == ARM_SUCCESS)
        fprintf(stdout, "Produkt:          %s\n", szInfo[0]);

    nResult = ArmGetModelName(szInfo[0], _countof(szInfo[0]));
    if(nResult == ARM_SUCCESS)
        fprintf(stdout, "Modell:           %s\n", szInfo[0]);

    nResult = ArmGetSerialNumber(szInfo[0], _countof(szInfo[0]));
    if(nResult == ARM_SUCCESS)
        fprintf(stdout, "Seriennummer:     %s\n", szInfo[0]);

    nResult = ArmGetVersion(szInfo[0], szInfo[1], _countof(szInfo[0]));
    if(nResult == ARM_SUCCESS)
    {
        fprintf(stdout, "Treiber Version:  %s\n", szInfo[0]);
        fprintf(stdout, "Firmware Version: %s\n", szInfo[1]);
    }
    fprintf(stdout, "---------------------------------------------------------------------\n");

    // ARM_FULL: ArmDll32 calculates and updates the Cartesian
    // position and roll-pitch-yaw orientation of the stylus tip.
    // 20: 20ms MINIMUM update period.  Armdll32 will update data no
    // more than 20 times per second.  Updates only occur when device
    // status has changed.  This can be set to a minimum of 3ms which
    // would mean that data will be updated no more than 333 times per
    // second.  See Armdll32 v2.0 programmer's reference for more details.
    nResult = ArmSetUpdateEx(ARM_FULL, 20);
    if (ARM_SUCCESS != nResult)
    {
        fprintf(stderr, "Die Aktualisierungsrate konnte nicht gesetzt werden!\n");
        ArmDisconnect();
        ArmEnd();
        return EXIT_FAILURE;
    }

    ArmSetLengthUnits(ARM_MM);
    ArmSetAngleUnits(ARM_RADIANS);

    while (!(GetAsyncKeyState(VK_ESCAPE) & 0x8000))
    {
        // Get the Stylus position
        nResult = ArmGetTipPosition(&l3TipPosition);
        if (ARM_NOT_CONNECTED == nResult)
        {
            fprintf(stderr, "Die Verbindung wurde unterbrochen!\n");
            ArmEnd();
            return EXIT_FAILURE;
        }

        if (CheckButton())
        {
            if (!nPoints)
            {
                nSystem += 1;
                _snprintf_s(szFile, _TRUNCATE, DATA_FNAME, nSystem);
                err = fopen_s(&fp, szFile, "w");
                if (err)
                {
                    strerror_s(szInfo[0], err);
                    fprintf(stderr, "%s: %s\n", szFile, szInfo[0]);
                    ArmDisconnect();
                    ArmEnd();
                    return EXIT_FAILURE;
                }
            }

            nPoints += 1;
            fprintf(stdout, "%d@%s: X = %.3f, Y = %.3f, Z = %.3f\n", nPoints, szFile, l3TipPosition.x, l3TipPosition.y, l3TipPosition.z);
            fprintf(fp, "%.3f;%.3f;%.3f\n", l3TipPosition.x, l3TipPosition.y, l3TipPosition.z);

            if (nPoints == 3)
            {
                fprintf(stdout, "Der Datensatz %s wurde abgespeichert.\n", szFile);
                fclose(fp);
                nPoints = 0;
            }
        }
    }

    /* Time to end this session.*/
    if (nPoints)
    {
        fclose(fp);
        DeleteFile(szFile);
    }

    ArmDisconnect();
    ArmEnd();

    return EXIT_SUCCESS;
}

bool CheckButton()
{
    static DWORD dwLastButtonsState = 0;
    DWORD dwCurrentButtonsState;
    bool bResult;

    ArmGetButtonsState(&dwCurrentButtonsState);

    bResult = !(dwLastButtonsState & ARM_BUTTON_1) && (dwCurrentButtonsState & ARM_BUTTON_1);
    dwLastButtonsState = dwCurrentButtonsState;

    return bResult;
}
