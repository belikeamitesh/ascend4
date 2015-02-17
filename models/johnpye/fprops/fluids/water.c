/*	ASCEND modelling environment
	Copyright (C) 2008-2009 Carnegie Mellon University

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2, or (at your option)
	any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330,
	Boston, MA 02111-1307, USA.
*/

#include "../helmholtz.h"

#define WATER_R 461.51805 /* J/kg·K */
#define WATER_TC 647.096 /* K */
#define WATER_RHOC 322.
#define WATER_PC 22.064e6 /* Pa */

/**
Ideal gas data for Water/Steam, from IAPWS-95.
http://www.iapws.org/relguide/IAPWS95.pdf
*/
const IdealData ideal_data_water = {
	-8.32044648201 /* constant */
	, 6.6832105268 /* linear */
	, WATER_TC /* Tstar */
	, WATER_R /* cpstar J/kgK */
	, 1 /* power terms */
	, (const IdealPowTerm[]){
		{1. + 3.00632,  0}
	}
	, 5 /* exponential terms */
	, (const IdealExpTerm []){
		/* b, beta */
		{0.012436, 1.28728967*WATER_TC}
		,{0.97315,  3.53734222*WATER_TC}
		,{1.27950, 7.74073708*WATER_TC}
		,{0.96956, 9.24437796*WATER_TC}
		,{0.24873, 27.5075105*WATER_TC}
	}
};


/**
Residual (non-ideal) property data for Water/Steam, from IAPWS-95.
http://www.iapws.org/relguide/IAPWS95.pdf
*/
const HelmholtzData helmholtz_data_water = {
	"water"
	, /* R */ WATER_R /* J/kg/K */
	, /* M */ 18.015242 /* kg/kmol -- G S Kell, J Phys Chem Ref Data (6) 1109 (1977) */
	, /* rho_star */ WATER_RHOC /* kg/m³ */
	, /* T_star */ WATER_TC /* K */

	, /* T_c */ WATER_TC
	, /* rho_c */ WATER_RHOC
	, /* T_t */ 273.16

	, 0.344 /* acentric factor, source: Reid, Prausnitz & Polling */
	, &ideal_data_water
	, 51 /* np */
	, (const HelmholtzPowTerm[]){
		/* a_i, t_i, d_i, l_i */
		{0.12533547935523E-1, -0.5, 1, 0}
		,{0.78957634722828E1, 0.875, 1, 0}
		,{-0.87803203303561E1, 1, 1, 0}
		,{0.31802509345418, 0.5, 2, 0}
		,{-0.26145533859358, 0.75, 2, 0}
		,{-0.78199751687981E-2, 0.375, 3, 0}
		,{0.88089493102134E-2, 1, 4, 0}
		,{-0.66856572307965, 4, 1, 1}
		,{0.20433810950965, 6, 1, 1}
		,{-0.66212605039687E-4, 12, 1, 1}
		,{-0.19232721156002, 1, 2, 1}
		,{-0.25709043003438, 5, 2, 1}
		,{0.16074868486251, 4, 3, 1}
		,{-0.40092828925807E-1, 2, 4, 1}
		,{0.39343422603254E-6, 13, 4, 1}
		,{-0.75941377088144E-5, 9, 5, 1}
		,{0.56250979351888E-3, 3, 7, 1}
		,{-0.15608652257135E-4, 4, 9, 1}
		,{0.11537996422951E-8, 11, 10, 1}
		,{0.36582165144204E-6, 4, 11, 1}
		,{-0.13251180074668E-11, 13, 13, 1}
		,{-0.62639586912454E-9, 1, 15, 1}
		,{-0.10793600908932, 7, 1, 2}
		,{0.17611491008752E-1, 1, 2, 2}
		,{0.22132295167546, 9, 2, 2}
		,{-0.40247669763528, 10, 2, 2}
		,{0.58083399985759, 10, 3, 2}
		,{0.49969146990806E-2, 3, 4, 2}
		,{-0.31358700712549E-1, 7, 4, 2}
		,{-0.74315929710341, 10, 4, 2}
		,{0.47807329915480, 10, 5, 2}
		,{0.20527940895948E-1, 6, 6, 2}
		,{-0.13636435110343, 10, 6, 2}
		,{0.14180634400617E-1, 10, 7, 2}
		,{0.83326504880713E-2, 1, 9, 2}
		,{-0.29052336009585E-1, 2, 9, 2}
		,{0.38615085574206E-1, 3, 9, 2}
		,{-0.20393486513704E-1, 4, 9, 2}
		,{-0.16554050063734E-2, 8, 9, 2}
		,{0.19955571979541E-2, 6, 10, 2}
		,{0.15870308324157E-3, 9, 10, 2}
		,{-0.16388568342530E-4, 8, 12, 2}
		,{0.43613615723811E-1, 16, 3, 3}
		,{0.34994005463765E-1, 22, 4, 3}
		,{-0.76788197844621E-1, 23, 4, 3}
		,{0.22446277332006E-1, 23, 5, 3}
		,{-0.62689710414685E-4, 10, 14, 4}
		,{-0.55711118565645E-9, 50, 3, 6}
		,{-0.19905718354408, 44, 6, 6}
		,{0.31777497330738, 46, 6, 6}
		,{-0.11841182425981, 50, 6, 6}
	}
	, 3 /* gaussian terms */
	, (const HelmholtzGausTerm[]){
		/* n, t, d, alpha, beta, gamma, epsilon */
		{-0.31306260323435e2, 0, 3, 20, 150, 1.21, 1}
		,{0.31546140237781e2, 1, 3, 20, 150, 1.21, 1}
		,{-0.25213154341695e4,4, 3, 20, 250, 1.25, 1}
	}
	, 2 /* critical terms */
	, (const HelmholtzCritTerm[]){
		/* n, a, b, beta, A, B, C, D */
		{-0.14874640856724, 3.5, 0.85, 0.3, 0.32, 0.2, 28, 700}
		,{0.31806110878444, 3.5, 0.95, 0.3, 0.32, 0.2, 32, 800}
	}
};

