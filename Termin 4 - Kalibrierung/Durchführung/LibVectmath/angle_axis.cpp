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

// $Id: angle_axis.C,v 1.8 2003/02/25 15:12:08 serge Exp $

/** \file
 * Implémente les méthodes de la classe Angle_axis et les fonctions
 * qui y sont reliées. <p>
 * 
 * La classe Angle_axis est une classe dérivée de la classe Vector.
 * Elle supporte les cas particuliers des vecteurs-angle_axis.
 * Toutes les fonctionnalités de Vector sont disponibles si
 * applicables.  De plus, certaines entre elles peuvent avoir été
 * optimisées afin de tirer profit de la spécialisation du 
 * vecteur-angle_axis.  Les vecteurs-angle_axis sont de longueur 3 ou 4 et
 * de forme (tnx, tny, tnz) ou (theta, nx, ny, nz). Par défaut, le
 * Angle_axis est de longueur 3 (les 3 premiers éléments du vecteur
 * expriment l'orientation). Si l'on désire travailler avec un
 * Angle_axis de longueur 4 (les 4 premiers éléments expriment
 * l'orientation), il faut le spécifier à la déclaration.  En plus
 * de la longueur, les Angle_axis peuvent être divisés en 2
 * catégories : les vecteurs-angle_axis colonnes et les 
 * vecteurs-angle_axis lignes.  Pour référencer le Xème élément d'un
 * angle_axis ligne ou colonne P, on notera P[X - 1] car l'indexation
 * débute à 0.
 *
 * \author Régis Houde    \date 17 novembre 97
 */

/*---------------------------------------------------------------------------*/
/*                     	      FICHIERS D'INCLUSIONS                          */
/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include <iostream>

#include "vectmath.h"
//#include "Utilities/debug.h"

/**
 * Constructeur à partir d'un autre Angle_axis.
 * \author Régis Houde    \date 4 juin 98
 */
Angle_axis::Angle_axis(const Angle_axis &a) : Vector(ANGLE3_DIM)
{  
    if(a.msize() == msize())
    {
        for(int i = 0; i < size; ++i)
            (*this)[i] = a.get(i);
    }
    else if(a.msize() == ANGLE4_DIM) // On convertit de 4 a 3
    {
        double k = a.get(ANGLE4_THETA);
      
        (*this)[ANGLE3_TNX] = k*a.get(ANGLE4_NX);
        (*this)[ANGLE3_TNY] = k*a.get(ANGLE4_NY);
        (*this)[ANGLE3_TNZ] = k*a.get(ANGLE4_NZ);
    }
	else if(a.msize() == ANGLE3_DIM) // on convertit de 3 a 4
	{
        double k = sqrt(a.get(ANGLE3_TNX)*a.get(ANGLE3_TNX) + a.get(ANGLE3_TNY)*a.get(ANGLE3_TNY) + a.get(ANGLE3_TNZ)*a.get(ANGLE3_TNZ));
		if(k > VM_SMALL)
		{
			(*this)[ANGLE4_THETA] = k;
			(*this)[ANGLE4_NX] = a.get(ANGLE3_TNX)/k;
			(*this)[ANGLE4_NY] = a.get(ANGLE3_TNY)/k;
			(*this)[ANGLE4_NZ] = a.get(ANGLE3_TNZ)/k;
		}

	}
    else vmError("Angle_axis::Angle_axis(const Angle_axis &a) : dimensions problems\n");
}

/**
 * Constructeur à partir d'un autre Angle_axis_xyz.
 * \author Régis Houde    \date 10 novembre 99
 */
Angle_axis::Angle_axis(const Angle_axis_xyz &a) : Vector(3)
{  
    if(a.msize() == ANGLE6_DIM)
    {
        (*this)[ANGLE3_TNX] = a.get(ANGLE6_TNX);
        (*this)[ANGLE3_TNY] = a.get(ANGLE6_TNY);
        (*this)[ANGLE3_TNZ] = a.get(ANGLE6_TNZ);
    }
    else if(a.msize() == ANGLE7_DIM)
    {
        double k = a.get(ANGLE7_THETA);
      
        (*this)[ANGLE3_TNX] = k*a.get(ANGLE7_NX);
        (*this)[ANGLE3_TNY] = k*a.get(ANGLE7_NY);
        (*this)[ANGLE3_TNZ] = k*a.get(ANGLE7_NZ);
    }
    else
        vmError("Angle_axis::Angle_axis(const Angle_axis_xyz &a) : dimensions problems\n");
}

