// Termin4_2.cpp
// Praktikum Robotersimulation Kalibrierung
// Aufgabe 2: Vergroesserung des MicroScribe-Messraums mit mehreren Bezugssystemen
// Fabian Alexander Wilms 735162

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cmath>
#include <vector>
#include <vectmath.h>

#define DATA_FNAME "4_2_System%d.txt"

Transform get_Transform(Position K0o, Position K0x, Position K0y, Position K1o, Position K1x, Position K1y)
{
	// Ihr Code

	Position myPos[6] = { K0o, K0x, K0y, K1o, K1x, K1y };

	Vector3 x, y, z;
	Matrix T1;
	Matrix T2;
	Transform T1_Transform;
	Transform T2_Transform;
	Vector4 homogen(0, 0, 0, 1);
	Matrix homogen_t = mc_transpose(homogen);

	for (int i = 0; i <= 1; i++)
	{
		Vector3 x_minus_origin = myPos[1 + i * 3] - myPos[0 + i * 3];
		Matrix x_minus_origin_t = mc_transpose(x_minus_origin);
		Vector3 y_minus_origin = myPos[2 + i * 3] - myPos[0 + i * 3];
		Matrix y_minus_origin_t = mc_transpose(y_minus_origin);
		x = x_minus_origin / mc_length(x_minus_origin);
		Vector3 b2 = y_minus_origin - (Vector3)((double)((y_minus_origin_t*(Matrix)x_minus_origin) / (x_minus_origin_t*(Matrix)x_minus_origin)) * x_minus_origin);
		y = (Matrix) b2 / mc_length(b2);
		z = mc_vectorial_product(x, y);

		if (i == 0)
		{
			// T1 bestimmen
			T1 = x;
			T1 = mc_concatenate(T1, y, 1);
			T1 = mc_concatenate(T1, z, 1);
			T1 = mc_concatenate(T1, myPos[0], 1);
			T1 = mc_concatenate(T1, homogen_t, 0);
		}
		else {
			// T2 bestimmen
			T2 = x;
			T2 = mc_concatenate(T2, y, 1);
			T2 = mc_concatenate(T2, z, 1);
			T2 = mc_concatenate(T2, myPos[3], 1);
			T2 = mc_concatenate(T2, homogen_t, 0);
		}
	}
	T1_Transform = (Transform)T1;
	T2_Transform = (Transform)T2;
	Transform T1_Transform_inv = mc_pinv(T1_Transform);
	Transform T12 = T1_Transform_inv * T2_Transform;
	T12.print("\nTransformationsmatrix Sys1 nach Sys2");

	return T12;
}

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

	Transform myT;
	Transform T = mc_identity(4);

	for (int i = 0; i < SysDefs.size()-1; i=i+2)
	{
		//printf("\nSize: %d", (int)SysDefs.size());
		printf("\nSystem %d nach %d", i + 1, i+2);
		myT = get_Transform(SysDefs[i].Origin, SysDefs[i].X, SysDefs[i].Y, SysDefs[i+1].Origin, SysDefs[i+1].X, SysDefs[i+1].Y);
		T *= myT;
	}

	T.print("\nVerkettete Transformationen");

	Transform Tsoll = mc_identity(4);

	Tsoll.print("\nSoll");

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
