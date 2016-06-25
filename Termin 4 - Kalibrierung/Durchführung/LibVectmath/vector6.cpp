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

// $Id: vector6.C,v 1.7 2002/08/30 15:35:33 ch7905 Exp $

/** \file
 * Impl�mente les m�thodes de la classe Vector6 et les fonctions qui
 * y sont reli�es.
 * \author R�gis Houde     \date 8 d�cembre 1998
 */

/*---------------------------------------------------------------------------*/
/*                     	      FICHIERS D'INCLUSIONS                          */
/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include "vectmath.h"

/**
 * Constructeur � partir de 6 doubles.
 * \author R�gis Houde     \date 8 d�cembre 98
 * \param v1 premier �l�ment
 * \param v2 deuxi�me �l�ment
 * \param v3 troisi�me �l�ment
 * \param v4 quatri�me �l�ment
 * \param v5 cinqui�me �l�ment
 * \param v6 sixi�me �l�ment
 */
Vector6::Vector6(double v1, double v2, double v3, double v4, double v5,
                 double v6) : Vector(6)
{
    (*this)[0] = v1;
    (*this)[1] = v2;
    (*this)[2] = v3;
    (*this)[3] = v4;
    (*this)[4] = v5;
    (*this)[5] = v6;
}

/**
 * Constructeur � partir d'un autre Vector6.
 * \author R�gis Houde    \date 8 d�cembre 98
 * \param m le vecteur � copier
 */
Vector6::Vector6(const Vector6 &m)
{
  
    if(m.ncol() > m.nrow())
    {
        if(New(1, 6) == -1)
            vmError("Can't allocate memory\n");   
    }	 
    else  
    {
        if(New(6, 1) == -1)
            vmError("Can't allocate memory\n");      
    }	
    (*this)[0] = m.get(0);
    (*this)[1] = m.get(1);
    (*this)[2] = m.get(2);
    (*this)[3] = m.get(3);
    (*this)[4] = m.get(4);
    (*this)[5] = m.get(5);
}

/**
 * Constructeur � partir d'un Vector.
 * \author R�gis Houde    \date 8 d�cembre 98
 * \param m le vecteur � copier
 */
Vector6::Vector6(const Vector &m)
{
    if(!m.is_a_vector(6))
        vmError("Vector6::Vector6(const Vector &m) : Dimensions do not match\n"); 
    

    if(m.ncol() > m.nrow())
    {
        if(New(1, 6) == -1)
            vmError("Can't allocate memory\n"); 
    }  
    else  
    {
        if(New(6, 1) == -1)
            vmError("Can't allocate memory\n"); 
    }	
    (*this)[0] = m.get(0);
    (*this)[1] = m.get(1);
    (*this)[2] = m.get(2);
    (*this)[3] = m.get(3);
    (*this)[4] = m.get(4);
    (*this)[5] = m.get(5);
      
}

/**
 * Constructeur � partir d'une Matrix.
 * \author R�gis Houde    \date 8 d�cembre 98
 * \param m la matrice � copier
 */
Vector6::Vector6(const Matrix &m)
{ 
    if(!m.is_a_vector(6))
        vmError("Vector6::Vector6(const Matrix &m) : -> Dimensions do not match\n"); 
  
    if(m.ncol() > m.nrow())
    {
        if(New(1, 6) == -1)
            vmError("Can't allocate memory\n"); 
	
        (*this)[0] = m.get(0, 0);
        (*this)[1] = m.get(0, 1);
        (*this)[2] = m.get(0, 2);
        (*this)[3] = m.get(0, 3);
        (*this)[4] = m.get(0, 4);
        (*this)[5] = m.get(0, 5);
    }  
    else  
    {
        if(New(6, 1) == -1)
            vmError("Can't allocate memory\n");      

        (*this)[0] = m.get(0, 0);
        (*this)[1] = m.get(1, 0);
        (*this)[2] = m.get(2, 0);
        (*this)[3] = m.get(3, 0);
        (*this)[4] = m.get(4, 0);
        (*this)[5] = m.get(5, 0);
    }	
}

/*---------------------------------------------------------------------------*/
/*                               FIN DU FICHIER                              */
/*---------------------------------------------------------------------R.H.--*/
