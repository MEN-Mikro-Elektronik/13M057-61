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
/*                  FMB-USER-INTERFACE DEFINES and TYPES                     */
/*                                                                           */
/*  Filename    : PB_FMB.H                                                   */
/*  Version     : 5.00                                                       */
/*                5.01                                                       */
/*  Date        : 10.10.95                                                   */
/*                17.01.96                                                   */
/*  Author      : SOFTING GmbH                                               */
/*                                                                           */
/*  Description : This file contains the types and defines of the Fieldbus-  */
/*                Basic Management User-Interface                            */
/*  CHANGE_NOTES                                                             */
/*                                                                           */
/*  date      name      change                                               */
/*  -----------------------------------------------------------------------  */
/*                                                                           */
/*****************************************************************************/
/************************ PVCS SECTION ****************************************
$Workfile:   pb_fmb.h  $
$Author: AGromann $
$Archive:   P:\profi.vcs\pbv50\papi\inc_glb\cp5412a2\pb_fmb.h_v  $
$Revision: 1.1 $
$Date: 2014/11/05 10:52:39 $
$Modtime:   13 Oct 1995 14:11:12  $
***********************  END PVCS SECTION ************************************/

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

#ifndef __PB_FMB__
#define __PB_FMB__

/*****************************************************************************/
/***     FMB service identifier                                            ***/
/*****************************************************************************/
#define FMB_SET_CONFIGURATION     27
#define FMB_SET_BUSPARAMETER      22
#define FMB_FM2_EVENT             19
#define FMB_RESET                 20
#define FMB_EXIT                  21

/*****************************************************************************/
/***     FMB data structures                                               ***/
/*****************************************************************************/

/****************************************************************************/
/* FMB-Configuration-Management                                             */
/****************************************************************************/

/* --- VFD-Configuration -------------------------------------------------- */
typedef struct _T_FMB_CONFIG_VFD
{
  USIGN16     max_no_vfds;              /* max. number of VFD's             */
  USIGN16     max_no_obj_descr;         /* max. number of OD object descr.  */
  USIGN8      max_obj_name_length;      /* max. size of OD object name      */
  USIGN8      max_obj_ext_length;       /* max. size of OD object extension */  
} T_FMB_CONFIG_VFD;


/* --- CRL-Configuration -------------------------------------------------- */
typedef struct _T_FMB_CONFIG_CRL
{
  USIGN16     max_no_fal_sdbs;          /* max. number of FAL-SDBs          */
  USIGN16     max_no_fdl_sdbs;          /* max. number of FDL-SDBs          */
  USIGN16     max_no_data_buffer;       /* max. number of PDU buffers       */
  USIGN16     max_no_api_buffer;        /* max. number of API buffers       */
  USIGN16     max_no_poll_entries;      /* max. number of poll list entries */
  USIGN16     max_no_subscr_entries;    /* max. number of subscr. list entries / see notes below */
  PB_BOOL     resrc_check;              /* for future use / see notes below */
  USIGN8      max_no_parallel_req;      /* for future use / see notes below */
  USIGN8      max_no_parallel_ind;      /* for future use / see notes below */
  USIGN8      dummy;                    /* alignment byte                   */
} T_FMB_CONFIG_CRL;

/* Notes on T_FMB_CONFIG_CRL: ------------------------------------------------

component 'max_no_subscr_entries' is of significance for PROFIBUS-PA only
component 'resrc_check' is introduced for future use - for version 5.01 
          set "resrc_check = PB_TRUE;"
component 'max_no_parallel_req' is introduced for future use - for 
          version 5.01 set "max_no_parallel_req = 0;"
component 'max_no_parallel_ind' is introduced for future use - for 
          version 5.01 set "max_no_parallel_ind = 0;"

end of notes on T_FMB_CONFIG_CRL ------------------------------------------ */


/* --- DP-Configuration --------------------------------------------------- */
typedef struct _T_FMB_CONFIG_DP
{
  USIGN8      max_number_slaves;      /* maximum number DP Slaves supported */
  USIGN8      max_slave_output_len;     /* max. length of slave output data */
  USIGN8      max_slave_input_len;       /* max. length of slave input data */
  USIGN8      max_slave_diag_entries; /* max. number entries in diag buffer */
  USIGN8      max_slave_diag_len;          /* max. length of one diag entry */
  USIGN8      reserved;                                   /* alignment byte */
  USIGN16     max_bus_para_len;         /* max. length of bus parameter set */
  USIGN16     max_slave_para_len;     /* max. length of slave parameter set */
} T_FMB_CONFIG_DP;


