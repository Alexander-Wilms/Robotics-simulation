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

// $Id: vmcomplex.h,v 1.5 2002/09/16 19:41:23 fugazi Exp $

/** \file
 * \author Jean Côté    \date 18 décembre 96
 */

#ifndef _VMCOMPLEX_H_
#define _VMCOMPLEX_H_

#include <math.h>

class vmcomplex
{
public:
  vmcomplex (double r = 0, double i = 0): re (r), im (i) { }
  vmcomplex (const vmcomplex& r): re (r.real()), im (r.imag()) { }
  double real() const { return re; }
  double imag() const { return im; }
  friend vmcomplex operator + (const vmcomplex&, const vmcomplex&) ;
  friend vmcomplex operator + (const vmcomplex&, double) ;
  friend vmcomplex operator + (double, const vmcomplex&) ;
//  friend vmcomplex operator - (const vmcomplex&, const vmcomplex&) ;
//  friend vmcomplex operator - (const vmcomplex&, double) ;
//  friend vmcomplex operator - (double, const vmcomplex&) ;
  friend vmcomplex operator * (const vmcomplex&, const vmcomplex&) ;
  friend vmcomplex operator * (const vmcomplex&, double) ;
  friend vmcomplex operator * (double, const vmcomplex&) ;
  friend vmcomplex operator / (const vmcomplex&, const vmcomplex&) ;
  friend vmcomplex operator / (const vmcomplex&, double) ;
  friend vmcomplex operator / (double, const vmcomplex&) ;
  friend int operator != (const vmcomplex&, double) ;
private:
  double re, im;
};

inline double imag(const vmcomplex& x) { return x.imag(); }
inline double real(const vmcomplex& x) { return x.real(); }
vmcomplex mc_pow(const vmcomplex&, double);
vmcomplex mc_sqrt(const vmcomplex& x);

inline vmcomplex operator + (const vmcomplex& x, const vmcomplex& y) {return vmcomplex (real (x) + real (y), imag (x) + imag (y));}
inline vmcomplex operator + (const vmcomplex& x, double y) { return vmcomplex(real(x)+y, imag(x)); }
inline vmcomplex operator + (double x, const vmcomplex& y) { return vmcomplex(x+real(y), imag(y)); }
inline vmcomplex operator - (const vmcomplex& x, const vmcomplex& y) { return vmcomplex(real(x)-real(y), imag(x)-imag (y)); }
inline vmcomplex operator - (const vmcomplex& x, double y) { return vmcomplex(real(x)-y, imag(x)); }
inline vmcomplex operator - (double x, const vmcomplex& y) { return vmcomplex(x -real(y), -imag(y)); }
inline vmcomplex operator * (const vmcomplex& x, const vmcomplex& y) { return vmcomplex(real(x)*real(y) - imag(x)*imag(y),real(x)*imag(y) + imag(x)*real(y)); }
inline vmcomplex operator * (const vmcomplex& x, double y) { return vmcomplex(real(x)*y, imag(x)*y); }
inline vmcomplex operator * (double x, const vmcomplex& y) { return vmcomplex(x*real(y), x*imag(y)); }
inline vmcomplex operator / (const vmcomplex& x, double y) { return vmcomplex(real(x)/y, imag(x)/y); }
vmcomplex operator / (const vmcomplex&, const vmcomplex&) ;
vmcomplex operator / (double, const vmcomplex&) ;
inline int operator != (const vmcomplex& x, double y) { return(real(x) != y || imag(x) != 0); }
inline vmcomplex operator - (const vmcomplex& x) { return vmcomplex(-real(x), -imag(x)); }

#endif // _VMCOMPLEX_H_
