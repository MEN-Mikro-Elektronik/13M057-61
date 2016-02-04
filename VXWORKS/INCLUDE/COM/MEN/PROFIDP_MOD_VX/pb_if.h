/*****************************************************************************/
/*                                                                           */
/*                              SOFTING GmbH                                 */
/*                           Dingolfinger Str. 2                             */
/*                            D-81673 Muenchen                               */
/*                       Phone: (++49)-(0)89-413004-0                        */
/*                       Fax:   (++49)-(0)89-491018                          */
/*                                                                           */
/*                  Copyright (C) SOFTING GmbH 1995,1996                     */
/*                            All Rights Reserved                            */
/*                                                                           */
/*****************************************************************************/
/*****************************************************************************/
/*                   PROFIBUS APPLICATION PROGRAM INTERFACE                  */
/*                                                                           */
/*  Filename    : PB_IF.H                                                    */
/*  Version     : 5.00                                                       */
/*                5.01                                                       */
/*  Date        : 10.10.95                                                   */
/*                17.01.96                                                   */
/*  Author      : SOFTING GmbH                                               */
/*                                                                           */
/*  Description : This file contains the global function declarations,       */
/*                defines and types of the PROFIBUS Communication Interface  */
/*                                                                           */
/*  CHANGE_NOTES                                                             */
/*                                                                           */
/*  date      name      change                                               */
/*  -----------------------------------------------------------------------  */
/*  13.12.95  Boe       data structure T_PROFI_INIT                          */
/*                      - add member TOKEN_DRIVE                             */
/*                      - remove member RESERVED_4                           */
/*  16.01.96  Boe       - add macro to set T_PROFI_INIT data structure       */
/*                        with default values                                */
/*                                                                           */
/*****************************************************************************/
/************************ PVCS SECTION ****************************************
$Workfile:   pb_if.h  $
$Author: AGromann $
$Archive:   P:\profi.vcs\pbv50\papi\inc_glb\cp5412a2\pb_if.h_v  $
$Revision: 1.1 $
$Date: 2014/11/05 10:52:40 $
$Modtime:   12 Oct 1995 14:41:00  $
***********************  END PVCS SECTION ************************************/

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

#ifndef __PB_IF__
#define __PB_IF__

/*****************************************************************************/
/* INTERFACE DEFINES                                                         */
/*****************************************************************************/

/* --- contants to receive an indication or confirmation ------------------- */
#define CON_IND_RECEIVED    PB_ERR(0x0001)  /* ind. or con. has been received        */
#define NO_CON_IND_RECEIVED PB_ERR(0x0000)  /* nothing has been received             */

/* --- values > CON_IND_RECEIVED are error returns (see pb_err.h) ---------- */

/* --- OK return value ----------------------------------------------------- */
#define E_OK                0        /* no errors                            */
/* --- values != E_OK are error returns (see pb_err.h) --------------------- */

/* --- service result ------------------------------------------------------ */
#define NEG                 0x01    /* result for negative response          */
#define POS                 0x00    /* result for positive response          */

/* --- service priority ---------------------------------------------------- */
#define LOW                 0x00    /* high priority                         */
#define HIGH                0x01    /* low priority                          */


/* --- service primitives -------------------------------------------------- */
#define REQ                 0x00   /* request primitive                      */
#define CON                 0x01   /* confirmation primitive                 */
#define IND                 0x02   /* indication primitive                   */
#define RES                 0x03   /* response primitive                     */


/* --- layer identifiers --------------------------------------------------- */
#define USR                 0x00      /* identifier USER                     */
#define FMS                 0x01      /* identifier FMS                      */
#define LLI                 0x02      /* identifier LLI                      */
#define FDL                 0x03      /* identifier FDL                      */
#define FM7                 0x04      /* identifier FM7                      */
#define FMB                 0x05      /* identifier FMB                      */
#define FMS_USR             0x06      /* identifier FMS-USER                 */
#define FM7_USR             0x07      /* identifier FM7-USER                 */
#define FMB_USR             0x08      /* identifier FMB-User                 */
#define SM7                 0x09      /* identifier SM7                      */
#define SM7_USR             0x0A      /* identifier SM7_USR                  */
#define DP                  0x0B      /* identifier DP                       */
#define DP_USR              0x0C      /* identifier DP-USER                  */
#define FDLIF               0x0D      /* identifier FDLIF                    */
#define FDLIF_USR           0x0E      /* identifier FDLIF-User               */

