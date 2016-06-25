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

// $Id: rotation.h,v 1.6 2003/02/25 15:12:08 serge Exp $

/** \file
 * \author Régis Houde    \date 17 novembre 97
 */

/*---------------------------------------------------------------------------*/
/*                     	      FICHIERS D'INCLUSIONS                          */
/*---------------------------------------------------------------------------*/
#ifndef ROTATION_H
#define ROTATION_H

#include "matrix.h"
#include "position.h"

/*---------------------------------------------------------------------------*/
/*                     	      CLASSES ET DEFINES                             */
/*---------------------------------------------------------------------------*/

class Vector;		// Ces 2 classes sont étroitement reliées
class Vector3;		// Ces 2 classes sont étroitement reliées
class Transform;	// Ces 2 classes sont étroitement reliées
class Euler_angle;	// Ces 2 classes sont étroitement reliées
class Euler_angle_xyz;	// Ces 2 classes sont étroitement reliées
class Quaternion;	// Ces 2 classes sont étroitement reliées
class Quaternion_xyz;	// Ces 2 classes sont étroitement reliées
class Angle_axis;	// Ces 2 classes sont étroitement reliées
class Angle_axis_xyz;	// Ces 2 classes sont étroitement reliées
class Position;		// Ces 2 classes sont étroitement reliées

# define ROT_DIM 3

/**
 * Permet de travailler avec la classe Matrix en mode Rotation.
 * \author Régis Houde    \date 17 novembre 97
 */
class Rotation : public Matrix 
{
public:
    inline Rotation() : Matrix(ROT_DIM, ROT_DIM){};
    Rotation(const Matrix &m); 
    Rotation(const Transform &t); 
    Rotation(const Euler_angle &e); 
    Rotation(const Euler_angle_xyz &e); 
    Rotation(const Quaternion &q); 
    Rotation(const Quaternion_xyz &q); 
    Rotation(const Angle_axis &a); 
    Rotation(const Angle_axis_xyz &a); 
    inline ~Rotation() {};
	
    Rotation &ortho();			// Using mc_svd
    Rotation &ortho_fast();			// Using cross_product
    Rotation &ortho_very_fast();		// Using Angle_axis_xyz
	
    inline Matrix& print(const char *msg = NULL, const char *name = "Rotation") {return(Matrix::print(msg, name));};
	
    friend Rotation mc_inv(const Rotation&);
    friend Rotation mc_transpose(const Rotation&);
    friend Vector mc_vect(const Rotation&);
	
    friend Rotation operator * (const Rotation&, const Rotation&);
    friend Rotation operator / (const Rotation&, const Rotation&);
    friend void operator + (const Rotation&, const Rotation&);
//    inline friend Rotation operator + (const Rotation &R, double s) {return (Matrix)R + s;};
    inline friend Rotation operator * (const Rotation &R, double s) {return (Matrix)R*s;};
    inline friend Rotation operator / (double s, const Rotation &R) {return s/(Matrix)R;};
    friend Vector3 operator * (const Rotation &R, const Vector3 &V);
    inline friend Position operator * (const Rotation &R, const Position &P) {return (Position)(R*(Vector3)P);};

    friend Rotation mc_Rx(double =0.0);
    friend Rotation mc_Ry(double =0.0);
    friend Rotation mc_Rz(double =0.0);
    friend Vector mc_rz_rx_ry(const Rotation&, double&, double&, double&);
    friend Vector mc_rz_rx_ry(const Rotation&);
    friend Rotation mc_rz_rx_ry(Vector&);
    friend Vector mc_rz_ry_rx(const Rotation&, double&, double&, double&);
    friend Vector mc_rz_ry_rx(const Rotation&);
	
};

#endif // ROTATION_H

/*---------------------------------------------------------------------------*/
/*                               FIN DU FICHIER                              */
/*-------------------------------------------------------------------J.Cô .--*/
