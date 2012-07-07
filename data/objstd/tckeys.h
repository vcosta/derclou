/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa
  Portions copyright (c) 2005 Jens Granseuer

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef MODULE_TCKEYS
#define MODULE_TCKEYS

/*
 * Solve database key conflicts.
 */
 
typedef enum {
    _Environment_TheClou,
    _Player_Player_1,
    _London_London_1,
    _London_Jail,
    _London_Escape,
    _Evidence_Evidence_1,
    _CompleteLoot_LastLoot,
    _Person_Old_Matt,

    _Location_Holland_Street,
    _Location_Fat_Mans_Pub,
    _Location_Cars_Vans_Office,
    _Location_Tools_Shop,
    _Location_Policestation,
    _Location_Highgate_Out,
    _Location_Hotel,
    _Location_Walrus,
    _Location_Parker,
    _Location_Maloya,
    _Location_Pooly,
    _Location_Nirvana,

    _Ability_Autos,
    _Ability_Schloesser,
    _Ability_Sprengstoff,
    _Ability_Safes,
    _Ability_Elektronik,
    _Ability_Aufpassen,
    _Ability_Kampf
} KeyConflictE;

/* implemented in data/database.c */
U32 getKey(KeyConflictE key);

#define _K(x)                               getKey(_ ## x)


/*
// std
#define Environment_TheClou                 UINT32_C(  20)
#define Player_Player_1                     UINT32_C(  21)
#define London_London_1                     UINT32_C(  22)
#define London_Jail                         UINT32_C(  23)
#define London_Escape                       UINT32_C(  24)
#define Evidence_Evidence_1                 UINT32_C(  25)
#define CompleteLoot_LastLoot               UINT32_C(  27)
#define Person_Old_Matt                     UINT32_C(  28)

#define Location_Holland_Street             UINT32_C(  77)
#define Location_Fat_Mans_Pub               UINT32_C(  81)
#define Location_Cars_Vans_Office           UINT32_C(  82)
#define Location_Tools_Shop                 UINT32_C(  83)
#define Location_Policestation              UINT32_C(  84)
#define Location_Highgate_Out               UINT32_C(  98)
#define Location_Hotel                      UINT32_C( 124)
#define Location_Walrus                     UINT32_C( 125)
#define Location_Parker                     UINT32_C( 128)
#define Location_Maloya                     UINT32_C( 129)
#define Location_Pooly                      UINT32_C( 130)
#define Location_Nirvana                    UINT32_C( 133)

#define Ability_Autos                       UINT32_C( 140)
#define Ability_Schloesser                  UINT32_C( 141)
#define Ability_Sprengstoff                 UINT32_C( 142)
#define Ability_Safes                       UINT32_C( 143)
#define Ability_Elektronik                  UINT32_C( 144)
#define Ability_Aufpassen                   UINT32_C( 145)
#define Ability_Kampf                       UINT32_C( 146)


// profidisk
#define Environment_TheClou                 UINT32_C(  28)
#define Player_Player_1                     UINT32_C(  29)
#define London_London_1                     UINT32_C(  30)
#define London_Jail                         UINT32_C(  31)
#define London_Escape                       UINT32_C(  32)
#define Evidence_Evidence_1                 UINT32_C(  33)
#define CompleteLoot_LastLoot               UINT32_C(  35)
#define Person_Old_Matt                     UINT32_C(  36)

#define Location_Holland_Street             UINT32_C(  95)
#define Location_Fat_Mans_Pub               UINT32_C(  99)
#define Location_Cars_Vans_Office           UINT32_C( 100)
#define Location_Tools_Shop                 UINT32_C( 101)
#define Location_Policestation              UINT32_C( 102)
#define Location_Highgate_Out               UINT32_C( 116)
#define Location_Hotel                      UINT32_C( 142)
#define Location_Walrus                     UINT32_C( 143)
#define Location_Parker                     UINT32_C( 146)
#define Location_Maloya                     UINT32_C( 147)
#define Location_Pooly                      UINT32_C( 148)
#define Location_Nirvana                    UINT32_C( 151)

#define Ability_Autos                       UINT32_C( 173)
#define Ability_Schloesser                  UINT32_C( 174)
#define Ability_Sprengstoff                 UINT32_C( 175)
#define Ability_Safes                       UINT32_C( 176)
#define Ability_Elektronik                  UINT32_C( 177)
#define Ability_Aufpassen                   UINT32_C( 178)
#define Ability_Kampf                       UINT32_C( 179)
*/

