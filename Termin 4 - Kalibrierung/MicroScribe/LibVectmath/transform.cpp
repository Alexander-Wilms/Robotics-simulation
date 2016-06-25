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

// $Id: transform.C,v 1.11 2003/02/25 15:12:08 serge Exp $

/** \file
 * Implémente les méthodes de la classe Transform et les fonctions
 * qui y sont reliées. <p>
 * 
 * La classe Transform permet de faire des calculs matriciels sur
 * des matrices homogènes 4 x 4.  Toutes les fonctionnalités de
 * Matrix sont disponibles.  De plus, certaines d'entre-elles
 * peuvent avoir été optimisées afin de tirer profit de la
 * spécialisation de la matrice homogène.  Par exemple, une
 * inversion d'une matrice homogène sera beaucoup plus rapide que
 * l'inversion d'une matrice ordinaire.  Une matrice homogène T
 * aura 4 lignes et 4 colonnes.  Pour référencer l'élément situé
 * sur la 3e ligne et la 2e colonnes de la matrice homogène R, on
 * notera R[2][1] car les lignes et les colonnes sont numérotées à
 * partir de 0.
 *
 * \author Régis Houde    \date 17 novembre 97
 */

/*---------------------------------------------------------------------------*/
/*                     	      FICHIERS D'INCLUSIONS                          */
/*---------------------------------------------------------------------------*/
#include <stdio.h>

#include "vectmath.h"

/**
 * Constructeur par défaut d'une Transform. On construit une matrice 4x4 dans
 * laquelle on copie une matrice identité 4x4.
 * \author Régis Houde    \date 4 juin 98
 */
Transform::Transform() : Matrix(4, 4) 
{
    //(*this) =  mc_identity(4);
    
    // C'est plus rapide de remplir chaque terme 1 a 1! 
    v[0][0] = 
    v[1][1] = 
    v[2][2] =
    v[3][3] = 1;
    
    v[0][1] = 
    v[0][2] =
    v[0][3] =
    v[1][0] =
    v[1][2] =
    v[1][3] =
    v[2][0] =
    v[2][1] =
    v[2][3] =
    v[3][0] =
    v[3][1] =
    v[3][2] = 0;
  
};

/**
 * Constructeur à partir d'une Matrix.
 * \author Régis Houde    \date 4 juin 98
 * \param m la matrice d'entrée
 */
Transform::Transform(const Matrix &m) : Matrix(4, 4) 
{
    (*this)[3][0] = (*this)[3][1] = (*this)[3][2] = 0.0; 
    (*this)[3][3] = 1.0;
  

    if((m.nrow() != 4) || m.ncol() != 4)
        vmError("Transform: matrix must be 4 x 4\n");
    
    // Last nrow is always 0 0 0 1 ! We must not copy it
    for(register int i = 0;i < 4; i++)
        for(register int j = 0;j < 3; j++)
            (*this)[j][i] = m.get(j, i);
}

/**
 * Constructeur à partir d'une Rotation.
 * \author Régis Houde    \date 4 juin 98
 * \param m la matrice d'entrée
 */
Transform::Transform(const Rotation &m) : Matrix(4, 4) 
{
    (*this)[3][0] = (*this)[3][1] = (*this)[3][2] = 0.0; 
    (*this)[3][3] = 1.0;
  
    (*this)[0][3] = (*this)[1][3] = (*this)[2][3] = 0.0; 
  
    // this->Transform();

    // Last nrow is always 0 0 0 1 ! We must not copy it
    for(register int i = 0; i < 3; i++)
        for(register int j = 0; j < 3; j++)
            (*this)[j][i] = m.get(j, i);
}

/**
 * Constructeur à partir d'une Position.
 * \author Régis Houde    \date 4 juin 98
 * \param p le vecteur d'entrée
 */
Transform::Transform(const Position &p) : Matrix(4, 4) 
{
    (*this)[0][1] = 
    (*this)[0][2] = 
    (*this)[1][0] = 
    (*this)[1][2] = 
    (*this)[2][0] = 
    (*this)[2][1] = 
    (*this)[3][0] = 
    (*this)[3][1] = 
    (*this)[3][2] = 0.0; 
  
    (*this)[0][0] =
    (*this)[1][1] =
    (*this)[2][2] =
    (*this)[3][3] = 1.0;
  
    (*this)[TRANSFORM_X][3] = p.get(POS_X);
    (*this)[TRANSFORM_Y][3] = p.get(POS_Y);
    (*this)[TRANSFORM_Z][3] = p.get(POS_Z);
}

/**
 * Constructeur à partir d'une Euler_angle.
 * \author Régis Houde    \date 4 juin 98
 * \param e le vecteur d'entrée
 */
Transform::Transform(const Euler_angle &e) : Matrix(4, 4) 
{
    (*this) = (Rotation)e;
  
    (*this)[0][3] = (*this)[1][3] = (*this)[2][3] =    
    (*this)[3][0] = (*this)[3][1] = (*this)[3][2] = 0.0; 
    (*this)[3][3] = 1.0;
}