#ifdef TEST
# include "../test.h"
# include "../sat.h"

/*
	Test suite. These tests attempt to validate the current code using
	a few sample figures output by REFPROP 7.0.

	To run the test, compile and run as follows:

	./test.py water
*/
/*
	some code from freesteam, http://freesteam.sf.net/, which has been thoroughly
	validated already.
*/

const double n0[] = {
    0.0 /* placeholder */,
    -8.32044648201, 6.6832105268, 3.00632 /* const, linear, ln(tau) coeffs */
	, 0.012436, 0.97315, 1.27950, 0.96956, 0.24873 /* exponential coeffs */
};

const double gamma0[] = {
    0.0, 0.0, 0.0, 0.0,
    1.28728967,
    3.53734222,
    7.74073708,
    9.24437796,
    27.5075105
};

enum Limits{
    eGamma1 = 4,
    eGamma2 = 9,
};

# include <math.h>
# include <stdlib.h>
# include <stdio.h>
# include "../ideal_impl.h"
# include "../helmholtz_impl.h"

double phi0(const double delta, const double tau){
	int i;
    double sum = 0;
    for (i = eGamma1; i < eGamma2; i++)
    {
        sum += n0[i]*log(1-exp(-tau*gamma0[i]));
    }
    sum += log(delta) + n0[1] + n0[2]*tau + n0[3]*log(tau);
    return sum;
}

typedef struct{double T, rho, p, cv, w, s;} TestDataIAPWS95;
const TestDataIAPWS95 td[]; const unsigned ntd;

const TestDataSat tds[]; const unsigned ntds;

const TestData td1[]; const unsigned ntd1;