#define Environment_TheClou                 _K(Environment_TheClou)
#define Player_Player_1                     _K(Player_Player_1)
#define London_London_1                     _K(London_London_1)
#define London_Jail                         _K(London_Jail)
#define London_Escape                       _K(London_Escape)
#define Evidence_Evidence_1                 _K(Evidence_Evidence_1)
#define CompleteLoot_LastLoot               _K(CompleteLoot_LastLoot)
#define Person_Old_Matt                     _K(Person_Old_Matt)

#define Location_Holland_Street             _K(Location_Holland_Street)
#define Location_Fat_Mans_Pub               _K(Location_Fat_Mans_Pub)
#define Location_Cars_Vans_Office           _K(Location_Cars_Vans_Office)
#define Location_Tools_Shop                 _K(Location_Tools_Shop)
#define Location_Policestation              _K(Location_Policestation)
#define Location_Highgate_Out               _K(Location_Highgate_Out)
#define Location_Hotel                      _K(Location_Hotel)
#define Location_Walrus                     _K(Location_Walrus)
#define Location_Parker                     _K(Location_Parker)
#define Location_Maloya                     _K(Location_Maloya)
#define Location_Pooly                      _K(Location_Pooly)
#define Location_Nirvana                    _K(Location_Nirvana)

#define Ability_Autos                       _K(Ability_Autos)
#define Ability_Schloesser                  _K(Ability_Schloesser)
#define Ability_Sprengstoff                 _K(Ability_Sprengstoff)
#define Ability_Safes                       _K(Ability_Safes)
#define Ability_Elektronik                  _K(Ability_Elektronik)
#define Ability_Aufpassen                   _K(Ability_Aufpassen)
#define Ability_Kampf                       _K(Ability_Kampf)




/*
 * Data header file created with dc
 * (c) 1993 by ...and avoid panic by
 *
 * Database (offset, size) : tcMain (0, 10000)
 */

/* std */
#define Car_Jeep_1940                       UINT32_C(   1)
#define Car_Jeep_1945                       UINT32_C(   2)
#define Car_Triumph_Roadstar_1947           UINT32_C(   3)
#define Car_Triumph_Roadstar_1949           UINT32_C(   4)
#define Car_Cadillac_Club_1952              UINT32_C(   5)
#define Car_Opel_Olympia_1953               UINT32_C(   6)
#define Car_Standard_Vanguard_1950          UINT32_C(   7)
#define Car_Standard_Vanguard_1951          UINT32_C(   8)
#define Car_Standard_Vanguard_1953          UINT32_C(   9)
#define Car_Jaguar_XK_1950                  UINT32_C(  10)
#define Car_Pontiac_Streamliner_1944        UINT32_C(  11)
#define Car_Pontiac_Streamliner_1946        UINT32_C(  12)
#define Car_Pontiac_Streamliner_1949        UINT32_C(  13)
#define Car_Fiat_Topolino_1936              UINT32_C(  14)
#define Car_Fiat_Topolino_1940              UINT32_C(  15)
#define Car_Fiat_Topolino_1942              UINT32_C(  16)
#define Car_Morris_Minor_1948               UINT32_C(  17)
#define Car_Morris_Minor_1950               UINT32_C(  18)
#define Car_Morris_Minor_1953               UINT32_C(  19)

