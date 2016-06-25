#include <windows.h>
#include <stdio.h>
#include <queue>
#include "file.h"
#include <string> // for std::string
#include <stdlib.h> // for sleep()

/*
 *  In der Cell Struktur werden Ergebnisse aus dem Algorithmus gespeichert
 */
struct Cell
{
    bool bMarked;   // Zeigt an, ob das Feld schon besucht wurde (true = besucht)
    int  nLastX;    // Die X Koordinate des vorherigen Feldes (-1 = kein vorheriges Feld)
    int  nLastY;    // Die Y Koordinate des vorherigen Feldes
};

struct Coordinate
{
    int x, y;
};

// Uninteressant
bool operator<(const Coordinate& c1, const Coordinate& c2)  { return false; }
bool operator==(const Coordinate& c1, const Coordinate& c2) { return false; }

/*
 *  In dieser Tabelle steht die Reihenfolge in der die umliegenden
 *  Felder abgearbeitet werden sollen
 */
const int DirTable[8][2] = {
{  -1, 0 },  // oben 
{  0,  1 },  // rechts
{  1,  0 },  // unten
{ 0, -1 },  // links
{ 0, 0 },
{ 0, 0 },
{ 0, 0 },
{ 0, 0 }};
/*{  -1, 1 },  // rechts-oben
{  1,  1 },  // rechts-unten
{  1, -1 },  // links-unten
{ -1,  -1 }}; */// links-oben


// Globale Variablen
int height;  // Hoehe des Konfigurationsraums
int width;   // Breite des Konfigurationsraums

char **cspace;  // 2D Konfigurationsraum. Falls cspace[y][x] != 0, dann verursacht die Konfiguration <x,y> eine Kollision
Cell **aCells;  // In diesem 2D Feld werden die Ergebisse des Algorithmus abgelegt

// Funktionsprototypen
bool FindPath(int nStartX, int nStartY, int nGoalX, int nGoalY);
void ClearCells();

/*
 *  main
 */
int main()
{
    FILE *fp;
    int x, y, length = 0;

	std::string file_basename = "cspace";

	std::string file_load = file_basename + ".png";
    // Laden des Konfigurationsraums
    if (!LoadFromPNG(file_load.c_str(), &cspace, &width, &height))
        printf("Fehler beim Laden der PNG Datei\n");

    // Zellenarray allokieren (dynamisch auf dem Heap)
    aCells = new Cell*[height];
    for (y = 0; y < height; y++)
        aCells[y] = new Cell[width];

    // Datei:
    // cspace.png            (100,300) -> (300,20)
    // cspace_prismatic.png  (620,320) -> (550,410)
    // cspace_revolute.png   (350,180) -> (120,180)
    // Startposition
    int nStartX = 100;
    int nStartY = 300;
    // Zielposition
    int nGoalX = 300;
    int nGoalY = 20;

    DWORD dwStart = GetTickCount();

    // Pfad suchen
    bool bFound = FindPath(nStartX, nStartY, nGoalX, nGoalY);

    DWORD dwElapsed = GetTickCount() - dwStart;
    printf("Die Suche dauerte: %d ms\nErgebnis:\n", dwElapsed);

    // Überprüfen, ob ein Pfad gefunden wurde
    if (bFound)
    {
        if (fopen_s(&fp, "path.prg", "w"))
            fp = stdout;

        // Den Pfad vom Ziel zum Start zurückgehen und den Pfad markieren
        x = nGoalX;
        y = nGoalY;

		printf("Goal x=%d y=%d \n", x, y);
		

		int i = 0;
        while (aCells[y][x].nLastX != -1)   // Bei -1 sind wir am Start
        {
            // EASY-ROB Programm erzeugen
            // Die Gelenkwerte müssen entsprechend den Zellen angepasst werden!!!
            fprintf(fp, "JUMP_TO_AX %f %f\n", (x - width/2) * 0.002f, (y - height/2)  * 0.002f); // von -1m bis +1m in 20mm Schritten
            //fprintf(fp, "JUMP_TO_AX %f %f\n", x - width/2.0f, y - width/2.0f); // von -180° bis +180° in 1° Schritten
            cspace[y][x] = -1;              // Schwarzes Pixel setzen
            Cell cell = aCells[y][x];
            x = cell.nLastX;
            y = cell.nLastY;
            ++length;
        }

        printf("Pfad der Laenge %d gefunden in %d ms.\n", length, dwElapsed);

		std::string file_save = file_basename + "_bfs.png";
        // Bild ausgeben, so das man den Pfad sehen kann
        if (!SaveAsPNG(file_save.c_str(), cspace, width, height))
            printf("Fehler beim Erzeugen der PNG Datei\n");

        if (fp != stdout)
            fclose(fp);
    }
    else
        printf("Keinen Pfad gefunden!\n");

    // Cleanup
    for (y = 0; y < height; y++)    // Alle Cells vom Heap löschen
        delete [] aCells[y];
    delete [] aCells;

    return 0;
}

