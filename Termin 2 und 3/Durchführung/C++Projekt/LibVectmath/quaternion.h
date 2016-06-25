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

// $Id: quaternion.h,v 1.7 2003/03/31 20:36:21 regie Exp $

/** \file
* \author Régis Houde    \date 18 décembre 96
*/

/*---------------------------------------------------------------------------*/
/*                     	      FICHIERS D'INCLUSIONS                          */
/*---------------------------------------------------------------------------*/
#ifndef QUATERNION_H
#define QUATERNION_H

#include "vector4.h"
#include "quaternion_xyz.h"

class Matrix;		// Ces 2 classes sont étroitement reliées
class Vector;		// Ces 2 classes sont étroitement reliées
class Rotation;		// Ces 2 classes sont étroitement reliées
class Transform;	// Ces 2 classes sont étroitement reliées
class Euler_angle;	// Ces 2 classes sont étroitement reliées
class Euler_angle_xyz;	// Ces 2 classes sont étroitement reliées
class Angle_axis_xyz;	// Ces 2 classes sont étroitement reliées
class Quaternion_xyz;	// Ces 2 classes sont étroitement reliées

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
enum QuaternionVector4Index { QUAT4_QW, QUAT4_QX, QUAT4_QY, QUAT4_QZ, QUAT4_X, QUAT4_Y, QUAT4_Z, QUAT4_DIM };


/*---------------------------------------------------------------------------*/
/*                     	      CLASSES ET DEFINES                             */
/*---------------------------------------------------------------------------*/

/**
* Permet de travailler avec la classe Matrix en mode Quaternion.
* \author Régis Houde    \date 18 décembre 96
*/
class Quaternion : public Vector4
{
public:
    inline Quaternion() : Vector4() {};
    inline Quaternion(double qw, double qx, double qy, double qz) : Vector4(qw, qx, qy, qz) {}; 
    inline Quaternion(const Vector &v) : Vector4(v) {}; 
    inline Quaternion(const Vector4 &v) : Vector4(v) {}; 
    inline Quaternion(const Matrix &m) : Vector4(m) {};
    inline Quaternion(const Quaternion_xyz &q) : Vector4(q.get(QUAT7_QW), q.get(QUAT7_QX), q.get(QUAT7_QY), q.get(QUAT7_QZ)) {};
    
    Quaternion(const Transform &t); 
    Quaternion(const Rotation &r); 
    Quaternion(const Euler_angle &e); 
    Quaternion(const Euler_angle_xyz &e); 
    Quaternion(const Angle_axis_xyz &a); 
    
    inline ~Quaternion() {};
    
    Quaternion &unit();
    double norm() const;	
    double abs() const;	
    
    Quaternion derivative(Vector3 w);
    Vector3 angular_velocity(Quaternion q);
    
    inline Matrix& print(const char *msg = NULL, const char *name = "Quaternion") {return(Matrix::print(msg, name));};
    
    friend Quaternion operator * (const Quaternion&, const Quaternion&);	
    friend Quaternion operator / (const Quaternion&, const Quaternion&);	
//    inline friend Quaternion operator + (const Quaternion &Q1, const Quaternion &Q2) {return (Vector4)Q1 + (Vector4)Q2;};
//    inline friend Quaternion operator + (double s, const Quaternion &Q) {return s + (Vector4)Q;};
//    inline friend Quaternion operator + (const Quaternion &Q, double s) {return (Vector4)Q + s;};
    inline friend Quaternion operator / (double s, const Quaternion &Q) {return s/(Matrix)Q;};
    friend Quaternion mc_conjugate(const Quaternion&);	
    friend Quaternion mc_inv(const Quaternion&);	
    friend Quaternion mc_square(const Quaternion&);	
    
    //--------------------------------------------------------------------------
    // allow to read or set directly the value of a specific value (SSA030206)
    // as;
    //      Quaternion_xyz    E;
    //      double            ex;
    //      ex = E.x()           // read the X value of Quaternion_xyz
    //      E.x() = 10.0;        // set  the X value of Quaternion_xyz to 10.0
    //
    inline double &qw()  { return (*this)[QUAT4_QW]; };
    inline double &qx()  { return (*this)[QUAT4_QX]; };
    inline double &qy()  { return (*this)[QUAT4_QY]; };
    inline double &qz()  { return (*this)[QUAT4_QZ]; };
    
};

#endif // QUATERNION_H

/*---------------------------------------------------------------------------*/
/*                               FIN DU FICHIER                              */
/*-------------------------------------------------------------------J.Cô .--*/
