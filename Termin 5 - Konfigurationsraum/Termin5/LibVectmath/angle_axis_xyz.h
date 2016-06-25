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

// $Id: angle_axis_xyz.h,v 1.6 2003/02/25 15:12:08 serge Exp $

/** \file
 * \author Régis Houde    \date 18 décembre 96
 */

/*---------------------------------------------------------------------------*/
/*                     	      FICHIERS D'INCLUSIONS                          */
/*---------------------------------------------------------------------------*/
#ifndef ANGLE_AXIS_XYZ_H
#define ANGLE_AXIS_XYZ_H

#include "vector4.h"

class Matrix;		// Ces 2 classes sont étroitement reliées
class Vector;		// Ces 2 classes sont étroitement reliées
class Transform;	// Ces 2 classes sont étroitement reliées
class Rotation;		// Ces 2 classes sont étroitement reliées
class Euler_angle;	// Ces 2 classes sont étroitement reliées
class Euler_angle_xyz;	// Ces 2 classes sont étroitement reliées
class Quaternion;	// Ces 2 classes sont étroitement reliées
class Quaternion_xyz;	// Ces 2 classes sont étroitement reliées

/**
 * Permet de remplacer les nombres (0,1,2,...) par des constantes ANGLE6_TNX,...
 * \author Serge Sarraillon  \date 5 fevrier 2003
 */
enum Angle6VectorIndex { ANGLE6_TNX  , ANGLE6_TNY, ANGLE6_TNZ, ANGLE6_X, ANGLE6_Y, ANGLE6_Z, ANGLE6_DIM };
enum Angle7VectorIndex { ANGLE7_THETA, ANGLE7_NX , ANGLE7_NY , ANGLE7_NZ, ANGLE7_X, ANGLE7_Y, ANGLE7_Z, ANGLE7_DIM };


/*---------------------------------------------------------------------------*/
/*                     	      CLASSES ET DEFINES                             */
/*---------------------------------------------------------------------------*/

/**
 * Permet de travailler avec la classe Matrix en mode Angle_axis_xyz.
 * \author Régis Houde    \date 18 décembre 96
 */
class Angle_axis_xyz : public Vector
{
public:
    inline Angle_axis_xyz() : Vector(ANGLE6_DIM)
    {
        (*this)[ANGLE6_TNX] = (*this)[ANGLE6_TNY] = (*this)[ANGLE6_TNZ] =
        (*this)[ANGLE6_X]   = (*this)[ANGLE6_Y]   = (*this)[ANGLE6_Z]   = 0.0;
    };
	
    Angle_axis_xyz(const Angle_axis_xyz &a);
    Angle_axis_xyz(const Vector &v);
    Angle_axis_xyz(const Matrix &v);
    Angle_axis_xyz(double theta, double nx, double ny, double nz);
    Angle_axis_xyz(double tnx, double tny, double tnz);
    Angle_axis_xyz(double theta, double nx, double ny, double nz, double p1, double p2, double p3);
    Angle_axis_xyz(double tnx, double tny, double tnz, double p1, double p2, double p3);
    Angle_axis_xyz(int l);
    Angle_axis_xyz(const Transform &t); 
    Angle_axis_xyz(const Rotation &r); 
    Angle_axis_xyz(const Euler_angle &e); 
    Angle_axis_xyz(const Euler_angle_xyz &e); 
    Angle_axis_xyz(const Quaternion &q); 
    Angle_axis_xyz(const Quaternion_xyz &q); 
	
    Angle_axis_xyz& operator = (const Matrix &a);
    Angle_axis_xyz& operator = (const Vector &a);
    Angle_axis_xyz& operator = (const Angle_axis_xyz &a);
    Angle_axis_xyz& operator = (const Transform &t);
    Angle_axis_xyz& operator = (const Rotation &r);
    Angle_axis_xyz& operator = (const Euler_angle &e);
    Angle_axis_xyz& operator = (const Euler_angle_xyz &e);
    Angle_axis_xyz& operator = (const Quaternion &q);
    Angle_axis_xyz& operator = (const Quaternion_xyz &q);

    friend Angle_axis_xyz operator + (const Angle_axis_xyz&, const Angle_axis_xyz&);
	
    inline ~Angle_axis_xyz() {};
		
    inline Matrix& print(const char *msg = NULL, const char *name = "Angle_axis_xyz") {return(Matrix::print(msg, name));};
		
  //--------------------------------------------------------------------------
    // allow to read or set directly the value of a specific value (SSA030206)
    // as;
    //      Angle_axis_xyz   E;
    //      double           ex;
    //      ex = E.x()      // read the X value of Euler
    //      E.x() = 10.0;   // set the X value of Euler to 10.0
    //

    inline double &tnx()   { return (*this)[ANGLE6_TNX];   };
    inline double &tny()   { return (*this)[ANGLE6_TNY];   };
    inline double &tnz()   { return (*this)[ANGLE6_TNZ];   };  
    inline double &theta() { return (*this)[ANGLE7_THETA]; };
    inline double &nx()    { return (*this)[ANGLE7_NX];    };
    inline double &ny()    { return (*this)[ANGLE7_NY];    };        
    inline double &nz()    { return (*this)[ANGLE7_NZ];    };  
    inline double &x()
    {
        if (size == ANGLE6_DIM)  return (*this)[ANGLE6_X];
        else                     return (*this)[ANGLE7_X];
    };
    inline double &y()
    { 
        if (size == ANGLE6_DIM)  return (*this)[ANGLE6_Y];
        else                     return (*this)[ANGLE7_Y];
    };
    inline double &z()
    {
        if (size == ANGLE6_DIM)  return (*this)[ANGLE6_Z];
        else                     return (*this)[ANGLE7_Z];
    };

};

// Angle_axis_xyz actuel s'appelera bientot Angle_axis_xyz_xyz	
// On fait un typdef afin de faire le changement
// en douceur
typedef Angle_axis_xyz Angle_axis_xyz_xyz;

#endif // ANGLE_AXIS_XYZ_H

/*---------------------------------------------------------------------------*/
/*                               FIN DU FICHIER                              */
/*-------------------------------------------------------------------J.Cô .--*/
