/****************************************************************************
 *  MICROB (Modules Int�gr�s pour le Contr�le de ROBots)
 *  Copyright (C) 1996-2002 Hydro-Qu�bec
 *  Institut de recherche d'Hydro-Qu�bec (IREQ)
 *  1800, boul. Lionel-Boulet, Varennes (Qu�bec) Canada J3X 1S1
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

// $Id: euler_angle_xyz.C,v 1.6 2002/08/30 15:35:25 ch7905 Exp $

/** \file
 * Impl�mente les m�thodes de la classe Euler_angle_xyz et les
 * fonctions qui y sont reli�es.
 * 
 * La classe Euler_angle_xyz est une classe d�riv�e de la classe
 * Vector.  Elle supporte les cas particuliers des vecteur angles
 * d'Euler avec position.  Toutes les fonctionnalit�s de Vector
 * sont disponibles si applicables.  De plus, certaines d'entre-
 * elles peuvent avoir �t� optimis�es afin de tirer profit de la
 * sp�cialisation des Euler_angle_xyz.  Les Euler_angle_xyz sont de
 * longueur 6 et de forme (Rx, Ry, Rz, x, y, z). Il existe 2
 * sortes de vecteurs-euler_angle_xyz : les vecteurs-euler_angle_xyz
 * colonnes et les vecteurs- euler_angle_xyz lignes.  Pour r�f�rencer
 * le xe �l�ment d'un euler_angle_xyz ligne ou colonne E, on notera E[x
 * - 1] car l'indi�age d�bute � 0.
 *
 * \author R�gis Houde    \date 8 d�cembre 98
 */

/*---------------------------------------------------------------------------*/
/*                     	      FICHIERS D'INCLUSIONS                          */
/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "vectmath.h"

/*---------------------------------------------------------------------------*/
/*                               FIN DU FICHIER                              */
/*---------------------------------------------------------------------R.H.--*/
