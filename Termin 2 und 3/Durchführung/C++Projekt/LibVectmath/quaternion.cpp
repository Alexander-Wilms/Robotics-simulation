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

// $Id: quaternion.C,v 1.8 2003/02/25 15:12:08 serge Exp $

/** \file
 * Impl�mente les m�thodes de la classe Quaternion et les fonctions
 * qui y sont reli�es. <p>
 * 
 * La classe Quaternion est une classe d�riv�e de la classe Vector.
 * Elle supporte les cas particuliers des vecteurs-quaternion.
 * Toutes les fonctionnalit�s de Vector sont disponibles si
 * applicables.  De plus, certaines d'entre-elles peuvent avoir �t�
 * optimis�es afin de tirer profit de la sp�cialisation du vecteur-
 * quaternion.  Les vecteurs-quaternion sont de longueur 4 et de
 * forme (q0, q1, q2, q3). Il existe 2 sortes de vecteurs-
 * quaternion : les vecteurs-quaternion colonnes et les vecteurs-
 * quaternion lignes.  Pour r�f�rencer le xe �l�ment d'un
 * quaternion ligne ou colonne P, on notera P[x - 1] car l'indi�age
 * d�bute � 0.
 * 
 * Note : Le quaternion � norme unitaire (unit quaternion) est un
 * cas particulier des quaternions.  Il est �galement connu sous le
 * nom de "Param�tres d'Euler" ou "Quaternion d'Euler" et est
 * particuli�rement utile pour exprimer une rotation rigide dans
 * l'espace.  La plupart des constructeurs de la librairie retourne
 * un quaternion � norme unitaire � l'exception de l'initialisation
 * directe des �l�ments du vecteur.  Suite aux manipulations
 * alg�briques (addition, soustraction, etc.)  ou autre, il revient
 * � l'usager de rendre le quaternion unitaire au besoin (voir la
 * m�thode unit).
 *
 * \author R�gis Houde    \date 17 novembre 97
 */

/*---------------------------------------------------------------------------*/
/*                     	      FICHIERS D'INCLUSIONS                          */
/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include <iostream>
#include "vectmath.h"

/*---------------------------------------------------------------------------*/
/*            VARIABLES GLOBALES ET STATIQUES, MACROS ET DEFINES             */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                    PREDECLARATION DES PROCEDURES STATIQUES                */
/*---------------------------------------------------------------------------*/
static double mc_max3(double x, double y, double z);
static int mc_sign(double x);

/*---------------------------------------------------------------------------*/
/*                            PROCEDURES PUBLIQUES                           */
/*---------------------------------------------------------------------------*/

/**
 * Constructeur � partir d'une Transform.
 * \author R�gis Houde    \date 4 juin 98
 * \param t la Transform � convertir
 */
Quaternion::Quaternion(const Transform &t) : Vector4()
{
    Rotation r(t);
  
    (*this) = r;
}


/**
 * Constructeur � partir d'une Rotation.
 * \author R�gis Houde    \date 4 juin 98
 * \param r la Rotation � convertir
 */
