// Termin4_1.cpp
// Praktikum Robotersimulation Kalibrierung
// Aufgabe 1: Einmessen von Koordinatensystemen mit dem Abtastgerät MicroScribe
// Fabian Alexander Wilms 735162

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cmath>
#include <vectmath.h>

#define DATA_FNAME1 "4_1_System1.txt"
#define DATA_FNAME2 "4_1_System2.txt"

bool LoadSys(const char* szFileName, Position& Origin, Position& X, Position& Y);

int main(int argc, const char* argv[])
{
	Position K0o, K0x, K0y, K1o, K1x, K1y;

	if (!LoadSys(DATA_FNAME1, K0o, K0x, K0y)
	||  !LoadSys(DATA_FNAME2, K1o, K1x, K1y))
		return EXIT_FAILURE;

	// Ihr Code

	Position myPos[6] = { K0o, K0x, K0y, K1o, K1x, K1y };

	Vector3 x,y,z;
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
		y = b2 / mc_length(b2);
		z = mc_vectorial_product(x, y);

		if (i == 0)
		{
			x.print("x System 1");
			y.print("y System 1");
			z.print("z System 1");
			// T1 bestimmen
			T1 = x;
			T1 = mc_concatenate(T1, y, 1);
			T1 = mc_concatenate(T1, z, 1);
			T1 = mc_concatenate(T1, K0o, 1);
			T1 = mc_concatenate(T1, homogen_t, 0);
			T1.print("Transformationsmatrix System 1");
		} else {
			x.print("x System 1");
			y.print("y System 1");
			z.print("z System 1");
			// T1 bestimmen
			T2 = x;
			T2 = mc_concatenate(T2, y, 1);
			T2 = mc_concatenate(T2, z, 1);
			T2 = mc_concatenate(T2, K1o, 1);
			T2 = mc_concatenate(T2, homogen_t, 0);
			T2.print("Transformationsmatrix System 2");
		}
	}

	T1_Transform = (Transform) T1;
	T2_Transform = (Transform) T2;

	Transform T1_Transform_inv = mc_pinv(T1_Transform);

	Transform T12 = T1_Transform_inv * T2_Transform;

	T12.print("Transformationsmatrix Sys 1 nach Sys 2");

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
