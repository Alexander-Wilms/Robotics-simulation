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

// $Id: roots.C,v 1.6 2002/08/30 15:35:29 ch7905 Exp $

/** \file
 * Fonctions permettant de trouver les racines de polynomes de 1, 2, 3 et 4e
 * degré.
 * \author Jean Côté    \date 18 décembre 96
 */

/*---------------------------------------------------------------------------*/
/*                     	      FICHIERS D'INCLUSIONS                          */
/*---------------------------------------------------------------------------*/
#include <iostream>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include <new.h>

#include "vmcomplex.h"
#include "roots.h"

/*---------------------------------------------------------------------------*/
/*            VARIABLES GLOBALES ET STATIQUES, MACROS ET DEFINES             */
/*---------------------------------------------------------------------------*/

/**
 * Évalue un polynôme pour la valeur x.
 * \author Jean Côté    \date 18 décembre 96
 * \param a Les coefficients du polynôme de degré nb (a[nb+1])
 * \param x la valeur à évaluer
 * \param nb le degré du polynôme
 * \return Le résultat.
 */
double mc_polynome(double *a,double x,int nb)
{
    register int i;			// degree of mc_polynome
    register double valeur = a[nb];  	// intermediate value of the mc_polynome
    register double xx = x;		// to speed-up

    for(i=nb-1;i>= 0;i--)
    {
        valeur = valeur*xx + a[i];
    }

    return(valeur);
}

/**
 * Calcule les racines d'un polynôme du 4e degré.
 * <pre>
 * 	    4       3       2
 * a4*x  + a3*x  + a2*x  + a1*x + a0 = 0
 * </pre>
 * Tiré du livre Formules et Tables de Mathématiques de la série Schaum.
 *
 * \author Jean Côté    \date 18 décembre 96
 * \param a4 les coefficients
 * \param a3 les coefficients
 * \param a2 les coefficients
 * \param a1 les coefficients
 * \param a0 les coefficients
 * \param x les quatres racines du polynôme.
 */
void mc_roots(double a4, double a3, double a2, double a1, double a0, vmcomplex *x)
{
    if(a4 == 0.0)
    {
    	mc_roots(a3, a2, a1, a0, x);
    	x[3] = 0.0;
    	return;
    }
    // normalization of coefficients
    double a = (double)a3/a4;
    double b = (double)a2/a4;
    double c = (double)a1/a4;
    double d = (double)a0/a4;

    // compute the coefficients
    // of the cubic root
    double p = -b;
    double q = a*c - 4*d;
    double r = -a*a*d + 4*b*d - c*c;

    // find the coefficients of the
    // cubic resolvant
    double ap = (3*q - p*p)/3;
    double bp = (2*p*p*p - 9*p*q + 27*r)/27;

    // compute the real root of the 
    // cubic mc_polynome
    double y;
    double com = bp*bp/4 + ap*ap*ap/27;
    if(com >= 0.0)
    {
	double scom = sqrt(com);
	double aa3 = -bp/2.0 + scom;
	double bb3 = -bp/2.0 - scom;
	if(aa3 < 0)
	    aa3 = -pow(-aa3,1.0/3.0);
	else
	    aa3 = pow(aa3,1.0/3.0);
	if(bb3 < 0)
	    bb3 = -pow(-bb3,1.0/3.0);
	else
	    bb3 = pow(bb3,1.0/3.0);
	y = aa3 + bb3 - p/3;
    }
    else
    {
    	double radius = sqrt(bp*bp/4.0 - com);
	double theta  = atan2(sqrt(-com), - bp/2.0);
	y = 2.0*pow(radius,1.0/3.0)*cos(theta/3.0) - p/3.0;
    }

    // compute the root for the 
    // quadric:
    vmcomplex dd, ee;
    vmcomplex rr = mc_sqrt((vmcomplex)0.25*a*a - b + y);
    if(fabs(real(rr)) != 0.0)
    {
	vmcomplex A = 0.75*a*a - rr*rr - 2.0*b;
	vmcomplex B = (a*b - 2.0*c - 0.25*a*a*a)/rr;
	dd = mc_sqrt(A + B);
	ee = mc_sqrt(A - B);
    }
    else
    {
	vmcomplex A = 0.75*a*a - 2*b;
	vmcomplex B = 2*mc_sqrt((vmcomplex)(y*y - 4*d));
	dd = mc_sqrt(A + B);
	ee = mc_sqrt(A - B);
    }

    x[0] = - 0.25*a + 0.5*rr + 0.5*dd;
    x[1] = - 0.25*a + 0.5*rr - 0.5*dd;
    x[2] = - 0.25*a - 0.5*rr + 0.5*ee;
    x[3] = - 0.25*a - 0.5*rr - 0.5*ee;
}


