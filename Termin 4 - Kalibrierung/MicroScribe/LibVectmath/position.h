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

// $Id: position.h,v 1.6 2003/02/25 15:12:08 serge Exp $

/** \file
 * \author Jean Côté    \date 18 décembre 96
 */

/*---------------------------------------------------------------------------*/
/*                     	      FICHIERS D'INCLUSIONS                          */
/*---------------------------------------------------------------------------*/
#ifndef POSITION_H
#define POSITION_H

#include "vector3.h"

class Transform;		// Ces 2 classes sont étroitement reliées
class Angle_axis_xyz;		// Ces 2 classes sont étroitement reliées
class Euler_angle_xyz;		// Ces 2 classes sont étroitement reliées
class Quaternion_xyz;		// Ces 2 classes sont étroitement reliées

/**
 * Permet de remplacer les nombres (0,1,2,...) par des constantes POS_X,POS_...
 * \author Serge Sarraillon  \date 5 fevrier 2003
 */
enum PositionVectorIndex { POS_X, POS_Y, POS_Z, POS_DIM };

/*---------------------------------------------------------------------------*/
/*                     	      CLASSES ET DEFINES                             */
/*---------------------------------------------------------------------------*/


/**
 * Permet de travailler avec la classe Matrix en mode Position.
 * \author Jean Côté   \date 18 décembre 96
 */

class Position : public Vector3 
{
public:
    inline Position() : Vector3() {};
    inline Position(double p) : Vector3(p) {};
    inline Position(double p1, double p2, double p3) : Vector3(p1, p2, p3) {};
    inline Position(const Vector &v)  : Vector3(v) {};
    inline Position(const Vector3 &v) : Vector3(v) {};
    inline Position(const Matrix &m)  : Vector3(m) {};
	
    Position(const Transform &m); 
    Position(const Angle_axis_xyz &m); 
    Position(const Euler_angle_xyz &e); 
    Position(const Quaternion_xyz &e); 
	
    inline ~Position() {};
	
    inline friend Position operator + (const Position &P1, const Position &P2) {return (Vector3)P1 + (Vector3)P2;};
    inline friend Position operator + (double s, const Position &P) {return s + (Vector3)P;};
    inline friend Position operator + (const Position &P, double s) {return (Vector3)P + s;};
    inline friend Position operator / (double s, const Position &P) {return s/(Matrix)P;};
    inline friend double   operator / (const Position &P1, const Position &P2) {return (Matrix)P1/(Matrix)P2;};
    inline friend Position operator / (const Position &P, double s) {return (Matrix)P/s;};
	
    inline Matrix& print(const char *msg = NULL, const char *name = "Position") {return(Matrix::print(msg, name));};

    //--------------------------------------------------------------------------
    // allow to read or set directly the value of a specific value (SSA030206)
    // as;
    //      Position    E;
    //      double      ex;
    //      ex = E.x()      // read the X value of Position
    //      E.x() = 10.0;   // set the X value of Position to 10.0
    //
    inline double &x()  { return (*this)[POS_X]; };
    inline double &y()  { return (*this)[POS_Y]; };
    inline double &z()  { return (*this)[POS_Z]; };

};

#endif // POSITION_H

/*---------------------------------------------------------------------------*/
/*                               FIN DU FICHIER                              */
/*-------------------------------------------------------------------J.Cô .--*/
