/*
 * Omu_Dependents.C
 *   -- class definitions
 *
 * rf, 7/31/01
 */

/*
    Copyright (C) 1997--2007  Ruediger Franke

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

#include "Omu_Dependents.h"

// flags 
const int Omu_Dependent::WRT_x = 	0x0001;
const int Omu_Dependent::WRT_u = 	0x0002;
const int Omu_Dependent::WRT_dx =	0x0004;
const int Omu_Dependent::WRT_xf = 	0x0004;
const int Omu_Dependent::WRT_q = 	0x0002;
const int Omu_Dependent::WRT_ALL = 	0x0007;

//==========================================================================

//--------------------------------------------------------------------------
Omu_Gradient::Omu_Gradient()
{
  _v = v_get(1);
  size(0);
}

//--------------------------------------------------------------------------
Omu_Gradient::~Omu_Gradient()
{
  v_free(_v);
}

//--------------------------------------------------------------------------
void Omu_Gradient::size(int dim) {
  v_resize(_v, dim);
  v_ones(_v);	// indicate dense Jacobian
  _is_zero = false;
  _is_constant = false;
}

//--------------------------------------------------------------------------
void Omu_Gradient::analyze_struct(bool is_constant)
{
  int i;
  int dim = _v->dim;

  _is_constant = is_constant;

  // check for zero
  if (is_constant) {
    _is_zero = true;
    for (i = 0; i < dim; i++) {
      _is_zero &= ((*this)[i] == 0.0);
    }
  }
}

//==========================================================================

//--------------------------------------------------------------------------
Omu_Jacobian::Omu_Jacobian()
{
  _m = m_get(1, 1);
  _zero_rows = iv_get(1);
  _zero_cols = iv_get(1);
  size(0, 0);
}

//--------------------------------------------------------------------------
Omu_Jacobian::~Omu_Jacobian()
{
  iv_free(_zero_cols);
  iv_free(_zero_rows);
  m_free(_m);
}

//--------------------------------------------------------------------------
void Omu_Jacobian::size(int nrows, int ncols)
{
  m_resize(_m, nrows, ncols);
  m_ones(_m);	// indicate dense Jacobian
  _is_zero = false;
  _is_ident = false;
  _is_scalar_constant = false;
  _is_constant = false;
  _sbw_lower = ncols - 1;
  _sbw_upper = ncols - 1;
  iv_resize(_zero_rows, nrows);
  iv_resize(_zero_cols, ncols);
  iv_zero(_zero_rows);
  iv_zero(_zero_cols);
}

//--------------------------------------------------------------------------
void Omu_Jacobian::adapt_size(int nrows)
{
  assert(nrows <= _m->max_m); // may not re-allocate memory
  _m->m = nrows;
}

//--------------------------------------------------------------------------
void Omu_Jacobian::analyze_struct(bool is_constant)
{
  int nrows = _m->m;
  int ncols = _m->n;
  int i, j, sbwi;

  _is_constant = is_constant;

  // determine semi-bandwidths (i.e. number of side-diagonals)
  _sbw_lower = 0;
  _sbw_upper = 0;
  for (i = 0; i < nrows; i++) {
    sbwi = i;		// maximum lower sbw in this row
    for (j = 0; j < i; j++) {
      if (j < ncols && (*this)[i][j] != 0.0)
        break;
      --sbwi;
    }
    _sbw_lower = max(sbwi, _sbw_lower);

    sbwi = ncols - i - 1;// maximum upper sbw in this row
    for (j = ncols - 1; j > i; j--) {
      if ((*this)[i][j] != 0.0)
        break;
      --sbwi;
    }
    _sbw_upper = max(sbwi, _sbw_upper);
  }

  // check for zero, ident and scalar
  if (is_constant && _sbw_lower < 1 && _sbw_upper < 1) {
    _is_zero = true;
    _is_ident = true;
    _is_scalar_constant = true;
    for (i = 0; i < min(nrows, ncols); i++) {
      _is_zero &= ((*this)[i][i] == 0.0);
      _is_ident &= ((*this)[i][i] == 1.0);
      _is_scalar_constant &= ((*this)[i][i] == (*this)[0][0]);
    }
  }

  // check for zero rows
  bool is_zero;
  for (i = 0; i < nrows; i++) {
    is_zero = true;
    for (j = 0; j < ncols; j++)
      is_zero &= ((*this)[i][j] == 0.0);
    _zero_rows[i] = (int)is_zero;
  }

  // check for zero cols
  for (j = 0; j < ncols; j++) {
    is_zero = true;
    for (i = 0; i < nrows; i++)
      is_zero &= ((*this)[i][j] == 0.0);
    _zero_cols[j] = (int)is_zero;
  }
}

//==========================================================================

//--------------------------------------------------------------------------
Omu_DependentVec::Omu_DependentVec()
{
  _required_J = false;
}

//==========================================================================

//--------------------------------------------------------------------------
Omu_Dependent::Omu_Dependent()
{
  _required_g = false;
}

//--------------------------------------------------------------------------
Omu_Dependent::~Omu_Dependent()
{
}


//==========================================================================
