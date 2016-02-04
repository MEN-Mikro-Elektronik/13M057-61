/*****************************************************************************/
/*                                                                           */
/*                              SOFTING GmbH                                 */
/*                           Dingolfinger Str. 2                             */
/*                            D-81673 Muenchen                               */
/*                       Phone: (++49)-(0)89-413004-0                        */
/*                       Fax:   (++49)-(0)89-491018                          */
/*                                                                           */
/*                    Copyright (C) SOFTING GmbH 1994,1995,1996              */
/*                            All Rights Reserved                            */
/*                                                                           */
/*****************************************************************************/
/*****************************************************************************/
/*                          PROFIBUS BASIC TYPES                             */
/*                                                                           */
/*  Filename    :   pb_type.h                                                */
/*  Version     :   5.00                                                     */
/*                  5.01                                                     */
/*  Date        :   10.10.95                                                 */
/*                  17.01.96                                                 */
/*  Author      :   SOFTING GmbH                                             */
/*                                                                           */
/*  Description : This file contains the PROFIBUS basic types                */
/*                                                                           */
/*  CHANGE_NOTES                                                             */
/*                                                                           */
/*  date      name      change                                               */
/*  -----------------------------------------------------------------------  */
/*                                                                           */
/*****************************************************************************/
/************************ PVCS SECTION ****************************************
$Workfile:   pb_type.h  $
$Author: AGromann $
$Archive:   P:\profi.vcs\pbv50\papi\inc_glb\cp5412a2\pb_type.h_v  $
$Revision: 1.1 $
$Date: 2014/11/05 10:52:42 $
$Modtime:   12 Oct 1995 14:41:00  $
***********************  END PVCS SECTION ************************************/

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

#ifndef __PB_TYPE__
#define __PB_TYPE__

#include <MEN/men_typs.h>

#ifndef FAR
# define FAR
#endif

#if 0  /* MSDOS */						/* set by MEN/typdefs.h kp@men */
# define FAR far
#endif

#if 0/* OS9 */
# define FAR
#endif

/*
#define FAR
*/
#ifndef NULL
   #ifdef __cplusplus
       #define NULL    0
   #else 
       #define NULL ((void *)0)
   #endif 
#endif 

#ifndef PB_VER
/* --- for new applications running with PROFIBUS V500 --------------------- */
/* --- NOTE: For applications develped with PROFIBUS V4XX invoke compiler    */
/*          with compiler switch "/DPB_VER=400"                              */
#define PB_VER  500
#endif


/* --- global type definitions --------------------------------------------- */
#ifndef VOID
#define     VOID                 void 
#endif

typedef     unsigned char        PB_BOOL;
#if 0   /* (PB_VER < 500) */
#undef      BOOL
#define     BOOL                 PB_BOOL
#endif

/* changed by ag */
typedef    /* unsigned char */ u_int8      OCTET;
typedef    /* unsigned char */ u_int8      USIGN8;
typedef    /* unsigned short*/ u_int16     USIGN16;
typedef    /* unsigned long */ u_int32     USIGN32; 

#if !defined(_ANSI_C) && !defined(_ANSI_EXT) /* kp@men */
#define signed
#endif

/* changed by ag */
typedef     signed char          PB_INT8;  /* INT8 */
typedef     signed short         PB_INT16; /* INT16 */
typedef     signed long          PB_INT32; /* INT32 */

typedef     char                 STRINGV;
typedef     char                 PB_CSTRING;

typedef     float                FLOAT;  

typedef     double               DOUBLE;  

/* --- PROFIBUS true and false definitions --------------------------------- */
#define     PB_TRUE              0xFF
#define     PB_FALSE             0x00

#if (PB_VER < 500)
#undef      TRUE
#undef      FALSE
#define     TRUE                 PB_TRUE
#define     FALSE                PB_FALSE
#endif

/* --- PROFIBUS function declarations -------------------------------------- */
#define     LOCAL                static              

#ifndef __KEYWORDS__			/* kp@men */
#define     PUBLIC              
#define     GLOBAL              
#endif


#if (PB_VER < 500)
#include "pb_conf.h"
#endif


#endif /* __PB_TYPE__ */

