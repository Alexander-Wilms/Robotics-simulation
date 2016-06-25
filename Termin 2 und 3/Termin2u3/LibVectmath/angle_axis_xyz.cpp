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

// $Id: angle_axis_xyz.C,v 1.7 2003/02/25 15:12:08 serge Exp $

/** \file
 * Implémente les méthodes de la classe Angle_axis_xyz et les fonctions
 * qui y sont reliées. <p>
 * 
 * La classe Angle_axis_xyz est une classe dérivée de la classe Vector.
 * Elle supporte les cas particuliers des vecteurs-angle_axis_xyz.
 * Toutes les fonctionnalités de Vector sont disponibles si
 * applicables.  De plus, certaines entre elles peuvent avoir été
 * optimisées afin de tirer profit de la spécialisation du 
 * vecteur-angle_axis_xyz.  Les vecteurs-angle_axis_xyz sont de longueur 6 
 * ou 7 et de forme (tnx, tny, tnz, p1, p2, p3) ou (theta, nx, ny, nz, p1,
 * p2, p3). Par défaut, le Angle_axis_xyz est de longueur 6 (les 3
 * premiers éléments du vecteur expriment l'orientation alors que
 * les 3 derniers expriment la position). Si l'on désire travailler
 * avec un Angle_axis_xyz de longueur 7 (les 4 premiers éléments
 * expriment l'orientation alors que les 3 derniers expriment la
 * position), il faut le spécifier à la déclaration.  En plus de
 * la longueur, les Angle_axis_xyz peuvent être divisés en 2
 * catégories : les vecteurs-angle_axis_xyz colonnes et les vecteurs-
 * angle_axis_xyz lignes.  Pour référencer le Xème élément d'un
 * angle_axis_xyz ligne ou colonne P, on notera P[X - 1] car l'indexation
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
 * Constructeur à partir d'un autre Angle_axis_xyz.
 * \author Régis Houde    \date 4 juin 98
 */
Angle_axis_xyz::Angle_axis_xyz(const Angle_axis_xyz &a) : Vector(ANGLE6_DIM)
{  
    if(a.msize() == ANGLE6_DIM)
    {
        for(int i = 0; i < Matrix::x*Matrix::y; ++i)
            (*this)[i] = a.get(i);
    }
    else if(a.msize() == ANGLE7_DIM)
    {
        double k = a.get(ANGLE7_THETA);
      
        (*this)[ANGLE6_TNX] = k*a.get(ANGLE7_NX);
        (*this)[ANGLE6_TNY] = k*a.get(ANGLE7_NY);
        (*this)[ANGLE6_TNZ] = k*a.get(ANGLE7_NZ);
        (*this)[ANGLE6_X] = a.get(ANGLE7_X);
        (*this)[ANGLE6_Y] = a.get(ANGLE7_Y);
        (*this)[ANGLE6_Z] = a.get(ANGLE7_Z);
    }
    else vmError("Angle_axis_xyz::Angle_axis_xyz(const Angle_axis_xyz &a) : dimensions problems\n");
}

/**
 * Constructeur à partir d'un Vector. On vérifie si le Vector est de longueur
 * 6 ou 7 puis on copie les éléments de v un à un.
 * \author Régis Houde    \date 4 juin 98
 */
Angle_axis_xyz::Angle_axis_xyz(const Vector &v)
{  

    if((v.msize() != ANGLE6_DIM) && (v.msize() != ANGLE7_DIM))
    {
        if((v.nrow() != 1) && (v.ncol() != 1))
            vmError("Angle_axis_xyz::Angle_axis_xyz(const Vector &v) : wrong dimensions");
    }
    New(v.nrow(), v.ncol());
        
    for(int i = 0; i < Matrix::x*Matrix::y; ++i)
        (*this)[i] = v.get(i);
}

/**
 * Constructeur à partir d'une Matrix. On vérifie si la Matrix est de dimension
 * 6x1, 1x6, 7x1 ou 1x7 puis on copie les éléments de v un à un.
 * \author Régis Houde    \date 6 juillet 1998
 */
