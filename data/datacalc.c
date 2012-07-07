/*
 * dataCalc.c
 * (c) 1993 by Helmut Gaberschek & Kaweh Kazemi, ...and avoid panic by
 * All rights reserved.
 *
 * 
 *
 * Rev   Date        Comment
 * 1     14-09-93    defines for cars
 */
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#include "data/datacalc.h"

S32 Round(S32 v, S32 p)
{				/* p...Stellen ! */
    register S32 i, z;

    for (i = 0, z = 1; i < p; i++, z *= 10);

    if (v / z != 0)
	v = (v / z) * z;

    return (v);
}

S32 CalcValue(S32 value, S32 min, S32 max, S32 fact, S32 perc)
{
    perc = ((perc * fact) - (perc * 128)) / 127;

    value = value + (value * perc) / 100;

    return clamp(value, min, max);
}

S32 ChangeAbs(S32 item, S32 value, S32 min, S32 max)
{
    item += value;

    return clamp(item, min, max);
}
