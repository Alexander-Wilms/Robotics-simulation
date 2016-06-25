/******************************************************************************
file:             main.cpp
created:          12.04.2004
last modified:    01.12.2015
******************************************************************************/
#include <windows.h>
#include <opcode.h>
#include "file.h"
#include "box.h"

bool collide(Box *box1, Box *box2);

/*
*  main
*/
int main()
{
	// Breite und Höhe des Konfigurationsraums
	int width = 1000;
	int height = 1000;
	int x, y, nTests = 0;

	// Konfigurationsraum initialisieren (dynamische Größe)
	char **cspace = new char*[height];
	for (y = 0; y < height; y++)
	{
		cspace[y] = new char[width];
		memset(cspace[y], 0, width);
	}

	const int nHind = 10;  // Anzahl der Hindernisse
	const int nRob = 1;   // Anzahl der Roboterglieder

	Box aHindernis[nHind];  // Unsere Hindernisse
	Box aRoboter[nRob];     // Roboterteile

	// Hindernisse initialisieren
	// Hierbei wird für jedes Hindernis die Größe (Skalierung) und die Position im Raum gesetzt
	aHindernis[0].Scale(0.30f, 0.05f, 0.10f);     // QUADER1   0.30000    0.05000    0.10000
	aHindernis[0].Translate(-0.7f, -0.4f, 0.0f);  // REFPOS   -0.7000000   -0.4000000    0.0000000    0.0000000    0.0000000    0.0000000

	aHindernis[1].Scale(0.05f, 0.60f, 0.10f);     // QUADER2   0.05000    0.60000    0.10000
	aHindernis[1].Translate(-0.25f, -0.30f, 0.0f);// REFPOS   -0.2500000   -0.3000000    0.0000000    0.0000000    0.0000000    0.0000000

	aHindernis[2].Scale(0.25f, 0.05f, 0.10f);     // QUADER3   0.25000    0.05000    0.10000
	aHindernis[2].Translate(-0.2f, 0.45f, 0.0f);  // REFPOS   -0.2000000    0.4500000    0.0000000    0.0000000    0.0000000    0.0000000

	aHindernis[3].Scale(0.55f, 0.05f, 0.10f);     // QUADER4   0.55000    0.05000    0.10000
	aHindernis[3].Translate(-0.15f, -0.25f, 0.0f);// REFPOS   -0.1500000   -0.2500000    0.0000000    0.0000000    0.0000000    0.0000000

	aHindernis[4].Scale(0.1f, 0.05f, 0.10f);      // QUADER5   0.10000    0.05000    0.10000
	aHindernis[4].Translate(-0.05f, 0.20f, 0.0f); // REFPOS   -0.0500000    0.2000000    0.0000000    0.0000000    0.0000000    0.0000000

	aHindernis[5].Scale(0.05f, 0.2f, 0.10f);      // QUADER6   0.05000    0.20000    0.10000
	aHindernis[5].Translate(0.05f, -0.6f, 0.0f);  // REFPOS    0.0500000   -0.6000000    0.0000000    0.0000000    0.0000000    0.0000000

	aHindernis[6].Scale(0.05f, 0.25f, 0.10f);     // QUADER7   0.05000    0.25000    0.10000
	aHindernis[6].Translate(0.25f, 0.25f, 0.0f);  // REFPOS    0.2500000    0.2500000    0.0000000    0.0000000    0.0000000    0.0000000

	aHindernis[7].Scale(0.05f, 0.25f, 0.10f);     // QUADER8   0.05000    0.25000    0.10000
	aHindernis[7].Translate(0.35f, -0.5f, 0.0f);  // REFPOS    0.3500000   -0.5000000    0.0000000    0.0000000    0.0000000    0.0000000

	aHindernis[8].Scale(0.2f, 0.05f, 0.10f);      // QUADER9   0.20000    0.05000    0.10000
	aHindernis[8].Translate(0.45f, 0.15f, 0.0f);  // REFPOS    0.4500000    0.1500000    0.0000000    0.0000000    0.0000000    0.0000000

	aHindernis[9].Scale(0.05f, 0.3f, 0.1f);       // QUADER10  0.05000    0.30000    0.10000
	aHindernis[9].Translate(0.2f, -0.15f, 0.0f);  // REFPOS    0.2000000   -0.1500000    0.0000000    0.0000000    0.0000000    0.0000000

												  // Roboterglieder initialisieren
	aRoboter[0].Scale(0.07f, 0.07f, 0.20f);       // QUADER    0.07000    0.07000    0.20000
	// aRoboter[0].Translate(-0.05f, -0.05f, 0.0f);  // REFPOS   -0.0500000   -0.0500000    0.0000000    0.0000000    0.0000000    0.0000000

	// Startzeit
	DWORD dwStart = GetTickCount();

	// Konfigurationsraum aufbauen
	// Ihr Code ...

	cspace[0][0] = 100;
	cspace[0][999] = 100;
	cspace[999][0] = 100;
	cspace[999][999] = 100;

	int nCollisions;

	for (int i = 0; i < width; i += 1)
	{
		for (int j = 0; j < height; j += 1)
		{
			aRoboter[0].Translate((float)(i - height / 2) / 1000, (float)(j - width / 2) / 1000, 0.0f);

			for (int k = 0; k < nHind; k++)
			{
				for (int l = 0; l < nRob; l++)
				{
					cspace[height - 1 - j][i] += (k + 1) * (int) collide(&aRoboter[l], &aHindernis[k]);
					if (cspace[height -1 - j][i] != 0) nCollisions++;
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
	if (!SaveAsPNG("cspace_prismatic.png", cspace, width, height))
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