Angle_axis_xyz::Angle_axis_xyz(const Matrix &v)
{  
    if((v.msize() != ANGLE6_DIM) && (v.msize() != ANGLE7_DIM))
    {
        if((v.nrow() != 1) && (v.ncol() != 1))
            vmError("Angle_axis_xyz::Angle_axis_xyz(const Matrix &v) : wrong dimensions");
    }

    New(v.nrow(), v.ncol());
  
    if(v.nrow() < v.ncol())
    {
        for(int i = 0; i < Matrix::x*Matrix::y; ++i)
            (*this)[i] = v.get(0, i);
    }
    else
    {
        for(int i = 0; i < Matrix::x*Matrix::y; ++i)
            (*this)[i] = v.get(i, 0);
    }
}

/**
 * Surcharge de l'opérateur = pour un autre Angle_axis_xyz.
 * Si les 2 Angle_axis_xyz sont de même dimension, on copie leurs éléments un à
 * un. Si les dimensions sont différentes (A(6) = B(7) ou A(7) = B(6)), on
 * convertit le Angle_axis_xyz avant de le copier.
 *
 * \author Régis Houde    \date 4 juin 98
 */
Angle_axis_xyz& Angle_axis_xyz::operator = (const Angle_axis_xyz &a)
{      
    if( size == a.msize())
    {
        for(int i = 0; i < size; ++i)
            (*this)[i] = a.get(i);
    }
    else if( (size == ANGLE6_DIM) && (a.msize() == ANGLE7_DIM) )
    {
        double k = a.get(ANGLE7_THETA);
      
        (*this)[ANGLE6_TNX] = k*a.get(ANGLE7_NX);
        (*this)[ANGLE6_TNY] = k*a.get(ANGLE7_NY);
        (*this)[ANGLE6_TNZ] = k*a.get(ANGLE7_NZ);
        (*this)[ANGLE6_X]  = a.get(ANGLE7_X);
        (*this)[ANGLE6_Y]  = a.get(ANGLE7_Y);
        (*this)[ANGLE6_Z]  = a.get(ANGLE7_Z);
    }
    else if((size == ANGLE7_DIM) && (a.msize() == ANGLE6_DIM))
    {
        double k = sqrt(a.get(ANGLE6_TNX)*a.get(ANGLE6_TNX)
                 + a.get(ANGLE6_TNY)*a.get(ANGLE6_TNY)
                 + a.get(ANGLE6_TNZ)*a.get(ANGLE6_TNZ));
        (*this)[ANGLE7_THETA] = k; 

        if(fabs(k) > VM_SMALL) 
        {  
            (*this)[ANGLE7_NX] = a.get(ANGLE6_TNX)/k;
            (*this)[ANGLE7_NY] = a.get(ANGLE6_TNY)/k;
            (*this)[ANGLE7_NZ] = a.get(ANGLE6_TNZ)/k;
        }  
        else
        {
            (*this)[ANGLE7_NX] = 1;
            (*this)[ANGLE7_NY] = 0;
            (*this)[ANGLE7_NZ] = 0;
        }
        (*this)[ANGLE7_X] = a.get(ANGLE6_X);
        (*this)[ANGLE7_Y] = a.get(ANGLE6_Y);
        (*this)[ANGLE7_Z] = a.get(ANGLE6_Z);
    }
    else vmError("Angle_axis_xyz::Angle_axis_xyz(const Angle_axis_xyz &a) : dimensions problems\n");

    return (*this);
}

/**
 * Surcharge de l'opérateur = pour un Vector.
 * Cette fonction accepte les Vector de longueur 3, 4, 6 et 7. Dans le cas des
 * Vector de longueur 3 et 4, seule la rotation sera définie. Pour les Vector
 * de longueur 6 et 7, la rotation et la translation sont définis.
 *
 * \author Régis Houde    \date 4 juin 98
 * \bug On ne vérifie pas les incohérences de longueur de Vector. Par exemple,
 *      si on copie un Vector(4) dans un Angle_axis_xyz(6).
 */
Angle_axis_xyz& Angle_axis_xyz::operator = (const Vector &a)
{      
    switch(a.msize())
    {
        case 7:
            (*this)[6] = a.get(6);
        case 6 :
            (*this)[5] = a.get(5);
            (*this)[4] = a.get(4);
        case 4 :
            (*this)[3] = a.get(3);
        case 3 :
            (*this)[2] = a.get(2);
            (*this)[1] = a.get(1);
            (*this)[0] = a.get(0);
            break;
        default :
            vmError("Angle_axis_xyz& Angle_axis_xyz::operator = (const Vector &a) : a has incorrect lenght");
    }
    return (*this);
}

