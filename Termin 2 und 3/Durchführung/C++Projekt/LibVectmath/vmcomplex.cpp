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

// $Id: vmcomplex.C,v 1.8 2002/09/16 19:41:23 fugazi Exp $

/** \file
 * \author Jean Côté    \date 18 décembre 96
 */

#include "vmcomplex.h"

vmcomplex operator / (double x, const vmcomplex& y)
{
    double ar = fabs(real(y));
    double ai = fabs(imag(y));
    double nr, ni;
    double t, d;
    if(ar <= ai)
        {
	t = real(y)/imag(y);
	d = imag(y)*(1 + t*t);
	nr = x * t/d;
	ni = -x/d;
        }
    else
        {
	t = imag(y)/real(y);
	d = real(y)*(1 + t*t);
	nr = x/d;
	ni = -x*t/d;
        }
    return vmcomplex(nr, ni);
}

vmcomplex operator / (const vmcomplex& x, const vmcomplex& y)
{
    double ar = fabs(real(y));
    double ai = fabs(imag(y));
    double nr, ni;
    double t, d;
    if(ar <= ai)
        {
	t = real(y)/imag(y);
	d = imag(y)*(1 + t*t);
	nr = (real(x)*t + imag(x))/d;
	ni = (imag(x)*t - real(x))/d;
        }
    else
        {
	t = imag(y)/real (y);
	d = real(y)*(1 + t*t);
	nr = (real(x) + imag(x)*t)/d;
	ni = (imag(x) - real(x)*t)/d;
        }
    return vmcomplex(nr, ni);
}
    
vmcomplex mc_pow(const vmcomplex& x, double y)
{
    vmcomplex logx(log(sqrt(real(x) * real(x) + imag(x) * imag(x))), atan2(imag(x),real(x)));
    vmcomplex v = y * logx;
    return vmcomplex(exp(real(v))*cos(imag(v)), exp(real(v))*sin(imag(v)));
}

vmcomplex mc_sqrt (const vmcomplex& x)
{
    double r = sqrt(real (x) * real (x) + imag (x) * imag (x));
    double nr, ni;
    if(r == 0.0)
	nr = ni = r;
    else if (real(x) > 0)
	{
	nr = sqrt(0.5*(r + real(x)));
	ni = imag(x)/nr/2;
        }
    else
        {
	ni = sqrt (0.5*(r - real(x)));
	if(imag(x) < 0)
	    ni = - ni;
	nr = imag(x)/ni/2;
        }
    return vmcomplex(nr, ni); 
}

/*---------------------------------------------------------------------------*/
/*                               FIN DU FICHIER                              */
/*---------------------------------------------------------------------R.H.--*/
