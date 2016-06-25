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

// $Id: vector4.h,v 1.5 2002/08/30 15:35:33 ch7905 Exp $

/** \file
 * \author Régis Houde    \date 23 janvier 98
 */

/*---------------------------------------------------------------------------*/
/*                     	      FICHIERS D'INCLUSIONS                          */
/*---------------------------------------------------------------------------*/
#ifndef VECTOR4_H
#define VECTOR4_H

#include "vector.h"
#include "transform.h"

/*---------------------------------------------------------------------------*/
/*                     	      CLASSES ET DEFINES                             */
/*---------------------------------------------------------------------------*/

/**
 * Permet de travailler avec la classe Matrix en mode Vector4.
 * \author Régis Houde    \date 23 janvier 1998
 */
class Vector4 : public Vector 
{
public:
    inline Vector4() : Vector(4) {};
    inline Vector4(double d) : Vector(4) {(*this)[0] = (*this)[1] = (*this)[2] = (*this)[3] = d;};
    Vector4(double v1, double v2, double v3, double v4); 
    Vector4(const Vector4 &m); 
    Vector4(const Vector &v); 
    Vector4(const Matrix &m); 
    inline ~Vector4() {};
	
    inline Matrix& print(const char *msg = NULL, const char *name = "Vector4") {return(Matrix::print(msg, name));};
	
	
};

// For homogeneous vector
extern Vector4 mc_vectorial_product(const Vector4 &a, const Vector4 &b);

#endif // VECTOR4_H

/*---------------------------------------------------------------------------*/
/*                               FIN DU FICHIER                              */
/*-------------------------------------------------------------------J.Cô .--*/