Quaternion::Quaternion(const Rotation &rot) : Vector4()
{
    double xp, yp, zp, scv, s2; 
    double vppp[3];
    Rotation r = rot;
    r.ortho();
    s2  = 0.25*(r.get(0, 0) + r.get(1, 1) + r.get(2, 2) + 1);     
	if(s2 < 0) 
	{
		(*this)[QUAT4_QW] = s2 = 0.0; // Ca peut arriver seulement du a des erreurs numeriques
	}
	else (*this)[QUAT4_QW] = sqrt(s2);  

    //printf("s2 = %f\n",s2);    
    xp = r.get(2, 1) - r.get(1, 2);
    yp = r.get(0, 2) - r.get(2, 0);
    zp = r.get(1, 0) - r.get(0, 1);

    /* NOTE :
       From Funda 1990 : IEEE T.R.A Vol 6 No.3 June 1990 p. 382
       (The simple method can be used)... however, as theta
       approaches 0, sin(theta/2) approaches 0 and the vector
       vp is poorly defined. This corresponds to the phisical reality
       of the axis of rotation beeing indeterminate for very small
       angular displacements. Similarly, a theta approaches 180,
       s = cos(theta/2) approaches 0, and again, the vector vp is
       poorly defined. However, physically, the rotational axis is well
       defined in this case. 
	
       C'est pourquoi la methode simple ne doit pas etre utilisee
    */
    // Simple method 
    // (*this)[1] = xp/(4*(*this)[0]);
    // (*this)[2] = yp/(4*(*this)[0]);
    // (*this)[3] = zp/(4*(*this)[0]);
    
   

    if(mc_max3(r.get(0, 0),r.get(1, 1), r.get(2, 2)) == r.get(0, 0))
    {
        vppp[0] = xp + mc_sign(xp)*(r.get(0, 0) - r.get(1, 1) - r.get(2, 2) + 1);
        vppp[1] = yp + mc_sign(xp)*(r.get(1, 0) + r.get(0, 1));
        vppp[2] = zp + mc_sign(xp)*(r.get(2, 0) + r.get(0, 2));
    }

    else if(mc_max3(r.get(0, 0),r.get(1, 1), r.get(2, 2)) == r.get(1, 1))
    {
        vppp[0] = xp + mc_sign(yp)*(r.get(0, 1) + r.get(1, 0));
        vppp[1] = yp + mc_sign(yp)*(r.get(1, 1) - r.get(0, 0) - r.get(2, 2) + 1);
        vppp[2] = zp + mc_sign(yp)*(r.get(2, 1) + r.get(1, 2));
    }
    else
    {
        vppp[0] = xp + mc_sign(zp)*(r.get(0, 2) + r.get(2, 0));
        vppp[1] = yp + mc_sign(zp)*(r.get(1, 2) + r.get(2, 1));
        vppp[2] = zp + mc_sign(zp)*(r.get(2, 2) - r.get(0, 0) - r.get(1, 1) + 1);
    }
 
    //scale to obtain unit quaternion
   
    double d = (vppp[0]*vppp[0] + vppp[1]*vppp[1] + vppp[2]*vppp[2]);
    if(fabs(d) > VM_VERY_SMALL)
    {
        scv = sqrt((1 - s2)/d);

    }
    else
    {
        scv = 0;   
 
    }

    (*this)[QUAT4_QX] = vppp[0]*scv;
    (*this)[QUAT4_QY] = vppp[1]*scv;
    (*this)[QUAT4_QZ] = vppp[2]*scv;

}

/**
 * Constructeur � partir d'un Euler_angle.
 * \author R�gis Houde    \date 4 juin 98
 * \param e le Euler_angle � convertir
 */
Quaternion::Quaternion(const Euler_angle &e) : Vector4()
{
    double r = e.get(EULER3_RX)/2, p = e.get(EULER3_RY)/2, y = e.get(EULER3_RZ)/2;
    double cosr = cos(r), sinr = sin(r);
    double cosp = cos(p), sinp = sin(p);
    double cosy = cos(y), siny = sin(y);
  
    (*this)[QUAT4_QW] = cosr*cosp*cosy + sinr*sinp*siny;
    (*this)[QUAT4_QX] = sinr*cosp*cosy - cosr*sinp*siny;
    (*this)[QUAT4_QY] = cosr*sinp*cosy + sinr*cosp*siny;
    (*this)[QUAT4_QZ] = cosr*cosp*siny - sinr*sinp*cosy;
  
}

/**
 * Constructeur � partir d'un Euler_angle_xyz.
 * \author R�gis Houde    \date 8 d�cembre 98
 * \param e le Euler_angle_xyz � convertir
 */
Quaternion::Quaternion(const Euler_angle_xyz &e) : Vector4()
{
    (*this) = (Euler_angle)e;
}

/**
 * Constructeur � partir d'une Angle_axis_xyz.
 * \author R�gis Houde    \date 4 juin 98
 * \param a le Angle_axis_xyz � convertir
 */
Quaternion::Quaternion(const Angle_axis_xyz &a) : Vector4()
{
    if(a.msize() == ANGLE7_DIM)
    {
        double theta2 = a.get(ANGLE7_THETA)/2;
        double st2 = sin(theta2);

        (*this)[QUAT4_QW] = cos(theta2);
        (*this)[QUAT4_QX] = st2*a.get(ANGLE7_NX);
        (*this)[QUAT4_QY] = st2*a.get(ANGLE7_NY);
        (*this)[QUAT4_QZ] = st2*a.get(ANGLE7_NZ);
    }
    else
    {
        double t = sqrt(a.get(ANGLE6_TNX)*a.get(ANGLE6_TNX) + a.get(ANGLE6_TNY)*a.get(ANGLE6_TNY) + a.get(ANGLE6_TNZ)*a.get(ANGLE6_TNZ));
        double theta2 = t/2;
        double st2 = sin(theta2);

        (*this)[QUAT4_QW] = cos(theta2);
        if(fabs(t) > 0.0)
        {
            (*this)[QUAT4_QX] = st2*a.get(ANGLE6_TNX)/t;
            (*this)[QUAT4_QY] = st2*a.get(ANGLE6_TNY)/t;
            (*this)[QUAT4_QZ] = st2*a.get(ANGLE6_TNZ)/t;
        }
        else
        {
            (*this)[QUAT4_QX] = 0;	// A verifier
            (*this)[QUAT4_QY] = 0;
            (*this)[QUAT4_QZ] = 0;
        }
    }  
}