/* profidisk extra */
#define Car_Rover_75_1949                   UINT32_C(  20)
#define Car_Rover_75_1950                   UINT32_C(  21)
#define Car_Rover_75_1952                   UINT32_C(  22)
#define Car_Ford_Model_T__1926              UINT32_C(  23)
#define Car_Bentley_Continental_Typ_R_1952  UINT32_C(  24)
#define Car_Bentley_Continental_Typ_R_1953  UINT32_C(  25)
#define Car_Fiat_634_N_1936                 UINT32_C(  26)
#define Car_Fiat_634_N_1943                 UINT32_C(  27)


/* profidisk extra */
#define Item_Lokomotive_vorne_rechts        UINT32_C( 290)
#define Item_Lokomotive_vorne_links         UINT32_C( 291)
#define Item_Lokomotive_oben                UINT32_C( 292)
#define Item_Lokomotive_seitlich            UINT32_C( 293)
#define Item_Lokomotive_Kabine              UINT32_C( 294)
#define Item_Lokomotive_Tuer                UINT32_C( 295)


/* std */
#define Item_Holztuer                       UINT32_C(9201)
#define Item_Stahltuer                      UINT32_C(9202)
#define Item_Gemaelde                       UINT32_C(9206)
#define Item_Tresorraum                     UINT32_C(9207)
#define Item_Mauer                          UINT32_C(9208)
#define Item_Mauerecke                      UINT32_C(9209)
#define Item_Alarmanlage_Z3                 UINT32_C(9210)
#define Item_Alarmanlage_X3                 UINT32_C(9211)
#define Item_Alarmanlage_Top                UINT32_C(9212)
#define Item_Steuerkasten                   UINT32_C(9213)
#define Item_WC                             UINT32_C(9214)
#define Item_Tresen                         UINT32_C(9223)
#define Item_Kuehlschrank                   UINT32_C(9224)
#define Item_Statue                         UINT32_C(9233)
#define Item_Fenster                        UINT32_C(9234)
#define Item_Kreuz                          UINT32_C(9236)
#define Item_Kranz                          UINT32_C(9237)
#define Item_Treppe                         UINT32_C(9238)
#define Item_Mauertor                       UINT32_C(9250)
#define Item_Steinmauer                     UINT32_C(9253)
#define Item_Nachtkaestchen                 UINT32_C(9260)
#define Item_Sockel                         UINT32_C(9261)
#define Item_Kasse                          UINT32_C(9262)
#define Item_Mikrophon                      UINT32_C(9264)
#define Item_Bild                           UINT32_C(9265)
#define Item_Stechuhr                       UINT32_C(9266)
#define Item_Vase                           UINT32_C(9268)
#define Item_Beutesack                      UINT32_C(9270)
#define Item_Wache                          UINT32_C(9272)

/* profidisk extra */
#define Item_Beichtstuhl                    UINT32_C(9273)
#define Item_Heiligenstatue                 UINT32_C(9281)
#define Item_Kerzenstaender                 UINT32_C(9282)
#define Item_Kirchenkreuz                   UINT32_C(9283)
#define Item_Gelaender                      UINT32_C(9284)
#define Item_verzierte_Saeule               UINT32_C(9285)
#define Item_Absperrung                     UINT32_C(9292)
#define Item_Hottentotten_Figur             UINT32_C(9293)
#define Item_Batman_Figur                   UINT32_C(9294)
#define Item_Dicker_Man                     UINT32_C(9295)
#define Item_Unbekannter                    UINT32_C(9296)
#define Item_Jack_the_Ripper_Figur          UINT32_C(9297)
#define Item_Koenigs_Figur                  UINT32_C(9298)
#define Item_Wache_Figur                    UINT32_C(9299)
#define Item_Miss_World_1952                UINT32_C(9300)
#define Item_Postsack                       UINT32_C(9306)
#define Item_Leiter                         UINT32_C(9307)


