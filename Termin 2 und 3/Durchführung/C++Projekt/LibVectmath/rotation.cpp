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

// $Id: rotation.C,v 1.14 2003/02/25 15:12:08 serge Exp $

/** \file
 * Impl�mente les m�thodes de la classe Rotation et les fonctions
 * qui y sont reli�es. <p>
 * 
 * La classe Rotation permet de faire des calculs matriciels sur
 * des matrices de rotation.  Les matrices de rotation sont des
 * matrices orthonormales de dimension 3 x 3.  Toutes les
 * fonctionnalit�s de Matrix sont disponibles.  De plus, certaines
 * d'entre-elles peuvent avoir �t� optimis�es afin de tirer profit
 * de la sp�cialisation de la matrice de rotation.  Par exemple,
 * une inversion d'une matrice de rotation sera beaucoup plus
 * rapide que l'inversion d'une matrice ordinaire.  Une matrice de
 * rotation R aura 3 lignes et 3 colonnes.  Pour r�f�rencer
 * l'�l�ment situ� sur la 3e ligne et la 2e colonnes de la matrice
 * de rotation R, on notera R[2][1] car les lignes et les colonnes
 * sont num�rot�es � partir de 0.
 *
 * \author R�gis Houde    \date 17 novembre 97
 */

/*---------------------------------------------------------------------------*/
/*                     	      FICHIERS D'INCLUSIONS                          */
/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include "vectmath.h"

/**
 * Constructeur � partir d'une Matrix.
 * \author R�gis Houde    \date 4 juin 98
 * \param m la matrice d'entr�e
 */
Rotation::Rotation(const Matrix &m) : Matrix(ROT_DIM, ROT_DIM) 
{
    if((m.nrow() != ROT_DIM) || m.ncol() != ROT_DIM)
        vmError("Rotation: matrix must be 3 x 3\n");

    for(register int i = 0; i < ROT_DIM; i++)
    {
        for(register int j = 0; j < ROT_DIM; j++)
            (*this)[j][i] = m.get(j, i);
    }
      
}

/**
 * Constructeur � partir d'une Transform.
 * \author R�gis Houde    \date 4 juin 98
 * \param m la matrice d'entr�e
 */
Rotation::Rotation(const Transform &m) : Matrix(ROT_DIM, ROT_DIM) 
{
    for(register int i = 0;i < ROT_DIM; i++)
    {
        for(register int j = 0;j < ROT_DIM; j++)
            (*this)[j][i] = m.get(j, i);     
    }
}

/**
 * Constructeur � partir d'un Euler_angle.
 * \author R�gis Houde    \date 4 juin 98
 * \param e le vecteur d'entr�e
 */
Rotation::Rotation(const Euler_angle &e) : Matrix(ROT_DIM, ROT_DIM) 
{
#ifdef XYZ_EULER_ANGLES
    // Easy rob version
    double cx = cos(e.get(EULER3_RX)), sx = sin(e.get(EULER3_RX));
    double cy = cos(e.get(EULER3_RY)), sy = sin(e.get(EULER3_RY));
    double cz = cos(e.get(EULER3_RZ)), sz = sin(e.get(EULER3_RZ));
    
    (*this).v[0][0] = cy*cz;
    (*this).v[1][0] = sx*sy*cz + cx*sz;
    (*this).v[2][0] = sx*sz - cx*sy*cz;
    (*this).v[0][1] = -sz*cy;
    (*this).v[1][1] = cx*cz - sx*sy*sz;
    (*this).v[2][1] = cx*sy*sz + sx*cz;
    (*this).v[0][2] = sy;
    (*this).v[1][2] = -sx*cy;
    (*this).v[2][2] = cx*cy;

#else
	// Original MICROB
    double cx = cos(e.get(EULER3_RX)), sx = sin(e.get(EULER3_RX));
    double cy = cos(e.get(EULER3_RY)), sy = sin(e.get(EULER3_RY));
    double cz = cos(e.get(EULER3_RZ)), sz = sin(e.get(EULER3_RZ));
    
    (*this).v[0][0] = cy*cz;
    (*this).v[1][0] = cy*sz;
    (*this).v[2][0] = -sy;
    (*this).v[0][1] = cz*sx*sy - cx*sz;
    (*this).v[1][1] = cx*cz + sx*sy*sz;
    (*this).v[2][1] = cy*sx;
    (*this).v[0][2] = cx*cz*sy + sx*sz;
    (*this).v[1][2] = cx*sy*sz - cz*sx;
    (*this).v[2][2] = cx*cy;
    
#endif // XYZ_EULER_ANGLES
}

