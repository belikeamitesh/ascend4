/*	ASCEND modelling environment
	Copyright (C) 2008-2013 John Pye

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2, or (at your option)
	any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*//** @file
	Ideal-gas components of helmholtz fundamental functions, calculated using
	terms in cp0 in a standard power series form. For details see the
	publications cited in the various fluid *.c files.

	John Pye, Jul 2008.
*/

#include <math.h>

#include "incompressible.h"
#include "cp0.h"
#include "refstate.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#define SQ(X) ((X)*(X))

#ifndef FPROPS_NEW
# error "Where is FPROPS_NEW??"
#endif

#ifndef FPROPS_ARRAY_COPY
# error "where is FPROPS_ARRAY_COPY??"
#endif

//static void incompressible_set_reference_std(FluidData *D, const ReferenceStateStd *R);

#define INCOMPRESSIBLE_DEBUG
#ifdef INCOMPRESSIBLE_DEBUG
# include "color.h"
# define MSG FPROPS_MSG
# define ERRMSG FPROPS_ERRMSG
#else
# define MSG(ARGS...) ((void)0)
# define ERRMSG(ARGS...) ((void)0)
#endif


// next three functions are the typical models to get data from incompressible fit coefficients

// evaluation function for polynamial type coefficients
double eval_poly(IncompressibleCoeff c, double T, double x) {

	double res = 0.0;
	int i,j;

	for(i=0;i<c.numc_r;i++)
		for(j=0;j<c.numc_c;j++)
			res += pow(x,j)*c.coeff[i][j]*pow(T,i);

	return res;

}

// evaluation function for exponential-polynomial type coefficients
double eval_exppoly(IncompressibleCoeff c, double T, double x) {

	return exp(eval_poly(c,T,x));

}

// evaluation function for exponential type coefficients
double eval_expo(IncompressibleCoeff c, double T, double x) {

	return exp(c.coeff[0][0]/(T+c.coeff[0][1])-c.coeff[0][2]);

}	



/*--------------------------------------------
  PREPARATION OF INCOMPRESSIBLE RUNDATA from FILEDATA
*/