/**
 * Rend le Quaternion unitaire.
 * \author R�gis Houde    \date 4 juin 98
 * \return la r�f�rence du quaternion devenu unitaire
 */
Quaternion &Quaternion::unit()
{
    double sc = abs();
   
    if(fabs(sc) > VM_SMALL)
    {
        sc = 1/sc;		// For speed
        (*this)[QUAT4_QW] *= sc;
        (*this)[QUAT4_QX] *= sc;
        (*this)[QUAT4_QY] *= sc;
        (*this)[QUAT4_QZ] *= sc;
    } 
    else
        vmError("The quaternion's norm is null, can't make it unit\n");
    
    return (*this);
}

/**
 * Exprime la vitesse angulaire d'un quaternion en une d�riv�e.
 * \author R�gis Houde    \date 11 mai 99
 * \param w La vitesse angulaire
 * \sa Quaternion::angular_velocity(Quaternion q)
 */
Quaternion Quaternion::derivative(Vector3 w)
{
    Quaternion q;
  
    q[0] =  .5*(                       - w[0]*(*this)[QUAT4_QX] - w[1]*(*this)[QUAT4_QY] - w[2]*(*this)[QUAT4_QZ]);
    q[1] =  .5*(w[0]*(*this)[QUAT4_QW]                          + w[2]*(*this)[QUAT4_QY] - w[1]*(*this)[QUAT4_QZ]);
    q[2] =  .5*(w[1]*(*this)[QUAT4_QW] - w[2]*(*this)[QUAT4_QX]                          + w[0]*(*this)[QUAT4_QZ]);
    q[3] =  .5*(w[2]*(*this)[QUAT4_QW] + w[1]*(*this)[QUAT4_QX] - w[0]*(*this)[QUAT4_QY]);
  
    return q;
}

/**
 * Exprime une d�riv�e de quaternion en vitesse angulaire.
 * \author R�gis Houde    \date 11 mai 99
 * \param q la d�riv�e
 * \return la vitesse angulaire
 * \sa Quaternion::derivative(Vector3 w)
 */
Vector3 Quaternion::angular_velocity(Quaternion q)
{
    Vector3 v;
  
    v[0] = 2.0*(-(*this)[1]*q[QUAT4_QW] + (*this)[0]*q[QUAT4_QX] + (*this)[3]*q[QUAT4_QY] - (*this)[2]*q[QUAT4_QZ]);
    v[1] = 2.0*(-(*this)[2]*q[QUAT4_QW] - (*this)[3]*q[QUAT4_QX] + (*this)[0]*q[QUAT4_QY] + (*this)[1]*q[QUAT4_QZ]);
    v[2] = 2.0*(-(*this)[3]*q[QUAT4_QW] + (*this)[2]*q[QUAT4_QX] - (*this)[1]*q[QUAT4_QY] + (*this)[0]*q[QUAT4_QZ]);
  
    return v;
}


/**
 * Surcharge de l'op�rateur "*" entre 2 Quaternions.
 * <pre>
 * 	q[0] = t0+t9-t5;
 * 	q[1] = t1+t9-t8;
 * 	q[2] = t2+t9-t7;
 * 	q[3] = t3+t9-t6;
 * 	
 * 	o�
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
 * \author R�gis Houde    \date 4 juin 98
 * \param q1 premier Quaternion � multiplier
 * \param q2 deuxi�me Quaternion � multiplier
 * \return q1*q2
 */
Quaternion operator * (const Quaternion& q1, const Quaternion& q2)
{
    Quaternion q;
  
    double t0 = (q1.get(QUAT4_QZ)-q1.get(QUAT4_QY))*(q2.get(QUAT4_QY)-q2.get(QUAT4_QZ));
    double t1 = (q1.get(QUAT4_QW)+q1.get(QUAT4_QX))*(q2.get(QUAT4_QW)+q2.get(QUAT4_QX));
    double t2 = (q1.get(QUAT4_QW)-q1.get(QUAT4_QX))*(q2.get(QUAT4_QY)+q2.get(QUAT4_QZ));
    double t3 = (q1.get(QUAT4_QZ)+q1.get(QUAT4_QY))*(q2.get(QUAT4_QW)-q2.get(QUAT4_QX));
    double t4 = (q1.get(QUAT4_QZ)-q1.get(QUAT4_QX))*(q2.get(QUAT4_QX)-q2.get(QUAT4_QY));
    double t5 = (q1.get(QUAT4_QZ)+q1.get(QUAT4_QX))*(q2.get(QUAT4_QX)+q2.get(QUAT4_QY));
    double t6 = (q1.get(QUAT4_QW)+q1.get(QUAT4_QY))*(q2.get(QUAT4_QW)-q2.get(QUAT4_QZ));
    double t7 = (q1.get(QUAT4_QW)-q1.get(QUAT4_QY))*(q2.get(QUAT4_QW)+q2.get(QUAT4_QZ));
    double t8 = t5+t6+t7;
    double t9 = (t4+t8)/2;
  
    q[QUAT4_QW] = t0+t9-t5;
    q[QUAT4_QX] = t1+t9-t8;
    q[QUAT4_QY] = t2+t9-t7;
    q[QUAT4_QZ] = t3+t9-t6;
  
    return q;  
  	
}