/**
 * Constructeur � partir d'un Euler_angle_xyz.
 * \author R�gis Houde    \date 8 d�cembre 98
 * \param e le vecteur d'entr�e
 */
Rotation::Rotation(const Euler_angle_xyz &e) : Matrix(ROT_DIM, ROT_DIM) 
{
    (*this) = (Euler_angle)e;
}

/**
 * Constructeur � partir d'un Quaternion.
 * \author R�gis Houde
 * \date 4 juin 98
 * \param q le vecteur d'entr�e
 */

// clean-up (SSA030205)

Rotation::Rotation(const Quaternion &q) : Matrix(ROT_DIM, ROT_DIM) 
{
    double dx2 = 2.0*q.get(QUAT4_QX)*q.get(QUAT4_QX);
    double dy2 = 2.0*q.get(QUAT4_QY)*q.get(QUAT4_QY);
    double dz2 = 2.0*q.get(QUAT4_QZ)*q.get(QUAT4_QZ);
    double dxy = 2.0*q.get(QUAT4_QX)*q.get(QUAT4_QY); 
    double dwz = 2.0*q.get(QUAT4_QW)*q.get(QUAT4_QZ);
    double dxz = 2.0*q.get(QUAT4_QX)*q.get(QUAT4_QZ);
    double dwy = 2.0*q.get(QUAT4_QW)*q.get(QUAT4_QY);
    double dyz = 2.0*q.get(QUAT4_QY)*q.get(QUAT4_QZ);
    double dwx = 2.0*q.get(QUAT4_QW)*q.get(QUAT4_QX);
  
    (*this).v[0][0] = 1.0 - dy2 - dz2;
    (*this).v[1][0] = dxy + dwz;
    (*this).v[2][0] = dxz - dwy;
  
    (*this).v[0][1] = dxy - dwz;
    (*this).v[1][1] = 1.0 - dx2 - dz2;
    (*this).v[2][1] = dyz + dwx;
  
    (*this).v[0][2] = dxz + dwy;
    (*this).v[1][2] = dyz - dwx;
    (*this).v[2][2] = 1 - dx2 - dy2;
}

/**
 * Constructeur � partir d'un Quaternion_xyz.
 * \author R�gis Houde    \date 9 d�cembre 98
 * \param q le vecteur d'entr�e
 */
Rotation::Rotation(const Quaternion_xyz &q) : Matrix(ROT_DIM, ROT_DIM) 
{
    (*this) = (Quaternion)q;
}

/**
 * Constructeur � partir d'un Angle_axis.
 * \author R�gis Houde    
 * \date 4 juin 98
 * \param "const Angle_axis &a" : le vecteur d'entr�e
 */
Rotation::Rotation(const Angle_axis &a) : Matrix(ROT_DIM, ROT_DIM) 
{
    if(a.msize() == 4)
    {	// Angle_axis_xyz avec theta, nx, ny, nz
        double x = a.get(1);
        double y = a.get(2);
        double z = a.get(3);
        double cost = cos(a.get(0));
        double sint = sin(a.get(0));
        double umct = 1 - cost;

        (*this).v[0][0] = x*x*umct + cost;
        (*this).v[1][0] = x*y*umct + z*sint;
        (*this).v[2][0] = x*z*umct - y*sint;

        (*this).v[0][1] = x*y*umct - z*sint;
        (*this).v[1][1] = y*y*umct + cost;
        (*this).v[2][1] = y*z*umct + x*sint;

        (*this).v[0][2] = x*z*umct + y*sint;
        (*this).v[1][2] = y*z*umct - x*sint;
        (*this).v[2][2] = z*z*umct + cost;
    }
    else	// Angle_axis_xyz avec tnx, tny, tnz
    {
        double t = sqrt(a.get(0)*a.get(0) + a.get(1)*a.get(1) + a.get(2)*a.get(2));
        if(fabs(t) > VM_SMALL) 
        {
            double x = a.get(0)/t;
            double y = a.get(1)/t;
            double z = a.get(2)/t;
            double cost = cos(t);
            double sint = sin(t);
            double umct = 1 - cost;

            (*this).v[0][0] = x*x*umct + cost;
            (*this).v[1][0] = x*y*umct + z*sint;
            (*this).v[2][0] = x*z*umct - y*sint;

            (*this).v[0][1] = x*y*umct - z*sint;
            (*this).v[1][1] = y*y*umct + cost;
            (*this).v[2][1] = y*z*umct + x*sint;

            (*this).v[0][2] = x*z*umct + y*sint;
            (*this).v[1][2] = y*z*umct - x*sint;
            (*this).v[2][2] = z*z*umct + cost;
        }
        else (*this) = mc_identity(3);	// Singularity de Angle_axis - 3
    }    
}