/* std */
#define Tool_Hand                           UINT32_C(9501)
#define Tool_Dietrich                       UINT32_C(9502)
#define Tool_Hammer                         UINT32_C(9503)
#define Tool_Axt                            UINT32_C(9504)
#define Tool_Kernbohrer                     UINT32_C(9505)
#define Tool_Handschuhe                     UINT32_C(9506)
#define Tool_Schuhe                         UINT32_C(9507)
#define Tool_Maske                          UINT32_C(9508)
#define Tool_Chloroform                     UINT32_C(9509)
#define Tool_Brecheisen                     UINT32_C(9510)
#define Tool_Winkelschleifer                UINT32_C(9511)
#define Tool_Bohrmaschine                   UINT32_C(9512)
#define Tool_Bohrwinde                      UINT32_C(9513)
#define Tool_Schloszstecher                 UINT32_C(9514)
#define Tool_Schneidbrenner                 UINT32_C(9515)
#define Tool_Sauerstofflanze                UINT32_C(9516)
#define Tool_Stethoskop                     UINT32_C(9517)
#define Tool_Batterie                       UINT32_C(9518)
#define Tool_Stromgenerator                 UINT32_C(9519)
#define Tool_Elektroset                     UINT32_C(9520)
#define Tool_Schutzanzug                    UINT32_C(9521)
#define Tool_Dynamit                        UINT32_C(9522)
#define Tool_Elektrohammer                  UINT32_C(9523)
#define Tool_Glasschneider                  UINT32_C(9524)
#define Tool_Fusz                           UINT32_C(9525)
#define Tool_Strickleiter                   UINT32_C(9526)
#define Tool_Funkgeraet                     UINT32_C(9527)
#define Tool_Stechkarte                     UINT32_C(9528)


/* std */
#define Loot_Gebeine                        UINT32_C(9618)
#define Loot_Kronjuwelen                    UINT32_C(9633)
#define Loot_Ring_des_Abtes                 UINT32_C(9634)
#define Loot_Koffer                         UINT32_C(9635)
#define Loot_Dokument                       UINT32_C(9636)


/* std */
#define Person_Matt_Stuvysunt               UINT32_C(9801)
#define Person_Paul_O_Conner                UINT32_C(9802)
#define Person_Ken_Addison                  UINT32_C(9803)
#define Person_Frank_Meier                  UINT32_C(9804)
#define Person_Herbert_Briggs               UINT32_C(9805)
#define Person_Tony_Allen                   UINT32_C(9806)
#define Person_Margret_Allen                UINT32_C(9807)
#define Person_Mathew_Black                 UINT32_C(9808)
#define Person_Miguel_Garcia                UINT32_C(9809)
#define Person_Mike_Seeger                  UINT32_C(9810)
#define Person_Albert_Liet                  UINT32_C(9811)
#define Person_Mohammed_Abdula              UINT32_C(9812)
#define Person_Serge_Fontane                UINT32_C(9813)
#define Person_Frank_Maloya                 UINT32_C(9814)
#define Person_Neil_Grey                    UINT32_C(9815)
#define Person_Mark_Hart                    UINT32_C(9816)
#define Person_Jim_Danner                   UINT32_C(9817)
#define Person_Robert_Bull                  UINT32_C(9818)
#define Person_Thomas_Groul                 UINT32_C(9819)
#define Person_Helen_Parker                 UINT32_C(9820)
#define Person_Dan_Stanford                 UINT32_C(9821)
#define Person_John_Gludo                   UINT32_C(9822)
#define Person_Miles_Chickenwing            UINT32_C(9823)
#define Person_Mary_Bolton                  UINT32_C(9824)
#define Person_Eric_Pooly                   UINT32_C(9825)
#define Person_Prof_Emil_Schmitt            UINT32_C(9826)
#define Person_Elisabeth_Schmitt            UINT32_C(9827)
#define Person_Margrete_Briggs              UINT32_C(9828)
#define Person_Peter_Brook                  UINT32_C(9829)
#define Person_Samuel_Rosenblatt            UINT32_C(9830)
#define Person_Lucas_Grull                  UINT32_C(9831)
#define Person_Marc_Smith                   UINT32_C(9832)
#define Person_Jiri_Poulin                  UINT32_C(9833)
#define Person_Garry_Stevenson              UINT32_C(9834)
#define Person_Al_Mel                       UINT32_C(9835)
#define Person_Jack_Motart                  UINT32_C(9836)
#define Person_Kevin_Smith                  UINT32_C(9837)
#define Person_Mike_Kahn                    UINT32_C(9838)
#define Person_Frank_De_Silva               UINT32_C(9839)
#define Person_Justin_White                 UINT32_C(9840)
#define Person_John_O_Keef                  UINT32_C(9841)
#define Person_Luthmilla_Nervesaw           UINT32_C(9842)
#define Person_Thomas_Smith                 UINT32_C(9843)
#define Person_Ben_Riggley                  UINT32_C(9844)
#define Person_Richard_Doil                 UINT32_C(9845)
#define Person_Sabien_Pardo                 UINT32_C(9846)
#define Person_Red_Stanson                  UINT32_C(9847)
#define Person_Pater_James                  UINT32_C(9848)

