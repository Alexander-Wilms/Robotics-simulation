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

// $Id: angle_axis.h,v 1.6 2003/02/25 15:12:08 serge Exp $

/** \file
 * \author Régis Houde    \date 18 décembre 96
 */

/*---------------------------------------------------------------------------*/
/*                     	      FICHIERS D'INCLUSIONS                          */
/*---------------------------------------------------------------------------*/
#ifndef ANGLE_AXIS_H
#define ANGLE_AXIS_H

#include "vector4.h"

class Matrix;		// Ces 2 classes sont étroitement reliées
class Vector;		 // Ces 2 classes sont étroitement reliées
class Transform;	  // Ces 2 classes sont étroitement reliées
class Rotation;		   // Ces 2 classes sont étroitement reliées
class Euler_angle;	    // Ces 2 classes sont étroitement reliées
class Euler_angle_xyz;	// Ces 2 classes sont étroitement reliées
class Quaternion;	    // Ces 2 classes sont étroitement reliées
class Quaternion_xyz;	// Ces 2 classes sont étroitement reliées

/**
 * Permet de remplacer les nombres (0,1,2,...) par des constantes ANGLE6_TNX,...
 * \author Serge Sarraillon  \date 7 fevrier 2003
 */
enum Angle3VectorIndex { ANGLE3_TNX  , ANGLE3_TNY, ANGLE3_TNZ, ANGLE3_DIM            };
enum Angle4VectorIndex { ANGLE4_THETA, ANGLE4_NX , ANGLE4_NY , ANGLE4_NZ, ANGLE4_DIM };

/*---------------------------------------------------------------------------*/
/*                     	      CLASSES ET DEFINES                             */
/*---------------------------------------------------------------------------*/

/**
 * Permet de travailler avec la classe Matrix en mode Angle_axis.
 * \author Régis Houde     \date 18 décembre 96
 */
class Angle_axis : public Vector
{
public:
    inline Angle_axis() : Vector(ANGLE3_DIM) {(*this)[ANGLE3_TNX] = (*this)[ANGLE3_TNY] = (*this)[ANGLE3_TNZ] = 0.0;};
	
    Angle_axis(const Angle_axis &a);
    Angle_axis(const Angle_axis_xyz &a); 
    Angle_axis(const Vector &v);
    Angle_axis(const Matrix &v);
    Angle_axis(double theta, double nx, double ny, double nz);
    Angle_axis(double tnx, double tny, double tnz);
    Angle_axis(int l);
    Angle_axis(const Transform &t); 
    Angle_axis(const Rotation &r); 
    Angle_axis(const Euler_angle &e); 
    Angle_axis(const Euler_angle_xyz &e); 
    Angle_axis(const Quaternion &q); 
    Angle_axis(const Quaternion_xyz &q); 
	
    Angle_axis& operator = (const Matrix &a);
    Angle_axis& operator = (const Vector &a);
    Angle_axis& operator = (const Angle_axis &a);
    Angle_axis& operator = (const Transform &t);
    Angle_axis& operator = (const Rotation &r);
    Angle_axis& operator = (const Euler_angle &e);
    Angle_axis& operator = (const Euler_angle_xyz &e);
    Angle_axis& operator = (const Quaternion &q);
    Angle_axis& operator = (const Quaternion_xyz &q);

    friend Angle_axis operator + (const Angle_axis&, const Angle_axis&);
	
    inline ~Angle_axis() {};
		
    inline Matrix& print(const char *msg = NULL, const char *name = "Angle_axis") {return(Matrix::print(msg, name));};

    
    //--------------------------------------------------------------------------
    // allow to read or set directly the value of a specific value (SSA030206)
    // as;
    //      Angle_axis       E;
    //      double           ex;
    //      ex = E.x()      // read the X value of Euler
    //      E.x() = 10.0;   // set the X value of Euler to 10.0
    //

    inline double &tnx()   { return (*this)[ANGLE3_TNX];   };
    inline double &tny()   { return (*this)[ANGLE3_TNY];   };
    inline double &tnz()   { return (*this)[ANGLE3_TNZ];   };  
    inline double &theta() { return (*this)[ANGLE4_THETA]; };
    inline double &nx()    { return (*this)[ANGLE4_NX];    };
    inline double &ny()    { return (*this)[ANGLE4_NY];    };        
    inline double &nz()    { return (*this)[ANGLE4_NZ];    };        
};

#endif // ANGLE_AXIS_H

/*---------------------------------------------------------------------------*/
/*                               FIN DU FICHIER                              */
/*-------------------------------------------------------------------J.Cô .--*/