/**
 * Constructeur à partir d'une Euler_angle_xyz.
 * \author Régis Houde    \date 8 décembre 98
 * \param e le vecteur d'entrée
 */
Transform::Transform(const Euler_angle_xyz &e) : Matrix(4, 4) 
{
    (*this) = (Rotation)e;
  
    (*this)[TRANSFORM_X][3] = e.get(EULER6_X);
    (*this)[TRANSFORM_Y][3] = e.get(EULER6_Y);
    (*this)[TRANSFORM_Z][3] = e.get(EULER6_Z); 
  
    (*this)[3][0] = (*this)[3][1] = (*this)[3][2] = 0.0;   
    (*this)[3][3] = 1.0;
}

/**
 * Constructeur à partir d'un Quaternion.
 * \author Régis Houde    \date 4 juin 98
 * \param q le vecteur d'entrée
 */
Transform::Transform(const Quaternion &q) : Matrix(4, 4) 
{
    (*this) = (Rotation)q;
  
    (*this)[0][3] = (*this)[1][3] = (*this)[2][3] = 0.0;   
  
    (*this)[3][0] = (*this)[3][1] = (*this)[3][2] = 0.0; 
    (*this)[3][3] = 1.0;  
}

/**
 * Constructeur à partir d'un Quaternion_xyz.
 * \author Régis Houde    \date 9 décembre 98
 * \param q le vecteur d'entrée
 */
Transform::Transform(const Quaternion_xyz &q) : Matrix(4, 4) 
{
    (*this) = (Rotation)q;
  
    (*this)[TRANSFORM_X][3] = q.get(QUAT7_X);
    (*this)[TRANSFORM_Y][3] = q.get(QUAT7_Y);
    (*this)[TRANSFORM_Z][3] = q.get(QUAT7_Z);   
  
    (*this)[3][0] = (*this)[3][1] = (*this)[3][2] = 0.0; 
    (*this)[3][3] = 1.0;
}

/**
 * Constructeur à partir d'un Angle_axis_xyz.
 * \author Régis Houde     \date 4 juin 98
 * \param a le vecteur d'entrée
 */
Transform::Transform(const Angle_axis_xyz &a) : Matrix(4, 4) 
{
    if(a.msize() == ANGLE7_DIM)
    {
        (*this) = (Rotation)a;

        (*this)[3][0] = (*this)[3][1] = (*this)[3][2] = 0.0; 
        (*this)[3][3] = 1.0;

        (*this)[TRANSFORM_X][3] = a.get(ANGLE7_X);
        (*this)[TRANSFORM_Y][3] = a.get(ANGLE7_Y);
        (*this)[TRANSFORM_Z][3] = a.get(ANGLE7_Z);   
    }
    else
    {
        (*this) = (Rotation)a;


        (*this)[3][0] = (*this)[3][1] = (*this)[3][2] = 0.0; 
        (*this)[3][3] = 1.0;

        (*this)[TRANSFORM_X][3] = a.get(ANGLE6_X);
        (*this)[TRANSFORM_Y][3] = a.get(ANGLE6_Y);
        (*this)[TRANSFORM_Z][3] = a.get(ANGLE6_Z);   
    }  
}

/**
 * Calcule l'inverse de la transform.
 * <pre>
 *  	L'inverse d'une Transform:
 * 
 * Q =  	|R     X|
 *          | 3x3  Y|
 *          |      Z|
 *          |0 0 0 1|
 * 
 *  est :
 * 
 * 	 -1
 * Q  = 	|     T      	   	T  |X|	|
 *          |    R    	      -R   |Y|	|
 *          |     3x3    	   	3x3|Z|	|
 *          |0     0      0	 	1    	|
 * </pre>
 *
 * \author Jean Côté  \author Régis Houde  \date 18 novembre 97
 * \param mi matrice d'entrée
 * \return matrice inverse 
 */

//
// Code restructuration (SSA030205)
//
Transform mc_inv(const Transform& ti)
{
    Transform t;
        
    // position
    double x = -ti.v[TRANSFORM_X][3];
    double y =  ti.v[TRANSFORM_Y][3];
    double z =  ti.v[TRANSFORM_Z][3];

    // translation
    t[TRANSFORM_X][3] = ti.v[0][0]*x - ti.v[1][0]*y - ti.v[2][0]*z;
    t[TRANSFORM_Y][3] = ti.v[0][1]*x - ti.v[1][1]*y - ti.v[2][1]*z;
    t[TRANSFORM_Z][3] = ti.v[0][2]*x - ti.v[1][2]*y - ti.v[2][2]*z;
        
    // Rotation
    t[0][0] = ti.v[0][0];
    t[0][1] = ti.v[1][0];
    t[0][2] = ti.v[2][0];
  
    t[1][0] = ti.v[0][1];
    t[1][1] = ti.v[1][1];
    t[1][2] = ti.v[2][1];
  
    t[2][0] = ti.v[0][2];
    t[2][1] = ti.v[1][2];
    t[2][2] = ti.v[2][2];
  
    return(t);
    	
}

