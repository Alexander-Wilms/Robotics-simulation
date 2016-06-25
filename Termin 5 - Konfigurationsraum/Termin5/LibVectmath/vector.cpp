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

// $Id: vector.C,v 1.8 2003/02/25 15:12:08 serge Exp $

/** \file
 * Impl�mente les m�thodes de la classe Vector et les fonctions qui
 * y sont reli�es. <p>
 * 
 * Les classes Vector, Vecrtor3 et Vector4 sont des classes
 * d�riv�es de la classe Matrix.  Elles supportent les cas
 * particuliers des vecteurs.  Toutes les fonctionnalit�s de Matrix
 * sont disponibles si applicables.  De plus, certaines d'entre-
 * elles peuvent avoir �t� optimis�es afin de tirer profit de la
 * sp�cialisation du vecteur.  Les Vector peuvent �tre de longueur
 * quelconque alors que les Vector3 et les Vector4 sont de longueur
 * 3 et 4 respectivement.  Il existe 2 sortes de vecteurs : les
 * vecteurs colonnes et les vecteurs lignes.  Le vecteur colonne
 * est un cas particulier d'une matrice (nrow x 1) et le vecteur
 * ligne en est un d'une matrice (1 x ncol). Pour r�f�rencer le xe
 * �l�ment d'un vecteur ligne ou colonne V, on notera V[x - 1] car
 * l'indi�age d�bute � 0.
 *
 * \author R�gis Houde    \date 17 novembre 97
 */

/*---------------------------------------------------------------------------*/
/*                     	      FICHIERS D'INCLUSIONS                          */
/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include "vectmath.h"
//#include "Utilities/debug.h"

/**
 * Constructeur � partir d'une Matrix.
 * \author R�gis Houde    \date 4 juin 98
 * \param m la matrice � copier
 */
Vector::Vector(const Matrix &m)
{	
    if((m.ncol() != 1) && (m.nrow() != 1))
        vmError("Vector::Vector(const Matrix &m) : m is not N x 1 or 1 x N\n");
    
    if(m.ncol() > m.nrow()) 
    {
        if(New(1, m.ncol()) == -1)
            vmError("Can't allocate memory\n");
        for(int i=0;i<m.ncol();i++)
            (*this)[i] = m.get(0, i);
    }  
    else  
    {
        if(New(m.nrow(), 1) == -1)
            vmError("Can't allocate memory\n");
        for(int i=0;i<m.nrow();i++)
            (*this)[i] = m.get(i, 0);
    }	
}

/**
 * Constructeur � partir d'un Vector.
 * \author R�gis Houde    \date 4 juin 98
 * \param m le vecteur � copier
 */
Vector::Vector(const Vector &m) : Matrix(m.nrow(), m.ncol())
{
    for(int i=0;i<m.nrow()*m.ncol();i++)
        (*this)[i] = m.get(i);
}

/**
 * Calcule la norme d'un vecteur.
 * La racine carr�e de la somme des carr�es des �l�ments du vecteur.
 * \author Jean C�t�    \date 18 d�cembre 96
 * \return la norme
 * \sa Vector::unit
 */
double Vector::norm() const
{
    double n = 0.0;
    for(int i=0;i<nrow()*ncol();i++)
        n += get(i)*get(i);
    return(sqrt(n));
}

/**
 * Surcharge de l'op�rateur �galit� entre un vecteur et un scalaire.
 * \author Jean C�t�    \date 27 janvier 97
 * \param value La valeur � copier dans le vecteur
 * \return La r�f�rence du vecteur de sortie
 * \remarks Le vecteur de sortie sera de dimension 1x1 avec la valeur value
 */
Vector& Vector::operator = (const double value)
{
    if(y == 0 || x == 0 || v == NULL)
    {
	if(New(1,1) == -1)
	    vmError("can't allocate memory\n");
	if(v == NULL)
	    vmError("Vector::operator = (const Vector& ve.v = NULL)\n");
    }

    for(register int j = 0;j < size; j++)
	(*this)[j] = value;

    return *this;
}

/**
 * Surcharge de l'op�rateur �galit� entre deux vecteurs.
 * \author Jean C�t�    \date 27 janvier 97
 * \param value La valeur � copier dans le vecteur
 * \return La r�f�rence du vecteur de sortie
 * \remarks Le vecteur de sortie sera de dimension 1x1 avec la valeur value
 */
Vector& Vector::operator = (const Vector &V)
{    
    if(y != V.y || x != V.x) 
    {
        Delete();
        if(New(V.nrow(), V.ncol()) == -1)
            vmError("Can't allocate memory\n");
    }  
    for(int i=0;i<V.nrow()*V.ncol();i++)
        (*this)[i] = V.get(i);


    return (*this);    
}