/**
 * Constructeur à partir d'un Vector. On vérifie si le Vector est de longueur 3
 * ou 4 puis on copie les éléments de v un à un.
 * \author Régis Houde    \date 4 juin 98
 */
Angle_axis::Angle_axis(const Vector &v)
{  

    if((v.msize() != ANGLE3_DIM) && (v.msize() != ANGLE4_DIM))
        if((v.nrow() != 1) && (v.ncol() != 1))
            vmError("Angle_axis::Angle_axis(const Vector &v) : wrong dimensions");

    New(v.nrow(), v.ncol());
        
    for(int i = 0; i < size; ++i)
        (*this)[i] = v.get(i);
}

/**
 * Constructeur à partir d'une Matrix.    On vérifie si la Matrix est de
 * dimension 3x1, 1x3, 4x1 ou 1x4 puis on copie les éléments de v un à un.
 * \author Régis Houde    \date 6 juillet 1998
 */
Angle_axis::Angle_axis(const Matrix &v)
{  

    if((v.msize() != ANGLE3_DIM) && (v.msize() != ANGLE4_DIM))
        if((v.nrow() != 1) && (v.ncol() != 1))
            vmError("Angle_axis::Angle_axis(const Matrix &v) : wrong dimensions");

    New(v.nrow(), v.ncol());
  
    if(v.nrow() < v.ncol())
    {
        for(int i = 0; i < size; ++i)
            (*this)[i] = v.get(0, i);
    }
    else
    {
        for(int i = 0; i < size; ++i)
            (*this)[i] = v.get(i, 0);
    }
}

/**
 * Surcharge de l'opérateur = pour un autre Angle_axis. Si les 2 Angle_axis sont
 * de même dimension, on copie leurs éléments un à un. Si les dimensions sont
 * différentes (A(3) = B(4) ou A(4) = B(3)), on convertit le Angle_axis avant
 * de le copier.
 * \author Régis Houde    \date 4 juin 98
 */
Angle_axis& Angle_axis::operator = (const Angle_axis &a)
{      
    if(size == a.msize())
    {
        for(int i = 0; i < size; ++i)
            (*this)[i] = a.get(i);
    }
    else if((size == ANGLE3_DIM) && (a.msize() == ANGLE4_DIM))
    {
        double k = a.get(ANGLE4_THETA);
      
        (*this)[ANGLE3_TNX] = k*a.get(ANGLE4_NX);
        (*this)[ANGLE3_TNY] = k*a.get(ANGLE4_NY);
        (*this)[ANGLE3_TNZ] = k*a.get(ANGLE4_NZ);
    }
    else if((size == ANGLE4_DIM) && (a.msize() == ANGLE3_DIM))
    {
        double k = sqrt( a.get(ANGLE3_TNX)*a.get(ANGLE3_TNX) + a.get(ANGLE3_TNY)*a.get(ANGLE3_TNY)
                       + a.get(ANGLE3_TNZ)*a.get(ANGLE3_TNZ));
        (*this)[ANGLE4_THETA] = k; // theta

        if(fabs(k) > VM_SMALL) 
        {  
            (*this)[ANGLE4_NX] = a.get(ANGLE3_TNX)/k;
            (*this)[ANGLE4_NY] = a.get(ANGLE3_TNY)/k;
            (*this)[ANGLE4_NZ] = a.get(ANGLE3_TNZ)/k;
        }  
        else
        {
            (*this)[ANGLE4_NX] = 1;
            (*this)[ANGLE4_NY] = 0;
            (*this)[ANGLE4_NZ] = 0;
        }
    }
    else
        vmError("Angle_axis::Angle_axis(const Angle_axis &a) : dimensions problems\n");

    return (*this);
}

/**
 * Surcharge de l'opérateur = pour un Vector.
 * Cette fonction accepte les Vector de longueur 3, ou 4.
 * \author Régis Houde    \date 4 juin 98
 * \bug On ne vérifie pas les incohérences de longueur de Vector. Par exemple,
 *      si on copie un Vector(4) dans un Angle_axis(3).
 */
Angle_axis& Angle_axis::operator = (const Vector &a)
{      
    switch(a.msize())
    {
    case ANGLE4_DIM :
        (*this)[3] = a.get(3);
    case ANGLE3_DIM :
        (*this)[2] = a.get(2);
        (*this)[1] = a.get(1);
        (*this)[0] = a.get(0);
        break;
    default :
        vmError("Angle_axis& Angle_axis::operator = (const Vector &a) : a has incorrect lenght");
    }
    return (*this);
}