/**
 * Surcharge de l'opérateur "*" entre deux transformées.
 * On fait tous les calculs directement sans utiliser de boucle
 * pour optimiser la fonction
 * \author Régis Houde    \date 18 novembre 97
 * \param t1 la transformée de gauche
 * \param t2 la transformée de droite
 * \return le résultat t1 * t2
 */
// todo
Transform operator * (const Transform& t1, const Transform& t2)
{
    Transform t;

    t.v[0][0] = t1.v[0][0]*t2.v[0][0] + t1.v[0][1]*t2.v[1][0] + t1.v[0][2]*t2.v[2][0];
    t.v[0][1] = t1.v[0][0]*t2.v[0][1] + t1.v[0][1]*t2.v[1][1] + t1.v[0][2]*t2.v[2][1];
    t.v[0][2] = t1.v[0][0]*t2.v[0][2] + t1.v[0][1]*t2.v[1][2] + t1.v[0][2]*t2.v[2][2];
    t.v[0][3] = t1.v[0][0]*t2.v[0][3] + t1.v[0][1]*t2.v[1][3] + t1.v[0][2]*t2.v[2][3] + t1.v[0][3];
  
    t.v[1][0] = t1.v[1][0]*t2.v[0][0] + t1.v[1][1]*t2.v[1][0] + t1.v[1][2]*t2.v[2][0];
    t.v[1][1] = t1.v[1][0]*t2.v[0][1] + t1.v[1][1]*t2.v[1][1] + t1.v[1][2]*t2.v[2][1];
    t.v[1][2] = t1.v[1][0]*t2.v[0][2] + t1.v[1][1]*t2.v[1][2] + t1.v[1][2]*t2.v[2][2];
    t.v[1][3] = t1.v[1][0]*t2.v[0][3] + t1.v[1][1]*t2.v[1][3] + t1.v[1][2]*t2.v[2][3] + t1.v[1][3];
  
    t.v[2][0] = t1.v[2][0]*t2.v[0][0] + t1.v[2][1]*t2.v[1][0] + t1.v[2][2]*t2.v[2][0];
    t.v[2][1] = t1.v[2][0]*t2.v[0][1] + t1.v[2][1]*t2.v[1][1] + t1.v[2][2]*t2.v[2][1];
    t.v[2][2] = t1.v[2][0]*t2.v[0][2] + t1.v[2][1]*t2.v[1][2] + t1.v[2][2]*t2.v[2][2];
    t.v[2][3] = t1.v[2][0]*t2.v[0][3] + t1.v[2][1]*t2.v[1][3] + t1.v[2][2]*t2.v[2][3] + t1.v[2][3];
    

    return t;
}

/**
 * Surcharge de l'opérateur "*" entre une Transform et une Position.
 * On fait tous les calculs directement sans utiliser de boucle
 * pour optimiser la fonction
 * \author Régis Houde    \date 18 novembre 97
 * \param t la transformée de gauche
 * \param p la position
 * \return le résultat t * (p[0], p[1], p[2], 1)
 */
Position operator * (const Transform& t, const Position& p)
{
    Position p1;

    p1[POS_X] = t.v[0][0]*p.get(POS_X) + t.v[0][1]*p.get(POS_Y) + t.v[0][2]*p.get(POS_Z) + t.v[TRANSFORM_X][3];
  
    p1[POS_Y] = t.v[1][0]*p.get(POS_X) + t.v[1][1]*p.get(POS_Y) + t.v[1][2]*p.get(POS_Z) + t.v[TRANSFORM_Y][3];
  
    p1[POS_Z] = t.v[2][0]*p.get(POS_X) + t.v[2][1]*p.get(POS_Y) + t.v[2][2]*p.get(POS_Z) + t.v[TRANSFORM_Z][3];
    
    return p1;
}

/**
 * Surcharge de l'opérateur "*" entre une Transform et un Vector4.
 * On fait tous les calculs directement sans utiliser de boucle
 * pour optimiser la fonction
 * \author Jean Cote    \date 17 avril 98
 * \param t la transformée de gauche
 * \param p le vecteur
 * \return le résultat t * (p[0], p[1], p[2], p[3])
 */
//todo
Vector4 operator * (const Transform& t, const Vector4& p)
{
    Vector4 p1;

    p1[0] = t.v[0][0]*p.get(0) + t.v[0][1]*p.get(1) + t.v[0][2]*p.get(2) + t.v[0][3]*p.get(3);
  
    p1[1] = t.v[1][0]*p.get(0) + t.v[1][1]*p.get(1) + t.v[1][2]*p.get(2) + t.v[1][3]*p.get(3);
  
    p1[2] = t.v[2][0]*p.get(0) + t.v[2][1]*p.get(1) + t.v[2][2]*p.get(2) + t.v[2][3]*p.get(3);

    p1[3] = p.get(3);   

    return p1;
}

/**
 * Surcharge de l'opérateur / entre deux matrices.
 * \author Jean Côté    \date 17 décembre 96
 * \param mn le terme de gauche
 * \param md le terme de doite
 * \return Le résultat
 */
Transform operator / (const Transform& mn, const Transform& md)
{
    return (mc_inv(md)*mn);
}

