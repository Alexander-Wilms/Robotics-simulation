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

// $Id: quaternion_xyz.h,v 1.6 2003/02/25 15:12:08 serge Exp $

/** \file
 * \author Régis Houde    \date 9 décembre 98
 */

/*---------------------------------------------------------------------------*/
/*                     	      FICHIERS D'INCLUSIONS                          */
/*---------------------------------------------------------------------------*/
#ifndef QUATERNION_XYZ_H
#define QUATERNION_XYZ_H

#include "vector4.h"
#include "vector7.h"
#include "position.h"

class Matrix;		// Ces 2 classes sont étroitement reliées
class Vector;		// Ces 2 classes sont étroitement reliées
class Position;		// Ces 2 classes sont étroitement reliées
class Rotation;		// Ces 2 classes sont étroitement reliées
class Transform;	// Ces 2 classes sont étroitement reliées
class Euler_angle;	// Ces 2 classes sont étroitement reliées
class Euler_angle_xyz;	// Ces 2 classes sont étroitement reliées
class Angle_axis_xyz;	// Ces 2 classes sont étroitement reliées
class Quaternion;	// Ces 2 classes sont étroitement reliées

/*---------------------------------------------------------------------------
  Quaternions extend the concept of rotation in three dimensions to
  rotation in four dimensions. This avoids the problem of "gimbal-lock"
  and allows for the implementation of smooth and continuous rotation.

  In effect, they may be considered to add a additional rotation angle
  to spherical coordinates ie. Longitude, Latitude and Rotation angles

  A Quaternion is defined using four floating point values |w x y z|.

  These are calculated from the combination of the three coordinates
  of the rotation axis and the rotation angle.

  Given the rotation axis and angle, the following algorithm may be
  used to generate a quaternion:


    sin_a = sin( angle / 2 )
    cos_a = cos( angle / 2 )

    q0 = qw    = cos_a
    q1 = qx    = axis -> x / sin_a
    q2 = qy    = axis -> y / sin_a
    q3 = qz    = axis -> z / sin_a
 
  ---------------------------------------------------------------------------*/

/**
 * Permet de remplacer les nombres (0,1,2,...) par des constantes QUAT_QX,...
 * \author Serge Sarraillon  \date 5 fevrier 2003
 */
enum QuaternionVector7Index { QUAT7_QW, QUAT7_QX, QUAT7_QY, QUAT7_QZ, QUAT7_X, QUAT7_Y, QUAT7_Z, QUAT7_DIM };


/*---------------------------------------------------------------------------*/
/*                     	      CLASSES ET DEFINES                             */
/*---------------------------------------------------------------------------*/

/**
 * Permet de travailler avec la classe Matrix en mode Quaternion_xyz.
 * \author Régis Houde    \date 18 décembre 96
 */
class Quaternion_xyz : public Vector7
{
public:
    inline Quaternion_xyz() : Vector7() {};
    inline Quaternion_xyz(double qw, double qx, double qy, double qz, double x, double y, double z) : Vector7(QUAT7_QW, QUAT7_QX, QUAT7_QY, QUAT7_QZ, QUAT7_X, QUAT7_Y, QUAT7_Z) {}; 
    inline Quaternion_xyz(const Vector &v)   : Vector7(v) {}; 
    inline Quaternion_xyz(const Vector7 &v)  : Vector7(v) {}; 
    inline Quaternion_xyz(const Matrix &m)   : Vector7(m) {};
    inline Quaternion_xyz(const Position &p) : Vector7(0, 0, 0, 0, p.get(POS_X), p.get(POS_Y), p.get(POS_Z)) {};

    Quaternion_xyz(const Transform &t); 
    Quaternion_xyz(const Rotation &r); 
    Quaternion_xyz(const Euler_angle &e); 
    Quaternion_xyz(const Euler_angle_xyz &e); 
    Quaternion_xyz(const Angle_axis_xyz &a); 
    Quaternion_xyz(const Quaternion &q);
    Quaternion_xyz(const Quaternion &q, const Position &p);
	
    inline ~Quaternion_xyz() {};
	
    Quaternion_xyz &unit();
    double norm() const;	
    double abs() const;	
	
    inline Matrix& print(const char *msg = NULL, const char *name = "Quaternion_xyz") {return(Matrix::print(msg, name));};
	
    friend Quaternion_xyz operator * (const Quaternion_xyz&, const Quaternion_xyz&);	
    friend Quaternion_xyz operator / (const Quaternion_xyz&, const Quaternion_xyz&);	
//    inline friend Quaternion_xyz operator + (const Quaternion_xyz &Q1, const Quaternion_xyz &Q2) {return (Vector7)Q1 + (Vector7)Q2;};
//    inline friend Quaternion_xyz operator + (double s, const Quaternion_xyz &Q) {return s + (Vector7)Q;};
//    inline friend Quaternion_xyz operator + (const Quaternion_xyz &Q, double s) {return (Vector7)Q + s;};
    // La ligne suivante est commente car conflit dans transform.h
    //inline friend Quaternion_xyz operator + (const Quaternion &Q, const Position &P) {return Quaternion_xyz(Q, P);};
    inline friend Quaternion_xyz operator / (double s, const Quaternion_xyz &Q) {return s/(Matrix)Q;};
    friend Quaternion_xyz mc_conjugate(const Quaternion_xyz&);	
    friend Quaternion_xyz mc_inv(const Quaternion_xyz&);	
    friend Quaternion_xyz mc_square(const Quaternion_xyz&);	


    //--------------------------------------------------------------------------
    // allow to read or set directly the value of a specific value (SSA030206)
    // as;
    //      Quaternion_xyz    E;
    //      double            ex;
    //      ex = E.x()           // read the X value of Quaternion_xyz
    //      E.x() = 10.0;        // set  the X value of Quaternion_xyz to 10.0
    //
    inline double &qw()  { return (*this)[QUAT7_QW]; };
    inline double &qx()  { return (*this)[QUAT7_QX]; };
    inline double &qy()  { return (*this)[QUAT7_QY]; };
    inline double &qz()  { return (*this)[QUAT7_QZ]; };
    inline double &x()   { return (*this)[QUAT7_X];  };
    inline double &y()   { return (*this)[QUAT7_Y];  };
    inline double &z()   { return (*this)[QUAT7_Z];  };
	
};

#endif // QUATERNION_XYZ_H

/*---------------------------------------------------------------------------*/
/*                               FIN DU FICHIER                              */
/*-------------------------------------------------------------------J.Cô .--*/