/**
 * Constructeur � partir d'un Angle_axis_xyz.
 * \author R�gis Houde    \date 4 juin 98
 * \param a le vecteur d'entr�e
 */
Rotation::Rotation(const Angle_axis_xyz &a) : Matrix(ROT_DIM, ROT_DIM) 
{
    if(a.msize() == ANGLE7_DIM)
    {	// Angle_axis_xyz avec theta, nx, ny, nz
        double x = a.get(ANGLE7_NX);
        double y = a.get(ANGLE7_NY);
        double z = a.get(ANGLE7_NZ);
        double cost = cos(a.get(ANGLE7_THETA));
        double sint = sin(a.get(ANGLE7_THETA));
        double umct = 1 - cost;

        (*this).v[0][0] = x*x*umct + cost;
        (*this).v[1][0] = x*y*umct + z*sint;
        (*this).v[2][0] = x*z*umct - y*sint;

        (*this).v[0][1] = x*y*umct - z*sint;
        (*this).v[1][1] = y*y*umct + cost;
        (*this).v[2][1] = y*z*umct + x*sint;

        (*this).v[0][2] = x*z*umct + y*sint;
        (*this).v[1][2] = y*z*umct - x*sint;
        (*this).v[2][2] = z*z*umct + cost;
    }
    else	// Angle_axis_xyz avec tnx, tny, tnz
    {
        double t = sqrt (  a.get(ANGLE6_TNX)*a.get(ANGLE6_TNX)
                         + a.get(ANGLE6_TNY)*a.get(ANGLE6_TNY)
                         + a.get(ANGLE6_TNZ)*a.get(ANGLE6_TNZ) );
        if(fabs(t) > VM_SMALL) 
        {
            double x = a.get(ANGLE6_TNX)/t;
            double y = a.get(ANGLE6_TNY)/t;
            double z = a.get(ANGLE6_TNZ)/t;
            double cost = cos(t);
            double sint = sin(t);
            double umct = 1 - cost;

            (*this).v[0][0] = x*x*umct + cost;
            (*this).v[1][0] = x*y*umct + z*sint;
            (*this).v[2][0] = x*z*umct - y*sint;

            (*this).v[0][1] = x*y*umct - z*sint;
            (*this).v[1][1] = y*y*umct + cost;
            (*this).v[2][1] = y*z*umct + x*sint;

            (*this).v[0][2] = x*z*umct + y*sint;
            (*this).v[1][2] = y*z*umct - x*sint;
            (*this).v[2][2] = z*z*umct + cost;
        }
        else (*this) = mc_identity(ROT_DIM);	// Singularity de Angle_axis_xyz - 6
    }    
  
}

/**
 * Calcule l'inverse de la rotation.
 * \author Jean C�t�   \author R�gis Houde    \date 18 novembre 97
 * \param mi matrice d'entr�e
 * \return matrice inverse ou pseudo-inverse.
 */