/* profidisk extra */
#define Person_Tom_Cooler                   UINT32_C(9849)
#define Person_Sid_Palmer                   UINT32_C(9850)
#define Person_Dean_Esters                  UINT32_C(9851)
#define Person_Melanie_Morgan               UINT32_C(9853)
#define Person_Prof_Marcus_Green            UINT32_C(9852)
#define Person_Rosi_Lambert                 UINT32_C(9854)
#define Person_Rod_Masterson                UINT32_C(9855)
#define Person_Tina_Olavson                 UINT32_C(9856)
#define Person_Phil_Ciggy                   UINT32_C(9857)
#define Person_Pere_Ubu                     UINT32_C(9858)




/*
 * Data header file created with dc
 * (c) 1993 by ...and avoid panic by
 *
 * Database (offset, size) : tcBuild (500000, 10000)
 */

/* std */
#define Building_Kiosk                      UINT32_C(509000)
#define Building_Tante_Emma_Laden           UINT32_C(509001)
#define Building_Juwelier                   UINT32_C(509002)
#define Building_Old_Curiosity_Shop         UINT32_C(509003)
#define Building_Pink_Villa                 UINT32_C(509004)
#define Building_Seniorenheim               UINT32_C(509005)
#define Building_Grab_von_Karl_Marx         UINT32_C(509006)
#define Building_Villa                      UINT32_C(509007)
#define Building_Kenwood_House              UINT32_C(509008)
#define Building_Ham_House                  UINT32_C(509009)
#define Building_Osterly_Park_House         UINT32_C(509010)
#define Building_Chiswick_House             UINT32_C(509011)
#define Building_Suterbys                   UINT32_C(509012)
#define Building_British_Museum             UINT32_C(509013)
#define Building_Natural_Museum             UINT32_C(509014)
#define Building_National_Galery            UINT32_C(509015)
#define Building_Victoria_und_Albert_Museum UINT32_C(509016)
#define Building_Bank_of_England            UINT32_C(509017)
#define Building_Tower_of_London            UINT32_C(509018)
#define Building_Starford_Kaserne           UINT32_C(509019)

/* profidisk extra */
#define Building_Baker_Street               UINT32_C(509101)
#define Building_Madame_Tussaud             UINT32_C(509102)
#define Building_Westminster_Abbey          UINT32_C(509103)
#define Building_Downing_Street_10          UINT32_C(509104)
#define Building_Postzug                    UINT32_C(509105)
#define Building_Tate_Gallery               UINT32_C(509106)
#define Building_Buckingham_Palace          UINT32_C(509107)
#define Building_Bulstrode_Chemestry_Ltd    UINT32_C(509108)

#endif