PureFluid *incompressible_prepare(const EosData *E, const ReferenceState *ref){
	PureFluid *P = FPROPS_NEW(PureFluid); 
	P->data = FPROPS_NEW(FluidData);
#define D P->data->corr.incomp

	//MSG("...");

	/* metadata */
	P->name = E->name;
	P->source = E->source;
	P->type = FPROPS_INCOMPRESSIBLE;

	switch(E->type){
	case FPROPS_INCOMPRESSIBLE:
		MSG("Incompressible");
		D = FPROPS_NEW(IncompressibleRunData);
#define IC E->data.incomp
		D->T_base = IC->T_base;
		D->T_max = IC->T_max;
		D->T_min = IC->T_min;
		D->x_base = IC->x_base;
		D->x_max = IC->x_max;
		D->x_min = IC->x_min;
		D->T_minPsat = IC->T_minPsat;
		int i,j;
#define INCOMP_PREP(Q) \
	D->Q.type = IC->Q.type; \
	D->Q.numc_r = IC->Q.numc_r; \
	D->Q.numc_c = IC->Q.numc_c; \
	D->Q.coeff = (double**)malloc(D->Q.numc_r*sizeof(double*)); \
	for(i=0;i<D->Q.numc_r;i++) \
		D->Q.coeff[i] = (double*)malloc(D->Q.numc_c*sizeof(double)); \
	for(i=0;i<D->Q.numc_r;i++) \
		for(j=0;j<D->Q.numc_c;j++) \
			D->Q.coeff[i][j] = IC->Q.coeff[i][j]; 
		INCOMP_PREP(T_freeze)
		INCOMP_PREP(conductivity)
		INCOMP_PREP(density)
		INCOMP_PREP(specific_heat)
		INCOMP_PREP(viscosity)
		INCOMP_PREP(saturation_pressure)
#undef IC
#undef INCOMP_PREP
	break;/*
	case FPROPS_HELMHOLTZ:
		MSG("Helmholtz");
		D->M = E->data.helm->M;
		if(E->data.helm->R == 0){
			P->data->R = R_UNIVERSAL / D->M;
		}else{
			P->data->R = E->data.helm->R;
		}
		D->T_t = 0;
		D->T_c = E->data.helm->T_c;
		D->p_c = 0;
		D->rho_c = E->data.helm->rho_c;
		D->omega = 0;
		D->Tstar = 1;
		D->rhostar = 1;
		D->cp0 = cp0_prepare(E->data.helm->ideal, D->R, D->Tstar);
		D->corr.helm = NULL;

		if(ref == NULL){
			ref = &(E->data.helm->ref);
		}
		break;*/
	default:
		ERRMSG("Unsupported source data type in incompressible_prepare");
		FPROPS_FREE(P->data);
		FPROPS_FREE(P);
		return NULL;
	}

	// function pointers... more to come still? 
#define FN(VAR1,VAR2) P->VAR1##_fn = &incomp_eval_##VAR2
	FN(rho,density);  FN(cp,specific_heat); FN(cv,specific_heat); 
	FN(lam,conductivity); FN(mu,viscosity); 
	FN(T_freeze,T_freeze); FN(p_sat,saturation_pressure);
	FN(u,internal_energy); FN(s,entropy);
#undef FN

// FN(h); FN(a); FN(g); FN(w); FN(dpdrho_T); FN(sat);// to be added for incompressible

/*	//MSG("Setting reference state...");
	// set the reference point
	switch(ref->type){
	case FPROPS_INCOMPRESSIBLE:
		MSG("No reference state required in current incompressible implementation.\n");
		FPROPS_FREE(P->data);
		FPROPS_FREE(P);
		return NULL;
		break;
	case FPROPS_REF_PHI0:
		MSG("Applying PHI0 reference data");
		P->data->cp0->c = ref->data.phi0.c;
		P->data->cp0->m = ref->data.phi0.m;
		break;
	case FPROPS_REF_REF0:
		//MSG("Applying ref0 reference state");
		switch(P->data->ref0.type){
		case FPROPS_REF_TPHG:
			{
				//MSG("TPHG");
				ReferenceState *ref0 = &(P->data->ref0);
				//MSG("T0 = %f, p0 = %f, h0 = %f, g0 = %f",ref0->data.tphg.T0,ref0->data.tphg.p0,ref0->data.tphg.h0,ref0->data.tphg.g0);
				FpropsError res = FPROPS_NO_ERROR;
				double rho0 = ref0->data.tphg.p0 / D->R / ref0->data.tphg.T0;
				double T0 = ref0->data.tphg.T0;
				double s0 = (ref0->data.tphg.h0 - ref0->data.tphg.g0) / T0;
				double h0 = ref0->data.tphg.h0;

				P->data->cp0->c = 0;
				P->data->cp0->m = 0;
				//MSG("T0 = %f, rho0 = %f",T0,rho0);
				//MSG("btw, p = %f", D->R * T0 *rho0); // is OK
				res = FPROPS_NO_ERROR;
				double h1 = ideal_h(T0, rho0, P->data, &res);
				double s1 = ideal_s(T0, rho0, P->data, &res);
				if(res)ERRMSG("error %d",res);
				//MSG("h1 = %f",h1);
				P->data->cp0->c = -(s0 - s1)/D->R;
				P->data->cp0->m = (h0 - h1)/D->R/P->data->Tstar;

				h0 = ideal_h(T0,rho0, P->data, &res);
				if(res)ERRMSG("error %d",res);
				//MSG("new h0(T0,rho0) = %f", h0);
				//double g0 = ideal_g(T0,rho0, P->data, &res);
				//if(res)ERRMSG("error %d",res);
				//MSG("new g0(T0,rho0) = %f", g0);
				//MSG("DONE");
			}
			break;
		default:
			ERRMSG("Unsupported type of reference state (ref0) in incompressible_prepare");
			FPROPS_FREE(P->data); FPROPS_FREE(P);
			return NULL;
		}
		break;
	default:
		ERRMSG("Unsupported type of reference state requested in incompressible_prepare.\n");
		FPROPS_FREE(P->data);
		FPROPS_FREE(P);
		return NULL;
	}*/

	return P;
}

#undef D
#define D data->corr.incomp

#define PROP_EVAL(Q) \
	double incomp_eval_ ## Q (double T, double p, const FluidData *data, FpropsError *err) { \
		assert(T>0&&T>=D->T_min&&T<=D->T_max); \
		if(!strcmp(D->Q.type,"polynomial")) return eval_poly(D->Q,T-D->T_base,1.0); \
		else if(!strcmp(D->Q.type,"exppolynomial")) return eval_exppoly(D->Q,T-D->T_base,1.0); \
		else if(!strcmp(D->Q.type,"exponential")) return eval_expo(D->Q,T,1.0); \
		MSG("Invalid request"); \
		*err = FPROPS_INVALID_REQUEST; \
		return 0; \
	}

// declaration and definition of property evaluation functions for conductivity, density, specific-heat and viscosity
PROP_EVAL(T_freeze)
PROP_EVAL(conductivity)
PROP_EVAL(density)
PROP_EVAL(specific_heat)
PROP_EVAL(viscosity)
PROP_EVAL(saturation_pressure)
		
#undef PROP_EVAL