/**
 * Surcharge de l'opérateur + entre une Rotation et une Position.
 * <pre>
 * 		|R[0][0] R[0][1] R[0][2] P[0] |
 * 	T =	|R[1][0] R[1][1] R[1][2] P[1] |
 * 		|R[2][0] R[2][1] R[2][2] P[2] |
 * 		|   0       0       0	  1   |
 * </pre>
 * \author Régis Houde    \date 4 juin 98
 * \param R le terme de gauche
 * \param P le terme de droite
 * \return le résultat
 */
Transform operator + (const Rotation& R, const Position& P)
{
    Transform T;

    T = R; 
  
    T.v[TRANSFORM_X][3] = P.get(POS_X);
    T.v[TRANSFORM_Y][3] = P.get(POS_Y);
    T.v[TRANSFORM_Z][3] = P.get(POS_Z);
  
    return T;
}

/**
 * Surcharge de l'opérateur + entre un Euler_angle et une Position.
 * \author Régis Houde    \date 4 juin 98
 * \param e le terme de gauche
 * \param p le terme de droite
 * \return le résultat
 */
Transform operator + (const Euler_angle& e, const Position& p)
{
    Transform T;
    Rotation R(e);
  
    T = R + p;
  
    return T;
}

/**
 * Surcharge de l'opérateur + entre un Quaternion et une Position.
 * \author Régis Houde    \date 4 juin 98
 * \param q le terme de gauche
 * \param p le terme de droite
 * \return le résultat
 */
Transform operator + (const Quaternion& q, const Position& p)
{
    Transform T;
    Rotation R(q);
  
    T = R + p;
  
    return T;
}

/**
 * Surcharge de l'opérateur + entre un Angle_axis_xyz et une Position.
 * \author Régis Houde    \date 4 juin 98
 * \param a le terme de gauche
 * \param p le terme de droite
 * \return le résultat
 */
Transform operator + (const Angle_axis_xyz& a, const Position& p)
{
    Transform T;
    Rotation R(a);
    Position p1(p);
  
    if(a.msize() == ANGLE7_DIM)
    {
        p1[POS_X] += a.get(ANGLE7_X);
        p1[POS_Y] += a.get(ANGLE7_Y);
        p1[POS_Z] += a.get(ANGLE7_Z);
    }
    else
    {
        p1[POS_X] += a.get(ANGLE6_X);
        p1[POS_Y] += a.get(ANGLE6_Y);
        p1[POS_Z] += a.get(ANGLE6_Z);
    }  
  
    T = R + p1;
  
    return T;
}


/**
 * Orthonormalise la partie Rotation d'une Transform.
 * On fait la decomposition en valeurs singulières de la matrice
 * puis on multiplie U par mc_transpose(V).
 * \author Régis Houde    \date 13 février 98
 * \return la matrice orthonormalisée
 * \sa Rotation::ortho
 */
Transform &Transform::ortho()
{
    (*this) = ((Rotation)(*this)).ortho() + (Position)(*this);
    return *this;
}

/**
 * Approxime une orthonormalisation de la partie Rotation.
 * \author Régis Houde    \date 4 juin 98
 * \return la matrice orthonormalisée
 * \sa Rotation::ortho_fast
 */
Transform &Transform::ortho_fast()	
{					
    (*this) = ((Rotation)(*this)).ortho_fast() + (Position)(*this);
    
    return *this;
}

/**
 * Approxime une orthonormalisation de la partie Rotation.
 * \author Régis Houde    \date 4 juin 98
 * \return la matrice orthonormalisée
 * \sa Rotation::ortho_very_fast
 */
Transform & Transform::ortho_very_fast()	
{		
    (*this) = ((Rotation)(*this)).ortho_very_fast() + (Position)(*this);

    return (*this);
}

/**
 * Calcule la pose sous forme matricielle 4x4.
 * Soit:
 * <pre>
 *    mc_Rx = {{  1,   0,   0},         tilt   ou   yaw
 *          {  0,  cx, -sx},
 *          {  0,  sx,  cx}}
 *    mc_Ry = {{ cy,   0,  sy},         swing  ou   pitch
 *          {  0,   1,   0},
 *          {-sy,   0,  cy}}
 *    mc_Rz = {{ cz, -sz,   0},         pan    ou   roll
 *          { sz,  cz,   0},
 *          {  0,   0,   1}}
 * 
 *    mc_Rz*mc_Rx*mc_Ry = {{cy cz - sx sy sz, -cx sz,  cz sy + cy sx sz}, 
 *                {cz sx sy + cy sz,  cx cz, -cy cz sx + sy sz}, 
 *                               {          -cx sy,     sx,             cx cy}}
 * </pre>
 *
 * \author Jean Côté    \date 18 décembre 96
 * \param rx rotation autour de l'axe des X
 * \param ry rotation autour de l'axe des Y
 * \param rz rotation autour de l'axe des Z
 * \param x  position par rapport à l'origine X=0
 * \param y  position par rapport à l'origine Y=0
 * \param z  position par rapport à l'origine Z=0
 * \return La matrice de pose mc_Rz*mc_Rx*mc_Ry
 */
