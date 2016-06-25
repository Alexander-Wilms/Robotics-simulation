/******************************************************************************
file:             main.cpp
created:          12.04.2004
last modified:    01.12.2015
******************************************************************************/

#include <windows.h>
#include <opcode.h>
#include <vectmath.h>
#include "file.h"
#include "box.h"

#define SQR(a)      ((a)*(a))
#define PIH         (PI*0.5)
#define ROT_X       1
#define ROT_Y       2
#define ROT_Z       3
#define TRANS_X     4
#define TRANS_Y     5
#define TRANS_Z     6
#define DEG2RAD     (PI/180.0)
#define RAD2DEG     (180.0/PI)

struct KinematicData
{
	int    typ;         // Achstyp translatorisch (TRANS_X, TRANS_Y, TRANS_Z) oder rotatorisch (ROT_X, ROT_Y, ROT_Z)
	double trans_x;     // Translation um trans_x in mm
	double trans_y;     // Translation um trans_y in mm
	double trans_z;     // Translation um trans_z in mm
	double rot_x;       // Rotation um rot_x in rad
	double rot_y;       // Rotation um rot_y in rad
	double rot_z;       // Rotation um rot_z in rad
};

bool collide(Box *box1, Box *box2);
Transform ForwardKinematics(int dof, Transform WK2Basis, KinematicData robot[], Vector axes);

/*
*  main
*/
int main()
{
	// Breite und Höhe des Konfigurationsraums
	int width = 360;
	int height = 360;
	int x, y, i, nTests = 0;

	KinematicData DOF2[2];
	Transform T1, T2, WK2Basis;
	int dof = 2;
	Vector axes(dof);
	WK2Basis = mc_identity(4);  // Frame vom Weltkoordinatensystem zur Roboterbasis

	// Initalisierung der kinematischen Struktur des Roboters
	for (i = 0; i < dof; i++)
	{
		DOF2[i].typ = ROT_Z;
		DOF2[i].trans_x = DOF2[i].trans_y = DOF2[i].trans_z = 0.0;
		DOF2[i].rot_x = DOF2[i].rot_y = DOF2[i].rot_z = 0.0;
	}

	DOF2[0].trans_x = 0.180;    // In Metern, nicht Millimeter
	DOF2[1].trans_x = 0.230;

	// Konfigurationsraum initialisieren (dynamische Größe)
	char **cspace = new char*[height];
	for (y = 0; y < height; y++)
	{
		cspace[y] = new char[width];
		memset(cspace[y], 0, width);
	}

	const int nHind = 5;  // Anzahl der Hindernisse
	const int nRob = 2;  // Anzahl der Roboterglieder

	Box aHindernis[nHind];
	Box aRoboter[nRob];

	// Hindernisse initialisieren
	aHindernis[0].Scale(0.10f, 0.10f, 0.20f);     // QUADER1   0.1    0.1    0.2
	aHindernis[0].Translate(0.2f, 0.15f, 0.0f);   // REFPOS    0.2    0.15   0.0   0 0 45

	aHindernis[1].Scale(0.04f, 0.10f, 0.20f);     // QUADER2   0.04   0.1    0.2
	aHindernis[1].Translate(0.0f, 0.25f, 0.0f);   // REFPOS    0.0    0.25   0.0   0 0 0

	aHindernis[2].Scale(0.04f, 0.10f, 0.20f);     // QUADER3   0.4    0.1    0.2
	aHindernis[2].Translate(0.25f, -0.15f, 0.0f); // REFPOS    0.25  -0.15   0.0   0 0 0

	aHindernis[3].Scale(0.2f, 0.05f, 0.20f);      // QUADER4   0.2    0.05   0.2
	aHindernis[3].Translate(-0.25f, -0.15f, 0.0f);// REFPOS   -0.25  -0.15   0.0   0 0 0

	aHindernis[4].Scale(0.1f, 0.20f, 0.20f);      // QUADER5   0.1    0.2    0.2
	aHindernis[4].Translate(-0.3f, 0.1f, 0.0f);   // REFPOS   -0.3    0.1    0.0   0 0 0

	// Roboter initialisieren
	// Ihr Code ...
	aRoboter[0].Scale(0.18f, 0.05f, 0.20f);       // QUADER    0.18000    0.05000    0.20000  
	aRoboter[1].Scale(0.23f, 0.05f, 0.20f);       // QUADER    0.23000    0.05000    0.20000
												 
	// Startzeit
	DWORD dwStart = GetTickCount();

	// Konfigurationsraum aufbauen
	// Ihr Code ...
	int nCollisions = 0;

	for (int j = 0; j < width; j += 1)
	{
		for (int k = 0; k < height; k += 1)
		{
			axes[0] = j - width / 2;
			axes[1] = k - height / 2;

			// Berechnen der Vorwärtstransformation
			T1 = ForwardKinematics(1, WK2Basis, DOF2, axes);
			// Setzen der homogenen Transformationsmatrix in Opcode
			aRoboter[0].m_Matrix.Set((float)T1[0][0], (float)T1[1][0], (float)T1[2][0], (float)T1[3][0],
				(float)T1[0][1], (float)T1[1][1], (float)T1[2][1], (float)T1[3][1],
				(float)T1[0][2], (float)T1[1][2], (float)T1[2][2], (float)T1[3][2],
				(float)T1[0][3], (float)T1[1][3], (float)T1[2][3], (float)T1[3][3]);

			// Berechnen der Vorwärtstransformation
			T2 = ForwardKinematics(2, WK2Basis, DOF2, axes);
			// Setzen der homogenen Transformationsmatrix in Opcode
			aRoboter[1].m_Matrix.Set((float)T2[0][0], (float)T2[1][0], (float)T2[2][0], (float)T2[3][0],
				(float)T2[0][1], (float)T2[1][1], (float)T2[2][1], (float)T2[3][1],
				(float)T2[0][2], (float)T2[1][2], (float)T2[2][2], (float)T2[3][2],
				(float)T2[0][3], (float)T2[1][3], (float)T2[2][3], (float)T2[3][3]);

			for (int i = 0; i < nHind; i++)
			{
				for (int l = 0; l < nRob; l++)
				{
					cspace[height - 1 - k][j] += (i + 1)  * (int)collide(&aRoboter[l], &aHindernis[i]);
					if (cspace[height - 1 -  k][j] != 0) nCollisions++;
					nTests++;
				}
			}
		}
	}

	// Zeit für das Aufstellen des Konfigurationsraumes ausgeben (in ms)
	DWORD dwElapsed = GetTickCount() - dwStart;
	float percentage = (float) nCollisions / nTests;
	printf("\nBerechnung dauerte %d ms\n", dwElapsed);
	printf("Anzahl Kollisionstests: %d\n", nTests);
	printf("Anzahl Kollisionen: %d\n", nCollisions);
	printf("Anteil Kollisionen: %f \n", percentage);

	// Konfigurationsraum als PNG speichern
	if (!SaveAsPNG("cspace_revolute.png", cspace, width, height))
		printf("Fehler beim Erzeugen der PNG Datei\n");
	else
		printf("PNG erfolgreich erzeugt\n");

	return 0;
}