/**
 * Surcharge de l'opérateur = pour une Matrix.
 * \author Régis Houde    \date 4 juin 98
x */
Angle_axis_xyz& Angle_axis_xyz::operator = (const Matrix &a)
{      

    if (a.ncol() != 1 && a.nrow() != 1)
        vmError("Angle_axis_xyz& Angle_axis_xyz::operator = (const Matrix &a) : a is not a vector");

    Vector v = a;
  
    *this = v;
      
    return (*this);
}

/**
 * Constructeur à partir de theta, nx, ny et nz. On crée un Angle_axis_xyz de
 * longueur 7. A = [theta, nx, ny, nz, 0, 0, 0].
 * \author Régis Houde    \date 4 juin 98
 * \param theta angle en radian
 * \param nx élément x du vecteur unitaire
 * \param ny élément y du vecteur unitaire
 * \param nz élément z du vecteur unitaire
 */
Angle_axis_xyz::Angle_axis_xyz(double theta, double nx, double ny, double nz) : Vector(ANGLE7_DIM)
{
    (*this)[ANGLE7_THETA] = theta;
    (*this)[ANGLE7_NX] = nx;
    (*this)[ANGLE7_NY] = ny;
    (*this)[ANGLE7_NZ] = nz;
  
    (*this)[ANGLE7_X] = (*this)[ANGLE7_Y] = (*this)[ANGLE7_Z] = 0;
}

/**
 * Constructeur à partir de tnx, tny et tnz. On crée un Angle_axis_xyz de
 * longueur 6. A = [tnx, tny, tnz, 0, 0, 0].
 * \author Régis Houde    \date 4 juin 98
 * \param tnx élément x du vecteur unitaire multiplié par un angle theta
 * \param tny élément y du vecteur unitaire multiplié par un angle theta
 * \param tnz élément z du vecteur unitaire multiplié par un angle theta
 */
Angle_axis_xyz::Angle_axis_xyz(double tnx, double tny, double tnz) : Vector(ANGLE6_DIM)
{
    (*this)[ANGLE6_TNX] = tnx;
    (*this)[ANGLE6_TNY] = tny;
    (*this)[ANGLE6_TNZ] = tnz;
  
    (*this)[ANGLE6_X] = (*this)[ANGLE6_Y] = (*this)[ANGLE6_Z] = 0;
}

/**
 * Constructeur à partir de theta, nx, ny, nz, px, x, y et z. On crée un
 * Angle_axis_xyz de longueur 7. A = [theta, nx, ny, nz, p1, p2, p3].
 * \author Régis Houde    \date 4 juin 98
 * \param theta angle en radian
 * \param nx élément x du vecteur unitaire
 * \param ny élément y du vecteur unitaire
 * \param nz élément z du vecteur unitaire
 * \param p1 élément x de la translation
 * \param p2 élément y de la translation
 * \param p3 élément z de la translation
 */
Angle_axis_xyz::Angle_axis_xyz(double theta, double nx, double ny, double nz, double p1, double p2, double p3) : Vector(ANGLE7_DIM)
{
    (*this)[ANGLE7_THETA] = theta;
    (*this)[ANGLE7_NX]    = nx;
    (*this)[ANGLE7_NY]    = ny;
    (*this)[ANGLE7_NZ]    = nz;
    (*this)[ANGLE7_X]    = p1;
    (*this)[ANGLE7_Y]    = p2;
    (*this)[ANGLE7_Z]    = p3;
}

/**
 * Constructeur à partir de tnx, tny, tnz, x, y et z. On crée un Angle_axis_xyz
 * de longueur 6. A = [tnx, tny, tnz, p1, p2, p3].
 * \author Régis Houde    \date 4 juin 98
 * \param tnx élément x du vecteur unitaire multiplié par un angle theta
 * \param tny élément y du vecteur unitaire multiplié par un angle theta
 * \param tnz élément z du vecteur unitaire multiplié par un angle theta
 * \param p1 élément x de la translation
 * \param p2 élément y de la translation
 * \param p3 élément z de la translation
 */
Angle_axis_xyz::Angle_axis_xyz(double tnx, double tny, double tnz, double p1,
                               double p2, double p3) : Vector(ANGLE6_DIM)
{
    (*this)[ANGLE6_TNX] = tnx;
    (*this)[ANGLE6_TNY] = tny;
    (*this)[ANGLE6_TNZ] = tnz;
    (*this)[ANGLE6_X]  = p1;
    (*this)[ANGLE6_Y]  = p2;
    (*this)[ANGLE6_Z]  = p3;
}