/*
 *  FindPath - Breadth-First (Breitensuche)
 *  Algorithmus zum finden eines Pfades vom Start zum Endpunkt (goal)
 */
bool FindPath(int nStartX, int nStartY, int nGoalX, int nGoalY)
{
    printf("Suche Pfad von x=%d y=%d nach x=%d y=%d\n", nStartX, nStartY, nGoalX, nGoalY);
	
	// initalize cells
	for (int my_y = 0; my_y < height; my_y++)
	{
		for (int my_x = 0; my_x < width; my_x++)
		{
			aCells[my_y][my_x].bMarked = false;
		}
	}

    // create queue
    std::queue<Coordinate> queue;

    // starting cell
    Coordinate c;
    c.x = nStartX;
    c.y = nStartY;

    //  Folgende Zeilen demonstrieren die Manipulation von Queues (Warteschlange)
    //  Einfach auskommentieren und testen

	/*
    printf("Queue leer? %d\n", queue.empty());       // Am Anfang ist die Queue leer

    queue.push(c);                                   // Element in die Queue anstellen
    printf("Queue leer? %d\n", queue.empty());       // Jetzt sollte die Queue nicht mehr leer sein

    printf("Elemente in Queue: %d\n", queue.size()); // Anzahl Elemente in der Queue

    Coordinate test = queue.front();                 // front() gibt das forderste Element in der Queue zurück,
    printf("x: %d, y: %d\n", test.x, test.y);        // jedoch wird das Element dadurch nicht entfernt!

    queue.pop();                                     // Entfernt das erste Element aus der Queue

    printf("Queue leer? %d\n", queue.empty());       // Nun sollte die Queue wieder leer sein
	*/

	// push first cell into queue
	queue.push(c);

	// mark starting cell for backtracing
	aCells[nStartY][nStartX].nLastX = -1;

	// mark starting cell 
	 aCells[c.y][c.x].bMarked = 1;

	int color = 1;

	// while there's still cells in the queue...
    while (!queue.empty())
    {
		// get the first element in the queue
		c.x = queue.front().x;
		c.y = queue.front().y;
		queue.pop();

		// if it has the desired coordinates we terminate
		if (c.x == nGoalX && c.y == nGoalY)
		{
			printf("found: %d %d \n", c.x, c.y);
			printf("last: %d %d \n", aCells[c.y][c.x].nLastX, aCells[c.y][c.x].nLastY);
			return true;
		}
		
		Coordinate temp;
		for (int i = 0; i < 8; i++)
		{
			// make sure we're still within the array's bounds
			if (c.y + DirTable[i][1] >= 0 && c.y + DirTable[i][1] < height && c.x + DirTable[i][0] >= 0 && c.x + DirTable[i][0] < width)
			{
				// if we haven't checked the next cell yet...
				if (aCells[c.y + DirTable[i][1]][c.x + DirTable[i][0]].bMarked == false)
				{
					// mark cell
					aCells[c.y + DirTable[i][1]][c.x + DirTable[i][0]].bMarked = true;
						
					// if there's no collsion in the next cell...
					if (cspace[c.y + DirTable[i][1]][c.x + DirTable[i][0]] == 0 || cspace[c.y + DirTable[i][1]][c.x + DirTable[i][0]] == 127)
					{
						// watch the BFS as it happens
						// cspace[c.y + DirTable[i][1]][c.x + DirTable[i][0]] = 127;
						cspace[c.y + DirTable[i][1]][c.x + DirTable[i][0]] = color;
						 SaveAsPNG("cspace_bfs_debug.png", cspace, width, height);
						 getchar();

						aCells[c.y + DirTable[i][1]][c.x + DirTable[i][0]].nLastX = c.x;
						aCells[c.y + DirTable[i][1]][c.x + DirTable[i][0]].nLastY = c.y;
						temp.x = c.x + DirTable[i][0];
						temp.y = c.y + DirTable[i][1];
						queue.push(temp);
						color++;
					}
				}
			}
		}
    }
    return false; // Default Return
}

/*
 *  ClearCells()
 *  Setzt alle Zellen auf die Initialwerte zurück
 */
void ClearCells()
{
    // Zellen zurücksetzen
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            aCells[y][x].bMarked = false;   // Nicht besucht
            aCells[y][x].nLastX = -1;       // Keine vorherige Zelle
            aCells[y][x].nLastY = -1;
        }
    }
}