Rotation mc_inv(const Rotation& ri)
{
    // The inverse of a 3x3 rotation/orthogonal matrix is equal to
    // its mc_transpose.

    Rotation r;      
            	    	  
    r[0][0] = ri.v[0][0];
    r[0][1] = ri.v[1][0];
    r[0][2] = ri.v[2][0];
  
    r[1][0] = ri.v[0][1];
    r[1][1] = ri.v[1][1];
    r[1][2] = ri.v[2][1];
  
    r[2][0] = ri.v[0][2];
    r[2][1] = ri.v[1][2];
    r[2][2] = ri.v[2][2];
  
    return(r);
}

/**
 * Retourne la transpos�e d'une Rotation.
 * \author R�gis Houde    \date 4 juin 98
 * \param ri la matrice d'entr�e
 * \return la transpos�e de la matrice d'entr�e
 */
Rotation mc_transpose(const Rotation& ri)
{
    Rotation r;      
            	    	  
    r[0][0] = ri.v[0][0];
    r[0][1] = ri.v[1][0];
    r[0][2] = ri.v[2][0];
  
    r[1][0] = ri.v[0][1];
    r[1][1] = ri.v[1][1];
    r[1][2] = ri.v[2][1];
  
    r[2][0] = ri.v[0][2];
    r[2][1] = ri.v[1][2];
    r[2][2] = ri.v[2][2];
  
    return(r);
}

/**
 * Calcule le Vect de la matrice.
 * <pre>
 *               Soit M =  a  b  c  
 *                         e  f  g  
 *                         i  j  k  
 *                          
 *               Vect = {(j-g)/2, (c-i)/2, (e-b)/2}
 * </pre>
 *
 * \author Jean C�t�    \date 18 d�cembre 96
 * \param mi la matrice d'entr�e
 * \return Le Vect
 */
Vector mc_vect(const Rotation& mi)
{
    Vector ms(3);
    ms[0] = (mi.v[2][1] - mi.v[1][2])/2.0;
    ms[1] = (mi.v[0][2] - mi.v[2][0])/2.0;
    ms[2] = (mi.v[1][0] - mi.v[0][1])/2.0;

    return ms;
}

/**
 * Surcharge de l'op�rateur "*" entre deux rotations. On fait tous les calculs
 * directement sans utilis� de boucle pour optimiser la fonction.
 * \author R�gis Houde    \date 18 novembre 97
 * \param t1 la rotation de gauche
 * \param t2 la rotation de droite
 * \return le r�sultat t1 * t2
 */
Rotation operator * (const Rotation& t1, const Rotation& t2)
{
    Rotation t;

    t.v[0][0] = t1.v[0][0]*t2.v[0][0] + t1.v[0][1]*t2.v[1][0] + t1.v[0][2]*t2.v[2][0];
    t.v[0][1] = t1.v[0][0]*t2.v[0][1] + t1.v[0][1]*t2.v[1][1] + t1.v[0][2]*t2.v[2][1];
    t.v[0][2] = t1.v[0][0]*t2.v[0][2] + t1.v[0][1]*t2.v[1][2] + t1.v[0][2]*t2.v[2][2];
  
    t.v[1][0] = t1.v[1][0]*t2.v[0][0] + t1.v[1][1]*t2.v[1][0] + t1.v[1][2]*t2.v[2][0];
    t.v[1][1] = t1.v[1][0]*t2.v[0][1] + t1.v[1][1]*t2.v[1][1] + t1.v[1][2]*t2.v[2][1];
    t.v[1][2] = t1.v[1][0]*t2.v[0][2] + t1.v[1][1]*t2.v[1][2] + t1.v[1][2]*t2.v[2][2];
  
    t.v[2][0] = t1.v[2][0]*t2.v[0][0] + t1.v[2][1]*t2.v[1][0] + t1.v[2][2]*t2.v[2][0];
    t.v[2][1] = t1.v[2][0]*t2.v[0][1] + t1.v[2][1]*t2.v[1][1] + t1.v[2][2]*t2.v[2][1];
    t.v[2][2] = t1.v[2][0]*t2.v[0][2] + t1.v[2][1]*t2.v[1][2] + t1.v[2][2]*t2.v[2][2];
    
    return t;
}

/**
 * Surcharge de l'op�rateur "*" entre une Rotation et un Vector3.
 * \author R�gis Houde    \date 7 mars 2000
 * \param r la Rotation
 * \param v le Vector3
 * \return le resultat
 */