/**
 * Surcharge de l'op�rateur / entre 2 Quaternions.
 * \author R�gis Houde    \date 4 juin 98
 * \param q1 le num�rateur
 * \param q2 le d�num�rateur
 */
Quaternion operator / (const Quaternion& q1, const Quaternion& q2)
{  
    return q1*mc_inv(q2);  
}

/**
 * Retourne la conjugu�e d'un Quaternion.
 * <pre>
 * 	q[0] =  q1[0];
 * 	q[1] = -q1[1];
 * 	q[2] = -q1[2];
 * 	q[3] = -q1[3];
 * </pre>
 *
 * \author R�gis Houde    \date 4 juin 98
 * \param q1 quaternion dont on veut conna�tre la conjugu�e
 * \return la conjugu�e
 */
Quaternion mc_conjugate(const Quaternion& q1)
{
    Quaternion q;
  
    q[QUAT4_QW] =  q1.get(QUAT4_QW);
    q[QUAT4_QX] = -q1.get(QUAT4_QX);
    q[QUAT4_QY] = -q1.get(QUAT4_QY);
    q[QUAT4_QZ] = -q1.get(QUAT4_QZ);
  
    return q;
}  

/**
 * Retourne l'inverse d'un Quaternion.
 * \author R�gis Houde    \date 4 juin 98
 * \param q1 le quaternion � inverser 
 * \return l'inverse de q1
 */
Quaternion mc_inv(const Quaternion &q1)
{  
    return mc_conjugate(q1)/q1.norm();
}  

/**
 * Retourne le carr� d'un Quaternion.
 * <pre>
 * 	q[0] = q1[0]*q1[0] - q1[1]*q1[1] - q1[2]*q1[2] - q1[3]*q1[3];
 * 	q[1] = q1[1]*n;
 * 	q[2] = q1[2]*n;
 * 	q[3] = q1[3]*n;
 * 	
 * 	o� n = q1.norm()
 * </pre>
 *
 * \author R�gis Houde    \date 4 juin 98
 * \param q1 le quaternion dont veut conna�tre le carr�
 * \return le carr� de q1
 */
Quaternion mc_square(const Quaternion &q1)
{
    Quaternion q;
    double n = q1.norm();
  
    q[QUAT4_QW] = q1.get(QUAT4_QW)*q1.get(QUAT4_QW) - q1.get(QUAT4_QX)*q1.get(QUAT4_QX)
                - q1.get(QUAT4_QY)*q1.get(QUAT4_QY) - q1.get(QUAT4_QZ)*q1.get(QUAT4_QZ);
    q[QUAT4_QX] = q1.get(QUAT4_QX)*n;
    q[QUAT4_QY] = q1.get(QUAT4_QY)*n;
    q[QUAT4_QZ] = q1.get(QUAT4_QZ)*n;
  
    return q;
}

/**
 * Retourne la norme d'un Quaternion.
 * \author R�gis Houde    \date 4 juin 98
 * \return la norme du Quaternion
 */
double Quaternion::norm() const
{
    return get(QUAT4_QW)*get(QUAT4_QW) + get(QUAT4_QX)*get(QUAT4_QX) + get(QUAT4_QY)*get(QUAT4_QY) + get(QUAT4_QZ)*get(QUAT4_QZ);
}

/**
 * Retourne la valeur absolue d'un Quaternion.
 * \author R�gis Houde    \date 4 juin 98
 * \return la valeur absolue du Quaternion
 */
double Quaternion::abs() const
{
    return sqrt(norm());
}

/*---------------------------------------------------------------------------*/
/*                            PROCEDURES STATIQUES                           */
/*---------------------------------------------------------------------------*/

/**
 * Retourne la valeur maximale entre 3 doubles.
 * \author R�gis Houde    \date 4 juin 98
 */
static double mc_max3(double x, double y, double z)
{
    if(x >= y && x >= z) return x;
    if(y >= z) return y;
    return z;
}

/**
 * Retourne le signe d'un double. if(x >= 0), retourne 1 sinon retourne -1.
 * \author R�gis Houde    \date 4 juin 98
 */
static int mc_sign(double x)
{
    if (x >= 0) return 1;
    return -1; 
}

/*---------------------------------------------------------------------------*/
/*                               FIN DU FICHIER                              */
/*---------------------------------------------------------------------R.H.--*/