/**
 * Surcharge de l'op�rateur = Matrix.
 * \author R�gis Houde    \date 4 juin 98
 * \param m la matrice � copier
 */
Vector& Vector::operator = (const Matrix &m)
{
    if((m.ncol() == 0) && (m.nrow() == 0))
    {
        Delete();
        New(0, 0);
        return (*this);
    }
  
    if((m.ncol() != 1) && (m.nrow() != 1))
        vmError("Vector::operator = (const Matrix &m) : m is not N x 1 or 1 x N\n");
    
    if(ncol() != m.ncol() || nrow() != m.nrow()) Delete();
    
    if(m.ncol() > m.nrow())
    {
        Delete();
        if(New(1, m.ncol()) == -1)
            vmError("Can't allocate memory\n");
        for(int i=0;i<m.ncol();i++)
            (*this)[i] = m.get(0, i);
    }  
    else  
    {
        Delete();
        if(New(m.nrow(), 1) == -1)
            vmError("Can't allocate memory\n");
        for(int i=0;i<m.nrow();i++)
            (*this)[i] = m.get(i, 0);
    }	

    return (*this);    
}

/**
 * Rend le vecteur unitaire.
 * On divise chaque �l�ment par sa norme (Vector::norm)
 * \author R�gis Houde    \date 4 juin 98
 * \return l'objet rendu unitaire
 * \sa Vector::norm
 */
Vector & Vector::unit()
{
    double sc = norm();
   
    if(fabs(sc) > VM_SMALL)
    {
        sc = 1/sc;		// For speed
        for(int i = 0; i < size; ++i)
            (*this)[i] *= sc;
    } 
    else
        vmError("The vector's norm is null, can't make it unit\n");
    
    return (*this);
}

/**
 * Cr�e un vecteur ligne de dimension 1 x d.
 * La commande "Vector v(6);" permet de cr�er un vecteur de dimension
 * 6. La fonction mc_transpose(6) permet de cr�er un vecteur de dimension
 * 1 x 6. Exemple : "Vector v = mc_transpose(6);"
 * \author R�gis Houde    \date 4 juin 98
 * \param d Longueur du vecteur ligne
 * \return un vecteur de dimension 1 x d
 */
Vector mc_transpose(int d)
{
    Matrix m(1, d);
    return (Vector)m;
}

/**
 *	Calulates the length of the vector
 *	\author Michel Menne	\date 20. september 2003
 *	\param a vector
 *	\return the lenght of the vector
 */
double mc_length(const Vector &a)
{
	int i;
	double s = 0;

	for(i = 0; i < a.msize(); ++i)
        s += a.get(i) * a.get(i);

	return sqrt(s);
}

/**
 * Calcule le produit scalaire entre deux vecteurs.
 * \author Jean C�t�    \date 4 fevrier 98
 * \param a vecteur de gauche
 * \param b vecteur de droite
 * \return le produit
 */
double mc_scalar_product(const Vector &a, const Vector &b)
{
    int i;
    double s = 0;

    if((a.msize()) != (b.msize()))
        vmError("mc_scalar_product : dimensions do not match\n"); 

    for(i = 0; i < a.msize(); ++i)
        s += a.get(i)*b.get(i);

    return(s);
}

/**
 * Surcharge de l'op�rateur "-" entre deux vecteurs.
 * \author R�gis Houde    \date 4 juin 02
 * \param v1 le vecteur de gauche
 * \param v2 le vecteur de droite
 * \return La diff�rence des matrices (m1 - m2)
 */
Vector operator - (const Vector& v1,const Vector& v2)
{
    if((v1.dim() != v2.dim())  
       || (v1.ncol() == 0 || v1.nrow() == 0) || v1.v == NULL || v2.v == NULL)
        vmError("Vector: improper dimensions: operator-(Vector,Vector)\n");

    Vector v(v1.dim());

    for(register int i = 0;i < v.dim(); i++)
	    v[i] = v1.get(i) - v2.get(i);

    return v;
}

/**
 * Surcharge de l'op�rateur "+" entre deux vecteurs.
 * \author R�gis Houde    \date 4 juin 02
 * \param v1 le vecteur de gauche
 * \param v2 le vecteur de droite
 * \return La diff�rence des matrices (m1 - m2)
 */
Vector operator + (const Vector& v1,const Vector& v2)
{
    if((v1.dim() != v2.dim())  
       || (v1.ncol() == 0 || v1.nrow() == 0) || v1.v == NULL || v2.v == NULL)
        vmError("Vector: improper dimensions: operator-(Vector,Vector)\n");

    Vector v(v1.dim());

    for(register int i = 0;i < v.dim(); i++)
	    v[i] = v1.get(i) + v2.get(i);

    return v;
}
/*---------------------------------------------------------------------------*/
/*                               FIN DU FICHIER                              */
/*---------------------------------------------------------------------R.H.--*/
