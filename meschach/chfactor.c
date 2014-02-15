
/**************************************************************************
**
** Copyright (C) 1993 David E. Steward & Zbigniew Leyk, all rights reserved.
**
**			     Meschach Library
** 
** This Meschach Library is provided "as is" without any express 
** or implied warranty of any kind with respect to this software. 
** In particular the authors shall not be liable for any direct, 
** indirect, special, incidental or consequential damages arising 
** in any way from use of the software.
** 
** Everyone is granted permission to copy, modify and redistribute this
** Meschach Library, provided:
**  1.  All copies contain this copyright notice.
**  2.  All modified copies shall carry a notice stating who
**      made the last modification and the date of such modification.
**  3.  No charge is made for this software or works derived from it.  
**      This clause shall not be construed as constraining other software
**      distributed on the same medium as this software, nor is a
**      distribution fee considered a charge.
**
***************************************************************************/


/*
	Matrix factorisation routines to work with the other matrix files.
*/

/* CHfactor.c 1.2 11/25/87 */
static	char	rcsid[] = "$Id: chfactor.c,v 1.2 2002/12/09 10:57:47 e_arnold Exp $";

#include	<stdio.h>
#include	<math.h>
#include	"matrix.h"
#include        "matrix2.h"


/* Most matrix factorisation routines are in-situ unless otherwise specified */

/* CHfactor -- Cholesky L.L' factorisation of A in-situ */
MAT	*CHfactor(A)
MAT	*A;
{
	u_int	i, j, k, n;
	Real	**A_ent, *A_piv, *A_row, sum, tmp;

	if ( A==(MAT *)NULL )
		m_error(E_NULL,"CHfactor");
	if ( A->m != A->n )
		m_error(E_SQUARE,"CHfactor");
	n = A->n;	A_ent = A->me;

	for ( k=0; k<n; k++ )
	{	
		/* do diagonal element */
		sum = A_ent[k][k];
		A_piv = A_ent[k];
		for ( j=0; j<k; j++ )
		{
			/* tmp = A_ent[k][j]; */
			tmp = *A_piv++;
			sum -= tmp*tmp;
		}
		if ( sum <= 0.0 )
			m_error(E_POSDEF,"CHfactor");
		A_ent[k][k] = sqrt(sum);

		/* set values of column k */
		for ( i=k+1; i<n; i++ )
		{
			sum = A_ent[i][k];
			A_piv = A_ent[k];
			A_row = A_ent[i];
			sum -= __ip__(A_row,A_piv,(int)k);
			/************************************************
			for ( j=0; j<k; j++ )
				sum -= A_ent[i][j]*A_ent[k][j];
				sum -= (*A_row++)*(*A_piv++);
			************************************************/
			A_ent[j][i] = A_ent[i][j] = sum/A_ent[k][k];
		}
	}

	return (A);
}


/* CHsolve -- given a CHolesky factorisation in A, solve A.x=b */
VEC	*CHsolve(A,b,x)
MAT	*A;
VEC	*b,*x;
{
	if ( A==(MAT *)NULL || b==(VEC *)NULL )
		m_error(E_NULL,"CHsolve");
	if ( A->m != A->n || A->n != b->dim )
		m_error(E_SIZES,"CHsolve");
	x = v_resize(x,b->dim);
	Lsolve(A,b,x,0.0);
	Usolve(A,x,x,0.0);

	return (x);
}

/* LDLfactor -- L.D.L' factorisation of A in-situ */
MAT	*LDLfactor(A)
MAT	*A;
{
	u_int	i, k, n, p;
	Real	**A_ent;
	Real d, sum;
	static VEC	*r = VNULL;

	if ( ! A )
		m_error(E_NULL,"LDLfactor");
	if ( A->m != A->n )
		m_error(E_SQUARE,"LDLfactor");
	n = A->n;	A_ent = A->me;
	r = v_resize(r,n);
	MEM_STAT_REG(r,TYPE_VEC);

	for ( k = 0; k < n; k++ )
	{
		sum = 0.0;
		for ( p = 0; p < k; p++ )
		{
		    r->ve[p] = A_ent[p][p]*A_ent[k][p];
		    sum += r->ve[p]*A_ent[k][p];
		}
		d = A_ent[k][k] -= sum;

		if ( d == 0.0 )
		    m_error(E_SING,"LDLfactor");
		for ( i = k+1; i < n; i++ )
		{
		    sum = __ip__(A_ent[i],r->ve,(int)k);
		    /****************************************
		    sum = 0.0;
		    for ( p = 0; p < k; p++ )
			sum += A_ent[i][p]*r->ve[p];
		    ****************************************/
		    A_ent[i][k] = (A_ent[i][k] - sum)/d;
		}
	}

	return A;
}

VEC	*LDLsolve(LDL,b,x)
MAT	*LDL;
VEC	*b, *x;
{
	if ( ! LDL || ! b )
		m_error(E_NULL,"LDLsolve");
	if ( LDL->m != LDL->n )
		m_error(E_SQUARE,"LDLsolve");
	if ( LDL->m != b->dim )
		m_error(E_SIZES,"LDLsolve");
	x = v_resize(x,b->dim);

	Lsolve(LDL,b,x,1.0);
	Dsolve(LDL,x,x);
	LTsolve(LDL,x,x,1.0);

	return x;
}

/* MCHfactor -- Modified Cholesky L.L' factorisation of A in-situ */
MAT	*MCHfactor(A,tol)
MAT	*A;
Real	tol;
{
	u_int	i, j, k, n;
	Real	**A_ent, *A_piv, *A_row, sum, tmp;

	if ( A==(MAT *)NULL )
		m_error(E_NULL,"MCHfactor");
	if ( A->m != A->n )
		m_error(E_SQUARE,"MCHfactor");
	if ( tol <= 0.0 )
	        m_error(E_RANGE,"MCHfactor");
	n = A->n;	A_ent = A->me;

	for ( k=0; k<n; k++ )
	{	
		/* do diagonal element */
		sum = A_ent[k][k];
		A_piv = A_ent[k];
		for ( j=0; j<k; j++ )
		{
			/* tmp = A_ent[k][j]; */
			tmp = *A_piv++;
			sum -= tmp*tmp;
		}
		if ( sum <= tol )
			sum = tol;
		A_ent[k][k] = sqrt(sum);

		/* set values of column k */
		for ( i=k+1; i<n; i++ )
		{
			sum = A_ent[i][k];
			A_piv = A_ent[k];
			A_row = A_ent[i];
			sum -= __ip__(A_row,A_piv,(int)k);
			/************************************************
			for ( j=0; j<k; j++ )
				sum -= A_ent[i][j]*A_ent[k][j];
				sum -= (*A_row++)*(*A_piv++);
			************************************************/
			A_ent[j][i] = A_ent[i][j] = sum/A_ent[k][k];
		}
	}

	return (A);
}