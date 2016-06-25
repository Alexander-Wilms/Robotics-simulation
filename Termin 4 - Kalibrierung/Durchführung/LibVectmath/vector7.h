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

// $Id: vector7.h,v 1.5 2002/08/30 15:35:33 ch7905 Exp $

/** \file
 * \author Régis Houde    \date 8 décembre 1998
 */

/*---------------------------------------------------------------------------*/
/*                     	      FICHIERS D'INCLUSIONS                          */
/*---------------------------------------------------------------------------*/
#ifndef VECTOR7_H
#define VECTOR7_H

#include "vector.h"
#include "transform.h"

/*---------------------------------------------------------------------------*/
/*                     	      CLASSES ET DEFINES                             */
/*---------------------------------------------------------------------------*/

/**
 * Permet de travailler avec la classe Matrix en mode Vector7.
 * \author Régis Houde    \date 8 décembre 1998
 */
class Vector7 : public Vector 
{
public:
    inline Vector7() : Vector(7) {};
    inline Vector7(double d) : Vector(7) {(*this)[0] = (*this)[1] = (*this)[2] = (*this)[3] = (*this)[4] = (*this)[5] = (*this)[6] = d;};
    Vector7(double v1, double v2, double v3, double v4, double v5, double v6, double v7); 
    Vector7(const Vector7 &m); 
    Vector7(const Vector &v); 
    Vector7(const Matrix &m); 
    inline ~Vector7() {};
	
    inline Matrix& print(const char *msg = NULL, const char *name = "Vector7") {return(Matrix::print(msg, name));};
	
};

#endif // VECTOR7_H

/*---------------------------------------------------------------------------*/
/*                               FIN DU FICHIER                              */
/*-------------------------------------------------------------------J.Cô .--*/
