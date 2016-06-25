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

// $Id: quaternion_xyz.C,v 1.6 2003/02/25 15:12:08 serge Exp $

/** \file
 * Implémente les méthodes de la classe Quaternion_xyz et les fonctions
 * qui y sont reliées. <p>
 * 
 * La classe Quaternion_xyz est une classe dérivée de la classe Vector.
 * Elle supporte les cas particuliers des vecteurs-quaternion_xyz.
 * Toutes les fonctionnalités de Vector sont disponibles si
 * applicables.  De plus, certaines d'entre-elles peuvent avoir été
 * optimisées afin de tirer profit de la spécialisation du vecteur-
 * quaternion_xyz.  Les vecteurs-quaternion_xyz sont de longueur 7 et de
 * forme; qw (q0), qx (q1), qy (q2), (q3) qz, x, y, z). Il existe 2 sortes de vecteurs-
 * quaternion_xyz : les vecteurs-quaternion_xyz colonnes et les vecteurs-
 * quaternion_xyz lignes.  Pour référencer le xe élément d'un
 * quaternion_xyz ligne ou colonne P, on notera P[x - 1] car l'indiçage
 * débute à 0.
 * 
 * Note : Le Quaternion_xyz est l'équivalent du Quaternion auquel
 * on ajoute une position.
 *
 * \author Régis Houde    \date 17 novembre 97
 */

/*---------------------------------------------------------------------------*/
/*                     	      FICHIERS D'INCLUSIONS                          */
/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include <iostream>
#include "vectmath.h"

/**
 * Constructeur à partir d'une Transform.
 * \author Régis Houde    \date 9 décembre 98
 * \param t la Transform à convertir
 */
Quaternion_xyz::Quaternion_xyz(const Transform &t) : Vector7()
{  
    (*this) = (Quaternion)t;
    (*this)[QUAT7_X] = t.get(TRANSFORM_X, 3);
    (*this)[QUAT7_Y] = t.get(TRANSFORM_Y, 3);
    (*this)[QUAT7_Z] = t.get(TRANSFORM_Z, 3);
}

/**
 * Constructeur à partir d'une Rotation.
 * \author Régis Houde    \date 9 décembre 98
 * \param r la Rotation à convertir
 */
Quaternion_xyz::Quaternion_xyz(const Rotation &r) : Vector7()
{
    (*this) = (Quaternion) r;
}

/**
 * Constructeur à partir d'un Euler_angle.
 * \author Régis Houde    \date 9 décembre 98
 * \param e le Euler_angle à convertir
 */
Quaternion_xyz::Quaternion_xyz(const Euler_angle &e) : Vector7()
{
    (*this) = (Quaternion) e;
  
}

/**
 * Constructeur à partir d'un Euler_angle_xyz.
 * \author Régis Houde    \date 8 décembre 98
 * \param e le Euler_angle_xyz à convertir
 */
Quaternion_xyz::Quaternion_xyz(const Euler_angle_xyz &e) : Vector7()
{
    (*this) = (Euler_angle)e;
    (*this)[QUAT7_X] = e.get(EULER6_X);
    (*this)[QUAT7_Y] = e.get(EULER6_Y);
    (*this)[QUAT7_Z] = e.get(EULER6_Z);
}

/**
 * Constructeur à partir d'une Angle_axis_xyz.
 * \author Régis Houde    \date 9 décembre 98
 * \param a le Angle_axis_xyz à convertir
 */
Quaternion_xyz::Quaternion_xyz(const Angle_axis_xyz &a) : Vector7()
{
    (*this) = (Quaternion)a;
  
    if(a.msize() == QUAT7_DIM)
    {
        (*this)[QUAT7_X] = a.get(ANGLE7_X);
        (*this)[QUAT7_Y] = a.get(ANGLE7_Y);
        (*this)[QUAT7_Z] = a.get(ANGLE7_Z);
    }
    else
    {
        (*this)[QUAT7_X] = a.get(ANGLE6_X);
        (*this)[QUAT7_Y] = a.get(ANGLE6_Y);
        (*this)[QUAT7_Z] = a.get(ANGLE6_Z);
    }
}

/**
 * Constructeur à partir d'un Quaternion.
 * \author Régis Houde    \date 9 décembre 98
 * \param q le Quaternion à convertir
 */
Quaternion_xyz::Quaternion_xyz(const Quaternion &q) : Vector7(q.get(QUAT4_QW), q.get(QUAT4_QX), q.get(QUAT4_QY), q.get(QUAT4_QZ), 0, 0, 0) 
{
}

/**
 * Constructeur à partir d'un Quaternion et d'une Position.
 * \author Régis Houde    \date 9 décembre 98
 * \param q le Quaternion
 * \param p la Position
 */
Quaternion_xyz::Quaternion_xyz(const Quaternion &q, const Position &p) : Vector7(q.get(QUAT4_QW), q.get(QUAT4_QX), q.get(QUAT4_QY), q.get(QUAT4_QZ), p.get(POS_X), p.get(POS_Y), p.get(POS_Z))
{
}
	
/**
 * Rend le Quaternion_xyz unitaire.
 * \author Régis Houde    \date 9 décembre 98
 * \return la référence du quaternion_xyz devenu unitaire
 */
Quaternion_xyz &Quaternion_xyz::unit()
{
    double sc = abs();
   
    if(fabs(sc) > VM_SMALL)
    {
        sc = 1/sc;		// For speed
        (*this)[QUAT7_QW] *= sc;
        (*this)[QUAT7_QX] *= sc;
        (*this)[QUAT7_QY] *= sc;
        (*this)[QUAT7_QZ] *= sc;
    } 
    else
        vmError("The quaternion_xyz's norm is null, can't make it unit\n");
    
    return (*this);
}

