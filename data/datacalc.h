/*
 * dataCalc.h
 * (c) 1993 by Helmut Gaberschek & Kaweh Kazemi, ...and avoid panic by
 * All rights reserved.
 *
 * 
 *
 * Rev   Date        Comment
 * 1     08-09-93    defines for cars
 * 2     09-09-93    defines for buidlings, player
 */
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

/*
 * set : changes absolutely 
 */

/* defines for random numbers */

#ifndef MODULE_DATACALC
#define MODULE_DATACALC

#include "theclou.h"

#ifndef __STDARG_H
#include <stdarg.h>
#endif

#ifndef MODULE_LIST
#include "list/list.h"
#endif

#ifndef MODULE_RANDOM
#include "random/random.h"
#endif

#define   tcDerivation(min,max)          (CalcRandomNr(min,max))

S32 Round(S32 v, S32 p);
S32 CalcValue(S32 value, S32 min, S32 max, S32 fact, S32 perc);
S32 ChangeAbs(S32 item, S32 value, S32 min, S32 max);

#endif
