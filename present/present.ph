/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/
/*
 *
 * Present.ph
 *
 */


#define BAR_WIDTH         	 255

struct PresentControl {
    GC *gc;

    uword us_BarWidth;
    uword us_BarHeight;

    ubyte uch_FCol;
    ubyte uch_BCol;
    ubyte uch_TCol;
};

struct presentationInfo {
    NODE link;

    char extendedText[70];
    U32 extendedNr;
    U32 maxNr;

    ubyte presentHow;
};
