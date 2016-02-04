/*****************************************************************************/
/*                                                                           */
/*                              SOFTING GmbH                                 */
/*                           Dingolfinger Str. 2                             */
/*                            D-81673 Muenchen                               */
/*                       Phone: (++49)-(0)89-413004-0                        */
/*                       Fax:   (++49)-(0)89-491018                          */
/*                                                                           */
/*                   Copyright (C) SOFTING GmbH 1994,1995,1996               */
/*                            All Rights Reserved                            */
/*                                                                           */
/*****************************************************************************/
/*****************************************************************************/
/*           PROFIBUS CONFIGURATION AND IMPLEMENTATION DEFINITIONS           */
/*                                                                           */
/*  Filename    :  PB_CONF.H                                                 */
/*  Version     :  5.00                                                      */
/*                 5.01                                                      */
/*  Date        :  10.10.95                                                  */
/*                 17.01.96                                                  */
/*  Author      :  SOFTING GmbH                                              */
/*                                                                           */
/*  Description : This file contains the PROFIBUS configuration and          */
/*                implementation definitions                                 */
/*                                                                           */
/*  CHANGE_NOTES                                                             */
/*                                                                           */
/*  date      name      change                                               */
/*  -----------------------------------------------------------------------  */
/*  15.01.96  Boe       remove IRQ-line to host H1_INT_3                     */
/*                      modify IRQ-line values H1_INT_5 .. H1_INT_15         */
/*  15.01.96  Boe       remove following defines:                            */
/*                      - H_TO_C_IRQ_VAL_OFFSET                              */
/*                      - C_TO_H_IRQ_VAL_OFFSET                              */
/*                      - REQ_IRQ_VALUE                                      */
/*                      - ACK_IRQ_VALUE                                      */
/*                                                                           */
/*****************************************************************************/
/************************ PVCS SECTION ****************************************
$Workfile:   pb_conf.h  $
$Author: AGromann $
$Archive:   P:\profi.vcs\pbv50\papi\inc_glb\cp5412a2\pb_conf.h_v  $
$Revision: 1.1 $
$Date: 2014/11/05 10:52:34 $
$Modtime:   12 Oct 1995 14:41:00  $
***********************  END PVCS SECTION ************************************/

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

#ifndef __PB_CONF__
#define __PB_CONF__

/*****************************************************************************/
/*              COMPILER SWITCHES                                            */
/*****************************************************************************/
/*
typedef     unsigned char        PB_BOOL;
PB_BOOL t;
*/
#undef DOS_APP
#define CALL_CONV              /* default function call convention        */
#ifndef NULL
   #define NULL (VOID*) 0
#endif


/*****************************************************************************/
/*              HW  Implementation Constants                                 */
/*****************************************************************************/
/* --- DPR access mode ---------------------------------------------------- */
#define USING_8BIT                      1
#define USING_16BIT_LA                  2
#define USING_16BIT_SA                  3
#define DPR_ACCESS_MODE                 USING_8BIT

/* --- DPR page size ------------------------------------------------------ */
#define USING_64K_PAGE                  0
#define USING_16K_PAGES                 1
#define PB_PAGE_SIZE                    USING_16K_PAGES 

/* --- IRQ line to host --------------------------------------------------- */
#define NO_INT_TO_HOST                  0x00
#define H1_INT_5                        0x10
#define H1_INT_10                       0x20
#define H1_INT_11                       0x30
#define H1_INT_12                       0x40
#define H1_INT_15                       0x50
#define INT_TO_HOST                     NO_INT_TO_HOST



/*****************************************************************************/
/*              CMI Implementation Constants                                 */
/*****************************************************************************/

/* --- timeout to wait for controller response ----------------------------- */
#define CMI_TIMEOUT                  03   /* timeout after 120 seconds       */

/* --- host identifiers ---------------------------------------------------- */
#define CMI_V300                     0xA0 /* host driver CMI version 3.0     */
#define HOST_TYPE                    CMI_V300

/* --- controller identifier ----------------------------------------------- */
#define TYPE_EV331                   0xCA /* EV331      controller board     */

#define CONTROLLER_TYPE              TYPE_EV331    