/**
 * Constructeur à partir d'une Transform.
 * \author Régis Houde    \date 4 juin 98
 */
Angle_axis_xyz::Angle_axis_xyz(const Transform &t) : Vector(ANGLE6_DIM)
{
    (*this) = (Rotation) t;  
    (*this)[ANGLE6_X] = t.get(TRANSFORM_X, 3);
    (*this)[ANGLE6_Y] = t.get(TRANSFORM_Y, 3);
    (*this)[ANGLE6_Z] = t.get(TRANSFORM_Z, 3);
}

/**
 * Surcharge de l'opérateur = pour une Transform.
 * \author Régis Houde    \date 4 juin 98
 */
Angle_axis_xyz& Angle_axis_xyz::operator = (const Transform &t)
{
    if(size == ANGLE7_DIM)
    {
        (*this) = (Rotation) t;  
        (*this)[ANGLE7_X] = t.get(TRANSFORM_X, 3);
        (*this)[ANGLE7_Y] = t.get(TRANSFORM_Y, 3);
        (*this)[ANGLE7_Z] = t.get(TRANSFORM_Z, 3);
    }
    else
    {
        (*this) = (Rotation) t;  
        (*this)[ANGLE6_X] = t.get(TRANSFORM_X, 3);
        (*this)[ANGLE6_Y] = t.get(TRANSFORM_Y, 3);
        (*this)[ANGLE6_Z] = t.get(TRANSFORM_Z, 3);
    }  
    return (*this);
}

/**
 * Constructeur à partir d'une Rotation.
 * \author Régis Houde    \date 4 juin 98
 */
Angle_axis_xyz::Angle_axis_xyz(const Rotation &r) : Vector(ANGLE6_DIM)
{
    double val = (r.get(0, 0) + r.get(1, 1) + r.get(2, 2) - 1)/2;
    double theta = (fabs(val)>=1.0) ? 0:acos(val); // theta

    if(fabs(sin(theta)) > VM_SMALL)
    {
        double k = 0.5*theta/sin(theta);

        (*this)[ANGLE6_TNX] = k*(r.get(2, 1) - r.get(1, 2));
        (*this)[ANGLE6_TNY] = k*(r.get(0, 2) - r.get(2, 0));
        (*this)[ANGLE6_TNZ] = k*(r.get(1, 0) - r.get(0, 1));
    }
    else
    {
        (*this)[ANGLE6_TNX] = 0;
        (*this)[ANGLE6_TNY] = 0;
        (*this)[ANGLE6_TNZ] = 0;
    }  
    (*this)[ANGLE6_X] = 0;
    (*this)[ANGLE6_Y] = 0;
    (*this)[ANGLE6_Z] = 0;
  
}

/**
 * Surcharge de l'opérateur = pour une Rotation.
 * \author Régis Houde    \date 4 juin 98
 */
Angle_axis_xyz& Angle_axis_xyz::operator = (const Rotation &r)
{
	double vx =0, vy =0, vz =0, theta;
	double cosa = (r.get(0, 0) + r.get(1, 1) + r.get(2, 2) - 1)*.5;
	if(cosa <= -1) cosa = -1; // Erreur d'arrondi
	if(cosa >= 1) cosa = 1; // Erreur d'arrondi
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
	
	
    if(size == ANGLE7_DIM)
    {
		(*this)[ANGLE7_THETA] = 0;
		(*this)[ANGLE7_NX] = vx;
		(*this)[ANGLE7_NY] = vy;
		(*this)[ANGLE7_NZ] = vz;
		(*this)[ANGLE7_X] = 0;
		(*this)[ANGLE7_Y] = 0;
		(*this)[ANGLE7_Z] = 0;
 		unit();                         // Cette partie doit etre unitaire (erreur d'arondi ou matrice de rotation
                                        // non orthonormale) 
		(*this)[ANGLE7_THETA] = theta;  // On doit laisser a la fin a cause de unit()
   }
    else
    {
		(*this)[ANGLE6_TNX] = vx*theta;
		(*this)[ANGLE6_TNY] = vy*theta;
		(*this)[ANGLE6_TNZ] = vz*theta;
		(*this)[ANGLE6_X]  = 0;
		(*this)[ANGLE6_Y]  = 0;
		(*this)[ANGLE6_Z]  = 0;
		
    }
    return (*this);
}