/**
 * Surcharge de l'opérateur "*" entre 2 Quaternion_xyzs.
 * <pre>
 * 	q[0] = t0+t9-t5;
 * 	q[1] = t1+t9-t8;
 * 	q[2] = t2+t9-t7;
 * 	q[3] = t3+t9-t6;
 * 	
 * 	où
 * 	
 * 	t0 = (q1[3] - q1[2])*(q2[2] - q2[3]);
 * 	t1 = (q1[0] + q1[1])*(q2[0] + q2[1]);
 * 	t2 = (q1[0] - q1[1])*(q2[2] + q2[3]);
 * 	t3 = (q1[3] + q1[2])*(q2[0] - q2[1]);
 * 	t4 = (q1[3] - q1[1])*(q2[1] - q2[2]);
 * 	t5 = (q1[3] + q1[1])*(q2[1] + q2[2]);
 * 	t6 = (q1[0] + q1[2])*(q2[0] - q2[3]);
 * 	t7 = (q1[0] - q1[2])*(q2[0] + q2[3]);
 * 	t8 = t5 + t6 + t7;
 * 	t9 = (t4 + t8)/2;
 * 
 * </pre>
 *
 * \author Régis Houde    \date 9 décembre 98
 * \param q1 premier Quaternion_xyz à multiplier
 * \param q2 deuxième Quaternion_xyz à multiplier
 * \return q1*q2
 */
Quaternion_xyz operator * (const Quaternion_xyz& q1, const Quaternion_xyz& q2)
{
    Quaternion_xyz q;
  
    // On utilise les Transform pour cette opération
    // A vérifier et à optimiser!!!
    q = (Transform)q1 * (Transform)q2;
  
    return q;  
  	
}

/**
 * Surcharge de l'opérateur / entre 2 Quaternion_xyzs.
 * \author Régis Houde    \date 9 décembre 98
 * \param q1 le numérateur
 * \param q2 le dénumérateur
 */
Quaternion_xyz operator / (const Quaternion_xyz& q1, const Quaternion_xyz& q2)
{ 
    Quaternion_xyz q;
  
    // On utilise les Transform pour cette opération
    // A vérifier et à optimiser!!!
    q = (Transform)q1 / (Transform)q2;
  
    return q;  
 
}

/**
 * Retourne la conjuguée d'un Quaternion_xyz.
 * <pre>
 * 	q[0] = q1[0];
 * 	q[1] = -q1[1];
 * 	q[2] = -q1[2];
 * 	q[3] = -q1[3];
 * </pre>
 *
 * \author Régis Houde    \date 9 décembre 98
 * \param q1 quaternion_xyz dont on veut connaître la conjuguée
 * \return la conjuguée
 */
Quaternion_xyz mc_conjugate(const Quaternion_xyz& q1)
{
    Quaternion_xyz q;
  
    q[0] =  q1.get(QUAT7_QW);
    q[1] = -q1.get(QUAT7_QX);
    q[2] = -q1.get(QUAT7_QY);
    q[3] = -q1.get(QUAT7_QZ);
  
    return q;
}  

/**
 * Retourne l'inverse d'un Quaternion_xyz.
 * mc_conjugate(q1)/q1.norm();
 * \author Régis Houde    \date 9 décembre 98
 * \param q1 le quaternion_xyz à inverser 
 * \return l'inverse de q1
 */
Quaternion_xyz mc_inv(const Quaternion_xyz &q1)
{  
    return mc_conjugate(q1)/q1.norm();
}  

/**
 * Retourne le carré d'un Quaternion_xyz.
 * <pre>
 * 	q[0] = q1[0]*q1[0] - q1[1]*q1[1] - q1[2]*q1[2] - q1[3]*q1[3];
 * 	q[1] = q1[1]*n;
 * 	q[2] = q1[2]*n;
 * 	q[3] = q1[3]*n;
 * 	
 * 	où n = q1.norm()
 * </pre>
 *
 * \author Régis Houde    \date 9 décembre 98
 * \param q1 le quaternion_xyz dont veut connaître le carré
 * \return le carré de q1
 */
Quaternion_xyz mc_square(const Quaternion_xyz &q1)
{
    Quaternion_xyz q;
    double n = q1.norm();
  
    q[QUAT7_QW]  = q1.get(QUAT7_QW)*q1.get(QUAT7_QW)
                - q1.get(QUAT7_QX)*q1.get(QUAT7_QX)
                - q1.get(QUAT7_QY)*q1.get(QUAT7_QY)
                - q1.get(QUAT7_QZ)*q1.get(QUAT7_QZ);
    // vecteur
    q[QUAT7_QX]  = q1.get(QUAT7_QX)*n;
    q[QUAT7_QY]  = q1.get(QUAT7_QY)*n;
    q[QUAT7_QZ]  = q1.get(QUAT7_QZ)*n;
  
    return q;
}

/**
 * Retourne la norme d'un Quaternion_xyz.
 * retourne q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]
 * \author Régis Houde    \date 9 décembre 98
 * \return la norme du Quaternion_xyz
 */
double Quaternion_xyz::norm() const
{
    return ( get(QUAT7_QW)*get(QUAT7_QW) + get(QUAT7_QX)*get(QUAT7_QX) 
           + get(QUAT7_QY)*get(QUAT7_QY) + get(QUAT7_QZ)*get(QUAT7_QZ) );
}

/**
 * Retourne la valeur absolue d'un Quaternion_xyz.
 * \author Régis Houde    \date 9 décembre 98
 * \return la valeur absolue du Quaternion_xyz
 */
double Quaternion_xyz::abs() const
{
    return sqrt(norm());
}

/*---------------------------------------------------------------------------*/
/*                               FIN DU FICHIER                              */
/*---------------------------------------------------------------------R.H.--*/