Transform mc_rz_rx_ry(double rx, double ry, double rz, double x, double y,
                      double z)
{
    Transform R;

    double cx = cos(rx), sx = sin(rx);
    double cy = cos(ry), sy = sin(ry);
    double cz = cos(rz), sz = sin(rz);

    R.v[0][0] = cy*cz - sx*sy*sz;
    R.v[1][0] = cz*sx*sy + cy*sz;
    R.v[2][0] = -cx*sy;
    R.v[0][1] = -cx*sz;
    R.v[1][1] = cx*cz;
    R.v[2][1] = sx;
    R.v[0][2] = cz*sy + cy*sx*sz;
    R.v[1][2] = -cy*cz*sx + sy*sz;
    R.v[2][2] = cx*cy;

    R.v[TRANSFORM_X][3] = x;
    R.v[TRANSFORM_Y][3] = y;
    R.v[TRANSFORM_Z][3] = z;

    R.v[3][0] = R.v[3][1] = R.v[3][2] = 0;
    R.v[3][3] = 1;

    return(R);
}

Transform mc_rx_ry_rz(double rx, double ry, double rz, double x, double y,
                      double z)
{
	Rotation R = mc_Rx( rx ) * mc_Ry( ry ) * mc_Rz( rz );
	Transform T( R );

    T.v[TRANSFORM_X][3] = x;
    T.v[TRANSFORM_Y][3] = y;
    T.v[TRANSFORM_Z][3] = z;

    T.v[3][0] = T.v[3][1] = T.v[3][2] = 0;
    T.v[3][3] = 1;

    return(T);
}

/**
 * Calcule la rotation sous forme matricielle 4x4.
 * \author Jean Côté    \date 22 janvier 97
 * \param vector Orientation
 * \return La matrice de rotation mc_Rz*mc_Rx*mc_Ry 
 * \sa Rotation mc_rz_ry_rx
 */
Transform mc_rz_ry_rx(Vector &vector)
{
    Transform T;
    
    double cx = cos(vector[EULER6_RX]), sx = sin(vector[EULER6_RX]);
    double cy = cos(vector[EULER6_RY]), sy = sin(vector[EULER6_RY]);
    double cz = cos(vector[EULER6_RZ]), sz = sin(vector[EULER6_RZ]);

    T.v[0][0] = cy*cz;
    T.v[1][0] = cy*sz;
    T.v[2][0] = -sy;
    T.v[0][1] = cz*sx*sy - cx*sz;
    T.v[1][1] = cx*cz + sx*sy*sz;
    T.v[2][1] = cy*sx;
    T.v[0][2] = cx*cz*sy + sx*sz;
    T.v[1][2] = cx*sy*sz - cz*sx;
    T.v[2][2] = cx*cy;
    
    if(vector.msize() == EULER6_DIM)
    {
        T.v[TRANSFORM_X][3] = vector[EULER6_X];
        T.v[TRANSFORM_Y][3] = vector[EULER6_Y];
        T.v[TRANSFORM_Z][3] = vector[EULER6_Z];
    }
    else
    {
        T.v[0][3] = 0;
        T.v[1][3] = 0;
        T.v[2][3] = 0;
    }
    
    return(T);
}

/**
 * Calcule la pose sous forme matricielle 4x4.
 * Soit:
 * <pre>
 *    mc_Rx = {{  1,   0,   0},         tilt   ou   yaw
 *          {  0,  cx, -sx},
 *          {  0,  sx,  cx}}
 *    mc_Ry = {{ cy,   0,  sy},         swing  ou   pitch
 *          {  0,   1,   0},
 *          {-sy,   0,  cy}}
 *    mc_Rz = {{ cz, -sz,   0},         pan    ou   roll
 *          { sz,  cz,   0},
 *          {  0,   0,   1}}
 * 
 *    mc_Rz*mc_Ry*mc_Rx = {{cy cz, cz sx sy - cx sz, cx cz sy + sx sz}, 
 *                {cy sz, cx cz + sx sy sz, cx sy sz - cz sx}, 
 *                {  -sy,            cy sx,            cx cy}}
 * </pre>
 *
 * \author Jean Côté    \date 22 janvier 97
 * \param rx rotation autour de l'axe des X
 * \param ry rotation autour de l'axe des Y
 * \param rz rotation autour de l'axe des Z
 * \param x  position par rapport à l'origine X=0
 * \param y  position par rapport à l'origine Y=0
 * \param z  position par rapport à l'origine Z=0
 * \return La matrice de pose mc_Rz*mc_Ry*mc_Rx
 */
Transform mc_rz_ry_rx(double rx, double ry, double rz, double x, double y,
                      double z)
{
    Transform R;

    double cx = cos(rx), sx = sin(rx);
    double cy = cos(ry), sy = sin(ry);
    double cz = cos(rz), sz = sin(rz);

    R.v[0][0] = cy*cz;
    R.v[1][0] = cy*sz;
    R.v[2][0] = -sy;
    R.v[0][1] = cz*sx*sy - cx*sz;
    R.v[1][1] = cx*cz + sx*sy*sz;
    R.v[2][1] = cy*sx;
    R.v[0][2] = cx*cz*sy + sx*sz;
    R.v[1][2] = cx*sy*sz - cz*sx;
    R.v[2][2] = cx*cy;

    R.v[TRANSFORM_X][3] = x;
    R.v[TRANSFORM_Y][3] = y;
    R.v[TRANSFORM_Z][3] = z;

    R.v[3][0] = R.v[3][1] = R.v[3][2] = 0;
    R.v[3][3] = 1;

    return(R);
}

