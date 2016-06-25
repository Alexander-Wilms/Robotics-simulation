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

// $Id: euler_angle.h,v 1.7 2003/02/25 15:12:08 serge Exp $

/** \file
 * \author Jean Côté    \date 18 décembre 96
 */

/*---------------------------------------------------------------------------*/
/*                     	      FICHIERS D'INCLUSIONS                          */
/*---------------------------------------------------------------------------*/

#ifndef EULER_ANGLE_H
#define EULER_ANGLE_H

#include "vector3.h"

class Matrix;		// Ces 2 classes sont étroitement reliées
class Vector;		// Ces 2 classes sont étroitement reliées
class Rotation;		// Ces 2 classes sont étroitement reliées
class Transform;	// Ces 2 classes sont étroitement reliées
class Euler_angle_xyz;	// Ces 2 classes sont étroitement reliées
class Angle_axis_xyz;	// Ces 2 classes sont étroitement reliées
class Quaternion;	// Ces 2 classes sont étroitement reliées
class Quaternion_xyz;	// Ces 2 classes sont étroitement reliées

/**
 * Permet de remplacer les nombres (0,1,2,...) par des constantes EULER_...
 * \author Serge Sarraillon  \date 7 fevrier 2003
 */

enum EulerVector3Index { EULER3_RX, EULER3_RY, EULER3_RZ, EULER3_DIM };

/*---------------------------------------------------------------------------*/
/*                     	      CLASSES ET DEFINES                             */
/*---------------------------------------------------------------------------*/

/**
 * Permet de travailler avec la classe Matrix en mode Euler_angle.
 * \author Jean Côté    \date 18 décembre 96
 */
class Euler_angle : public Vector3
{
public:
    inline Euler_angle() : Vector3() {};
    inline Euler_angle(double val) : Vector3(val) {};
    inline Euler_angle(double e1, double e2, double e3) : Vector3(e1, e2, e3) {};	
    inline Euler_angle(const Vector &v) : Vector3(v) {}; 
    inline Euler_angle(const Vector3 &v) : Vector3(v) {};
    inline Euler_angle(const Matrix &m) : Vector3(m) {};
	
    Euler_angle(const Rotation &r); 
    Euler_angle(const Transform &m); 
    Euler_angle(const Quaternion &q); 
    Euler_angle(const Quaternion_xyz &q); 
    Euler_angle(const Angle_axis_xyz &a); 
    Euler_angle(const Euler_angle_xyz &a); 
	
    inline ~Euler_angle() {};
	
    inline friend Euler_angle operator + (const Euler_angle &E1, const Euler_angle &E2) {return (Vector3)E1 + (Vector3)E2;};
    inline friend Euler_angle operator + (double s, const Euler_angle &E) {return s + (Vector3)E;};
    inline friend Euler_angle operator + (const Euler_angle &E, double s) {return (Vector3)E + s;};
    inline friend Euler_angle operator * (const Euler_angle &E, double s) {return (Vector3)E * s;};
    inline friend Euler_angle operator / (double s, const Euler_angle &E) {return s/(Vector3)E;};
    inline friend Euler_angle operator / (const Euler_angle &E, double s) {return (Vector3)E/s;};
    inline friend double operator / (const Euler_angle &E1, const Euler_angle &E2) {return (Vector3)E1/(Vector3)E2;};
	
    inline Matrix& print(const char *msg = NULL, const char *name = "Euler_angle") {return(Matrix::print(msg, name));};

    //--------------------------------------------------------------------------
    // allow to read or set directly the value of a specific value (SSA030206)
    // as;
    //      Euler_angle      E;
    //      double           ex;
    //      ex = E.x()      // read the X value of Euler
    //      E.x() = 10.0;   // set the X value of Euler to 10.0
    //
    inline double &rx() { return (*this)[EULER3_RX]; };
    inline double &ry() { return (*this)[EULER3_RY]; };
    inline double &rz() { return (*this)[EULER3_RZ]; };    
};

#endif // EULER_ANGLE_H

/*---------------------------------------------------------------------------*/
/*                               FIN DU FICHIER                              */
/*-------------------------------------------------------------------J.Cô .--*/