/**
 * Calcule les racines d'un polynôme du 3e degré.
 * <pre>
 * 	    3       2
 * a3*x  + a2*x  + a1*x + a0 = 0
 * </pre>
 * Tiré du livre Formules et Tables de Mathématiques de la série Schaum.
 *
 * \author Jean Côté    \date 18 décembre 96
 * \param a3 les coefficients
 * \param a2 les coefficients
 * \param a1 les coefficients
 * \param a0 les coefficients
 * \param x les racines
 */
void mc_roots(double a3, double a2, double a1, double a0, vmcomplex *x)
{
    if(a3 == 0.0)
    {
    	mc_roots(a2, a1, a0, x);
    	x[2] = 0.0;
    	return;
    }
    // normalization of the coefficients
    double p = a2/a3;
    double q = a1/a3;
    double r = a0/a3;

    // find the coefficients of the
    // cubic resolvant
    double ap = (3*q - p*p)/3;
    double bp = (2*p*p*p - 9*p*q + 27*r)/27;

    // compute the real root of the 
    // cubic mc_polynome
    vmcomplex com = mc_sqrt((vmcomplex)bp*bp/4 + ap*ap*ap/27);
    vmcomplex som1 = -bp/2.0 + com;
    vmcomplex som2 = -bp/2.0 - com;
    vmcomplex S,T;
    
    if(real(som1) < 0)
    	S = -mc_pow(-som1,(double)1.0/3.0);
    else
    	S = mc_pow(som1,(double)1.0/3.0);
    if(real(som2) < 0)
    	T = -mc_pow(-som2,(double)1.0/3.0);
    else
    	T = mc_pow(som2,(double)1.0/3.0);
    vmcomplex ST = -(S + T)/2.0 - p/3.0;
    vmcomplex I(0.0,1.0);
    vmcomplex STI = I*(S - T)*0.866025403784439;
    x[0] = S + T - p/3.0;
    x[1] = ST + STI;
    x[2] = ST - STI;
}

/**
 * Calcule les racines d'un polynôme du 2e degré.
 * <pre>
 * 	   2
 * a*x  + b*x + c = 0
 * </pre>
 *
 * \author Jean Côté    \date 18 décembre 96
 * \param a les coefficients
 * \param b les coefficients
 * \param c les coefficients
 * \param x les racines
 */
void mc_roots(double a, double b, double c, vmcomplex *x)
{
    if(a == 0.0)
    {
    	mc_roots(b, c, x);
    	x[1] = 0.0;
    	return;
    }
    double bp = 0.5*b;
    vmcomplex radical = mc_sqrt((vmcomplex)(bp*bp - a*c));
    x[0] = (-bp + radical)/a;
    x[1] = (-bp - radical)/a;
}

/**
 * Calcule la racine de l'équation linéaire a*x + b = 0!!!.
 * \author Jean Côté    \date 18 décembre 96
 * \param a les coefficients
 * \param b les coefficients
 * \param x la racine
 */
void mc_roots(double a, double b, vmcomplex *x)
{
    if(a != 0.0)
	x[0] = -b/a;
    else
    	x[0] = 0.0;
}

/*---------------------------------------------------------------------------*/
/*                               FIN DU FICHIER                              */
/*-------------------------------------------------------------------J.Cô.---*/
