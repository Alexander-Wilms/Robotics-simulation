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

// $Id: matrix.h,v 1.15 2003/04/04 20:58:39 regie Exp $

/** \file
 * \author Jean Côté    \date 18 décembre 96
 */

/*---------------------------------------------------------------------------*/
/*                     	      FICHIERS D'INCLUSIONS                          */
/*---------------------------------------------------------------------------*/
#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>
#include "vmcomplex.h"


#define MATRIX_DIM 7            //dimension minimale dynamique
#define DIMENSION_MAX_MATRIX 	100

class Vector;		// Ces 2 classes sont étroitement reliées


// Patch car on a des methodes qui s'appellent min et max et
// Il y a un comflit sur WindowsNT (a revoir)
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

/**
 * Permet de manipuler les matrices de dimensions diverses.
 * \author Jean Côté    \date 18 décembre 96
 */
class Matrix 
{
public:
    enum Type { GENERAL, ORTHOGONAL };
private:
    enum Type type;
    Matrix(int,int,double **);
     
        	
protected:
    int x, y, size;
    int New(int = 0,int = 0);
    int Delete();
    double vfix[MATRIX_DIM][MATRIX_DIM];
    double *pvfix[MATRIX_DIM];
    double **v;

public:
    // Constructor
    inline Matrix() {x = y = size = 0; type = GENERAL; v = NULL;};
    Matrix(int, int);
    Matrix(const Matrix&);
	
    // Destructor
    ~Matrix();
	
    inline int ncol()   const { return x;    }
    inline int nrow()   const { return y;    }
    inline int msize()  const { return size; }
	

    inline double *operator [] (int row) { return(v[row]); }

    inline double get(int row, int col) const {
        if (row < y && col < x) return(v[row][col]);
        else return 0;
    }

    inline void set_type(Type t) { type = t; }
    Matrix& operator = (const Matrix&);
    Matrix& operator = (const vmcomplex &);
    Matrix& operator = (double);
    operator double() const;
    inline void operator = (Type t) { type = t; }
    inline int operator == (Type t) { return(type == t); }
    inline int operator != (Type t) { return(type != t); }
    Matrix& operator += (const Matrix&);
    Matrix& operator += (const double);
    Matrix& operator -= (const Matrix&);
    Matrix& operator -= (const double);
    Matrix& operator *= (const Matrix&);
    Matrix& operator *= (const double);
    Matrix& operator /= (const Matrix&);
    Matrix& operator /= (const double);
    int operator == (const Matrix&);
    int operator == (const double);
    int operator != (const Matrix&);
    int operator != (const double);
    Vector col(int);
    Vector row(int);
    Matrix& swap_row(int, int);
    Matrix& swap_col(int, int);
    Matrix& insert_row(int, double val = 0.0);
    Matrix& insert_col(int, double val = 0.0);
	
    double max();
    double min();

    int is_a_vector(unsigned int size) const;
    int is_a_vector() const;
	
    int save(FILE *fp);
    int save(const char *);
    int load(FILE *fp);
    int load(const char *);
    int load(int h, int l, double*);
    int dump(double *);

    Matrix& print(const char *msg = NULL, const char *name = "Matrix");

    friend Matrix mc_transpose(const Matrix&);
    friend Matrix mc_sub_matrix(const Matrix&,int,int);
    friend Matrix mc_sub_matrix(const Matrix&,int,int,int,int);
    friend Matrix mc_del_row(const Matrix&,int);
    friend Matrix mc_del_col(const Matrix&,int);
    friend Matrix mc_del_row_col(const Matrix&,int, int); 
    friend Matrix mc_concatenate(const Matrix&, const Matrix&, int option = 0);
    friend Matrix mc_sub_add(const Matrix&, int y, int x, const Matrix&);
    friend double mc_det(const Matrix&);
    friend double mc_trace(const Matrix&);
    friend Matrix mc_inv(const Matrix&);
    friend Matrix mc_pinv(const Matrix&);
    friend Vector mc_diag(const Matrix&);
    friend Matrix mc_diag(const Vector &);
    friend Matrix mc_identity(int = 4);
    friend Matrix mc_jordan(int, int);
    friend Matrix operator - (double,const Matrix&);
    friend Matrix operator - (const Matrix&, double);
    friend Matrix operator - (const Matrix&, const Matrix&);
    friend Matrix operator - (const Matrix&);
    friend Matrix operator + (double,const Matrix&);
    friend Matrix operator + (const Matrix&, double);
    friend Matrix operator + (const Matrix&, const Matrix&);
    friend Matrix operator / (const Matrix&, double);
    friend Matrix operator / (double, const Matrix&);
    friend Matrix operator / (const Matrix&, const Matrix&);
    friend Matrix operator * (double,const Matrix&);
    friend Matrix operator * (const Matrix&,double);
    friend Matrix operator * (const Matrix&, const Matrix&);
    friend int mc_LU_decomposition(Matrix&,int *);
    friend void mc_backsubstitution_LU(Matrix&,int *,Matrix&);
    friend Matrix mc_inverse_LU(const Matrix&);
    friend int mc_eigen_values(const Matrix& m, vmcomplex *vp);
    friend int mc_eigen_values(const Matrix& m);
    friend int mc_matrice_positive(const Matrix& m);
    friend int mc_positive_matrix(const Matrix& m);
    friend Matrix mc_abs(const Matrix& m);
    friend Matrix mc_square(const Matrix& m);
    friend int mc_comp_matrix(Matrix &A, Matrix &B, double threshold);
};
	
extern Matrix mc_null_matrix;	
extern Matrix mc_ok_matrix;	
extern Matrix mc_more_matrix;	

#endif // MATRIX_H

/*---------------------------------------------------------------------------*/
/*                               FIN DU FICHIER                              */
/*-------------------------------------------------------------------J.Cô .--*/
