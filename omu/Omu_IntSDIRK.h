/*
 * Omu_IntSDIRK.h --
 *   -- class for integrating an Ode over a stage
 *   -- using implicit Runge Kutta method (Radau IIa - 3. order)
 *
 * hl, 03/08/00
 */

/*
    Copyright (C) 2000  Hartmut Linke

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

#ifndef Omu_IntSDIRK_H
#define Omu_IntSDIRK_H

#include "Omu_Integrator.h"

//--------------------------------------------------------------------------
class Omu_IntSDIRK: public Omu_Integrator {

 public:

  Omu_IntSDIRK();
  ~Omu_IntSDIRK();

  char *name() {return "SDIRK";}

  // interface routine from Omu_Integrator
  void solve(int kk, Real tstart, Real tend,
	     const Omu_States &x, const Omu_Vector &u,
	     Omu_Program *sys, VECP xt,
	     MATP Sx, MATP Su);

  void init_stage(int k, const Omu_States &x, const Omu_Vector &u,
		  bool sa);

  void init_yprime(int , double ,const VECP ,const VECP , VECP );
  void jac(int ,double ,const VECP ,const VECP , const VECP , VECP );
  void res(double ,const VECP ,const VECP , const VECP , VECP);
  void realloc();
  void init_method();
  void init_yprime_pred(MATP );
  void solve_stage(int , VECP , VECP );
  void solve_final(VECP , VECP );
  void sensitivity();

  private:

  bool          _recalc_jac;
  bool          _stiffly_accurate;
  bool          _sens_at_once;

  int           _output;
  int           _n_splitt_tape_eval;
  short         _tag;

  // backing store sys and current stage
  Omu_Program	*_sys;

  int		_mu;	     // upper semi-bandwidth
  int		_ml;	     // lower semi-bandwidth
  int           _na;         // number of algebraic states
  int           _nod;        // number of over determining equations

  double        _t;

  int           _irk_stages;
  int           _maxiters;
  int           _modnewtonsteps;
  int           _newtonsteps;
  int           _nsteps;

  MATP          _irk_jac;
  MATP          _irk_jacf;

  PERM          *_ppivot;

  IVECP         _x_algebraic;

  VECP          _irk_delta;
  VECP          _irk_res;
  VECP          _irk_y;
  VECP          _irk_yprime;

  VECP          _par;
  VECP          _y;
  VECP          _y0;
  VECP          _yprime0;
  VECP          _yprime1;
  VECP          _yprime2;

  MATP          _yprime;

  // parameters of irk method

  double        _gamma0;
  VECP          _b;
  VECP          _b_err;
  VECP          _c;
  MATP          _a;
  MATP          _a_pred;

  // stepsize selection
  double        _hinit;
  double        _h_new;
  double        _h;
  VECP          _err;



  // vectors and matrices for low level _sys->continuous callback
  VECP		_cx;
  VECP		_cu;
  VECP		_cxp;
  VECP		_cF;
  VECP		_cFh;
  MATP		_cFx;
  MATP		_cFxh;
  MATP		_cFu;
  MATP		_cFxp;

  // sensitivity equations
  MATP          _Sxd;
  MATP          _Sxd0;
  MATP          _Sxx;
  MATP          _Sxx0;
  MATP          _Sxu;
  MATP          _Sxu0;
  MATP          _SF;
  MATP          _Sh;
  PERM          *_Spivot;

  short         **_nz;         // sparsity pattern


  MATP          _U;
  MATP          _Z;

};

#endif