/**
 * Constructeur à partir d'un Euler_angle.
 * \author Régis Houde    \date 4 juin 98
 * \param e Le Euler_angle à convertir
 */
Angle_axis_xyz::Angle_axis_xyz(const Euler_angle &e) : Vector(ANGLE6_DIM)
{
    (*this) = (Rotation) e;  
}

/**
 * Constructeur à partir d'un Euler_angle_xyz.
 * \author Régis Houde    \date 8 décembre 98
 * \param e Le Euler_angle_xyz à convertir
 */
Angle_axis_xyz::Angle_axis_xyz(const Euler_angle_xyz &e) : Vector(ANGLE6_DIM)
{
    (*this) = (Rotation) e + (Position) e;  
}

/**
 * Surcharge de l'opérateur = pour une Euler_angle.
 * \author Régis Houde    \date 4 juin 98
 * \param e Le Euler_angle à convertir
 */
Angle_axis_xyz& Angle_axis_xyz::operator = (const Euler_angle &e)
{
    (*this) = (Rotation) e;
    return (*this);
}

/**
 * Surcharge de l'opérateur = pour une Euler_angle_xyz.
 * \author Régis Houde    \date 8 décembre 98
 * \param e Le Euler_angle_xyz à convertir
 */
Angle_axis_xyz& Angle_axis_xyz::operator = (const Euler_angle_xyz &e)
{
    (*this) = (Rotation) e + (Position) e;
    return (*this);
}

/**
 * Constructeur à partir d'un Quaternion.
 * \author Régis Houde    \date 4 juin 98
 * \param q le vecteur à convertir
 */
Angle_axis_xyz::Angle_axis_xyz(const Quaternion &q) : Vector(ANGLE6_DIM)
{
    // À revoir : cette fonction pourrait être plus rapide 
    // si on ne passe pas par les rotations
    (*this) = (Rotation) q;  

}

/**
 * Constructeur à partir d'un Quaternion_xyz.
 * \author Régis Houde    \date 4 juin 98
 * \param q le vecteur à convertir
 */
Angle_axis_xyz::Angle_axis_xyz(const Quaternion_xyz &q) : Vector(ANGLE6_DIM)
{
    // À revoir : cette fonction pourrait être plus rapide 
    // si on ne passe pas par les rotations
    (*this) = (Rotation) q + (Position)q;  
}

/**
 * Surcharge de l'opérateur = pour un Quaternion.
 * \author Régis Houde    \date 4 juin 98
 * \param q l'élement à convertir
 */
Angle_axis_xyz& Angle_axis_xyz::operator = (const Quaternion &q)
{
    (*this) = (Rotation) q;

    return (*this);
}

/**
 * Surcharge de l'opérateur = pour un Quaternion_xyz.
 * \author Régis Houde    \date 4 juin 98
 * \param q l'élement à convertir
 */
Angle_axis_xyz& Angle_axis_xyz::operator = (const Quaternion_xyz &q)
{
    (*this) = (Rotation) q + (Position)q;

    return (*this);
}

/**
 * Constructeur à partir d'un int (6 ou 7). On crée le vecteur de longueur 6 ou
 * 7 puis on le rempli de la façon suivante:
 * <pre>
 *    A(6) = [0, 0, 0, 0, 0, 0]
 *    A(7) = [0, 1, 0, 0, 0, 0, 0]
 * </pre>
 * Par définition, un Angle_axis_xyz(7) contient un vecteur unitaire aux 2e,
 * 3e et 4e éléments. On choisit arbitrairement d'y placer le vecteur [1, 0, 0].
 *
 * \author Régis Houde    \date 4 juin 98
 * \param l 6 ou 7
 */
Angle_axis_xyz::Angle_axis_xyz(int l)
{
    if (l == ANGLE7_DIM)
    {
        if(New(ANGLE7_DIM, 1) == -1)
            vmError("Angle_axis_xyz::Angle_axis_xyz(int l) :can't allocate memory\n");

        (*this)[ANGLE7_THETA] = 0;
        (*this)[ANGLE7_NX]    = 1;
        (*this)[ANGLE7_NY] = 
        (*this)[ANGLE7_NZ] = 
        (*this)[ANGLE7_X]  = 
        (*this)[ANGLE7_Y]  = 
        (*this)[ANGLE7_Z]  = 0;
    }  
    else if (l == ANGLE6_DIM)
    {
        if(New(ANGLE6_DIM, 1) == -1)
            vmError("Angle_axis_xyz::Angle_axis_xyz(int l) :can't allocate memory\n");
        (*this)[ANGLE6_TNX] = 
        (*this)[ANGLE6_TNY] = 
        (*this)[ANGLE6_TNZ] = 
        (*this)[ANGLE6_X]   = 
        (*this)[ANGLE6_Y]   = 
        (*this)[ANGLE6_Z]   = 0;
    }  
    else
    {
        vmError("Angle_axis_xyz::Angle_axis_xyz(int l) : l must be 6 or 7\n");    
    }
    
}