Vector3 operator * (const Rotation& r, const Vector3& v)
{
    Vector3 m;
  
    m[0] = r.get(0, 0)*v.get(0) + r.get(0, 1)*v.get(1) + r.get(0, 2)*v.get(2);
    m[1] = r.get(1, 0)*v.get(0) + r.get(1, 1)*v.get(1) + r.get(1, 2)*v.get(2);
    m[2] = r.get(2, 0)*v.get(0) + r.get(2, 1)*v.get(1) + r.get(2, 2)*v.get(2);
  
    return m;
}

/**
 * Surcharge de l'op�rateur / entre deux matrices.
 * \author Jean C�t�    \date 17 d�cembre 96
 * \param mn le terme de gauche
 * \param md le terme de doite
 * \return Le r�sultat
 */
Rotation operator / (const Rotation& mn, const Rotation& md)
{
    return (mc_inv(md)*mn);
}

/**
 * Surcharge de l'op�rateur + entre 2 Rotations. On imprime un message
 * indiquant que cette op�ration n'a pas de signification.
 * \author R�gis Houde    \date 4 juin 98
 */
void operator + (const Rotation& , const Rotation& )
{
    vmError("operator+(const Rotation&, const Rotation&) : non-sens operation\n");
}

/**
 * Orthonormalise une matrice de rotation. On fait la d�composition en valeurs
 * singuli�res (m = U*S*trans(V)) de la matrice puis on multiplie U par mc_transpose(V).
 * \author R�gis Houde    \date 13 f�vrier 98
 */
Rotation &Rotation::ortho()
{
  Matrix U, V;
  Vector S;

  mc_svd(*this, U, S, V);

  if (V == mc_null_matrix)
      ortho_fast();
  else
  {
      if(mc_det(U)*mc_det(V) >= 0.0)
          *this = U*mc_transpose(V);
      else
      {
          Matrix s(S.dim(),S.dim());
          s = 0.0;
          for(int i=0;i< S.dim();i++)
              s[i][i] = 1;
          s[S.dim()-1][S.dim()-1] = -1;
          *this = U*s*mc_transpose(V);
      }
  }
  
  return *this;
}


/**
 * Approxime une orthonormalisation de la Rotation. Soit les vecteurs colonnes
 * de la Rotation : va, vn et vo, on rend va unitaire puis on fait:
 * <pre>
 * 	vn = mc_vectorial_product(vo, va);
 * 	vo = mc_vectorial_product(va, vn).unit();
 * 	vn = mc_vectorial_product(vo, va);
 * </pre>
 * \author R�gis Houde    \date 4 juin 98
 * \bug Cet algorithme n'utilises pas du tout vn pour orthonormaliser.
 */
Rotation &Rotation::ortho_fast()	// Inspired from kali
{					// There must be something better!
    Vector3 va, vn, vo;
  
    va[0] = v[0][2];
    va[1] = v[1][2];
    va[2] = v[2][2];
  
    va.unit();
    
    vo[0] = v[0][1];
    vo[1] = v[1][1];
    vo[2] = v[2][1];
  
    vn = mc_vectorial_product(vo, va);
    vo = mc_vectorial_product(va, vn).unit();
    vn = mc_vectorial_product(vo, va);
  
    v[0][0] = vn[0];
    v[1][0] = vn[1];
    v[2][0] = vn[2];
  
    v[0][1] = vo[0];
    v[1][1] = vo[1];
    v[2][1] = vo[2];
  
    v[0][2] = va[0];
    v[1][2] = va[1];
    v[2][2] = va[2];
    
    return *this;
}

/**
 * Approxime une orthonormalisation de la Rotation. On transforme en
 * Angle_axis_xyz puis on retransforme en Rotation.
 * \author R�gis Houde    \date 4 juin 98
 */
