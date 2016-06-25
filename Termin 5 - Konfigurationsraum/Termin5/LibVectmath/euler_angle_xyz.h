/****************************************************************************
 *  MICROB (Modules Intégrés pour le Contrôle de ROBots)
 *  Copyright (C) 1996-2002 Hydro-Québec
 *  Institut de recherche d'Hydro-Québec (IREQ)
 *  1800, boul. Lionel-Boulet, Varennes (Québec) Canada J3X 1S1
 *  microb@robotique.ireq.ca, http://www.robotique.ireq.ca
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 ***************************************************************************/

// $Id: euler_angle_xyz.h,v 1.7 2003/02/25 15:12:08 serge Exp $

/** \file
 * \author Jean Côté    \date 8 décembre 98
 */

/*---------------------------------------------------------------------------*/
/*                     	      FICHIERS D'INCLUSIONS                          */
/*---------------------------------------------------------------------------*/
#ifndef EULER_ANGLE_XYZ_H
#define EULER_ANGLE_XYZ_H

#include "vector6.h"

class Matrix;		// Ces 2 classes sont étroitement reliées
class Vector;		// Ces 2 classes sont étroitement reliées
class Rotation;		// Ces 2 classes sont étroitement reliées
class Transform;	// Ces 2 classes sont étroitement reliées
class Euler_angle;	// Ces 2 classes sont étroitement reliées
class Angle_axis_xyz;	// Ces 2 classes sont étroitement reliées


/**
 * Permet de remplacer les nombres (0,1,2,...) par des constantes EULER_...
 * \author Serge Sarraillon  \date 5 fevrier 2003
 */
enum EulerVector6Index { EULER6_RX, EULER6_RY, EULER6_RZ, EULER6_X, EULER6_Y, EULER6_Z, EULER6_DIM };


/*---------------------------------------------------------------------------*/
/*                     	      CLASSES ET DEFINES                             */
/*---------------------------------------------------------------------------*/

/**
 * Permet de travailler avec la classe Matrix en mode Euler_angle_xyz.
 * \author Jean Côté    \date 18 décembre 96
 */

class Euler_angle_xyz : public Vector6
{
public:
    inline Euler_angle_xyz() : Vector6() {};
    inline Euler_angle_xyz(double val) : Vector6(val) {};
    inline Euler_angle_xyz(double e1, double e2, double e3, double p1, double p2, double p3) : Vector6(e1, e2, e3, p1, p2, p3) {};	
    inline Euler_angle_xyz(const Vector &v) : Vector6(v) {}; 
    inline Euler_angle_xyz(const Vector6 &v) : Vector6(v) {};
    inline Euler_angle_xyz(const Vector3 &v) : Vector6(v.get(EULER3_RX), v.get(EULER3_RY), v.get(EULER3_RZ), 0, 0, 0) {};
    inline Euler_angle_xyz(const Euler_angle &v) : Vector6(v.get(EULER3_RX), v.get(EULER3_RY), v.get(EULER3_RZ), 0, 0, 0) {};
    inline Euler_angle_xyz(const Euler_angle &v, const Position &p) : Vector6(v.get(EULER3_RX), v.get(EULER3_RY), v.get(EULER3_RZ), p.get(POS_X), p.get(POS_Y), p.get(POS_Z)) {};
    inline Euler_angle_xyz(const Position &p) : Vector6(0, 0, 0, p.get(POS_X), p.get(POS_Y), p.get(POS_Z)) {};
    inline Euler_angle_xyz(const Matrix &m) : Vector6(m) {};
	
    inline Euler_angle_xyz(const Rotation &r): Vector6(){Euler_angle_xyz e((Euler_angle)r); (*this) = e;}; 
    inline Euler_angle_xyz(const Transform &m): Vector6(){Euler_angle_xyz e((Euler_angle)m, (Position)m); (*this) = e;}; 
    inline Euler_angle_xyz(const Quaternion &q): Vector6(){Euler_angle_xyz e((Euler_angle)q); (*this) = e;};
    inline Euler_angle_xyz(const Angle_axis_xyz &a): Vector6(){Euler_angle_xyz e((Euler_angle)a, (Position)a); (*this) = e;};
    inline Euler_angle_xyz(const Quaternion_xyz &q): Vector6(){Euler_angle_xyz e((Euler_angle)q, (Position)q); (*this) = e;};
	
    inline ~Euler_angle_xyz() {};
	
    inline friend Euler_angle_xyz operator + (const Euler_angle_xyz &E1, const Euler_angle_xyz &E2) {return (Vector6)E1 + (Vector6)E2;};
    inline friend Euler_angle_xyz operator + (double s, const Euler_angle_xyz &E) {return s + (Vector6)E;};
    inline friend Euler_angle_xyz operator + (const Euler_angle_xyz &E, double s) {return (Vector6)E + s;};
    inline friend Euler_angle_xyz operator / (double s, const Euler_angle_xyz &E) {return s/(Vector6)E;};
    inline friend Euler_angle_xyz operator / (const Euler_angle_xyz &E, double s) {return (Vector6)E/s;};
    inline friend double operator / (const Euler_angle_xyz &E1, const Euler_angle_xyz &E2) {return (Vector6)E1/(Vector6)E2;};
	
    inline Matrix& print(const char *msg = NULL, const char *name = "Euler_angle_xyz") {return(Matrix::print(msg, name));};
	
    //--------------------------------------------------------------------------
    // allow to read or set directly the value of a specific value (SSA030206)
    // as;
    //      Euler_angle_xyz  E;
    //      double           ex;
    //      ex = E.x()      // read the X value of Euler
    //      E.x() = 10.0;   // set the X value of Euler to 10.0
    //
    inline double &x()   { return (*this)[EULER6_X];  };
    inline double &y()   { return (*this)[EULER6_Y];  };
    inline double &z()   { return (*this)[EULER6_Z];  };
    inline double &rx()  { return (*this)[EULER6_RX]; };
    inline double &ry()  { return (*this)[EULER6_RY]; };
    inline double &rz()  { return (*this)[EULER6_RZ]; };

};

#endif // EULER_ANGLE_XYZ_H

/*---------------------------------------------------------------------------*/
/*                               FIN DU FICHIER                              */
/*-------------------------------------------------------------------J.Cô .--*/
