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

// $Id: vector.h,v 1.6 2002/08/30 15:35:32 ch7905 Exp $

/** \file
 * \author Jean Côté    \date 18 décembre 96
 */

/*---------------------------------------------------------------------------*/
/*                     	      FICHIERS D'INCLUSIONS                          */
/*---------------------------------------------------------------------------*/
#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>

#include "matrix.h"

/*---------------------------------------------------------------------------*/
/*                     	      CLASSES ET DEFINES                             */
/*---------------------------------------------------------------------------*/

/**
 * Permet de travailler avec la classe Matrix en mode Vector.
 * \author Jean Côté    \date 18 décembre 96
 */
class Vector : public Matrix 
{
 private:	
    inline Vector(int h, int l) : Matrix(h, l) { };
 public:
    inline double &operator [] (int z) {if(nrow() > 1) return(v[z][0]); return(v[0][z]);};
    inline double get(int z) const {if(nrow() > 1) return(v[z][0]); return(v[0][z]);};
    inline Vector(int h) : Matrix(h, 1) { };
    inline Vector() : Matrix() { };
    Vector(const Matrix &m); 
    Vector(const Vector &m); 
    inline ~Vector() {};
    inline double norme() const {printf("norme : Use norm instead!!!\n");return norm();};
    double norm() const;
    Vector& operator = (const double);
    Vector& operator = (const Vector &);
    Vector& operator = (const Matrix &);
    Vector& unit();
    inline int dim() const {return nrow()*ncol();};
	
    inline Matrix& print(const char *msg = NULL, const char *name = "Vector") {return(Matrix::print(msg, name));};
	
    friend Vector mc_transpose(int);

	friend Vector operator - (const Vector&, const Vector&);
	friend Vector operator - (double s,const Vector &v) {return (Vector)(s - (Matrix)v);};
    friend Vector operator - (const Vector &v, double s) {return (Vector)((Matrix)v  - s);};
//	friend Matrix operator - (const Matrix &m,const Vector &v) {return (m - (Matrix)v);};
//    friend Matrix operator - (const Vector &v, const Matrix &m) {return ((Matrix)v  - m);};

	friend Vector operator + (const Vector&, const Vector&);
	friend Vector operator + (double s,const Vector &v) {return (Vector)(s + (Matrix)v);};
    friend Vector operator + (const Vector &v, double s) {return (Vector)((Matrix)v  + s);};
//	friend Matrix operator + (const Matrix &m,const Vector &v) {return (m + (Matrix)v);};
//    friend Matrix operator + (const Vector &v, const Matrix &m) {return ((Matrix)v  + m);};


};
	
extern double mc_scalar_product(const Vector &a, const Vector &b);
extern double mc_length(const Vector &a);


#endif // VECTOR_H

/*---------------------------------------------------------------------------*/
/*                               FIN DU FICHIER                              */
/*-------------------------------------------------------------------J.Cô .--*/