/**
 * Surcharge de l'opérateur + entre 2 Angles_axis. On vérifie que les parties
 * rotation des Angle_axis_xyz sont bien des vecteurs unitaires et qu'ils sont
 * colinéaires. On transforme ensuite les Angle_axis_xyz en Angle_axis_xyz(6)
 * puis on les additionne terme à terme. Si a1 et a2 sont de longueur 6, on
 * retourne un Angle_axis_xyz(6) sinon on retourne un angle_axis_xyz(7);
 *
 * \author Régis Houde    \date 4 juin 98
 * \param a1 vecteur de longueur 6 ou 7
 * \param a2 vecteur de longueur 6 ou 7
 * \return Si a1 et a2 sont de longueur 7, on retourne un vecteur de longueur 7.
 *         Sinon, on retourne un vecteur de longueur 6.
 */
Angle_axis_xyz operator + (const Angle_axis_xyz& a1, const Angle_axis_xyz& a2)
{
    Angle_axis_xyz n1(ANGLE7_DIM);  
    Angle_axis_xyz n2(ANGLE7_DIM);  
    Angle_axis_xyz o7(ANGLE7_DIM);
    Angle_axis_xyz n3(ANGLE6_DIM);  
    Angle_axis_xyz n4(ANGLE6_DIM);  
    Angle_axis_xyz o6(ANGLE6_DIM);
  
    // On travaille en Angle_axis_xyz(7)
  
    n1 = a1;
    n2 = a2;
  
    Vector v1(ANGLE3_DIM), v2(ANGLE3_DIM);
  
    v1[ANGLE3_TNX] = n1[ANGLE7_NX];
    v1[ANGLE3_TNY] = n1[ANGLE7_NY];
    v1[ANGLE3_TNZ] = n1[ANGLE7_NZ];
  
    v2[ANGLE3_TNX] = n2[ANGLE7_NX];
    v2[ANGLE3_TNY] = n2[ANGLE7_NY];
    v2[ANGLE3_TNZ] = n2[ANGLE7_NZ];
  
    if(fabs(v1.norm() - 1) > VM_SMALL)
    {
		Angle_axis a(ANGLE4_DIM);
		a = a1;
		a.print("a1");
		a = a2;
		a.print("a2");
		vmError("Angle_axis_xyz operator + (const Angle_axis_xyz& a1, const Angle_axis_xyz& a2) : \nrotation part of a1 is not unit\n");
    }
	
    if(fabs(v2.norm() - 1) > VM_SMALL)
    {
		Angle_axis a(ANGLE4_DIM);
		a = a1;
		a.print("a1");
		a = a2;
		a.print("a2");
		vmError("Angle_axis_xyz operator + (const Angle_axis_xyz& a1, const Angle_axis_xyz& a2) : \nrotation part of a2 is not unit\n");
    }
	
    double s = mc_scalar_product(v1, v2);
  
    if(fabs(fabs(s) - 1.0) > VM_SMALL)
    {
        vmError("Angle_axis_xyz operator + (const Angle_axis_xyz& a1, const Angle_axis_xyz& a2) : \na1 et a2 are not colinear\n");
    }
  
    // On revient en Angle_axis_xyz(6);
  
    n3 = a1;
    n4 = a2;
  
    // On additionne terme à terme 
    o6 = (Vector)n3 + (Vector)n4;
  
    // On retourne en Angle_axis_xyz(6) par défaut
    if((a1.size == ANGLE6_DIM) || (a2.size == ANGLE6_DIM))
    {
        return o6;
    }
    else  // On retourne en Angle_axis_xyz(7) si a1 et a2 sont de dim. 7
    {
        o7 = o6;
        return o7;
    }
}

/*---------------------------------------------------------------------------*/
/*                               FIN DU FICHIER                              */
/*---------------------------------------------------------------------R.H.--*/