/* --- polling or interrupt mode --------------------------------------------*/
#define REQ_ACK_BY_POLLING             0  /* request and ackn. by polling    */
#define REQ_BY_IR_ACK_BY_POLLING       1  /* request by IR, ackn. by polling */
#define REQ_BY_POLLING_ACK_BY_IR       2  /* request by polling, ackn. by IR */
#define REQ_ACK_BY_IR                  3  /* request and acknowledge by IR   */

#define POLL_INT_MODE                  REQ_ACK_BY_POLLING 

/* --- address mode -------------------------------------------------------- */
#define ABS_32BIT_MOTOROLA             0  /* linear absolute 32 bit Motorola */
#define ABS_16BIT_MOTOROLA             1  /* linear absolute 16 bit Motorola */
#define REL_32BIT_MOTOROLA             2  /* linear relative 32 bit Motorola */
#define REL_16BIT_MOTOROLA             3  /* linear relative 16 bit Motorola */

#define ABS_32BIT_INTEL                8  /*  linear absolute 32 bit Intel   */
#define ABS_16BIT_INTEL                9  /*  linear absolute 16 bit Intel   */
#define REL_32BIT_INTEL               10  /*  linear relative 32 bit Intel   */
#define REL_16BIT_INTEL               11  /*  linear relative 32 bit Intel   */
#define SEGMENT_OFFSET_INTEL          12  /*  SEGEMENT/OFFSET Intel          */

#define ADDRESS_SWAP_MODE       ABS_32BIT_MOTOROLA


/*****************************************************************************/
/*              FMS, FM7 Implementation Constants                            */
/*                                                                           */
/* The constants given below define the sizes of various data structures in  */
/* the protocol software and thus influence memory consumption.              */
/*                                                                           */
/* NOTE: Do not change the following constants without recompiling the       */
/*       the protocol software on the communication controller               */
/*****************************************************************************/

/* -- constants of internal sizes of byte arrays --------------------------- */

#define VFD_STRING_LENGTH             32  /* max length of the VFD string   */
#define IDENT_STRING_LENGTH           32  /* max length of the Ident string */

#define ACCESS_NAME_LENGTH            32  /* max length for name adressing  */
#define OBJECT_NAME_LENGTH            32  /* max length of object name      */
#define EXTENSION_LENGTH              32  /* max length of object extension */
#define EXECUTION_ARGUMENT_LENGTH     32  /* max length of exec. argument   */
#define ERROR_DESCR_LENGTH            32  /* max length of error descript.  */
#define CRL_SYMBOL_LENGTH             32  /* max length of crl symbol name  */
#define CRL_EXTENSION_LENGTH           2  /* max length of crl extension    */

#if (PB_VER < 500)
#define KBL_SYMBOL_LENGTH             CRL_SYMBOL_LENGTH
#define KBL_EXTENSION_LENGTH          CRL_EXTENSION_LENGTH
#endif

#define MAX_FMS_PDU_LENGTH      241   /* max size of the FMS/FM7-PDU-Buffer  */
#define MAX_VAR_LIST_ELEMENTS    50   /* max count of variable list elements */
#define MAX_DOM_LIST_ELEMENTS    10   /* max count of domain list elements   */
#define MAX_VAR_RECORD_ELEMENTS  10   /* max count of record elements        */

#define MAX_COMREF           128   /* max supported communication references */
#define MAX_VFD                5   /* max supported VFDs                     */

#if (PB_VER < 500)
#define MAX_KBL_LEN            MAX_COMREF  /* max entries in CRL             */
#define MAX_PARA_LOC_SERVICES          5   /* max parallel local FMS-Services*/
#endif

/*****************************************************************************/
/* USEFUL MACROS                                                             */
/*****************************************************************************/

/*****************************************************************************/
/* MACRO TO CALCULATE MAX_xxxx_NAME_LENGTH                                   */
/*                                                                           */
/* This macro calculates the internal sizes of byte arrays in a way that the */
/* desired alignment on byte, word or long word boundaries is achieved.      */
/* The alignment is specified by the constant ALIGNMENT (e. g. longword = 4) */
/*                                                                           */
/*****************************************************************************/

#define ALIGNMENT                  0x02   /* alignment on word boundary      */

#define _NAME_LENGTH(length) (length + ALIGNMENT - (length % ALIGNMENT))

#endif