/* --- FDLIF-Configuration ------------------------------------------------- */
typedef struct _T_FMB_CONFIG_FDLIF
{
  USIGN8      send_req_credits;  /* max. number of send credits for SDA and SDN services */
  USIGN8      srd_req_credits;   /* max. number of send credits for SRD services */
  USIGN8      receive_credits;   /* max. number of receive credits */
  USIGN8      max_no_resp_saps;  /* max. number of FDL responder SAPs */ 
} T_FMB_CONFIG_FDLIF;


/* --- SM7-Configuration -------------------------------------------------- */
typedef struct _T_FMB_CONFIG_SM7
{
   USIGN16    reserved;
} T_FMB_CONFIG_SM7;


/* --- Fieldbus-Basic-Management-Configuration ---------------------------- */
typedef struct _T_FMB_SET_CONFIGURATION_REQ
{
  PB_BOOL             fms_active;        /* FMS and FM7 services are usable */
  PB_BOOL             dp_active;         /* DP services are usable          */
  PB_BOOL             fdlif_active;      /* FDLIF services are usable       */
  PB_BOOL             sm7_active;        /* SM7 services are usable / for PROFIBUS-PA only */
  USIGN16             fdl_evt_receiver;  /* receiver of FDL events: FMB_USR,FM7_USR,DP_USR,FDLIF_USR */
  USIGN16             data_buffer_length;/* max. size of PDU buffer         */
  T_FMB_CONFIG_VFD    vfd;               /* VFD configuration parameters    */
  T_FMB_CONFIG_CRL    crl;               /* CRL configuration parameters    */
  T_FMB_CONFIG_DP     dp;                /* DP configuration parameter      */
  T_FMB_CONFIG_FDLIF  fdlif;             /* FDLIF configuration parameter   */
  T_FMB_CONFIG_SM7    sm7;               /* SM7 configuration parameters    */
} T_FMB_SET_CONFIGURATION_REQ;


/****************************************************************************/
/* FMB-Set-FDL-Busparameter                                                 */
/****************************************************************************/

/* --- Baudrate ------------------------------------------------------------ */

#define KBAUD_9_6                  0x00
#define KBAUD_19_2                 0x01
#define KBAUD_93_75                0x02
#define KBAUD_187_5                0x03
#define KBAUD_500                  0x04
#define MBAUD_1_5                  0x06
#define MBAUD_3                    0x07
#define MBAUD_6                    0x08
#define MBAUD_12                   0x09


/* --- Medium-Redundancy -------------------------------------------------- */
#define NO_REDUNDANCY              0x00
#define BUS_A_HIGHPRIOR            0x01
#define BUS_B_HIGHPRIOR            0x02
#define REDUNDANCY                 0x03

/* --- In-Ring-Desired ---------------------------------------------------- */
#define IN_RING_DESIRED            PB_TRUE
#define NOT_IN_RING_DESIRED        PB_FALSE


typedef struct _T_FMB_SET_BUSPARAMETER_REQ
{
  USIGN8         loc_add;            /* local station                       */
  USIGN8         loc_segm;           /* local segment                       */
  USIGN8         baud_rate;          /* baud rate                           */
  USIGN8         medium_red;         /* medium redundancy                   */
  USIGN16        tsl;                /* slot time                           */
  USIGN16        min_tsdr;           /* min. station delay time resp.       */
  USIGN16        max_tsdr;           /* max. station delay time resp.       */
  USIGN8         tqui;               /* quiet time                          */
  USIGN8         tset;               /* setup time                          */
  USIGN32        ttr;                /* target token rotation time          */
  USIGN8         g;                  /* gap update factor                   */
  PB_BOOL        in_ring_desired;    /* active or passive station           */ 
  USIGN8         hsa;                /* highest station address             */
  USIGN8         max_retry_limit;    /* max. retry limit                    */  
} T_FMB_SET_BUSPARAMETER_REQ;


/****************************************************************************/
/* FMB-Event-Management                                                     */
/****************************************************************************/

#ifndef __PB_ERR__				/* kp@men */
#define FM2_FAULT_ADDRESS          0x01     /* duplicate address recognized */
#define FM2_FAULT_PHY              0x02     /* phys.layer is malfunctioning */
#define FM2_FAULT_TTO              0x03     /* time out on bus detected     */
#define FM2_FAULT_SYN              0x04     /* no receiver synchronization  */
#define FM2_FAULT_OUT_OF_RING      0x05     /* station out of ring          */
#define FM2_GAP_EVENT              0x06     /* new station in ring          */
#endif

typedef struct _T_FMB_FM2_EVENT_IND
{
  USIGN16    reason;                           /* reason code               */
} T_FMB_FM2_EVENT_IND;


#endif  /* __PB_FMB__ */