int main(void){
	double rho, T;
	const HelmholtzData *d;

	d = &helmholtz_data_water;
	double maxerr = 0;
	unsigned i;


#if 0
	/* these tests pass, but don't prove much */
	fprintf(stderr,"COMPARISON OF phi0 VALUES WITH THOSE FROM FREESTEAM\n");
	for(T = 300; T <= 900; T+= 100){
		for(rho = 900; rho >= 0.9; rho*=0.5){
			double delta = rho / d->rho_star;
			double tau = d->T_star / T;
			double p0 = phi0(delta,tau);

		 	ASSERT_TOL(helm_ideal, tau, delta, d->ideal, p0, p0*1e-5);
		}
	}
#endif

	/* LOW-LEVEL TEST DATA PROVIDED IN IAPWS95 */

	fprintf(stderr,"\nIAPWS95 TABLE 6 TESTS\n");
	T = 500.; /* K */
	rho = 838.025; /* kg/m³ */
	double tau = d->T_star / T;
	double delta = rho / d->rho_star;

	ASSERT_TOL(helm_ideal, tau, delta, d->ideal, 0.204797733E1, 1e-8);
	ASSERT_TOL(helm_ideal_tau, tau, delta, d->ideal, 0.904611106E1, 1e-8);
	ASSERT_TOL(HELM_IDEAL_DELTAU, tau, delta, d->ideal, 0., 1e-8);

	double phitt = helm_ideal_tautau(tau, d->ideal);
	double val = (-0.193249185E1);
	double err = phitt - val;
	if(fabs(err) > 1e-8){
		fprintf(stderr,"ERROR in helm_ideal_tautau near line %d\n",__LINE__);
		exit(1);
	}else{
		fprintf(stderr,"    OK, helm_ideal_tautau(%f) = %8.2e with %.6f%% err.\n"
			,tau,val,err/val*100.
		);
	}

	/* FIXME still need to implement helm_ideal_del, helm_ideal_deldel, helm_ideal_deltau */

	ASSERT_TOL(helm_resid, tau, delta, d, -0.342693206E1, 1e-8);
	ASSERT_TOL(helm_resid_del, tau, delta, d, -0.364366650, 1e-8);
	ASSERT_TOL(helm_resid_deldel, tau, delta, d, 0.856063701, 1e-8);
	ASSERT_TOL(helm_resid_tau, tau, delta, d, -0.581403435E1, 1e-8);
	ASSERT_TOL(helm_resid_tautau, tau, delta, d, -0.223440737E1, 1e-8);
	ASSERT_TOL(helm_resid_deltau, tau, delta, d, -0.112176915e1, 1e-8);

#if 0
	fprintf(stderr,"\nADDITIONAL LOW-LEVEL TESTS NEAR CRITICAL POINT\n");

	T = 647.; /* K */
	rho = 358.; /* kg/m³ */
	tau = d->T_star / T;
	delta = rho / d->rho_star;

	/* this test value calculated from pressure using REFPROP 8 */
	ASSERT_TOL(helmholtz_a, T, rho, d, -8.286875181e5, 1e-4);
	ASSERT_TOL(helm_resid_del, tau, delta, d, -7.14012024e-1, 1e-8);
	ASSERT_TOL(helmholtz_s, T, rho, d, 4.320923066e3, 5e-8);
	ASSERT_TOL(helmholtz_cv, T, rho, d, 6.183157277e3, 5e-7);
	ASSERT_TOL(helmholtz_p, T, rho, d, 2.203847557e7, 7e-4);
	ASSERT_TOL(helmholtz_cp, T, rho, d, 3.531798573e6, 1e-8);
	ASSERT_TOL(helmholtz_w, T, rho, d, 2.52140783e2, 1e-8);
#endif

	fprintf(stderr,"\nIAPWS95 TABLE 7 (SINGLE-PHASE) TESTS\n");
	for(i=0; i<ntd; ++i){
		double T = td[i].T;
		double rho = td[i].rho;
		double p = td[i].p * 1e6; /* Pa */
		double cv = td[i].cv * 1e3; /* J/kgK */
		double w = td[i].w; /* m/s */
		double s = td[i].s * 1e3; /* J/kgK */
		//fprintf(stderr,"T = %f, rho = %f, p = %f, w = %f, wcalc = %f\n",T,rho,p,w, helmholtz_w(T,rho,d));
		ASSERT_TOL(helmholtz_s_raw, T, rho, d, s, s*1e-8);
		ASSERT_TOL(helmholtz_p_raw, T, rho, d, p, p*1e-8);
		ASSERT_TOL(helmholtz_cv, T, rho, d, cv, cv*1e-8);
		ASSERT_TOL(helmholtz_w, T, rho, d, w, w*2e-5);
	}

	fprintf(stderr,"\nIAPWS95 TABLE 8 (SATURATION) TESTS (%d items)\n",ntds);
	for(i=0; i<ntds; ++i){
		double T = tds[i].T;
		double p = tds[i].p * 1e6; /* Pa */
		double rho_f = tds[i].rhof;
		double rho_g = tds[i].rhog;
		double h_f = tds[i].hf * 1e3;
		double h_g = tds[i].hg * 1e3;
		double s_f = tds[i].sf * 1e3;
		double s_g = tds[i].sg * 1e3;
		fprintf(stderr,"T = %f, p = %f bar, rho_f = %f, rho_g = %f\n",T,p/1e5,rho_f, rho_g);
		double rho_f_eval, rho_g_eval, p_eval;
		int res = fprops_sat_T(T, &p_eval, &rho_f_eval, &rho_g_eval, d);
		ASSERT_TOL_VAL(p_eval, p, 1e-8);
		ASSERT_TOL_VAL(rho_f_eval, rho_f, 1e-8);
		ASSERT_TOL_VAL(rho_g_eval, rho_g, 1e-8);
	}

#if 0
	fprintf(stderr,"\nPROBLEMATIC SATURATION VALUES\n");
	{
		double T = 3.7631475862e+02;
		T = 3.8920910345e+02;
		double p, rho_f, rho_g;
		int res = fprops_sat_T(T, &p, &rho_f, &rho_g, d);
		if(res){
			fprintf(stderr,"ERROR = %d\n",res);
		}else{
			fprintf(stderr,"OK :-)\n");
		}
		fprintf(stderr,"p_sat(T = %f) = %f bar, rho_f = %f, rho_g = %f\n", T, p/1e5, rho_f, rho_g);
	}
#endif

# if 1
	helm_run_test_cases(d, ntd1, td1, 'K');
# endif

	//helm_check_dpdrho_T(d, ntd1, td1);

# if 1
	//helm_check_d2pdrho2_T(d,ntd1, td1);

	fprintf(stderr,"Tests completed OK (maximum error = %0.8f%%)\n",maxerr);
# endif

	exit(0);
}

/* HIGHER-LEVEL TEST-DATA PROVIDED IN IAPWS95 */

