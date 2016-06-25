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

// $Id: svd.C,v 1.12 2002/09/27 14:27:08 regie Exp $

/** \file
 * Fonction permettant la decomposition en valeurs singulières d'une matrice.
 * \author Alain Croteau
 * \date 16 juin 94
 */

/*---------------------------------------------------------------------------*/
/*                     	      FICHIERS D'INCLUSIONS                          */
/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>

#include "vectmath.h"
#include "svd.h"

/*---------------------------------------------------------------------------*/
/*            VARIABLES GLOBALES ET STATIQUES, MACROS ET DEFINES             */
/*---------------------------------------------------------------------------*/


#include "ccmath/svduv.c"
#include "ccmath/sv2uv.c"
#include "ccmath/ldumat.c"
#include "ccmath/ldvmat.c"
#include "ccmath/qrbdv.c"


/**
 * Compute the singular value decomposition of matrix X
 * Actually uses a function taken from Daniel Atkinson's ccmath library 
 * (see the accompanying copyright notice in the ccmath directory)
 * For now, ccmath routines are used as is, this function simply converts
 * Microb Matrix type arguments to the format expected by ccmath (double*), 
 * calls the appropriate ccmath function and converts data back to Matrix
 * type
 * \author Alain Croteau
 * \date 16 septembre 2001
 * \param X input matrix
 * \param U left output matrix
 * \param V right output matrix
 * \param S singular values output vector
 */
void mc_svd(Matrix &X, Matrix &U, Vector &S, Matrix &V)
{
    int m,n;
    double *a;
    int transpose = (X.nrow() < X.ncol());

    if(transpose)
    {    
        n = X.nrow();
        m = X.ncol();
        a = new double[m*n];
        Matrix tr = mc_transpose(X);
        tr.dump(a);
    }
    else
    {
        m = X.nrow();
        n = X.ncol();
        a = new double[m*n];
        X.dump(a);
    }
    double *u = new double[m*m];
    double *v = new double[n*n];
    double *d = new double[n];

    // choose the more efficient of two svd algorithms (see ccmath doc)
    if(m <= 2*n)
    {
        svduv(d, a, u, m, v, n);
    }
    else
    {
        sv2uv(d, a, u, m, v, n);
    }

    if(transpose)
    {
        V.load(m, m, u);
        U.load(n, n, v);
        S.load(n, 1, d);
    }
    else
    {
        U.load(m, m, u);
        V.load(n, n, v);
        S.load(n, 1, d);
    }
    // on rajoute des 0 pour satisfaire à la proc. de test
    // peut-être que c'est la procédure de test qui est incorrecte
    for(int i=n; i < m; i++)
    {
        S.insert_row(i);
    } 

	delete [] a;
	delete [] u;
	delete [] v;
	delete [] d;
}


/*---------------------------------------------------------------------------*/
/*                            PROCEDURES STATIQUES                           */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/*                               FIN DU FICHIER                              */
/*---------------------------------------------------------------------R.H.--*/
