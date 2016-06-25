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

// $Id: position.C,v 1.7 2003/03/27 18:03:07 cro Exp $

/** \file
 * Implémente les méthodes de la classe Position et les fonctions
 * qui y sont reliées. <p>
 * 
 * La classe Position est une classe dérivée de la classe Vector.
 * Elle supporte les cas particuliers des vecteurs-position.
 * Toutes les fonctionnalités de Vector sont disponibles si
 * applicables.  De plus, certaines d'entre-elles peuvent avoir été
 * optimisées afin de tirer profit de la spécialisation du vecteur-
 * position.  Les vecteurs-position sont de longueur 3 et de forme
 * (x, y, z). Il peuvent quand même être utilisé sous forme de
 * vecteur homogène à facteur d'échelle unitaire de forme (x, y, z,
 * 1). Il existe 2 sortes de vecteurs-position : les vecteurs-
 * position colonnes et les vecteurs-position lignes.  Pour
 * référencer le xe élément d'une position ligne ou colonne P, on
 * notera P[x - 1] car l'indiçage débute à 0.
 *
 * \author Régis Houde    \date 17 novembre 97
 */

/*---------------------------------------------------------------------------*/
/*                     	      FICHIERS D'INCLUSIONS                          */
/*---------------------------------------------------------------------------*/
#include <stdio.h>

#include "vectmath.h"

/**
 * Constructeur à partir d'une Transform. On copie les éléments (0, 3), (1, 3)
 * et (2, 3) de la Transform dans les élements 0, 1 et 2 du vecteur Position.
 * \author Régis Houde    \date 4 juin 98
 */
Position::Position(const Transform &m) : Vector3()
{  
    (*this)[POS_X] = m.get(POS_X, 3);  // last col. of a Transform matrix
    (*this)[POS_Y] = m.get(POS_Y, 3);
    (*this)[POS_Z] = m.get(POS_Z, 3);
}

/**
 * Constructeur à partir d'un Angle_axis_xyz. On copie les éléments 3, 4, 5 de
 * l'Angle_axis_xyz 6 ou les elements 4, 5, 6 de l'Angle_axis_xyz 7 dans les
 * élements 0, 1 et 2 du vecteur Position.
 * \author Régis Houde    \date 4 juin 98
 */
Position::Position(const Angle_axis_xyz &m) : Vector3()
{  
    // 6 elements vectors
    if(m.msize() == ANGLE6_DIM)
    {
        (*this)[POS_X] = m.get(ANGLE6_X);
        (*this)[POS_Y] = m.get(ANGLE6_Y);
        (*this)[POS_Z] = m.get(ANGLE6_Z);
    }
    // 7 elements vectors
    else   
    {
        (*this)[POS_X] = m.get(ANGLE7_X);
        (*this)[POS_Y] = m.get(ANGLE7_Y);
        (*this)[POS_Z] = m.get(ANGLE7_Z);
    }
}


/**
 * Constructeur à partir d'un Euler_angle_xyz. On copie le vecteur position
 * seulement.
 * \author Régis Houde    \date 9 décembre 98
 * \param e le vecteur à convertir
 */
Position::Position(const Euler_angle_xyz &e) : Vector3()
{  
    (*this)[POS_X] = e.get(EULER6_X);
    (*this)[POS_Y] = e.get(EULER6_Y);
    (*this)[POS_Z] = e.get(EULER6_Z);
}

/**
 * Constructeur à partir d'un Quaternion_xyz. On copie le vecteur position
 * seulement.
 * \author Régis Houde    \date 9 décembre 98
 * \param q le vecteur à convertir
 */
Position::Position(const Quaternion_xyz &q) : Vector3()
{  
    (*this)[POS_X] = q.get(QUAT7_X);
    (*this)[POS_Y] = q.get(QUAT7_Y);
    (*this)[POS_Z] = q.get(QUAT7_Z);
}

/*---------------------------------------------------------------------------*/
/*                               FIN DU FICHIER                              */
/*---------------------------------------------------------------------R.H.--*/