const TestDataIAPWS95 td[] = {
	{300, 0.9965560e3, 0.992418352e-1, 0.413018112e1, 0.150151914e4, 0.393062643}
	,{300, 0.1005308e4, 0.200022515e2,  0.406798347e1, 0.153492501e4, 0.387405401}
	,{300, 0.1188202e4, 0.700004704e3,  0.346135580e1, 0.244357992e4, 0.132609616}
	,{500, 0.4350000,   0.999679423e-1, 0.150817541e1, 0.548314253e3, 0.794488271e1}
	,{500, 0.4532000e1, 0.999938125,    0.166991025e1, 0.535739001e3, 0.682502725e1}
	,{500, 0.8380250e3, 0.100003858e2,  0.322106219e1, 0.127128441e4, 0.256690919e1}
	,{500, 0.1084564e4, 0.700000405e3,  0.307437693e1, 0.241200877e4, 0.203237509e1}
	,{647, 0.3580000e3, 0.220384756e2,  0.618315728e1, 0.252145078e3, 0.432092307e1}
	,{900, 0.2410000,   0.100062559,    0.175890657e1, 0.724027147e3, 0.916653194e1}
	,{900, 0.5261500e2, 0.200000690e2,  0.193510526e1, 0.698445674e3, 0.659070225e1}
	,{900, 0.8707690e3, 0.700000006e3,  0.266422350e1, 0.201933608e4, 0.417223802e1}
};

const unsigned ntd = sizeof(td)/sizeof(TestDataIAPWS95);

const TestDataSat tds[] = {
	/* T, p (MPa), rho_f, rho_g, h_f (kJ/kg), h_g (kJ/kg), s_f (kJ/kgK), s_g (kJ/kgK) */
	{450, 0.932203564, 0.890341250e3, 0.481200360e1, 0.749161585e3, 0.277441078e4, 0.210865845e1, 0.660921221e1}
	,{275, 0.698451167e-3, 0.999887406e3, 0.550664919e-2, 0.775972202e1, 0.250428995e4, 0.283094670e-1, 0.910660121e1}
	,{625, 0.169082693e2, 0.567090385e3, 0.118290280e3, 0.168626976e4, 0.255071625e4, 0.380194683e1, 0.518506121e1}
};

const unsigned ntds = sizeof(tds)/sizeof(TestDataSat);

