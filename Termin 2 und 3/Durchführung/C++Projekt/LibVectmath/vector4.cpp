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

// $Id: vector4.C,v 1.7 2002/08/30 15:35:32 ch7905 Exp $

/** \file
 * Implémente les méthodes de la classe Vector4 et les fonctions qui y sont
 * reliées.
 * \author Régis Houde    \date 23 janvier 1998
 */

/*---------------------------------------------------------------------------*/
/*                     	      FICHIERS D'INCLUSIONS                          */
/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include "vectmath.h"

/**
 * Constructeur à partir de 4 doubles.
 * \author Régis Houde    \date 4 juin 98
 * \param v1 le premier élément
 * \param v2 le deuxième élément
 * \param v3 le troisième élément
 * \param v4 le quatrième élément
 */
Vector4::Vector4(double v1, double v2, double v3, double v4) : Vector(4) 
{
    (*this)[0] = v1;
    (*this)[1] = v2;
    (*this)[2] = v3;
    (*this)[3] = v4;
}

/**
 * Constructeur à partir d'un autre Vector4.
 * \author Régis Houde    \date 4 juin 98
 * \param m le vecteur à copier
 */
Vector4::Vector4(const Vector4 &m)
{
    if(m.ncol() > m.nrow())
    {
        if(New(1, 4) == -1)
            vmError("Can't allocate memory\n");   
    }	 
    else  
    {
        if(New(4, 1) == -1)
            vmError("Can't allocate memory\n");      
    }	
    (*this)[0] = m.get(0);
    (*this)[1] = m.get(1);
    (*this)[2] = m.get(2);
    (*this)[3] = m.get(3);
}

/**
 * Constructeur à partir d'un Vector.
 * \author Régis Houde   \date 4 juin 98
 * \param m le vecteur à copier
 */
Vector4::Vector4(const Vector &m)
{
    if(!m.is_a_vector(4))
        vmError("Vector4::Vector4(const Vector &m) : Dimensions do not match\n"); 
    
    if(m.ncol() > m.nrow())
    {
        if(New(1, 4) == -1)
            vmError("Can't allocate memory\n"); 
    }  
    else  
    {
        if(New(4, 1) == -1)
            vmError("Can't allocate memory\n"); 
    }	
    (*this)[0] = m.get(0);
    (*this)[1] = m.get(1);
    (*this)[2] = m.get(2);
    (*this)[3] = m.get(3);
      
}

/**
 * Constructeur à partir d'une Matrix.
 * \author Régis Houde    \date 4 juin 98
 * \param m la matrice à copier
 */
Vector4::Vector4(const Matrix &m)
{
    if(!m.is_a_vector(4))
        vmError("Vector4::Vector4(const Matrix &m) : Dimensions do not match\n"); 
  
    if(m.ncol() > m.nrow())
    {
        if(New(1, 4) == -1)
            vmError("Can't allocate memory\n"); 
	
        (*this)[0] = m.get(0, 0);
        (*this)[1] = m.get(0, 1);
        (*this)[2] = m.get(0, 2);
        (*this)[3] = m.get(0, 3);
    }  
    else  
    {
        if(New(4, 1) == -1)
            vmError("Can't allocate memory\n");      

        (*this)[0] = m.get(0, 0);
        (*this)[1] = m.get(1, 0);
        (*this)[2] = m.get(2, 0);
        (*this)[3] = m.get(3, 0);
    }	
}

/**
 * Calcule le produit vectoriel entre deux vecteurs.
 * \author Jean Côté    \date 18 décembre 96
 * \param a vecteur de gauche
 * \param b vecteur de droite
 * \return le produit
 */
Vector4 mc_vectorial_product(const Vector4 &a, const Vector4 &b)
{
    Vector4 r;
    
    r[0] = a.get(1)*b.get(2) - a.get(2)*b.get(1);
    r[1] = a.get(2)*b.get(0) - a.get(0)*b.get(2);
    r[2] = a.get(0)*b.get(1) - a.get(1)*b.get(0);
    r[3] = a.get(3)*b.get(3);
    
    if(fabs(r[3]) > VM_SMALL)
        r /= r[3];
    else
        vmError("mc_vectorial_product(const Vector4 &a, const Vector4 &b) : a[3] and b[3] must not be 0");  
    
    return(r);
}

/*---------------------------------------------------------------------------*/
/*                               FIN DU FICHIER                              */
/*---------------------------------------------------------------------R.H.--*/