/**
 * Calcule la pose sous forme matricielle 4x4.
 * \author Jean Côté    \date 18 décembre 96
 * \param vector Vecteur d'orientation
 * \param p Vecteur de position
 * \return La matrice de pose mc_Rz*mc_Rx*mc_Ry
 * \sa Transform mc_rz_rx_ry(rx, ry, rz, x, y, z) 
 */
Transform mc_rz_rx_ry(Vector &vector, Vector &p)
{
    Transform R;
    
    R = mc_rz_rx_ry(vector[0], vector[1], vector[2], p[0], p[1], p[2]);
    
    return(R);
}

/**
 * Calcule la pose sous forme matricielle 4x4.
 * \author Jean Côté    \date 22 janvier 97
 * \param vector Vecteur d'orientation
 * \param p Vecteur de position
 * \return La matrice de pose mc_Rz*mc_Rx*mc_Ry
 * \sa Transform mc_rz_ry_rx(rx, ry, rz, x, y, z)
 */
Transform mc_rz_ry_rx(Vector &vector, Vector &p)
{
    Transform R;
    
    R = mc_rz_ry_rx(vector[0], vector[1], vector[2], p[0], p[1], p[2]);
    
    return(R);
}

/**
 * Convertit une matrice de pose, en vecteur rz, rx, ry, x, y, z.
 * Fonction inverse de Transform mc_rz_rx_ry(rx, ry, rz, x, y, z)
 * \author Jean Côté    \date 18 décembre 96
 * \param Q La matrice de pose
 * \return Le vecteur de la solution.
 */
Vector mc_rz_rx_ry(const Transform &Q)
{
    double cx,sx,cy,sy,cz,sz,cyz,syz;
    Vector vector(EULER6_DIM); // it's a Euler_angle_xyz!
    
    sx = Q.v[2][1];
    if(fabs(sx) == 1.0)
    {
        cx  = 0.0;
        cyz = Q.v[0][0];
        syz = Q.v[1][0];
        vector[EULER6_RX] = atan2(sx,cx);
        vector[EULER6_RY] = 0;
        vector[EULER6_RZ] = atan2(syz,cyz);
    }
    else
    {
        cx = sqrt(Q.v[2][0]*Q.v[2][0] + Q.v[2][2]*Q.v[2][2]);
        sy = -Q.v[2][0]/cx;
        cy =  Q.v[2][2]/cx;
        sz = -Q.v[0][1]/cx;
        cz =  Q.v[1][1]/cx;
        vector[EULER6_RX] = atan2(sx,cx);
        vector[EULER6_RY] = atan2(sy,cy);
        vector[EULER6_RZ] = atan2(sz,cz);
    }
    
    vector[EULER6_X] = Q.v[TRANSFORM_X][3];
    vector[EULER6_Y] = Q.v[TRANSFORM_Y][3];
    vector[EULER6_Z] = Q.v[TRANSFORM_Z][3];

    return(vector);
}

/**
 * Convertit une matrice de pose, en vecteur rz, ry, rx, x, y, z.
 * Fonction inverse de Transform mc_rz_ry_rx(rx, ry, rz, x, y, z)
 * \author Jean Côté    \date 18 décembre 96
 * \param Q La matrice de pose
 * \return Le vecteur de la solution.
 */
Vector mc_rz_ry_rx(const Transform &Q)
{
    double cx,sx,cy,sy,cz,sz,cxz,sxz;
    Vector vector(EULER6_DIM); // it's a Euler_angle_xyz!
    
    sy = -Q.v[2][0];
    if(fabs(sy) == 1.0)
    {
        cy  = 0.0;
        cxz = Q.v[0][2];
        sxz = Q.v[1][2];
        if(sy < 0) 
        {
            cxz = -cxz;
            sxz = -sxz;
        }
        vector[EULER6_RX] = 0;
        vector[EULER6_RY] = atan2(sy,cy);
        vector[EULER6_RZ] = atan2(sxz,cxz);
    }
    else
    {
        cy = sqrt(Q.v[2][1]*Q.v[2][1] + Q.v[2][2]*Q.v[2][2]);
        sx = Q.v[2][1]/cy;
        cx = Q.v[2][2]/cy;
        sz = Q.v[1][0]/cy;
        cz = Q.v[0][0]/cy;
        vector[EULER6_X] = atan2(sx,cx);
        vector[EULER6_Y] = atan2(sy,cy);
        vector[EULER6_Z] = atan2(sz,cz);
    }
    
    vector[EULER6_RX] = Q.v[TRANSFORM_X][3];
    vector[EULER6_RY] = Q.v[TRANSFORM_Y][3];
    vector[EULER6_RZ] = Q.v[TRANSFORM_Z][3];
    
    return(vector);
}

