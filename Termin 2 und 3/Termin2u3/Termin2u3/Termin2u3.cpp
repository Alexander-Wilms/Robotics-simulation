/*---------------------------------------------------------------------------*/
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <iostream>
#include <algorithm>
#include <cassert>

#include <vectmath.h>
/*---------------------------------------------------------------------------*/
/*					DEKLARATION DER STATISCHEN PROZEDUREN				  */
/*---------------------------------------------------------------------------*/

#define SQR(a)	  ((a)*(a))
#define PI		  3.14159265
#define PIH		 (PI*0.5)
#define ROT_X	   1
#define ROT_Y	   2
#define ROT_Z	   3
#define TRANS_X	 4
#define TRANS_Y	 5
#define TRANS_Z	 6
#define DEG2RAD	 (PI/180.0)
#define RAD2DEG	 (180.0/PI)

struct KinematicData
{
	int	typ;		 // Achstyp translatorisch (TRANS_X, TRANS_Y, TRANS_Z) oder rotatorisch (ROT_X, ROT_Y, ROT_Z)
	double trans_x;	 // Translation um trans_x in mm
	double trans_y;	 // Translation um trans_y in mm
	double trans_z;	 // Translation um trans_z in mm
	double rot_x;	   // Rotation um rot_x in rad
	double rot_y;	   // Rotation um rot_y in rad
	double rot_z;	   // Rotation um rot_z in rad
};

Transform RobotPose(KinematicData robot[], Transform WK2Basis, Vector axes);
Transform ForwardKinematics(KinematicData robot[], Vector axes, int qi, int qn);
Vector6 InverseKinematicsKR3(Transform T, KinematicData robot[], Transform WK2Basis);
Matrix ikNumDiff(KinematicData robot[], Vector axes);
Vector InverseKinematics(Transform T, KinematicData robot[], Transform WK2Basis, Vector axes);

/*---------------------------------------------------------------------------*/
/*								Helper									 */
/*---------------------------------------------------------------------------*/
Matrix ikSVD(Matrix M);
errno_t IpoLin(KinematicData robot[], Vector qs, Vector qe, int steps, const char* filename, bool numerical_solution);

/*---------------------------------------------------------------------------*/
/*							HAUPTPROGRAMM								  */
/*---------------------------------------------------------------------------*/
int main(void)
{
	KinematicData KR3[6];
	Transform T, WK2Basis;
	Matrix R;
	int dof = 6;

	// Frame vom Weltkoordinatensystem zur Roboterbasis
	WK2Basis = mc_identity(4);
	//WK2Basis = Transform(mc_Rz(90.0 * DEG2RAD)) * Position(100.0, 200.0, 300.0);

	// Gelenkwinkel
	//Vector6 axes(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	//Vector6 axes(10.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	Vector6 axes(10.0, 20.0, -30.0, 40.0, 50.0, 60.0);
	//Vector6 axes(-10.0, 20.0, -30.0, 40.0, -50.0, 60.0);
	//Vector6 axes(0.0, -142.65, 96.1763, 0.0, 51.4737, 0.0);

	// Initalisierung der kinematischen Struktur des KR3
	for(int i = 0; i < dof; i++)
	{
		KR3[i].typ = ROT_Z;
		KR3[i].trans_x = KR3[i].trans_y = KR3[i].trans_z = 0.0;
		KR3[i].rot_x   = KR3[i].rot_y   = KR3[i].rot_z   = 0.0;
	}

	// Ihr Code

	// Vorwärtstransformation
	T = RobotPose(KR3, WK2Basis, axes);
	axes.print("\nAxes = ");
	T.print("\nT");

	// Rückwärtstransformation (geometrische Lösung)
	axes = InverseKinematicsKR3(T, KR3, WK2Basis);
	axes.print("InverseKinematicsKR3 = ");

	// Rückwärtstransformation (numerische Lösung)
	axes = InverseKinematics(T, KR3, WK2Basis, Vector6(0.0, 0.0, -45.0, 0.0, 45.0, 0.0));
	axes.print("\nInverseKinematics = ");

	// Linearbewegung von qs nach qe
	//if (IpoLin(KR3, qs, qe, 100, "kr3.prg", true))
	//	fprintf(stderr, "Fehler beim Oeffnen der Datei\n");

	return EXIT_SUCCESS;
}