/**
 * Surcharge de l'opérateur = pour une Matrix.
 * \author Régis Houde    \date 4 juin 98
 */
Angle_axis& Angle_axis::operator = (const Matrix &a)
{      

    if (a.ncol() != 1 && a.nrow() != 1)
        vmError("Angle_axis& Angle_axis::operator = (const Matrix &a) : a is not a vector");

    Vector v = a;
  
    *this = v;
      
    return (*this);
}

/**
 * Constructeur à partir de theta, nx, ny et nz.
 * On crée un Angle_axis de longueur 4 A = [theta, nx, ny, nz, 0, 0, 0]
 * \author Régis Houde    \date 4 juin 98
 * \param theta angle en radian
 * \param nx élément x du vecteur unitaire
 * \param ny élément y du vecteur unitaire
 * \param nz élément z du vecteur unitaire
 */
Angle_axis::Angle_axis(double theta, double nx, double ny, double nz) : Vector(ANGLE4_DIM)
{
    (*this)[ANGLE4_THETA] = theta;
    (*this)[ANGLE4_NX]    = nx;
    (*this)[ANGLE4_NY]    = ny;
    (*this)[ANGLE4_NZ]    = nz;  
}

/**
 * Constructeur à partir de tnx, tny et tnz. On crée un Angle_axis de longueur
 * 3 A = [tnx, tny, tnz].
 * \author Régis Houde    \date 4 juin 98
 * \param tnx élément x du vecteur unitaire multiplié par un angle theta
 * \param tny élément y du vecteur unitaire multiplié par un angle theta
 * \param tnz élément z du vecteur unitaire multiplié par un angle theta
 */
Angle_axis::Angle_axis(double tnx, double tny, double tnz) : Vector(ANGLE3_DIM)
{
    (*this)[ANGLE3_TNX] = tnx;
    (*this)[ANGLE3_TNY] = tny;
    (*this)[ANGLE3_TNZ] = tnz;  
}

/**
 * Constructeur à partir d'une Transform.
 * \author Régis Houde     \date 4 juin 98
 */
Angle_axis::Angle_axis(const Transform &t) : Vector(3)
{
    (*this) = (Rotation) t;  
}

/**
 * Surcharge de l'opérateur = pour une Transform.
 * \author Régis Houde    \date 4 juin 98
 */
Angle_axis& Angle_axis::operator = (const Transform &t)
{
    (*this) = (Rotation) t;  
    return (*this);
}

/**
 * Constructeur à partir d'une Rotation.
 * \author Régis Houde    \date 4 juin 98
 */
Angle_axis::Angle_axis(const Rotation &r) : Vector(ANGLE3_DIM)
{
    double val = (r.get(0, 0) + r.get(1, 1) + r.get(2, 2) - 1)/2;
    double theta = (fabs(val)>=1.0) ? 0:acos(val); // theta

    if(fabs(sin(theta)) > VM_SMALL)
    {
        double k = 0.5*theta/sin(theta);

        (*this)[ANGLE3_TNX] = k*(r.get(2, 1) - r.get(1, 2));
        (*this)[ANGLE3_TNY] = k*(r.get(0, 2) - r.get(2, 0));
        (*this)[ANGLE3_TNZ] = k*(r.get(1, 0) - r.get(0, 1));
    }
    else
    {
        (*this)[ANGLE3_TNX] = 0;
        (*this)[ANGLE3_TNY] = 0;
        (*this)[ANGLE3_TNZ] = 0;
    }  
  
}

/**
 * Surcharge de l'opérateur = pour une Rotation.
 * \author Régis Houde    \date 4 juin 98
 */
