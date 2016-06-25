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

// $Id: transform.h,v 1.6 2003/02/25 15:12:08 serge Exp $

/** \file
 * \author Régis Houde    \date 17 novembre 97
 */

/*---------------------------------------------------------------------------*/
/*                     	      FICHIERS D'INCLUSIONS                          */
/*---------------------------------------------------------------------------*/
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "matrix.h"

/*---------------------------------------------------------------------------*/
/*                     	      CLASSES ET DEFINES                             */
/*---------------------------------------------------------------------------*/

class Vector;		// Ces 2 classes sont étroitement reliées
class Vector4;		// Ces 2 classes sont étroitement reliées
class Rotation;		// Ces 2 classes sont étroitement reliées
class Position;		// Ces 2 classes sont étroitement reliées
class Euler_angle;	// Ces 2 classes sont étroitement reliées
class Euler_angle_xyz;	// Ces 2 classes sont étroitement reliées
class Quaternion;	// Ces 2 classes sont étroitement reliées
class Quaternion_xyz;	// Ces 2 classes sont étroitement reliées
class Angle_axis_xyz;	// Ces 2 classes sont étroitement reliées

/**
 * Permet de remplacer les nombres (0,1,2,...) par des constantes TRANSFORM_X,...
 * \author Serge Sarraillon  \date 5 fevrier 2003
 */
enum TransformPositionIndex { TRANSFORM_X, TRANSFORM_Y, TRANSFORM_Z };


/**
 * Permet de travailler avec la classe Matrix en mode Transform.
 * \author Régis Houde    \date 17 novembre 97
 */
class Transform : public Matrix 
{
public:
    Transform();
    Transform(const Matrix &m); 
    Transform(const Rotation &m); 
    Transform(const Position &p); 
    Transform(const Euler_angle &e); 
    Transform(const Euler_angle_xyz &e); 
    Transform(const Quaternion &q); 
    Transform(const Quaternion_xyz &q); 
    Transform(const Angle_axis_xyz &a); 
    inline ~Transform() {};
	
    Transform &ortho();	// Using mc_svd
    Transform &ortho_fast();  // Using cross_product
    Transform &ortho_very_fast();  // Using Angle_axis_xyz	
	
    inline Matrix& print(const char *msg = NULL, const char *name = "Transform") {return(Matrix::print(msg, name));};
	
    friend Transform mc_inv(const Transform&);
	
    friend Transform operator * (const Transform&, const Transform&);
    friend Position operator * (const Transform&, const Position&);
    friend Vector4 operator * (const Transform&, const Vector4&);
    friend Transform operator / (const Transform&, const Transform&);
    friend Transform operator + (const Rotation&, const Position&);
    friend Transform operator + (const Euler_angle&, const Position&);
    friend Transform operator + (const Quaternion&, const Position&);
    friend Transform operator + (const Angle_axis_xyz&, const Position&);
//    inline friend Transform operator + (const Transform &T, double s) {return (Matrix)T + s;};
    inline friend Transform operator * (const Transform &T, double s) {return (Matrix)T*s;};
    inline friend Transform operator / (double s, const Transform &T) {return s/(Matrix)T;};
	
    friend Transform mc_rz_ry_rx(Vector&);
    friend Transform mc_rz_rx_ry(double =0.0, double =0.0, double =0.0, double =0.0, double =0.0, double =0.0);
    friend Transform mc_rz_ry_rx(double =0.0, double =0.0, double =0.0, double =0.0, double =0.0, double =0.0);
	friend Transform mc_rx_ry_rz(double =0.0, double =0.0, double =0.0, double =0.0, double =0.0, double =0.0);
	friend Vector mc_rz_rx_ry(const Transform&, double&, double&, double&);
    friend Vector mc_rz_rx_ry(const Transform&);
    friend Transform mc_rz_rx_ry(Vector&, Vector&);
    friend Vector mc_rz_ry_rx(const Transform&, double&, double&, double&);
    friend Vector mc_rz_ry_rx(const Transform&);
    friend Transform mc_rz_ry_rx(Vector&, Vector&);
	
    friend Transform mc_Derive(Transform &t);
    friend Transform mc_referential(Transform&, Transform&);
    friend Transform mc_division_mes_in_base_ref(Transform&, Transform&);

    friend Transform mc_match(const Matrix &m1, const Matrix &m2, int *rank = 0, double *condition_number = 0);
};

#endif // TRANSFORM_H

/*---------------------------------------------------------------------------*/
/*                               FIN DU FICHIER                              */
/*-------------------------------------------------------------------J.Cô .--*/
