// Termin4_2.cpp
// Praktikum Robotersimulation Kalibrierung
// Aufgabe 2: Vergroesserung des MicroScribe-Messraums mit mehreren Bezugssystemen

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cmath>
#include <vector>
#include <vectmath.h>

#define DATA_FNAME "System%d.txt"

typedef struct sysdef_t
{
    Position Origin, X, Y;
}
SYSDEF, *LPSYSDEF;

typedef std::vector<SYSDEF> SysDefVector;
typedef std::vector<Transform> TransformVector;

bool LoadSysN(const char* szFileName, SysDefVector& SysDefs);

int main(int argc, const char* argv[])
{
    SysDefVector SysDefs;

    if (!LoadSysN(DATA_FNAME, SysDefs))
        return EXIT_FAILURE;

    // Ihr Code

    return EXIT_SUCCESS;
}

bool LoadSysN(const char* szFileName, SysDefVector& SysDefs)
{
    FILE* fp;
    errno_t err;
    char szFile[_MAX_PATH], szBuffer[_MAX_PATH];
    SYSDEF sys;

    SysDefs.clear();

    for (int i = 1; true; ++i)
    {
        _snprintf_s(szFile, _TRUNCATE, szFileName, i);

        err = fopen_s(&fp, szFile, "r");
        if (err)
        {
            if (err == ENOENT)
                return !SysDefs.empty();

            strerror_s(szBuffer, err);
            fprintf(stderr, "%s: %s\n", szFile, szBuffer);
            break;
        }

        fprintf(stdout, "Lese Datensatz aus %s\n", szFile);

        if (!fgets(szBuffer, _countof(szBuffer) - 1, fp)
        ||  sscanf_s(szBuffer, "%lf;%lf;%lf", &sys.Origin.x(), &sys.Origin.y(), &sys.Origin.z()) != 3
        ||  !fgets(szBuffer, _countof(szBuffer) - 1, fp)
        ||  sscanf_s(szBuffer, "%lf;%lf;%lf", &sys.X.x(), &sys.X.y(), &sys.X.z()) != 3
        ||  !fgets(szBuffer, _countof(szBuffer) - 1, fp)
        ||  sscanf_s(szBuffer, "%lf;%lf;%lf", &sys.Y.x(), &sys.Y.y(), &sys.Y.z()) != 3)
        {
            fprintf(stderr, "%s: Ungueltiges Format!\n", szFile);
            break;
        }

        SysDefs.push_back(sys);

        fclose(fp);
    }

    return false;
}