/* Berechnung der Vorwärtstransformation inklusive der Roboterbasistransformation ******/
Transform RobotPose(KinematicData robot[], Transform WK2Basis, Vector axes)
{
	return WK2Basis * ForwardKinematics(robot, axes, 0, axes.dim() - 1);
}

/* Berechnung der Vorwärtstransformation von Gelenk qi bis Gelenk qn *******************/
Transform ForwardKinematics(KinematicData robot[], Vector axes, int qi, int qn)
{
	Transform T;

	assert(qi < axes.dim() && qn < axes.dim());

	while (qi <= qn)
	{
		// Ihr Code

		++qi;
	}

	return T;
}

/* Geometrische Berechnung der inversen Kinematik **************************************/
Vector6 InverseKinematicsKR3(Transform T, KinematicData robot[], Transform WK2Basis)
{
	Vector6 axes(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

	// Ihr Code

	return axes;
}

/* Berechnet die Jakobi-Matrix mit Hilfe des Vorwärtsdifferenzenquotienten *************/
Matrix ikNumDiff(KinematicData robot[], Vector axes)
{
	Matrix J;

	// Ihr Code

	return J;
}

/* Numerische Berechnung der inversen Kinematik ****************************************/
Vector InverseKinematics(Transform T, KinematicData robot[], Transform WK2Basis, Vector axes)
{
	// Ihr Code

	return axes;
}

/* Robuste (und teure) Variante zur Ermittlung der Moore-Penrose Inversen von M ********/
Matrix ikSVD(Matrix M)
{
	const double IK_SVD_SINGULAR = 1e-10;
	Matrix U, V;
	Vector s;

	mc_svd(M, U, s, V);

	Matrix Si(V.ncol(), U.ncol());
	Si = 0;

	for (int i = std::min(Si.nrow(), Si.ncol()); i--; Si[i][i] = (s[i] < IK_SVD_SINGULAR) ? 0.0 : 1.0 / s[i]);

	return V * Si * mc_transpose(U);
}

/* Erzeugt ein EASY-ROB Programm mit einer Linearbewegung

   Parameter:
		robot[]			 kinematische Struktur des Roboters
		qs				  Startpose in Gelenkkoordinaten
		qe				  Zielpose in Gelenkkoordinaten
		steps			   Anzahl der Interpolationsschritte
		filename			Ausgabedatei
		numerical_solution  true -> numerische Berechnung der IK via InverseKinematics()
							false -> geometrischen Berechnung der IK via InverseKinematicsKR3()

   Rückgabewert:
		errno im Fehlerfall, ansonsten 0
*/
errno_t IpoLin(KinematicData robot[], Vector qs, Vector qe, int steps, const char* filename, bool numerical_solution)
{
	FILE *fp;
	errno_t err;

	assert(robot && qs.dim() == qe.dim() && steps && filename && *filename);

	if (err = fopen_s(&fp, filename, "w"))
		return err;

	Angle_axis_xyz s(ForwardKinematics(robot, qs, 0, qs.dim() - 1));
	Angle_axis_xyz e(ForwardKinematics(robot, qe, 0, qe.dim() - 1));
	Vector6 se(e - s);

	for (int i = 0; i <= steps; ++i)
	{
		Angle_axis_xyz n(Vector6(s) + Vector6(se * (i / (double)steps)));

		if (!numerical_solution)
			qs = InverseKinematicsKR3(Transform(n), robot, mc_identity(4));
		else
			qs = InverseKinematics(Transform(n), robot, mc_identity(4), qs);

		fputs("JUMP_TO_AX", fp);
		for (int j = 0; j < qs.dim(); ++j)
			fprintf(fp, " %10.4f", qs[j]);
		fputs("\n", fp);
	}

	fclose(fp);

	return 0;
}
