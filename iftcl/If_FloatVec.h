/*
 * If_FloatVec.h --
 *     - old name for If_RealVec (provided for backward compatibility)
 *
 *  rf, 12/3/00
 */

/*
    Copyright (C) 1994--2001  Ruediger Franke

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; 
    version 2 of the License.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library (file COPYING.LIB);
    if not, write to the Free Software Foundation, Inc.,
    59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef If_FloatVec_H
#define If_FloatVec_H

#ifdef __GNUC__
// (standard C++ does not define #warning)
#warning "If_FloatVec is depreciated; use If_RealVec instead!"
#endif

#include "If_RealVec.h"

typedef If_RealVec If_FloatVec;

#endif