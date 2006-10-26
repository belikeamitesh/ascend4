/*
 *  Boundary Manipulator Module
 *  Created: 04/97
 *  Version: $Revision: 1.6 $
 *  Version control file: $RCSfile: bndman.c,v $
 *  Date last modified: $Date: 1997/07/18 12:13:57 $
 *  Last modified by: $Author: mthomas $
 *
 *  This file is part of the SLV solver.
 *
 *  The SLV solver is free software; you can redistribute
 *  it and/or modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  The SLV solver is distributed in hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the program; if not, write to the Free Software Foundation,
 *  Inc., 675 Mass Ave, Cambridge, MA 02139 USA.  Check the file named
 *  COPYING.  COPYING is found in ../compiler.
 *
 */

#include <math.h>
#include <utilities/ascConfig.h>
#include <general/list.h>
#include <compiler/instance_enum.h>
#include <compiler/fractions.h>
#include <compiler/compiler.h>
#include <utilities/ascMalloc.h>
#include <compiler/functype.h>
#include <compiler/safe.h>
#include <compiler/extfunc.h>
#include <compiler/dimen.h>
#include <compiler/expr_types.h>
#include <compiler/find.h>
#include <compiler/atomvalue.h>
#include <compiler/mathinst.h>
#include <compiler/relation_type.h>
#include <compiler/extfunc.h>
#include <compiler/rel_blackbox.h>
#include <compiler/vlist.h>
#include <compiler/relation.h>
#include <compiler/relation_util.h>
#include <compiler/relation_io.h>
#define _SLV_SERVER_C_SEEN_
#include "mtx.h"
#include "slv_types.h"
#include "var.h"
#include "rel.h"
#include "discrete.h"
#include "conditional.h"
#include "logrel.h"
#include "bnd.h"
#include "relman.h"
#include "logrelman.h"
#include "bndman.h"
#include "slv_server.h"


real64 bndman_real_eval(struct bnd_boundary *bnd)
{
  struct rel_relation *rel;
  real64 res;
  int32 status;

  if (bnd_kind(bnd)!=e_bnd_rel) {
    FPRINTF(stderr,"Incorrect bnd passed to bnd_real_eval.\n");
    return 0.0;
  }
  status = 0;
  rel = bnd_rel(bnd_real_cond(bnd));
  res = relman_eval(rel,&status,1);
  return res;
}


int32 bndman_log_eval(struct bnd_boundary *bnd)
{
  struct logrel_relation *lrel;
  int32 status,res;

  if (bnd_kind(bnd)!=e_bnd_logrel) {
    FPRINTF(stderr,"Incorrect bnd passed to bnd_log_eval.\n");
    return 0;
  }

  status = 0;
  lrel = bnd_logrel(bnd_log_cond(bnd));
  res = logrelman_eval(lrel,&status);
  return res;
}


int32 bndman_calc_satisfied(struct bnd_boundary *bnd)
{
  int32 logres;
  struct rel_relation *rel;
  real64 res,tol;
  boolean rstat;

  switch(bnd_kind(bnd)) {
    case e_bnd_rel:
      rel = bnd_rel(bnd_real_cond(bnd));
      res = bndman_real_eval(bnd); /* force to reset real residual */
      tol = bnd_tolerance(bnd);
      rstat = relman_calc_satisfied(rel,tol);
      if (rstat) {
        return 1;
      } else {
        return 0;
      }
    case e_bnd_logrel:
      logres = bndman_log_eval(bnd); /* force to reset boolean residual */
      return logres;
    default:
      FPRINTF(stderr,"Incorrect bnd passed to bnd_calc_satisfied.\n");
      return 0;
  }
}


int32 bndman_calc_at_zero(struct bnd_boundary *bnd)
{
  real64 tol,res;

  if (bnd_kind(bnd)!=e_bnd_rel) {
    FPRINTF(stderr,"Incorrect bnd passed to bnd_calc_at_zero.\n");
    return 0;
  }
  tol = bnd_tolerance(bnd);
  res = bndman_real_eval(bnd);

  if (fabs(tol) > fabs(res)) {
    return 1;
  } else {
   return 0;
  }
}