#define PAPI                0xF0      /* identifier Application-Layer-IF     */

#if (PB_VER < 500)
#define FMA7                FM7        /* identifier FMA7                    */
#define FMA2                FMB        /* identifier FMA2                   */
#define FMA7_USR            FM7_USR    /* identifier FMA7-USER              */
#endif


/* --- identifiers for data description interface -------------------------- */
/* --- 0x00 - 0x3F used by FMS -- (no identifiers defined) ----------------- */

/* --- 0x40 - 0x7F reserved by management -- (no identifiers defined) ------ */

/* --- 0x80 - 0x9F reserved by DP ------------------------------------------ */
#define ID_DP_SLAVE_IO_IMAGE         0x80 /* image for slave io datas        */
#define ID_DP_STATUS_IMAGE           0x81 /* image for status datas          */

/* --- 0xA0 - 0xEF reserved for future use -- (no identifiers defined) ----- */

/* --- 0xF0 - 0xFF reserved for diagonsis  --------------------------------- */
#define ID_EXCEPTION_IMAGE           0xF0 /* image for exeption datas        */
#define ID_FW_VERS_IMAGE             0xF1 /* image for firmware version      */

/* --- idebtifier for end of data description list ------------------------- */
#define EOF_DATA_DESCR_LIST          0x00 /* end of data description list    */


/*****************************************************************************/
/* PROFI INITIALIZATION DATA STRUCTURE                                       */
/*****************************************************************************/

typedef struct _T_PROFI_INIT
{
  USIGN32      dpr_address;          /* DPR address                        */
  PB_INT16        io_port_address;      /* IO port address                    */
  PB_BOOL      load_firmware;        /* PB_TRUE (download firmware)        */
  USIGN8       time_out;             /* time out interval in seconds       */
  PB_CSTRING      *loader_file;         /* loader file (LOAD_A2.BIN)          */
  PB_CSTRING      *firmware_file;       /* firmware file (PBFW_A2.LAD)        */
  USIGN8       host_type;            /* host type                          */
  USIGN8       irq_vector;           /* IRQ line to host                   */
  USIGN8       dpr_access_mode;      /* DPR access mode (8/16 Bit ...)     */
  USIGN8       address_mode;         /* host address mode                  */
  USIGN8       poll_int_mode;        /* host POLL / IRQ mode               */
  USIGN8       cntrl_type;           /* for internal use                   */
  USIGN8       reserved_0;           /* reserved                           */
  USIGN8       reserved_1;           /* reserved                           */
  USIGN16      reserved_2;           /* reserved                           */
  USIGN16      reserved_3;           /* reserved                           */
 } T_PROFI_INIT;

/* --- macro to set T_PROFI_INIT data structure with default values -------  */
#define _SET_PROFI_INIT_DEF_PARAM(init_param,dpr_address,io_port_address)    \
             init_param.dpr_address      = dpr_address;                      \
             init_param.load_firmware    = PB_FALSE;                         \
             init_param.io_port_address  = io_port_address;                  \
             init_param.irq_vector       = (USIGN8)   INT_TO_HOST;           \
             init_param.dpr_access_mode  = (USIGN8)   DPR_ACCESS_MODE;       \
             init_param.address_mode     = (USIGN8)   ADDRESS_SWAP_MODE;     \
             init_param.poll_int_mode    = (USIGN8)   POLL_INT_MODE;         \
             init_param.time_out         = (USIGN8)   CMI_TIMEOUT;           \
             init_param.loader_file      = (PB_CSTRING*) NULL;                  \
             init_param.firmware_file    = (PB_CSTRING*) NULL;                  \
             init_param.cntrl_type       = (USIGN8)   CONTROLLER_TYPE;       \
             init_param.host_type        = (USIGN8)   HOST_TYPE;
                    

