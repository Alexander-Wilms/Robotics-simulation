// Termin4_1.cpp
// Praktikum Robotersimulation Kalibrierung
// Aufgabe 1: Einmessen von Koordinatensystemen mit dem Abtastgerät MicroScribe

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cmath>
#include <vectmath.h>

#define DATA_FNAME1 "System1.txt"
#define DATA_FNAME2 "System2.txt"

bool LoadSys(const char* szFileName, Position& Origin, Position& X, Position& Y);

int main(int argc, const char* argv[])
{
    Position K0o, K0x, K0y, K1o, K1x, K1y;

    if (!LoadSys(DATA_FNAME1, K0o, K0x, K0y)
    ||  !LoadSys(DATA_FNAME2, K1o, K1x, K1y))
        return EXIT_FAILURE;

    // Ihr Code

    return EXIT_SUCCESS;
}

bool LoadSys(const char* szFileName, Position& Origin, Position& X, Position& Y)
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

    fprintf(stdout, "Lese Daten aus %s\n", szFileName);

    if (!fgets(szBuffer, _countof(szBuffer) - 1, fp)
    ||  sscanf_s(szBuffer, "%lf;%lf;%lf", &Origin.x(), &Origin.y(), &Origin.z()) != 3
    ||  !fgets(szBuffer, _countof(szBuffer) - 1, fp)
    ||  sscanf_s(szBuffer, "%lf;%lf;%lf", &X.x(), &X.y(), &X.z()) != 3
    ||  !fgets(szBuffer, _countof(szBuffer) - 1, fp)
    ||  sscanf_s(szBuffer, "%lf;%lf;%lf", &Y.x(), &Y.y(), &Y.z()) != 3)
    {
        fprintf(stderr, "%s: Ungueltiges Format!\n", szFileName);
        return false;
    }

    fclose(fp);

    return true;
}
