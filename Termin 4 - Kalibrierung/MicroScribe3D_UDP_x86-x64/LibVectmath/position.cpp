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

// $Id: position.C,v 1.7 2003/03/27 18:03:07 cro Exp $

/** \file
 * Impl�mente les m�thodes de la classe Position et les fonctions
 * qui y sont reli�es. <p>
 * 
 * La classe Position est une classe d�riv�e de la classe Vector.
 * Elle supporte les cas particuliers des vecteurs-position.
 * Toutes les fonctionnalit�s de Vector sont disponibles si
 * applicables.  De plus, certaines d'entre-elles peuvent avoir �t�
 * optimis�es afin de tirer profit de la sp�cialisation du vecteur-
 * position.  Les vecteurs-position sont de longueur 3 et de forme
 * (x, y, z). Il peuvent quand m�me �tre utilis� sous forme de
 * vecteur homog�ne � facteur d'�chelle unitaire de forme (x, y, z,
 * 1). Il existe 2 sortes de vecteurs-position : les vecteurs-
 * position colonnes et les vecteurs-position lignes.  Pour
 * r�f�rencer le xe �l�ment d'une position ligne ou colonne P, on
 * notera P[x - 1] car l'indi�age d�bute � 0.
 *
 * \author R�gis Houde    \date 17 novembre 97
 */

/*---------------------------------------------------------------------------*/
/*                     	      FICHIERS D'INCLUSIONS                          */
/*---------------------------------------------------------------------------*/
#include <stdio.h>

#include "vectmath.h"

/**
 * Constructeur � partir d'une Transform. On copie les �l�ments (0, 3), (1, 3)
 * et (2, 3) de la Transform dans les �lements 0, 1 et 2 du vecteur Position.
 * \author R�gis Houde    \date 4 juin 98
 */
Position::Position(const Transform &m) : Vector3()
{  
    (*this)[POS_X] = m.get(POS_X, 3);  // last col. of a Transform matrix
    (*this)[POS_Y] = m.get(POS_Y, 3);
    (*this)[POS_Z] = m.get(POS_Z, 3);
}

/**
 * Constructeur � partir d'un Angle_axis_xyz. On copie les �l�ments 3, 4, 5 de
 * l'Angle_axis_xyz 6 ou les elements 4, 5, 6 de l'Angle_axis_xyz 7 dans les
 * �lements 0, 1 et 2 du vecteur Position.
 * \author R�gis Houde    \date 4 juin 98
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
 * Constructeur � partir d'un Euler_angle_xyz. On copie le vecteur position
 * seulement.
 * \author R�gis Houde    \date 9 d�cembre 98
 * \param e le vecteur � convertir
 */
Position::Position(const Euler_angle_xyz &e) : Vector3()
{  
    (*this)[POS_X] = e.get(EULER6_X);
    (*this)[POS_Y] = e.get(EULER6_Y);
    (*this)[POS_Z] = e.get(EULER6_Z);
}

/**
 * Constructeur � partir d'un Quaternion_xyz. On copie le vecteur position
 * seulement.
 * \author R�gis Houde    \date 9 d�cembre 98
 * \param q le vecteur � convertir
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
