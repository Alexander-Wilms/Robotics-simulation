// MSTermin4_1u2.cpp
// Praktikum Robotersimulation Kalibrierung
// Testprogramm fuer Termin 4, Aufgabe 3

#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <armdll32.h>
#include <vectmath.h>

#pragma comment(lib, "armdll32.lib")

#define PI 3.14159265359
#define DEG2RAD (PI/180.0)
#define RAD2DEG (180.0/PI)

#define DATA_FNAME "Kalibrierung.txt"

bool LoadCal(const char* szFileName, Position& Pt, Position& Ppivot);
Transform ArmGetTransform();

int main(int argc, const char* argv[])
{
    Position Pt, Ppivot;
    int nResult;
    char szInfo[2][128];

    // The ArmStart function prepares ArmDll32 for operation.
    // Internally, it spawns a read thread used to retrieve
    // data from the device.
    nResult = ArmStart(NULL);
    if (ARM_SUCCESS != nResult)
    {
        fprintf(stderr, "ArmDll32 konnte nicht gestartet werden!\n");
        return EXIT_FAILURE;
    }

    fprintf(stdout, "Testprogramm fuer Termin 4, Aufgabe 3.\n");
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

    if (!LoadCal(DATA_FNAME, Pt, Ppivot))
    {
        ArmDisconnect();
        ArmEnd();
        return EXIT_FAILURE;
    }

    while (!(GetAsyncKeyState(VK_ESCAPE) & 0x8000))
    {
        Euler_angle_xyz ea(ArmGetTransform());
        Position p((Transform)ea * Pt - Ppivot);

        fprintf(stdout, "X = %7.2f, Y = %7.2f, Z = %7.2f, A = %7.2f, B = %7.2f, C = %7.2f\n",
            p.x(), p.y(), p.z(), ea.rx() * RAD2DEG, ea.ry() * RAD2DEG, ea.rz() * RAD2DEG);
    }

    /* Time to end this session.*/
    ArmDisconnect();
    ArmEnd();

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