// All incompressible fluids have an arbitrary reference state for enthalpy and entropy. During initialisation, the reference state is defined as a temperature of 20 °C and a pressure of 1 atm according to the U.S. National Institute of Standards and Technology.

double incomp_eval_internal_energy(double T, double p, const FluidData *data, FpropsError *err) {
	assert(T>0&&T>=D->T_min&&T<=D->T_max);
	assert(!strcmp(D->specific_heat.type,"polynomial"));
	double Tref = 293.15; // 20 °C
// We dont know which if the three statements below are required... no validation data as of now
	//T = T-fluid->T_base;
	//Tref = Tref-fluid->T_base;
	//x = x-fluid->x_base;
	double u = 0.0;
	int i,j;
	for(i=0;i<D->specific_heat.numc_r;i++)
		for(j=0;j<D->specific_heat.numc_c;j++)
			u += pow(1.0,j)/(i+1.0)*D->specific_heat.coeff[i][j]*(pow(T,i+1)-pow(Tref,i+1));
	return u;
	
}

double incomp_eval_entropy(double T, double p, const FluidData *data, FpropsError *err) {
	assert(T>0&&T>=D->T_min&&T<=D->T_max);
	assert(!strcmp(D->specific_heat.type,"polynomial"));
	double Tref = 293.15; // 20 °C
// We dont know which if the three statements below are required... no validation data as of now
	//T = T-fluid->T_base;
	//Tref = Tref-fluid->T_base;
	//x = x-fluid->x_base;
	double s = 0.0;
	int i,j;
	double log_term = log(T/Tref);
	for(i=0;i<D->specific_heat.numc_r-1;i++)
		for(j=0;j<D->specific_heat.numc_c;j++)
			s += pow(1.0,j)*(D->specific_heat.coeff[0][j]*log_term+1.0/(i+1.0)*D->specific_heat.coeff[i+1][j]*(pow(T,i+1)-pow(Tref,i+1)));
	return s;
	
}

#undef D

/*
#define DEFINE_TAU double tau = data->Tstar / T
#define DEFINE_TAUDELTA DEFINE_TAU; double delta = rho / data->rhostar

double ideal_p(double T, double rho, const FluidData *data, FpropsError *err){
	return data->R * T * rho;
}

double ideal_h(double T, double rho, const FluidData *data, FpropsError *err){
	DEFINE_TAUDELTA;
	return data->R * T * (1 + tau * ideal_phi_tau(tau,delta,data->cp0));
}

double ideal_s(double T, double rho, const FluidData *data, FpropsError *err){
	DEFINE_TAUDELTA;
	//double pht = ideal_phi_tau(tau,delta,data->cp0);
	//double ph = ideal_phi(tau,delta,data->cp0);
	return data->R * (tau * ideal_phi_tau(tau,delta,data->cp0) - ideal_phi(tau,delta,data->cp0));
}

double ideal_u(double T, double rho, const FluidData *data, FpropsError *err){
	return ideal_h(T,rho,data,err) - data->R * T;
}

double ideal_a(double T, double rho, const FluidData *data, FpropsError *err){
	return ideal_h(T,rho,data,err) - T * (data->R + ideal_s(T,rho,data,err));
}

double ideal_g(double T, double rho, const FluidData *data, FpropsError *err){
	//MSG("g(T=%f,rho=%f)...",T,rho);
	double h = ideal_h(T,rho,data,err);
	double s = ideal_s(T,rho,data,err);
	//MSG("h = %f, T = %f, s = %f, h-T*s = %f",h,T,s,h-T*s);
	return h - T * s;
}


//	Note that this function is called by ALL fluid types via 'fprops_cp0' which
//	means that it needs to include the scaling temperature within the structure;
//	we can't just define Tstar as a constant for ideal fluids.

double ideal_cp(double T, double rho, const FluidData *data, FpropsError *err){
	DEFINE_TAU;
	double res = data->R * (1. - SQ(tau) * ideal_phi_tautau(tau,data->cp0));
	return res;
}

double ideal_cv(double T, double rho, const FluidData *data, FpropsError *err){
	DEFINE_TAU;
	return - data->R * SQ(tau) * ideal_phi_tautau(tau,data->cp0);
}

double ideal_w(double T, double rho, const FluidData *data, FpropsError *err){
	DEFINE_TAU;
	double w2onRT = 1. - 1. / (SQ(tau) * ideal_phi_tautau(tau,data->cp0));
	return sqrt(data->R * T * w2onRT);
}

double ideal_dpdrho_T(double T, double rho, const FluidData *data, FpropsError *err){
	return data->R * T;
}

double ideal_sat(double T,double *rhof_ret, double *rhog_ret, const FluidData *data, FpropsError *err){
	MSG("Ideal gas: saturation calculation is not possible");
	*err = FPROPS_RANGE_ERROR;
	return 0;
}

*/