Angle_axis& Angle_axis::operator = (const Rotation &r)
{
	double vx =0, vy =0, vz =0, theta;
	double cosa = (r.get(0, 0) + r.get(1, 1) + r.get(2, 2) - 1)*0.5;
	if(cosa <= -1) cosa = -1; // Erreur d'arrondi
	if(cosa >=  1) cosa =  1; // Erreur d'arrondi
	double cosa1 = 1 - cosa;

	if(cosa1 <= 0) 
	{
		theta = 0;
		vx = 0;
		vy = 0;
		vz = 1;
	}
	else
	{
		if(r.get(0, 0) > cosa) vx = sqrt((r.get(0, 0) - cosa)/cosa1);
		if(r.get(1, 1) > cosa) vy = sqrt((r.get(1, 1) - cosa)/cosa1);
		if(r.get(2, 2) > cosa) vz = sqrt((r.get(2, 2) - cosa)/cosa1);
		if(r.get(2, 1) < r.get(1, 2)) vx = -vx;
		if(r.get(0, 2) < r.get(2, 0)) vy = -vy;
		if(r.get(1, 0) < r.get(0, 1)) vz = -vz;
		
		theta = acos(cosa); 
	}
	
	
    if(size == ANGLE4_DIM)
    {
		(*this)[ANGLE4_THETA] = 0;
		(*this)[ANGLE4_NX]    = vx;
		(*this)[ANGLE4_NY]    = vy;
		(*this)[ANGLE4_NZ]    = vz;
		unit(); // Cette partie doit etre unitaire (erreur d'arrondi ou matrice de rotation non orthonormale) 
		(*this)[ANGLE4_THETA] = theta;
    }
    else
    {
		(*this)[ANGLE3_TNX] = vx*theta;
		(*this)[ANGLE3_TNY] = vy*theta;
		(*this)[ANGLE3_TNZ] = vz*theta;
		
    }
    return (*this);
}

/**
 * Constructeur à partir d'un Euler_angle. A = (Rotation) e.
 * \author Régis Houde    \date 4 juin 98
 * \param e Le Euler_angle à convertir
 */
Angle_axis::Angle_axis(const Euler_angle &e) : Vector(ANGLE3_DIM)
{
    (*this) = (Rotation) e;  
}

/**
 * Constructeur à partir d'un Euler_angle_xyz. A = (Rotation) e + (Position) e.
 * \author Régis Houde    \date 8 décembre 98
 * \param e Le Euler_angle_xyz à convertir
 */
Angle_axis::Angle_axis(const Euler_angle_xyz &e) : Vector(ANGLE3_DIM)
{
    (*this) = (Rotation)e;  
}

/**
 * Surcharge de l'opérateur = pour une Euler_angle. A = (Rotation) e.
 * \author Régis Houde    \date 4 juin 98
 * \param e Le Euler_angle à convertir
 */
Angle_axis& Angle_axis::operator = (const Euler_angle &e)
{
    (*this) = (Rotation) e;
    return (*this);
}

/**
 * Surcharge de l'opérateur = pour une Euler_angle_xyz.
 * A = (Rotation) e + (Position) e.
 * \author Régis Houde    \date 8 décembre 98
 * \param e Le Euler_angle_xyz à convertir
 */
Angle_axis& Angle_axis::operator = (const Euler_angle_xyz &e)
{
    (*this) = (Rotation) e;
    return (*this);
}

/**
 * Constructeur à partir d'un Quaternion.
 * \author Régis Houde    \date 4 juin 98
 * \param q le vecteur à convertir
 */
Angle_axis::Angle_axis(const Quaternion &q) : Vector(ANGLE3_DIM)
{
    // A revoir : cette fonction pourrait etre plus rapide 
    // si on ne passe pas par les rotations
    (*this) = (Rotation) q;  

}

/**
 * Constructeur à partir d'un Quaternion_xyz.
 * \author Régis Houde    \date 4 juin 98
 * \param q le vecteur à convertir
 */
Angle_axis::Angle_axis(const Quaternion_xyz &q) : Vector(ANGLE3_DIM)
{
    // À revoir : cette fonction pourrait être plus rapide 
    // si on ne passe pas par les rotations
    (*this) = (Rotation) q;  
}

/**
 * Surcharge de l'opérateur = pour un Quaternion.
 * \author Régis Houde    \date 4 juin 98
 * \param q l'élement à convertir
 */
Angle_axis& Angle_axis::operator = (const Quaternion &q)
{
    (*this) = (Rotation) q;

    return (*this);
}

/**
 * Surcharge de l'opérateur = pour un Quaternion_xyz.
 * \author Régis Houde    \date 4 juin 98
 * \param q l'élement à convertir
 */
Angle_axis& Angle_axis::operator = (const Quaternion_xyz &q)
{
    (*this) = (Rotation) q;

    return (*this);
}

/**
 * Constructeur à partir d'un int (3 ou 4). On crée le vecteur de longueur 3
 * ou 4 puis on le rempli de la façon suivante:
 * <pre>
 *   A(3) = [0, 0, 0]
 *   A(4) = [0, 1, 0, 0]
 * </pre>
 * Par définition, un Angle_axis(4) contient un vecteur unitaire aux 2e, 3e
 * et 4e éléments. On choisit arbitrairement d'y placer le vecteur [1, 0, 0].
 * \author Régis Houde    \date 4 juin 98
 * \param l 3 ou 4
 */
