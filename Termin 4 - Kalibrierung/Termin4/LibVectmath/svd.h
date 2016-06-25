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

// $Id: svd.h,v 1.5 2002/08/30 15:35:30 ch7905 Exp $

/** \file
 * \author Regis Houde    \date 20 septembre 95
 */

#ifndef	SVD_H
#define SVD_H 

/*---------------------------------------------------------------------------*/
/*                     	      FICHIERS D'INCLUSIONS                          */
/*---------------------------------------------------------------------------*/
#include "matrix.h"
#include "vector.h"

/*---------------------------------------------------------------------------*/
/*                    PREDECLARATION DES PROCEDURES PUBLIQUES                */
/*---------------------------------------------------------------------------*/
void mc_svd(Matrix &X, Matrix &U, Vector &S, Matrix &V);
void mc_svbksb(Matrix &U, Vector &W, Matrix &V, Vector &B, Vector &X);

#endif 	/* SVD_H */

/*---------------------------------------------------------------------------*/
/*                               FIN DU FICHIER                              */
/*---------------------------------------------------------------------R.H.--*/