/**
 * Convertit une matrice de pose, en vecteur rz, rx, ry, x, y, z.
 * Fonction inverse de Transform mc_rz_rx_ry(rx, ry, rz, x, y, z)      
 * \author Jean Côté    \date 18 décembre 96
 * \param Q La matrice de pose
 * \param rx la rotation autour de l'axe des X
 * \param ry la rotation autour de l'axe des Y
 * \param rz la rotation autour de l'axe des Z
 * \return Le vecteur de la solution.
 */
Vector mc_rz_rx_ry(const Transform &Q, double &rx, double &ry, double &rz)
{
    Vector v; // it's a Euler_angle_xyz!
    
    v = mc_rz_rx_ry(Q);
    
    rx = v[EULER6_RX];
    ry = v[EULER6_RY];
    rz = v[EULER6_RZ];

    return(v);
}

/**
 * Convertit une matrice de pose, en vecteur rz, ry, rx, x, y, z.
 * Fonction inverse de Transform mc_rz_ry_rx(rx, ry, rz, x, y, z)      
 * \author Jean Côté    \date 22 janvier 97
 * \param Q La matrice de pose
 * \param rx la rotation autour de l'axe des X
 * \param ry la rotation autour de l'axe des Y
 * \param rz la rotation autour de l'axe des Z
 * \return Le vecteur de la solution.
 */
Vector mc_rz_ry_rx(const Transform &Q, double &rx, double &ry, double &rz)
{
    Vector v; // it's a Euler_angle_xyz!
    
    v = mc_rz_ry_rx(Q);
    
    rx = v[EULER6_RX];
    ry = v[EULER6_RY];
    rz = v[EULER6_RZ];

    return(v);
}

/**
 * Calcule la dérivé d'une pose de joint par rapport à theta.
 * Calcule la dérivé d'une matrice d'orientation et de position de
 * joint (calculé avec mc_HDlink) par rapport à theta.  La dérivé
 * d'une matrice HDLink est une simple multiplication par La
 * matrice: E = {{0,-1,0,0},{1,0,0,0},{0,0,0,0},{0,0,0,1}}
 * \author Jean Côté    \date 18 décembre 96
 * \param m Matrice d'entrée
 * \return Matrice dérivée
 */
Transform mc_Derive(Transform &t)
{
    Transform d;
    
    for(int i=0;i<4;i++)
    {
        d.v[0][i] = -t.v[1][i];
        d.v[1][i] = t.v[0][i];
        d.v[2][i] = 0.0;
    }
    
    return(d);
}

/**
 * La transformée "m" dans le référentiel "base".
 * Voir page 34 des notes de cours de Cléments Gosselin "Mécanique des
 * manipulateurs, automne 1993.
 * Rot[base]*m*inverse(Rot[base])
 * \author Jean Côté    \date 21 décembre 98
 * \param base le référentiel d'où on veut "voir" la transformée
 * \param m la transformée
 * \return la transformée "m" dans le référentiel "base"
 */
Transform mc_referential(Transform& base, Transform& m)
{
    Transform rot_base(base);
    rot_base[TRANSFORM_X][3] = rot_base[TRANSFORM_Y][3] = rot_base[TRANSFORM_Z][3] = 0.0;

    return(rot_base*m*mc_inv(rot_base));
}

/**
 * Voir / mais en gardant le résultat dans le repère de base.
 * <pre>
 * 	La formule T_mes_ds_0= pos_to*mc_inv(pos_from)*rot_to*rot_from_inv
 * 	est une expression compacte qui permet de calculer la différence
 * 	entre deux transformées tout en gardant la mesure de la
 * 	transformée résultante dans le repère de base(comme si le repère
 * 	0 était sur le repère 7). Évidemment, la série de calcul suivant
 * 	produit exactement le même résultat.
 * 	T_mes_ds_7= mc_inv(mto)*mfrom (ou mto/mfrom)
 * 	rot_T_mes_ds_0 = rot_real_pose*rot_T_mes_ds_7
 * 	pos_T_mes_ds_0 = rot_real_pose*pos_T_mes_ds_7
 * 
 *        mto
 * rep0-------->rep2
 *     \        ^
 *      \       | 
 * mfrom \      | T= pose du repere 2 p.r a 1 mesuree p.r a 1
 *        \     |                          (pas ce qu'on cherche)
 *         \--->rep1
 * 
 * 			On cherche pose de 2 p.r à l'origine de 1
 * 			mais mesuree p.r à l'orientation des axes de 0
 * </pre>
 * \author Michel Blain    \date 30 juillet 97
 * \param mto voir figure
 * \param mfrom voir figure
 * \bug La fonction mc_division_mes_in_base_ref ne devrait plus etre utilisee.
 *      Il faut plutot utiliser mc_referential
 */
