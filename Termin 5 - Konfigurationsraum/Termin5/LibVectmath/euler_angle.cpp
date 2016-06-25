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

// $Id: euler_angle.C,v 1.9 2003/02/25 15:12:08 serge Exp $

/** \file
 * Implémente les méthodes de la classe Euler_angle et les
 * fonctions qui y sont reliées. <p>
 * 
 * La classe Euler_angle est une classe dérivée de la classe
 * Vector.  Elle supporte les cas particuliers des angles d'Euler.
 * Toutes les fonctionnalités de Vector sont disponibles si
 * applicables.  De plus, certaines d'entre-elles peuvent avoir été
 * optimisées afin de tirer profit de la spécialisation des angles
 * d'euler.  Les Euler_angles sont de longueur 3 et de forme (Rx,
 * Ry, Rz). Il existe 2 sortes de vecteurs-euler_angle : les
 * vecteurs-euler_angle colonnes et les vecteurs- euler_angle
 * lignes.  Pour référencer le xe élément d'un euler_angle ligne ou
 * colonne E, on notera E[x - 1] car l'indiçage débute à 0..
 *
 * \author Régis Houde    \date 17 novembre 97
 */

/*---------------------------------------------------------------------------*/
/*                     	      FICHIERS D'INCLUSIONS                          */
/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "vectmath.h"

/**
 * Constructeur à partir d'une Rotation.
 * <pre>
 * Si abs(r[2][0]) == 1
 * 
 *   	E = [0, atan2(r[2][0], 0), atan2(r[1][2], r[0][2])]
 * Sinon
 * 	E = [atan2(sx,cx), atan2(sy,cy), atan2(sz,cz)]
 * 	
 * 	où 	sx = r[2][1]/cy
 * 		cx = r[2][2]/cy
 * 		sz = r[1][0]/cy
 * 		cz = r[0][0]/cy
 * 		cy = sqrt(r[2][1]*r[2][1] + r[2][2]*r[2][2])
 * </pre>
 *
 * \author Régis Houde    \date 4 juin 98
 * \param r la matrice Rotation à convertir en Euler_angle
 */
Euler_angle::Euler_angle(const Rotation &r) : Vector3()
{
#ifdef XYZ_EULER_ANGLES

    // Easy rob version
	double cx,sx,cy,sy,cz,sz,czx,szx;

    sy = r.get(0, 2);
    if(fabs(sy) == 1.0)
    {
        cy  = 0.0;
        czx = -r.get(2, 0); // czx =  r.get(2, 0);
        szx =  r.get(2, 1); // szx = -r.get(2, 1);
        if(sy < 0) 
		{
			czx = -czx;
			szx = -szx;
		}
        (*this)[EULER3_RX] = atan2(szx,czx);
        (*this)[EULER3_RY] = atan2(sy,cy);
        (*this)[EULER3_RZ] = 0;
    }
    else
    {
        cy = sqrt(r.get(1, 2)*r.get(1, 2) + r.get(2, 2)*r.get(2, 2));
        sx = -r.get(1, 2)/cy;
        cx =  r.get(2, 2)/cy;
        sz = -r.get(0, 1)/cy;
        cz =  r.get(0, 0)/cy;
        (*this)[EULER3_RX] = atan2(sx,cx);
        (*this)[EULER3_RY] = atan2(sy,cy);
        (*this)[EULER3_RZ] = atan2(sz,cz);
    }

#else

	// Original MICROB
	double cx,sx,cy,sy,cz,sz,cxz,sxz;

    sy = -r.get(2, 0);
    if(fabs(sy) == 1.0)
    {
        cy  = 0.0;
        cxz = r.get(0, 2);
        sxz = r.get(1, 2);
        if(sy < 0) 
		{
			cxz = -cxz;
			sxz = -sxz;
		}
        (*this)[EULER3_RX] = 0;
        (*this)[EULER3_RY] = atan2(sy,cy);
        (*this)[EULER3_RZ] = atan2(sxz,cxz);
    }
    else
    {
        cy = sqrt(r.get(2, 1)*r.get(2, 1) + r.get(2, 2)*r.get(2, 2));
        sx = r.get(2, 1)/cy;
        cx = r.get(2, 2)/cy;
        sz = r.get(1, 0)/cy;
        cz = r.get(0, 0)/cy;
        (*this)[EULER3_RX] = atan2(sx,cx);
        (*this)[EULER3_RY] = atan2(sy,cy);
        (*this)[EULER3_RZ] = atan2(sz,cz);
    }

#endif	// XYZ_EULER_ANGLES
}

/**
 * Constructeur à partir d'une Transform.
 * \author Régis Houde    \date 4 juin 98
 * \param t La Transform à convertir en Euler_angle
 */
Euler_angle::Euler_angle(const Transform &t) : Vector3()
{  
    (*this) = (Rotation) t;
}

/**
 * Constructeur à partir d'un Quaternion.
 * \author Régis Houde    \date 4 juin 98
 * \param q Le Quaternion à convertir en Euler_angle
 * \bug Devrait être optimisé pour ne pas avoir à convertir à une rotation.
 */
Euler_angle::Euler_angle(const Quaternion &q) : Vector3()
{  
    (*this) = (Rotation) q;
}

/**
 * Constructeur à partir d'un Quaternion_xyz.
 * \author Régis Houde    \date 4 juin 98
 * \param q Le Quaternion_xyz à convertir en Euler_angle
 * \bug Devrait être optimisé pour ne pas avoir à convertir à une rotation.
 */
Euler_angle::Euler_angle(const Quaternion_xyz &q) : Vector3()
{  
    (*this) = (Rotation) q;
}

/**
 * Constructeur à partir d'un Angle_axis_xyz.
 * \author Régis Houde    \date 4 juin 98
 * \param a Le Angle_axis_xyz à convertir en Euler_angle
 * \bug Devrait être optimisé pour ne pas avoir à convertir à une rotation.
 */
Euler_angle::Euler_angle(const Angle_axis_xyz &a) : Vector3()
{  
    (*this) = (Rotation) a;
}

/**
 * Constructeur à partir d'un Euler_angle_xyz. On copie terme à terme les 3
 * premiers éléments.
 * \author Régis Houde    \date 8 décembre 98
 * \param e Le Euler_angle_xyz à convertir en Euler_angle
 */
Euler_angle::Euler_angle(const Euler_angle_xyz &e) : Vector3()
{  
    (*this)[EULER3_RX] = e.get(EULER6_RX);
    (*this)[EULER3_RY] = e.get(EULER6_RY);
    (*this)[EULER3_RZ] = e.get(EULER6_RZ);
}

/*---------------------------------------------------------------------------*/
/*                               FIN DU FICHIER                              */
/*---------------------------------------------------------------------R.H.--*/