const TestData td1[] = {
/* {Temperature, Pressure, Density, Int. Energy, Enthalpy, Entropy, Cv, Cp, Cp0, Helmholtz}
, {(K), (MPa), (kg/m³), (kJ/kg), (kJ/kg), (kJ/kg-K), (kJ/kg-K), (kJ/kg-K), (kJ/kg-K), (kJ/kg)} */
  {3.189563289E+2, 1.000000001E-2, 9.898332754E+2, 1.917958417E+2, 1.918059444E+2, 6.491956046E-1, 4.046584194E+0, 4.180521426E+0, 1.870066063E+0, -1.526920512E+1}
, {3.189563289E+2, 1.E-2, 6.816572231E-2, 2.437157373E+3, 2.583858672E+3, 8.148820193E+0, 1.461502908E+0, 1.940043153E+0, 1.870066063E+0, -1.619604013E+2}
, {3.2315E+2, 1.E-2, 6.726318947E-2, 2.443297379E+3, 2.591967105E+3, 8.174076459E+0, 1.451722526E+0, 1.927979811E+0, 1.871345196E+0, -1.98155429E+2}
, {3.7315E+2, 1.E-2, 5.815174873E-2, 2.515492539E+3, 2.687456413E+3, 8.448856586E+0, 1.439010754E+0, 1.905785033E+0, 1.889796623E+0, -6.371982961E+2}
, {4.2315E+2, 1.E-2, 5.124744725E-2, 2.587907788E+3, 2.783039458E+3, 8.689221881E+0, 1.455856293E+0, 1.91992654E+0, 1.913179444E+0, -1.088936451E+3}
, {4.7315E+2, 1.E-2, 4.581778713E-2, 2.661343451E+3, 2.879599299E+3, 8.904884547E+0, 1.480446696E+0, 1.943421402E+0, 1.939987342E+0, -1.552002673E+3}
, {5.2315E+2, 1.E-2, 4.143175055E-2, 2.736087559E+3, 2.977448348E+3, 9.101451358E+0, 1.508569718E+0, 1.971012253E+0, 1.969014174E+0, -2.025336719E+3}
, {5.7315E+2, 1.E-2, 3.781350306E-2, 2.812280523E+3, 3.076736317E+3, 9.282689277E+0, 1.538635313E+0, 2.000784008E+0, 1.999506331E+0, -2.508092837E+3}
, {6.2315E+2, 1.E-2, 3.477715717E-2, 2.890004655E+3, 3.177549722E+3, 9.451311366E+0, 1.569977036E+0, 2.031948029E+0, 2.031074092E+0, -2.999580023E+3}
, {6.7315E+2, 1.E-2, 3.219256212E-2, 2.969317897E+3, 3.279948656E+3, 9.609360266E+0, 1.602313357E+0, 2.064169523E+0, 2.06354145E+0, -3.499222966E+3}
, {7.2315E+2, 1.E-2, 2.996577198E-2, 3.050267542E+3, 3.383981621E+3, 9.758422179E+0, 1.635510876E+0, 2.097289044E+0, 2.096820368E+0, -4.006535457E+3}
, {7.7315E+2, 1.E-2, 2.802723909E-2, 3.132894879E+3, 3.489690637E+3, 9.899756057E+0, 1.669469567E+0, 2.131192649E+0, 2.130832398E+0, -4.521101516E+3}
, {8.2315E+2, 1.E-2, 2.632436164E-2, 3.217235633E+3, 3.597111891E+3, 1.003437659E+1, 1.704072662E+0, 2.165755593E+0, 2.165471942E+0, -5.042561456E+3}
, {8.7315E+2, 1.E-2, 2.481661209E-2, 3.303318766E+3, 3.706274655E+3, 1.016311063E+1, 1.739172311E+0, 2.200825204E+0, 2.20059735E+0, -5.57060128E+3}
, {9.2315E+2, 1.E-2, 2.347225746E-2, 3.391165031E+3, 3.817199895E+3, 1.028663753E+1, 1.774593372E+0, 2.236223291E+0, 2.236037125E+0, -6.104944404E+3}
, {9.7315E+2, 1.E-2, 2.226609485E-2, 3.480785909E+3, 3.929899239E+3, 1.04055192E+1, 1.81014454E+0, 2.271756554E+0, 2.27160221E+0, -6.645345105E+3}
, {1.02315E+3, 1.E-2, 2.1177853E-2, 3.572183128E+3, 4.044374525E+3, 1.052022323E+1, 1.845630764E+0, 2.307228588E+0, 2.307098991E+0, -7.191583272E+3}
, {1.07315E+3, 1.E-2, 2.019104172E-2, 3.665348772E+3, 4.160617918E+3, 1.063114098E+1, 1.880863969E+0, 2.342450386E+0, 2.342340346E+0, -7.743460174E+3}
, {3.2315E+2, 9.999999999E-2, 9.88034468E+2, 2.093161381E+2, 2.094173491E+2, 7.03768015E-1, 4.026185837E+0, 4.181345361E+0, 1.871345196E+0, -1.810649594E+1}
, {3.727559289E+2, 1.E-1, 9.586315058E+2, 4.173995952E+2, 4.175039106E+2, 1.302758106E+0, 3.770208021E+0, 4.215222877E+0, 1.889629913E+0, -6.821121265E+1}
, {3.727559289E+2, 1.E-1, 5.903439801E-1, 2.505554911E+3, 2.674947677E+3, 7.358848946E+0, 1.554798204E+0, 2.078449429E+0, 1.889629913E+0, -2.374996638E+2}
, {3.7315E+2, 1.E-1, 5.896694907E-1, 2.506179846E+3, 2.675766371E+3, 7.361044114E+0, 1.553500952E+0, 2.076616047E+0, 1.889796623E+0, -2.40593765E+2}
, {4.2315E+2, 1.E-1, 5.163554899E-1, 2.582937555E+3, 2.776602582E+3, 7.614753124E+0, 1.495922292E+0, 1.984639274E+0, 1.913179444E+0, -6.392452295E+2}
, {4.7315E+2, 1.E-1, 4.603136527E-1, 2.658207492E+3, 2.875450668E+3, 7.835560577E+0, 1.498840352E+0, 1.97540788E+0, 1.939987342E+0, -1.049187995E+3}
, {5.2315E+2, 1.E-1, 4.156009552E-1, 2.733898454E+3, 2.974513878E+3, 8.034578143E+0, 1.518412435E+0, 1.989344909E+0, 1.969014174E+0, -1.469391101E+3}
, {5.7315E+2, 1.E-1, 3.789536427E-1, 2.810648619E+3, 3.074533139E+3, 8.217155719E+0, 1.544520444E+0, 2.012414405E+0, 1.999506331E+0, -1.899014181E+3}
, {6.2315E+2, 1.E-1, 3.483167344E-1, 2.888732001E+3, 3.175827021E+3, 8.386583701E+0, 1.573789537E+0, 2.039869109E+0, 2.031074092E+0, -2.337367632E+3}
, {6.7315E+2, 1.E-1, 3.223007344E-1, 2.968292388E+3, 3.278561616E+3, 8.545151842E+0, 1.60493276E+0, 2.06984826E+0, 2.06354145E+0, -2.783876574E+3}
, {7.2315E+2, 1.E-1, 2.999224554E-1, 3.049420354E+3, 3.38283987E+3, 8.694565865E+0, 1.637392267E+0, 2.101520411E+0, 2.096820368E+0, -3.238054951E+3}
, {7.7315E+2, 1.E-1, 2.804629849E-1, 3.132181123E+3, 3.488734413E+3, 8.83614818E+0, 1.670868603E+0, 2.134442128E+0, 2.130832398E+0, -3.699486843E+3}
, {8.2315E+2, 1.E-1, 2.63382991E-1, 3.216624634E+3, 3.596299873E+3, 8.970949673E+0, 1.705142613E+0, 2.1683126E+0, 2.165471942E+0, -4.167812589E+3}
, {8.7315E+2, 1.E-1, 2.48269273E-1, 3.302788787E+3, 3.705577255E+3, 9.099819036E+0, 1.740009928E+0, 2.20287838E+0, 2.20059735E+0, -4.642718204E+3}
, {9.2315E+2, 1.E-1, 2.347995983E-1, 3.390700204E+3, 3.816595311E+3, 9.223449402E+0, 1.775262304E+0, 2.237900336E+0, 2.236037125E+0, -5.123927111E+3}
, {9.7315E+2, 1.E-1, 2.227188051E-1, 3.48037436E+3, 3.929371022E+3, 9.342411706E+0, 1.810688091E+0, 2.273146636E+0, 2.27160221E+0, -5.611193591E+3}
, {1.02315E+3, 1.E-1, 2.118221216E-1, 3.571815787E+3, 4.04391001E+3, 9.457179614E+0, 1.846079228E+0, 2.308395614E+0, 2.307098991E+0, -6.104297535E+3}
, {1.07315E+3, 1.E-1, 2.019432602E-1, 3.665018583E+3, 4.160207181E+3, 9.568148715E+0, 1.881239044E+0, 2.343441181E+0, 2.342340346E+0, -6.603040211E+3}
, {2.7315E+2, 9.999999999E-1, 1.000299823E+3, -2.386750479E-2, 9.758327621E-1, -8.83057426E-5, 4.212768783E+0, 4.21499383E+0, 1.859016668E+0, 2.532088014E-4}
, {3.2315E+2, 1.E+0, 9.884268719E+2, 2.091816549E+2, 2.101933635E+2, 7.033511679E-1, 4.023884681E+0, 4.179273344E+0, 1.871345196E+0, -1.810627503E+1}
, {3.7315E+2, 1.E+0, 9.587706558E+2, 4.187982943E+2, 4.198412966E+2, 1.30650822E+0, 3.766775608E+0, 4.213604828E+0, 1.889796623E+0, -6.872524791E+1}
, {4.2315E+2, 1.E+0, 9.173054424E+2, 6.314125646E+2, 6.32502714E+2, 1.841216103E+0, 3.522740375E+0, 4.305377986E+0, 1.913179444E+0, -1.476980293E+2}
, {4.530280079E+2, 1.E+0, 8.87129266E+2, 7.613878381E+2, 7.625150695E+2, 2.138064469E+0, 3.395415576E+0, 4.404483983E+0, 1.928874342E+0, -2.072152492E+2}
, {4.530280079E+2, 1.E+0, 5.14504078E+0, 2.582746685E+3, 2.777108604E+3, 6.58501587E+0, 1.927129916E+0, 2.71137544E+0, 1.928874342E+0, -4.004499369E+2}
, {4.7315E+2, 1.E+0, 4.853858846E+0, 2.622242827E+3, 2.828264476E+3, 6.69554277E+0, 1.751994047E+0, 2.428060519E+0, 1.939987342E+0, -5.457532341E+2}
, {5.2315E+2, 1.E+0, 4.296515473E+0, 2.710375674E+3, 2.943122421E+3, 6.926482857E+0, 1.635413152E+0, 2.210639279E+0, 1.969014174E+0, -9.132138326E+2}
, {5.7315E+2, 1.E+0, 3.876151216E+0, 2.793644527E+3, 3.051632398E+3, 7.124623603E+0, 1.609629028E+0, 2.142490116E+0, 1.999506331E+0, -1.289833491E+3}
, {6.2315E+2, 1.E+0, 3.539774495E+0, 2.875681064E+3, 3.158184936E+3, 7.302874374E+0, 1.614274876E+0, 2.124806261E+0, 2.031074092E+0, -1.675105102E+3}
, {6.7315E+2, 1.E+0, 3.26151893E+0, 2.957868639E+3, 3.264474249E+3, 7.466942243E+0, 1.632101688E+0, 2.129287554E+0, 2.06354145E+0, -2.068503531E+3}
, {7.2315E+2, 1.E+0, 3.026206863E+0, 3.04085417E+3, 3.371300846E+3, 7.620014696E+0, 1.656639334E+0, 2.14517287E+0, 2.096820368E+0, -2.469559457E+3}
, {7.7315E+2, 1.E+0, 2.823959952E+0, 3.124987737E+3, 3.479100407E+3, 7.764148141E+0, 1.685062689E+0, 2.167661231E+0, 2.130832398E+0, -2.877863398E+3}
, {8.2315E+2, 1.E+0, 2.647916223E+0, 3.210480109E+3, 3.588135562E+3, 7.900793975E+0, 1.715942015E+0, 2.194296955E+0, 2.165471942E+0, -3.293058452E+3}
, {8.7315E+2, 1.E+0, 2.493091684E+0, 3.297466869E+3, 3.698575262E+3, 8.031035658E+0, 1.748436361E+0, 2.223657972E+0, 2.20059735E+0, -3.714831916E+3}
, {9.2315E+2, 1.E+0, 2.355746133E+0, 3.386037351E+3, 3.810531309E+3, 8.155711647E+0, 1.781977108E+0, 2.254824526E+0, 2.236037125E+0, -4.142907856E+3}
, {9.7315E+2, 1.E+0, 2.233001083E+0, 3.476249057E+3, 3.924076873E+3, 8.27548677E+0, 1.816136292E+0, 2.287145707E+0, 2.27160221E+0, -4.577040893E+3}
, {1.02315E+3, 1.E+0, 2.12259597E+0, 3.568135659E+3, 4.039256878E+3, 8.39089749E+0, 1.850569806E+0, 2.320130186E+0, 2.307098991E+0, -5.017011107E+3}
, {1.07315E+3, 1.E+0, 2.022725587E+0, 3.661712039E+3, 4.156094473E+3, 8.502382625E+0, 1.884992128E+0, 2.353392072E+0, 2.342340346E+0, -5.462619875E+3}
, {2.7315E+2, 1.E+1, 1.004821445E+3, 1.17101466E-1, 1.006911837E+1, 3.375702801E-4, 4.172137191E+0, 4.172661376E+0, 1.859016668E+0, 2.489414397E-2}
, {3.2315E+2, 1.E+1, 9.923080019E+2, 2.078610372E+2, 2.179385534E+2, 6.991973729E-1, 4.001377763E+0, 4.159151637E+0, 1.871345196E+0, -1.808459384E+1}
, {3.7315E+2, 1.E+1, 9.629337501E+2, 4.162312969E+2, 4.266162273E+2, 1.299562803E+0, 3.753122152E+0, 4.193495052E+0, 1.889796623E+0, -6.870056315E+1}
, {4.2315E+2, 1.E+1, 9.223214984E+2, 6.272681485E+2, 6.381103549E+2, 1.83134534E+0, 3.514302485E+0, 4.277325212E+0, 1.913179444E+0, -1.476656319E+2}
, {4.7315E+2, 1.E+1, 8.70935282E+2, 8.443136059E+2, 8.557955153E+2, 2.317415003E+0, 3.312585903E+0, 4.449131602E+0, 1.939987342E+0, -2.52171303E+2}
, {5.2315E+2, 1.E+1, 8.057027666E+2, 1.073388393E+3, 1.085799918E+3, 2.779235189E+0, 3.152168139E+0, 4.793364634E+0, 1.969014174E+0, -3.805684961E+2}
, {5.7315E+2, 1.E+1, 7.152875259E+2, 1.329353574E+3, 1.343333966E+3, 3.248793807E+0, 3.049342906E+0, 5.680706465E+0, 1.999506331E+0, -5.326925965E+2}
, {5.84147147E+2, 1.E+1, 6.884236923E+2, 1.393537996E+3, 1.408063934E+3, 3.360647955E+0, 3.04378308E+0, 6.123699888E+0, 2.006365616E+0, -5.69574919E+2}
, {5.84147147E+2, 1.E+1, 5.546308524E+1, 2.545192344E+3, 2.725492447E+3, 5.615950441E+0, 3.106489802E+0, 7.140822363E+0, 2.006365616E+0, -7.353490841E+2}
, {6.2315E+2, 1.E+1, 4.456394017E+1, 2.699627008E+3, 2.924023683E+3, 5.945939056E+0, 2.307133509E+0, 4.011708323E+0, 2.031074092E+0, -1.005584915E+3}
, {6.7315E+2, 1.E+1, 3.782672181E+1, 2.833083224E+3, 3.097446603E+3, 6.21408075E+0, 2.008406843E+0, 3.095280944E+0, 2.06354145E+0, -1.349925233E+3}
, {7.2315E+2, 1.0E+1, 3.357766295E+1, 2.944524023E+3, 3.242341058E+3, 6.421857194E+0, 1.892748344E+0, 2.747256902E+0, 2.096820368E+0, -1.699442006E+3}
, {7.7315E+2, 1.0E+1, 3.047786995E+1, 3.047020511E+3, 3.37512743E+3, 6.599470302E+0, 1.846727708E+0, 2.583029415E+0, 2.130832398E+0, -2.055359953E+3}
, {8.2315E+2, 1.0E+1, 2.804697481E+1, 3.14541416E+3, 3.501958852E+3, 6.758456245E+0, 1.833325327E+0, 2.499449759E+0, 2.165471942E+0, -2.417809098E+3}
, {8.7315E+2, 1.0E+1, 2.605661173E+1, 3.241978445E+3, 3.625758198E+3, 6.904474568E+0, 1.837303527E+0, 2.45763722E+0, 2.20059735E+0, -2.786663523E+3}
, {9.2315E+2, 1.0E+1, 2.437954952E+1, 3.337939539E+3, 3.748119391E+3, 7.040751249E+0, 1.851394456E+0, 2.439892401E+0, 2.236037125E+0, -3.161729976E+3}
, {9.7315E+2, 1.0E+1, 2.293722219E+1, 3.434021058E+3, 3.869993641E+3, 7.169320693E+0, 1.871707067E+0, 2.43704202E+0, 2.27160221E+0, -3.542803373E+3}
, {1.02315E+3, 1.0E+1, 2.167749316E+1, 3.530675524E+3, 3.991983477E+3, 7.291560761E+0, 1.895952733E+0, 2.443848326E+0, 2.307098991E+0, -3.929684869E+3}
, {1.07315E+3, 1.0E+1, 2.056387915E+1, 3.628195554E+3, 4.114485127E+3, 7.408454575E+0, 1.922680153E+0, 2.457089677E+0, 2.342340346E+0, -4.322187472E+3}
, {2.7315E+2, 1.E+2, 1.045277961E+3, -2.637275578E-1, 9.540460535E+1, -8.514665416E-3, 3.876084092E+0, 3.905222088E+0, 1.859016668E+0, 2.062053301E+0}
, {3.2315E+2, 1.E+2, 1.027427394E+3, 1.965895902E+2, 2.939200688E+2, 6.586511576E-1, 3.819599562E+0, 4.007073837E+0, 1.871345196E+0, -1.625353138E+1}
, {3.7315E+2, 1.E+2, 9.997617888E+2, 3.950856416E+2, 4.951094684E+2, 1.237476322E+0, 3.63626859E+0, 4.038493344E+0, 1.889796623E+0, -6.667864774E+1}
, {4.2315E+2, 1.E+2, 9.648461536E+2, 5.942862812E+2, 6.979297477E+2, 1.74750239E+0, 3.440192741E+0, 4.078104116E+0, 1.913179444E+0, -1.451693553E+2}
, {4.7315E+2, 1.E+2, 9.237401722E+2, 7.951412987E+2, 9.033968482E+2, 2.206386951E+0, 3.265242003E+0, 4.146005201E+0, 1.939987342E+0, -2.488106871E+2}
, {5.2315E+2, 1.E+2, 8.766596304E+2, 9.990587289E+2, 1.113128086E+3, 2.627674113E+0, 3.115848843E+0, 4.249854061E+0, 1.969014174E+0, -3.756089831E+2}
, {5.7315E+2, 1.E+2, 8.231712403E+2, 1.207646962E+3, 1.329128368E+3, 3.021897882E+0, 2.989179757E+0, 4.398661362E+0, 1.999506331E+0, -5.243538094E+2}
, {6.2315E+2, 1.E+2, 7.623392982E+2, 1.422807219E+3, 1.553982406E+3, 3.39791169E+0, 2.882467043E+0, 4.606987675E+0, 2.031074092E+0, -6.946014508E+2}
, {6.7315E+2, 1.00E+2, 6.929323479E+2, 1.646833758E+3, 1.79114799E+3, 3.763862515E+0, 2.794240764E+0, 4.894179762E+0, 2.06354145E+0, -8.868102945E+2}
, {7.2315E+2, 1.00E+2, 6.141579063E+2, 1.881911419E+3, 2.044735994E+3, 4.127090222E+0, 2.721899763E+0, 5.257678333E+0, 2.096820368E+0, -1.102593875E+3}
, {7.7315E+2, 1.00E+2, 5.282753857E+2, 2.126902829E+3, 2.316198038E+3, 4.489951453E+0, 2.655731575E+0, 5.568760706E+0, 2.130832398E+0, -1.344503137E+3}
, {8.2315E+2, 1.00E+2, 4.445529254E+2, 2.370975956E+3, 2.595921051E+3, 4.840535578E+0, 2.579333742E+0, 5.551646507E+0, 2.165471942E+0, -1.613510905E+3}
, {8.7315E+2, 1.00E+2, 3.742083364E+2, 2.597857316E+3, 2.865088134E+3, 5.158097698E+0, 2.488254086E+0, 5.168197415E+0, 2.20059735E+0, -1.905935689E+3}
, {9.2315E+2, 1.00E+2, 3.210161055E+2, 2.798941134E+3, 3.110451984E+3, 5.431492476E+0, 2.398456988E+0, 4.644743601E+0, 2.236037125E+0, -2.215141145E+3}
, {9.7315E+2, 1.00E+2, 2.820395674E+2, 2.976091183E+3, 3.330651364E+3, 5.663895105E+0, 2.325486994E+0, 4.181032807E+0, 2.27160221E+0, -2.535728338E+3}
, {1.02315E+3, 1.00E+2, 2.529966112E+2, 3.13520167E+3, 3.530463881E+3, 5.864193269E+0, 2.27341142E+0, 3.829832992E+0, 2.307098991E+0, -2.864747673E+3}
, {1.07315E+3, 1.E+2, 2.306404366E+2, 3.281691849E+3, 3.715267165E+3, 6.04058851E+0, 2.239640928E+0, 3.576379581E+0, 2.342340346E+0, -3.20076571E+3}
};

const unsigned ntd1 = sizeof(td1)/sizeof(TestData);

#endif

