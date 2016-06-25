// Termin4_3.cpp
// Praktikum Robotersimulation Kalibrierung
// Aufgabe 3: Kalibrierung des MicroScribe-Endeffektors
// Fabian Alexander Wilms 735162

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cmath>
#include <vector>
#include <vectmath.h>

#define DATA_FNAME_IN  "Kreisel.txt"
#define DATA_FNAME_OUT "Kalibrierung.txt"

typedef std::vector<Euler_angle_xyz> EulerAngleXYZVector;

bool LoadCalFrames(const char* szFileName, EulerAngleXYZVector& Frames);
bool SaveCal(const char* szFileName, Position& Pt, Position& Ppivot);

int main(int argc, const char* argv[])
{
	EulerAngleXYZVector Frames;
	Position Pt, Ppivot;

	if (!LoadCalFrames(DATA_FNAME_IN, Frames))
		return EXIT_FAILURE;

	// A * x = b
	//
	// | R1,nx R1,ox R1,ax -1  0  0 |					| -P1,x |
	// | R1,ny R1,oy R1,ay  0 -1  0 |	| Pt,x	 	|	| -P1,y |
	// | R1,nz R1,oz R1,az  0  0 -1 |	| Pt,y	 	|	| -P1,z |
	// | R2,nx R2,ox R2,ax -1  0  0 | * | Pt,z	 	| = | -P2,x |
	// | R2,ny R2,oy R2,ay  0 -1  0 |	| Ppivot,x 	|	| -P2,y |
	// | R2,nz R2,oz R2,az  0  0 -1 |	| Ppivot,y 	|	| -P2,z |
	// | R3,nx R3,ox R3,ax -1  0  0 |	| Ppivot,z 	|	| -P3,x |
	// | ...						|					| ...	|
	//
	// x = (A' * A)^(-1) * A' * b

	// Ihr Code

	Matrix A;
	Vector b;

	for (unsigned int i = 0; i < Frames.size(); i++) {
		if (i == 0) {
			b =  - (Position) Frames[i];
			A = (Rotation) Frames[i];
			A = mc_concatenate(A, - mc_identity(3), 1);
		}
		else {
			b = mc_concatenate(b, - (Position) Frames[i], 0);
			Matrix Block36 = (Rotation)Frames[i];
			Block36 = mc_concatenate(Block36, - mc_identity(3), 1);
			A = mc_concatenate(A, Block36, 0);
		}
	}

	Matrix AT = mc_transpose(A);
	Vector6 x = mc_inv(AT*A)*AT*b;

	x.print("x");

	// Validierung der Ergebnisse
	Pt = { x[0], x[1], x[2] };
	Ppivot = { x[3], x[4], x[5] };

	Pt.print("\nPt");
	Ppivot.print("\nPpivot");

	Position Ptest;
	Rotation R;
	Position p;

	for (unsigned int i = 0; i < Frames.size(); i++) {
			R = (Rotation) Frames[i];
			p = (Position) Frames[i];
			Ptest = R * Pt + p - Ppivot;
			Ptest.print("\nPtest");
	}

	// Standardabweichung berechnen
	Vector r;
	r = Vector (A * x) - b;
	double s0 = sqrt((double) (mc_transpose(r) * r) / (int) Frames.size());

	printf("\nStandardabweichung %f \n", s0);

	if (!SaveCal(DATA_FNAME_OUT, Pt, Ppivot))
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

bool LoadCalFrames(const char* szFileName, EulerAngleXYZVector& Frames)
{
	FILE* fp;
	errno_t err;
	char szBuffer[_MAX_PATH];
	Euler_angle_xyz ea;

	Frames.clear();

	err = fopen_s(&fp, szFileName, "r");
	if (err)
	{
		strerror_s(szBuffer, err);
		fprintf(stderr, "%s: %s\n", szFileName, szBuffer);
		return false;
	}

	fprintf(stdout, "Lese Datensatz aus %s\n", szFileName);

	while (fgets(szBuffer, _countof(szBuffer) - 1, fp))
	{
		if (sscanf_s(szBuffer, "%lf;%lf;%lf;%lf;%lf;%lf", &ea.x(), &ea.y(), &ea.z(), &ea.rx(), &ea.ry(), &ea.rz()) != 6)
			continue;

		Frames.push_back(ea);
	}

	fclose(fp);

	return !Frames.empty();
}

bool SaveCal(const char* szFileName, Position& Pt, Position& Ppivot)
{
	FILE* fp;
	errno_t err;
	char szBuffer[_MAX_PATH];
	Euler_angle_xyz ea;

	err = fopen_s(&fp, szFileName, "w");
	if (err)
	{
		strerror_s(szBuffer, err);
		fprintf(stderr, "%s: %s\n", szFileName, szBuffer);
		return false;
	}

	fprintf(stdout, "Schreibe Datensatz nach %s\n", szFileName);

	fprintf(fp, "%.6f;%.6f;%.6f\n", Pt.x(), Pt.y(), Pt.z());
	fprintf(fp, "%.6f;%.6f;%.6f\n", Ppivot.x(), Ppivot.y(), Ppivot.z());

	fclose(fp);

	return true;
}