Rotation &Rotation::ortho_very_fast()	
{		

    double val = (v[0][0] + v[1][1] + v[2][2] - 1)/2;
    if(val <  -1) 
    {    
        return ortho_fast();
    }
    double t = (fabs(val)>=1.0) ? 0:acos(val); // theta
    double x, y, z, k;
 
    if(fabs(sin(t)) > VM_SMALL)
    {
        k = 0.5/sin(t);
    }
    else
    {
        k = 1;
    }

    x = k*(v[2][1] - v[1][2]);
    y = k*(v[0][2] - v[2][0]);
    z = k*(v[1][0] - v[0][1]);

    double u = sqrt(x*x + y*y + z*z);	// On rend unitaire pour eviter les erreurs d'arrondi...  

    x /= u;
    y /= u;
    z /= u;
    
    double cost = cos(t);
    double sint = sin(t);
    double umct = 1 - cost;
  
    v[0][0] = x*x*umct + cost;
    v[1][0] = x*y*umct + z*sint;
    v[2][0] = x*z*umct - y*sint;
  
    v[0][1] = x*y*umct - z*sint;
    v[1][1] = y*y*umct + cost;
    v[2][1] = y*z*umct + x*sint;
  
    v[0][2] = x*z*umct + y*sint;
    v[1][2] = y*z*umct - x*sint;
    v[2][2] = z*z*umct + cost;
  
  		
    return (*this);
}

/**
 * Calcule la rotation sous forme matricielle 3x3.
 * \author Jean C�t�    \date 18 d�cembre 96
 * \param Orientation et position
 * \return La matrice de rotation mc_Rz*mc_Rx*mc_Ry 
 */
Rotation mc_rz_rx_ry(Vector &vector)
{
    Rotation R;
    
    double cx = cos(vector[0]), sx = sin(vector[0]);
    double cy = cos(vector[1]), sy = sin(vector[1]);
    double cz = cos(vector[2]), sz = sin(vector[2]);

    R.v[0][0] = cy*cz - sx*sy*sz;
    R.v[1][0] = cz*sx*sy + cy*sz;
    R.v[2][0] = -cx*sy;
    R.v[0][1] = -cx*sz;
    R.v[1][1] = cx*cz;
    R.v[2][1] = sx;
    R.v[0][2] = cz*sy + cy*sx*sz;
    R.v[1][2] = -cy*cz*sx + sy*sz;
    R.v[2][2] = cx*cy;
    
    return(R);
}

/**
 * Calcule la rotation d�g�n�r�e sous forme matricielle 3x3.
 * \author Jean C�t�    \date 18 d�cembre 96
 * \param rx La rotation autour de l'axe des X
 * \return La matrice de rotation mc_Rx
 * \sa mc_rz_rx_ry
 */
Rotation mc_Rx(double rx)
{
    Rotation R;

    double cx = cos(rx), sx = sin(rx);

    R.v[0][0] = 1;
    R.v[1][0] = 0;
    R.v[2][0] = 0;
    R.v[0][1] = 0;
    R.v[1][1] = cx;
    R.v[2][1] = sx;
    R.v[0][2] = 0;
    R.v[1][2] = -sx;
    R.v[2][2] = cx;
    
    return(R);
}

/**
 * Calcule la rotation d�g�n�r�e sous forme matricielle 3x3.
 * \author Jean C�t�    \date 18 d�cembre 96
 * \param rx La rotation autour de l'axe des Y
 * \return La matrice de rotation mc_Ry
 * \sa mc_rz_rx_ry
 */
Rotation mc_Ry(double ry)
{
    Rotation R;

    double cy = cos(ry), sy = sin(ry);

    R.v[0][0] = cy;
    R.v[1][0] = 0;
    R.v[2][0] = -sy;
    R.v[0][1] = 0;
    R.v[1][1] = 1;
    R.v[2][1] = 0;
    R.v[0][2] = sy;
    R.v[1][2] = 0;
    R.v[2][2] = cy;
    
    return(R);
}

/**
 * Calcule la rotation d�g�n�r�e sous forme matricielle 3x3.
 * \author Jean C�t�    \date 18 d�cembre 96
 * \param rx La rotation autour de l'axe des Z
 * \return La matrice de rotation mc_Rz
 * \sa mc_rz_rx_ry
 */
Rotation mc_Rz(double rz)
{
    Rotation R;

    double cz = cos(rz), sz = sin(rz);

    R.v[0][0] = cz;
    R.v[1][0] = sz;
    R.v[2][0] = 0;
    R.v[0][1] = -sz;
    R.v[1][1] = cz;
    R.v[2][1] = 0;
    R.v[0][2] = 0;
    R.v[1][2] = 0;
    R.v[2][2] = 1;
    
    return(R);
}