Angle_axis::Angle_axis(int l)
{
    if(l == ANGLE4_DIM)
    {
        if(New(ANGLE4_DIM, 1) == -1)
            vmError("Angle_axis::Angle_axis(int l) :can't allocate memory\n");
        (*this)[ANGLE4_THETA] = 0;
        (*this)[ANGLE4_NX]    = 1;
        (*this)[ANGLE4_NY]    = 
        (*this)[ANGLE4_NZ]    = 0;
    }  
    else if(l == ANGLE3_DIM)
    {
        if(New(ANGLE3_DIM, 1) == -1)
            vmError("Angle_axis::Angle_axis(int l) :can't allocate memory\n");
        (*this)[ANGLE3_TNX] = 
        (*this)[ANGLE3_TNY] = 
        (*this)[ANGLE3_TNZ] = 0;
    }  
    else
    {
        vmError("Angle_axis::Angle_axis(int l) : l must be 3 or 4\n");    
    }
}

/**
 * Surcharge de l'opérateur + entre 2 Angles_axis. On vérifie que les parties
 * rotation des Angle_axis sont bien des vecteurs unitaires et qu'ils sont
 * colinéaires. On transforme ensuite les Angle_axis en Angle_axis(3) puis on
 * les additionne terme à terme. Si a1 ou a2 sont de longueur 3, on retourne un
 * Angle_axis(3) sinon on retourne un angle_axis(4);
 * \author Régis Houde     \date 4 juin 98
 * \param a1 vecteur de longueur 3 ou 4
 * \param a2 vecteur de longueur 3 ou 4
 * \return Si a1 et a2 sont de longueur 4, on retourne un vecteur de 
 *         longueur 4. Sinon, on retourne un vecteur de longueur 3.
 */
Angle_axis operator + (const Angle_axis& a1, const Angle_axis& a2)
{
    Angle_axis n1(ANGLE4_DIM);  
    Angle_axis n2(ANGLE4_DIM);  
    Angle_axis o4(ANGLE4_DIM);
    Angle_axis n3(ANGLE3_DIM);  
    Angle_axis n4(ANGLE3_DIM);  
    Angle_axis o3(ANGLE3_DIM);
  
    // On travaille en Angle_axis(4)
  
    n1 = a1;
    n2 = a2;
  
    Vector v1(ANGLE3_DIM), v2(ANGLE3_DIM);
  
    v1[ANGLE3_TNX] = n1[ANGLE4_NX];
    v1[ANGLE3_TNY] = n1[ANGLE4_NY];
    v1[ANGLE3_TNZ] = n1[ANGLE4_NZ];
  
    v2[ANGLE3_TNX] = n2[ANGLE4_NX];
    v2[ANGLE3_TNY] = n2[ANGLE4_NY];
    v2[ANGLE3_TNZ] = n2[ANGLE4_NZ];
  
    if(fabs(v1.norm() - 1) > VM_SMALL)
    {
		Angle_axis a(ANGLE4_DIM);
		a = a1;
		a.print("a1");
		a = a2;
		a.print("a2");
        vmError("Angle_axis operator + (const Angle_axis& a1, const Angle_axis& a2) : \nrotation part of a1 is not unit\n");
    }
  
    if(fabs(v2.norm() - 1) > VM_SMALL)
    {
		Angle_axis a(ANGLE4_DIM);
		a = a1;
		a.print("a1");
		a = a2;
		a.print("a2");
        vmError("Angle_axis operator + (const Angle_axis& a1, const Angle_axis& a2) : \nrotation part of a2 is not unit\n");
    }
  
    double s = mc_scalar_product(v1, v2);
  
    if(fabs(fabs(s) - 1.0) > VM_SMALL)
    {
        vmError("Angle_axis operator + (const Angle_axis& a1, const Angle_axis& a2) : \na1 et a2 are not colinear\n");
    }
  
    // On revient en Angle_axis(3);
  
    n3 = a1;
    n4 = a2;
  
    // On additionne terme à terme 
    o3 = (Vector)n3 + (Vector)n4;
  
    // On retourne en Angle_axis(3) par défaut
    if((a1.size == ANGLE3_DIM) || (a2.size == ANGLE3_DIM))
    {
        return o3;
    }
    else  // On retourne en Angle_axis(4) si a1 et a2 sont de dim. 4
    {
        o4 = o3;
        return o4;
    }
}

/*---------------------------------------------------------------------------*/
/*                               FIN DU FICHIER                              */
/*---------------------------------------------------------------------R.H.--*/