Transform mc_division_mes_in_base_ref(Transform& mfrom, Transform& mto)
{
    printf("\n=====================ATTENTION===========================\n");
    printf("La fonction mc_division_mes_in_base_ref ne devrait plus etre utilisee.\n");
    printf("A la place, il faut utiliser: mc_referential avec une division de Transform\n");
    printf("Au lieu de:\n");
    printf("    Transform ref = mc_division_mes_in_base_ref(mfrom,mto);\n");
    printf("On aura:\n");
    printf("    Transform ref = mto/mfrom;\n");
    printf("    ref = mc_referential(mfrom,ref);\n");
    printf("=====================ATTENTION===========================\n\n");

    Transform pos_to;
    pos_to[TRANSFORM_X][3] = mto[TRANSFORM_X][3];
    pos_to[TRANSFORM_Y][3] = mto[TRANSFORM_Y][3];
    pos_to[TRANSFORM_Z][3] = mto[TRANSFORM_Z][3];
        	    
    Transform inv_pos_from;
    inv_pos_from[TRANSFORM_X][3] = -mfrom[TRANSFORM_X][3];
    inv_pos_from[TRANSFORM_Y][3] = -mfrom[TRANSFORM_Y][3];
    inv_pos_from[TRANSFORM_Z][3] = -mfrom[TRANSFORM_Z][3];
    	    
    Transform rot_to = (Rotation)mto;
	
    Transform rot_from_inv = mc_inv((Rotation)mfrom);
    	
    
    return pos_to*inv_pos_from*rot_to*rot_from_inv;
    
    
    //    return   	((Transform)(Position)mto)*
    //		mc_inv((Transform)(Position)mfrom)*
    //	((Transform)(Rotation)mto)*
    //	((Transform)mc_inv((Rotation)mfrom));

}


/**
 * Calcule la transformée entre 2 nuages de points correspondants.
 * Utilise la méthode de décomposition en valeurs singulières
 * \author Régis Houde    \date 12 mai 2000
 * \param m1 le premier nuage de points (3 x N)
 * \param m2 le deuxième nuage de points (3 x N)
 * \param rank pointeur où l'on mettra le rang de la matrice de covariance
 * \param condition_number pointeur où l'on mettra le nombre de conditionnement
 * \return la transformée entre les 2 nuages de points
 */
Transform mc_match(const Matrix &m1, const Matrix &m2, int *rank,
                   double *condition_number)
{
    Matrix cv = Matrix(3, 3);
    Rotation R;
    Vector3 u1, u2;
    int i;
    double cn;
    int rk;
    Transform T;
  
    if(m1.ncol() != 3 || m2.ncol()!=3 || m1.nrow() != m2.nrow())
        vmError("mc_match : dimensions do not match\n");
  
    // Compute the center of mass of both points group
    u1 = u2 = 0.0;
    for(i = 0; i < m1.nrow(); ++i)
    {
        u1[0] += m1.get(i, 0);
        u1[1] += m1.get(i, 1);
        u1[2] += m1.get(i, 2);
    
        u2[0] += m2.get(i, 0);
        u2[1] += m2.get(i, 1);
        u2[2] += m2.get(i, 2);
    }
    u1 /= m1.nrow();
    u2 /= m2.nrow();

    // Compute the covariance matrix
    cv = 0.0;
    for(i = 0; i < m1.nrow(); ++i)
    {
        cv[0][0] += (m2.get(i,0) - u2[0])*(m1.get(i,0) - u1[0]);
        cv[0][1] += (m2.get(i,0) - u2[0])*(m1.get(i,1) - u1[1]);
        cv[0][2] += (m2.get(i,0) - u2[0])*(m1.get(i,2) - u1[2]);
        cv[1][0] += (m2.get(i,1) - u2[1])*(m1.get(i,0) - u1[0]);
        cv[1][1] += (m2.get(i,1) - u2[1])*(m1.get(i,1) - u1[1]);
        cv[1][2] += (m2.get(i,1) - u2[1])*(m1.get(i,2) - u1[2]);
        cv[2][0] += (m2.get(i,2) - u2[2])*(m1.get(i,0) - u1[0]);
        cv[2][1] += (m2.get(i,2) - u2[2])*(m1.get(i,1) - u1[1]);
        cv[2][2] += (m2.get(i,2) - u2[2])*(m1.get(i,2) - u1[2]);
    }
  
    cv /= m1.nrow();
  
  
    Rotation u, v;
    Vector3 s;
  
    mc_svd(cv, u, s, v);
  
  
    if(s[2] > 1e-5)
    {
        cn = s[0]/s[2];
        rk = 3;
    }  
    else if(s[1] > 1e-5)
    {
        cn = s[0]/s[1];
        rk = 2;
    }
    else   
    {
        cn = 1e5;
        rk = 0;
    }  

    if(condition_number != 0) *condition_number = cn;
    if(rank != 0) *rank = rk;
  
    if(mc_det(u)*mc_det(v) > 0)
    {
        R = u*mc_transpose(v);
    }
    else
    {
        s[0] = 1;
        s[1] = 1;
        s[2] = -1;
    
        R = (Matrix)u*mc_diag(s)*(Matrix)mc_transpose(v);
    }
    
    Position P;
  
    P = u2 - (Vector3)((Matrix)R*(Vector)u1);  // Cast pour WindowsNT ??
  
    T = R + P;
  
    return T;
}

/*---------------------------------------------------------------------------*/
/*                               FIN DU FICHIER                              */
/*---------------------------------------------------------------------R.H.--*/