/**
 * Convertit une matrice de rotation, en vecteur rz, rx, ry.
 * Fonction inverse de Rotation mc_rz_rx_ry(rx, ry, rz)
 * \author Jean C�t�    \date 18 d�cembre 96
 * \param Q La matrice de rotation
 * \return Le vecteur de la solution.
 */
Vector mc_rz_rx_ry(const Rotation &Q)
{
    double cx,sx,cy,sy,cz,sz,cyz,syz;
    Vector vector(3);
    
    sx = Q.v[2][1];
    if(fabs(sx) == 1.0)
    {
        cx  = 0.0;
        cyz = Q.v[0][0];
        syz = Q.v[1][0];
        vector[0] = atan2(sx,cx);
        vector[1] = 0;
        vector[2] = atan2(syz,cyz);
    }
    else
    {
        cx = sqrt(Q.v[2][0]*Q.v[2][0] + Q.v[2][2]*Q.v[2][2]);
        sy = -Q.v[2][0]/cx;
        cy =  Q.v[2][2]/cx;
        sz = -Q.v[0][1]/cx;
        cz =  Q.v[1][1]/cx;
        vector[0] = atan2(sx,cx);
        vector[1] = atan2(sy,cy);
        vector[2] = atan2(sz,cz);
    }
    
    return(vector);
}

/**
 * Convertit une matrice de rotation, en vecteur rz, ry, rx.
 * Fonction inverse de Rotation mc_rz_ry_rx(rx, ry, rz)
 * \author Jean C�t�    \date 18 d�cembre 96
 * \param Q La matrice de rotation
 * \return Le vecteur de la solution.
 */
Vector mc_rz_ry_rx(const Rotation &Q)
{
    double cx,sx,cy,sy,cz,sz,cxz,sxz;
    Vector vector(3);
    
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
   
	vector[0] = 0;
	vector[1] = atan2(sy,cy);
	vector[2] = atan2(sxz,cxz);
    }
    else
    {
	cy = sqrt(Q.v[2][1]*Q.v[2][1] + Q.v[2][2]*Q.v[2][2]);
	sx = Q.v[2][1]/cy;
	cx = Q.v[2][2]/cy;
	sz = Q.v[1][0]/cy;
	cz = Q.v[0][0]/cy;
	vector[0] = atan2(sx,cx);
	vector[1] = atan2(sy,cy);
	vector[2] = atan2(sz,cz);
    }

    return(vector);
}

/**
 * Convertit une matrice de rotation, en vecteur rz, rx, ry.
 * Fonction inverse de Rotation mc_rz_rx_ry(rx, ry, rz)      
 * \author Jean C�t�    \date 18 d�cembre 96
 * \param Q La matrice de rotation
 * \param rx la rotation autour de l'axe des X
 * \param ry la rotation autour de l'axe des Y
 * \param rz la rotation autour de l'axe des Z
 * \return Le vecteur de la solution.
 */
Vector mc_rz_rx_ry(const Rotation &Q, double &rx, double &ry, double &rz)
{
    Vector v;
    
    v = mc_rz_rx_ry(Q);
    
    rx = v[0];
    ry = v[1];
    rz = v[2];

    return(v);
}

/**
 * Convertit une matrice de rotation, en vecteur rz, ry, rx.
 * Fonction inverse de Rotation mc_rz_ry_rx(rx, ry, rz)      
 * \author Jean C�t�    \date 22 janvier 97
 * \param Q La matrice de rotation
 * \param rx la rotation autour de l'axe des X
 * \param ry la rotation autour de l'axe des Y
 * \param rz la rotation autour de l'axe des Z
 * \return Le vecteur de la solution.
 */
Vector mc_rz_ry_rx(const Rotation &Q, double &rx, double &ry, double &rz)
{
    Vector v;
    
    v = mc_rz_ry_rx(Q);
    
    rx = v[0];
    ry = v[1];
    rz = v[2];

    return(v);
}

/*---------------------------------------------------------------------------*/
/*                               FIN DU FICHIER                              */
/*---------------------------------------------------------------------R.H.--*/
