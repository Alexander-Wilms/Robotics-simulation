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

// $Id: vectmath.h,v 1.7 2002/08/30 15:35:31 ch7905 Exp $

#ifndef VECTMATH_H
#define VECTMATH_H 

/* A revoir : on devrait utiliser des defines des compilateurs */
#define VM_SMALL 1e-10
#define VM_VERY_SMALL 1e-12
#define VM_VERY_VERY_SMALL 1e-16

// Changes by Michael Menne, 29.11.2003
// Support for XYZ Euleranlges added. 
// To enable XYZ Euleranlges, XYZ_EULER_ANGLES must be defined.
// Changed files: rotation.cpp, euler_anlge.cpp
#define XYZ_EULER_ANGLES	// Enable XYZ Eulerangles

#include <stdio.h>

#include "matrix.h"
#include "roots.h"
#include "rotation.h"
#include "transform.h"
#include "vector.h"
#include "vector3.h"
#include "vector4.h"
#include "vector6.h"
#include "vector7.h"
#include "position.h"
#include "quaternion.h"
#include "quaternion_xyz.h"
#include "euler_angle.h"
#include "euler_angle_xyz.h"
#include "angle_axis.h"
#include "angle_axis_xyz.h"
#include "svd.h"
//#include "Vectmath/valid_tests.h"	// unused, has been removed
//#include "Vectmath/register.h"	// unused, has been revomed

#define  mc_sign_of(X) ( ((double)(X) >= (double)0.0)  ? 1 : -1 )

// next line has been replaced to make vectmath independent from the OSDL library
//#define vmError(msg)	mic_error(Fatal_error, msg);
#define vmError(msg)	printf( msg );

#endif /* VECTMATH_H */