/*****************************************************************************/
/* PROFI SERVICE DESCRIPTION BLOCK                                           */
/*****************************************************************************/

typedef struct _T_PROFI_SERVICE_DESCR
{
  USIGN16     comm_ref;                /* communication reference            */
  USIGN8      layer;                   /* layer                              */
  USIGN8      service;                 /* service identifier                 */
  USIGN8      primitive;               /* service primitive                  */
  PB_INT8        invoke_id;               /* invoke id                          */
  PB_INT16       result;                  /* service result (POS or NEG)        */
} T_PROFI_SERVICE_DESCR;


/*****************************************************************************/
/* global function declarations                                              */
/*****************************************************************************/
#ifdef __cplusplus
  extern "C" {
#endif 

#if 0 /* the following prototypes are locates in pci.h now */

/*kp@men*/
/*
#if MSDOS || (OS9 && defined(COMPILING_DRIVER))
*/

/*kp@men: I use my old profi_init function */
/*softing likes to change function names all the time :-( */
FUNCTION extern PB_INT16 CALL_CONV profi_init
        (
          IN  USIGN32  dpr_address_a08,
		  IN  USIGN32  dpr_address_a24
        )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
This function is used to initialize the
Host-Interface and Controller with default values.

INPUT:  h_dpr_address         -> host base address of Dual-Port-Ram

Possible return values:
- E_OK                             -> interface is initialized
- E_IF_NO_CNTRL_RES                -> controller does not respond
- E_IF_INVALID_CNTRL_TYPE_VERSION  -> invalid controller type or software version
- E_IF_INIT_INVALID_PARAMETER      -> invalid initialize parameter
-----------------------------------------------------------------------------*/
;


FUNCTION extern PB_INT16 CALL_CONV profi_end(VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
This function is used to exit the Host-Interface and exit the Controller.

Possible return values:
- E_OK 
-----------------------------------------------------------------------------*/
;


FUNCTION extern PB_INT16 CALL_CONV profi_snd_req_res
         (
          IN T_PROFI_SERVICE_DESCR FAR *sdb_ptr,
          IN VOID                  FAR *data_ptr,
          IN PB_BOOL                   dummy
         )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
This function is used to send a service request or a service response
to the communication.


INPUT:  sdb_ptr                  -> pointer to SERVICE-DESCRIPTION-BLOCK
        data_ptr                 -> pointer to service specific data
        dummy                    -> dummy


Possible return values:

- E_OK                           -> no error occured

- E_IF_FATAL_ERROR               -> unrecoverable error in PROTOCOL SW

- E_IF_INVALID_LAYER             -> invalid layer
- E_IF_INVALID_SERVICE           -> invalid service identifier
- E_IF_INVALID_PRIMITIVE         -> invalid service primitive
- E_IF_INVALID_COMM_REF          -> invalid communication reference
- E_IF_RESOURCE_UNAVAILABLE      -> no resource available
- E_IF_NO_PARALLEL_SERVICES      -> no parallel services allowed
- E_IF_SERVICE_CONSTR_CONFLICT   -> service temporarily not executable
- E_IF_SERVICE_NOT_SUPPORTED     -> service not supported in subset
- E_IF_SERVICE_NOT_EXECUTABLE    -> service not executable

- E_IF_NO_CNTRL_RES              -> controller does not respond  (CMI_TIMEOUT)
- E_IF_INVALID_DATA_SIZE         -> not enough cmi data block memory
- E_IF_INVALID_CMI_CALL          -> invalid CMI call
----------------------------------------------------------------------------*/
;



FUNCTION extern PB_INT16 CALL_CONV profi_rcv_con_ind
         (
          OUT   T_PROFI_SERVICE_DESCR  FAR *sdb_ptr,
          INOUT VOID                   FAR *data_ptr,
          INOUT USIGN16                FAR *data_len
         )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function is used to receive a Service-Indication or a Service-Confirmation 
from the communication.


INPUT:  sdb_ptr           -> pointer to SERVICE-DESCRIPTION-BLOCK
INOUT:  data_ptr          -> pointer to service specific data
INOUT:  data_len          -> pointer to length of service specific data

Possible return values:

- CON_IND_RECEIVED        -> a confirmation or indication has been received
- NO_CON_IND_RECEIVED     -> no confirmation or indication has been received

- E_IF_FATAL_ERROR        -> unrecoverable error in PROTOCOL SW

- E_IF_INVALID_DATA_SIZE  -> size of data block provided not sufficient
- E_IF_CMI_ERROR          -> error occured in CMI
- E_IF_INVALID_CMI_CALL   -> invalid CMI call

----------------------------------------------------------------------------*/
;

/*kp@men this function has been added for drivers running in polled mode*/
FUNCTION extern PB_INT16 CALL_CONV profi_rcv_con_ind_wait
         (
          OUT   T_PROFI_SERVICE_DESCR  FAR *sdb_ptr,
          INOUT VOID                   FAR *data_ptr,
          INOUT USIGN16                FAR *data_len
         )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function is used to receive a Service-Indication or a Service-Confirmation 
from the communication.


INPUT:  sdb_ptr           -> pointer to SERVICE-DESCRIPTION-BLOCK
INOUT:  data_ptr          -> pointer to service specific data
INOUT:  data_len          -> pointer to length of service specific data

Possible return values:

- CON_IND_RECEIVED        -> a confirmation or indication has been received
- NO_CON_IND_RECEIVED     -> no confirmation or indication has been received

- E_IF_FATAL_ERROR        -> unrecoverable error in PROTOCOL SW

- E_IF_INVALID_DATA_SIZE  -> size of data block provided not sufficient
- E_IF_CMI_ERROR          -> error occured in CMI
- E_IF_INVALID_CMI_CALL   -> invalid CMI call

----------------------------------------------------------------------------*/
;



FUNCTION extern PB_INT16 CALL_CONV profi_set_data
        (
          IN  USIGN8   data_id,      /* id of data           */
          IN  USIGN16  offset,       /* offset in data descr */
          IN  USIGN16  data_size,    /* size of data         */
          IN  VOID FAR *data_ptr     /* pointer to data      */
        )

/*-----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function is used to write datas to CMI data area

possible identifiers: ID_DP_SLAVE_IO_IMAGE         image for slave io datas 
                      ID_DP_STATUS_IMAGE           image for status datas

possible return values:
- E_OK                          -> OK
- E_IF_SERVICE_CONSTR_CONFLICT  -> service not executable at time
- E_IF_SERVICE_NOT_SUPPORTED    -> service not supported
- E_IF_INVALID_DATA_SIZE        -> invalid user data size

-----------------------------------------------------------------------------*/
;


FUNCTION extern PB_INT16 CALL_CONV profi_get_data
        (
          IN     USIGN8       data_id,      /* id of data              */
          IN     USIGN16      offset,       /* offset in data descr    */
          INOUT  USIGN16  FAR *data_size,   /* pointer to size of data */
          INOUT  VOID     FAR *data_ptr     /* pointer to data         */
        )

/*-----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function is used to read  datas from CMI data area

possible identifiers: ID_DP_SLAVE_IO_IMAGE         image for slave io datas 
                      ID_DP_STATUS_IMAGE           image for status datas
                      ID_EXCEPTION_IMAGE           image for exeption datas 

possible return values:
- E_OK                          -> OK
- E_IF_SERVICE_CONSTR_CONFLICT  -> service not executable at time
- E_IF_SERVICE_NOT_SUPPORTED    -> service not supported
- E_IF_INVALID_DATA_SIZE        -> invalid user data size

-----------------------------------------------------------------------------*/
;

#endif /* if0 */

#ifdef __cplusplus
 }
/* #endif MSDOS || OS 9 */
#endif
#endif

