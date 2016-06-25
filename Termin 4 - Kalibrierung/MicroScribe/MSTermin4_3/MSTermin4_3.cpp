// MSTermin4_1u2.cpp
// Praktikum Robotersimulation Kalibrierung
// MicroScribe-Datenaufzeichnung fuer Termin 4, Aufgabe 3

#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <armdll32.h>
#include <vectmath.h>

#pragma comment(lib, "armdll32.lib")

#define DATA_FNAME "Kreisel.txt"

bool CheckButton(void);
Transform ArmGetTransform();

int main(int argc, const char* argv[])
{
    FILE* fp;
    int nResult, nPoints = 0;
    char szInfo[2][128];
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

    fprintf(stdout, "MicroScribe-Datenaufzeichnung fuer Termin 4, Aufgabe 3.\n");
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

    err = fopen_s(&fp, DATA_FNAME, "w");
    if (err)
    {
        strerror_s(szInfo[0], err);
        fprintf(stderr, "%s: %s\n", DATA_FNAME, szInfo[0]);
        ArmDisconnect();
        ArmEnd();
        return EXIT_FAILURE;
    }

    while (!(GetAsyncKeyState(VK_ESCAPE) & 0x8000))
    {
        if (CheckButton())
        {
            Euler_angle_xyz ea(ArmGetTransform());

            nPoints += 1;
            fprintf(stdout, "%d@%s: X = %.3f, Y = %.3f, Z = %.3f, A = %.5f, B = %.5f, C = %.5f\n", nPoints, DATA_FNAME, ea.x(), ea.y(), ea.z(), ea.rx(), ea.ry(), ea.rz());
            fprintf(fp, "%.3f;%.3f;%.3f;%.6f;%.6f;%.6f\n", ea.x(), ea.y(), ea.z(), ea.rx(), ea.ry(), ea.rz());
        }
    }

    /* Time to end this session.*/
    fclose(fp);

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

Transform ArmGetTransform()
{
    Transform T;
    arm_rec* par;

    par = (arm_rec*)ArmGetArmRec();

    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            T[r][c] = par->T[r][c];

    return T;
}