/*
*  collide
*  Diese Funktion prüft, ob die zwei Objekte box1 und box2 kollidieren.
*  Wenn sie kollidieren, wird true zurückgegeben, sonst false
*/
bool collide(Box *box1, Box *box2)
{
	// Build a quantized no-leaf tree
	Opcode::AABBTreeCollider TC;
	Opcode::BVTCache ColCache;

	// Create a tree collider
	TC.SetFirstContact(true);
	TC.SetFullBoxBoxTest(false);    // use coarse BV-BV tests
	TC.SetFullPrimBoxTest(false);   // use coarse primitive-BV tests
	TC.SetTemporalCoherence(false); // don't use temporal coherence

									// Setup BVTCache
	ColCache.Model0 = &box1->m_OpModel;
	ColCache.Model1 = &box2->m_OpModel;

	// Collision query
	bool IsOk = TC.Collide(ColCache, &box1->m_Matrix, &box2->m_Matrix);

	// Get collision status => if true, objects overlap
	bool Status = TC.GetContactStatus() != FALSE;

	return Status;
}

/***************************************************************************************/
Transform ForwardKinematics(int dof, Transform WK2Basis, KinematicData robot[], Vector axes)
{
	Transform T(WK2Basis);

	for (int i = 0; i < dof; i++)
	{
		switch (robot[i].typ)
		{
		case ROT_X:   T *= (Transform)mc_Rx(axes[i] * DEG2RAD);    break;
		case ROT_Y:   T *= (Transform)mc_Ry(axes[i] * DEG2RAD);    break;
		case ROT_Z:   T *= (Transform)mc_Rz(axes[i] * DEG2RAD);    break;
		case TRANS_X: T *= (Transform)Position(axes[i], 0.0, 0.0); break;
		case TRANS_Y: T *= (Transform)Position(0.0, axes[i], 0.0); break;
		case TRANS_Z: T *= (Transform)Position(0.0, 0.0, axes[i]); break;
		default:      assert(0);
		}

		if (dof - i == 1)
			break;

		T *= (Transform)Euler_angle_xyz(robot[i].rot_x * DEG2RAD, robot[i].rot_y * DEG2RAD, robot[i].rot_z * DEG2RAD, robot[i].trans_x, robot[i].trans_y, robot[i].trans_z);
	}

	return T;
}
