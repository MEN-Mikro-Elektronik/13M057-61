/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: profidp_drv_mod_vx.h
 *
 *       Author: kp
 *        $Date: 2014/11/05 10:52:31 $
 *    $Revision: 3.1 $
 *
 *  Description: Header file for PROFIDP driver
 *               - PROFIDP specific status codes
 *               - PROFIDP function prototypes
 *
 *     Switches: _ONE_NAMESPACE_PER_DRIVER_
 *               _LL_DRV_
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: profidp_mod_vx_drv.h,v $
 * Revision 3.1  2014/11/05 10:52:31  AGromann
 * Initial Revision
 *
 * Revision 2.5  2010/10/06 23:36:35  cs
 * R: driver ported to new MDIS_API and men_typs
 * M: optionally define new types for backward compatibility to MDIS4
 *
 * Revision 2.4  2004/07/22 11:45:31  AGromann
 * added error codes for timeout and FW alive checking
 *
 * Revision 2.3  2001/10/24 16:03:35  kp
 * added PROFIDP_CH_INPUT_LEN/PROFIDP_CH_OUTPUT_LEN
 * added macro to make unique symbols
 *
 * Revision 2.2  2000/06/15 15:08:34  gromann
 * added driver specific error codes
 *
 * Revision 2.1  2000/02/29 12:05:33  Gromann
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _PROFIDP_DRV_MOD_VX_H
#define _PROFIDP_DRV_MOD_VX_H

#ifdef __cplusplus
      extern "C" {
#endif


/*-----------------------------------------+
|  TYPEDEFS                                |
+-----------------------------------------*/
/* none */

/*-----------------------------------------+
|  DEFINES                                 |
+-----------------------------------------*/

/* PROFIDP specific status codes (STD) */			/* S,G: S=setstat, G=getstat */
#define PROFIDP_SIG_ON_EVENT_SET   M_DEV_OF+0x00	/* S: install signal */
#define PROFIDP_SIG_ON_EVENT_CLR   M_DEV_OF+0x01	/* S: uninstall signal */
#define PROFIDP_SET_STACK_STOP     M_DEV_OF+0x02    /* S: Stack to STOP */
#define PROFIDP_SET_STACK_CLEAR    M_DEV_OF+0x03    /* S: Stack to CLEAR */
#define PROFIDP_SET_STACK_OPERATE  M_DEV_OF+0x04    /* S: Stack to OPERATE */
#define PROFIDP_START_STACK        M_DEV_OF+0x05    /* S: set protocol stack to STOP CLEAR OPERATE */
#define PROFIDP_STOP_STACK         M_DEV_OF+0x06    /* S: set protocol stack to STOP */
#define PROFIDP_WAIT_TIMEOUT       M_DEV_OF+0x07    /* S,G: timeout value for PROFIDP_BLK_RCV_CON_IND_WAIT */
#define PROFIDP_NUM_CON_IND        M_DEV_OF+0x08    /* G: get number of CON/IND's in buffer */
#define PROFIDP_FM2_REASON         M_DEV_OF+0x09    /* G: get reason of FMB FM2 EVENT */
#define PROFIDP_BUFF_OVERFLOW      M_DEV_OF+0x0a    /* G: get buffer overflow info */
#define PROFIDP_MAX_INPUT_LEN      M_DEV_OF+0x0b    /* G: get max slave input length */
#define PROFIDP_MAX_OUTPUT_LEN     M_DEV_OF+0x0c    /* G: get max slave output length */
#define PROFIDP_CH_INPUT_LEN       M_DEV_OF+0x0d    /* G: get slave input length */
#define PROFIDP_CH_OUTPUT_LEN      M_DEV_OF+0x0e    /* G: get slave output length */


/* PROFIDP specific status codes (BLK)	*/			/* S,G: S=setstat, G=getstat */
#define   PROFIDP_BLK_CONFIG           M_DEV_BLK_OF+0x00 /* S: cofigurate M57-Module */
#define   PROFIDP_BLK_SEND_REQ_RES     M_DEV_BLK_OF+0x01 /* S: send request or respons */
#define   PROFIDP_BLK_SET_ALL_CH       M_DEV_BLK_OF+0x02 /* S: write data to channel */
#define   PROFIDP_BLK_GET_ALL_CH       M_DEV_BLK_OF+0x03 /* G: read data from channel */
#define   PROFIDP_BLK_GET_DIAG         M_DEV_BLK_OF+0x04 /* G: get diag data */
#define   PROFIDP_BLK_DATA_TRANSFER    M_DEV_BLK_OF+0x05 /* S: do data transfer service */
#define   PROFIDP_BLK_GET_SLAVE_DIAG   M_DEV_BLK_OF+0x06 /* G: get slave diag entry */
#define   PROFIDP_BLK_GET_CON_IND      M_DEV_BLK_OF+0x07 /* G: get CON/IND form buffer */
#define   PROFIDP_BLK_RCV_CON_IND      M_DEV_BLK_OF+0x08 /* G: receive CON/IND M57 */
#define   PROFIDP_BLK_RCV_CON_IND_WAIT M_DEV_BLK_OF+0x09 /* G: receive CON/IND M57 and wait until CON/IND occurs*/

/*--- PROFIDP specific error codes ---*/
#define PROFIDP_ERR_VERIFY_FW         (ERR_DEV+0x1)   /* error verify firmware */
#define PROFIDP_ERR_CONFIG            (ERR_DEV+0x2)   /* Profibus configuration error */
#define PROFIDP_ERR_NO_INPUTS         (ERR_DEV+0x3)   /* error no input slaves  */
#define PROFIDP_ERR_NO_OUTPUTS        (ERR_DEV+0x4)   /* error no output slaves */
#define PROFIDP_ERR_GET_DATA          (ERR_DEV+0x5)   /* error get data out of DPRAM */
#define PROFIDP_ERR_SET_DATA          (ERR_DEV+0x6)   /* error write data to DPRAM */
#define PROFIDP_ERR_NO_CON_IND        (ERR_DEV+0x7)   /* error no CON/IND in buffer */
#define PROFIDP_ERR_READ_CON_IND      (ERR_DEV+0x8)   /* error reading CON/IND */
#define PROFIDP_ERR_CON_IND_SEM       (ERR_DEV+0x9)   /* error CON/IND semaphore */
#define PROFIDP_ERR_START_STACK       (ERR_DEV+0xa)   /* error starting protocol stack */
#define PROFIDP_ERR_STOP_STACK        (ERR_DEV+0xb)   /* error stopping protocol stack */
#define PROFIDP_ERR_SEND_REQ_RES      (ERR_DEV+0xc)   /* error send request or respons */
#define PROFIDP_ERR_DATA_TRANSFER     (ERR_DEV+0xd)   /* error data transfer */
#define PROFIDP_ERR_SET_STACK         (ERR_DEV+0xe)   /* error seting protocol stack */
#define PROFIDP_ERR_VALIDATE_FW       (ERR_DEV+0xf)   /* error firmeare not valid */
#define PROFIDP_ERR_REQ_CON_TIMEOUT   (ERR_DEV+0x10)  /* timeout error during REQ or CON  */
#define PROFIDP_ERR_FW_NOT_ALIVE      (ERR_DEV+0x11)  /* PROFIDP-Firmware is not alive  */
#define PROFIDP_ERR_CREATING_ISR_TASK (ERR_DEV+0x12)  /* error creating ISR task */

/*
 * Macros to build unique, variant specific names for global symbols
 */
#ifndef  PROFIDP_VARIANT
# define PROFIDP_VARIANT PROFIDP
#endif

# define _PROFIDP_GLOBNAME(var,name) var##_MOD_VX_##name
#ifndef _ONE_NAMESPACE_PER_DRIVER_
# define PROFIDP_GLOBNAME(var,name) _PROFIDP_GLOBNAME(var,name)
#else
# define PROFIDP_GLOBNAME(var,name) _PROFIDP_GLOBNAME(PROFIDP,name)
#endif

#define __PROFIDP_MOD_VX_GetEntry			PROFIDP_GLOBNAME(PROFIDP_VARIANT,GetEntry)

/*-----------------------------------------+
|  BACKWARD COMPATIBILITY TO MDIS4         |
+-----------------------------------------*/
#ifndef U_INT32_OR_64
 /* we have an MDIS4 men_types.h and mdis_api.h included */
 /* only 32bit compatibility needed!                     */
 #define INT32_OR_64  int32
 #define U_INT32_OR_64 u_int32
 typedef INT32_OR_64  MDIS_PATH;
#endif /* U_INT32_OR_64 */

/*-----------------------------------------+
|  PROTOTYPES                              |
+-----------------------------------------*/
#ifdef _LL_DRV_
#ifndef _ONE_NAMESPACE_PER_DRIVER_
	extern void __PROFIDP_GetEntry(LL_ENTRY* drvP);
#endif
#endif /* _LL_DRV_ */


#ifdef __cplusplus
      }
#endif

#endif /* _PROFIDP_DRV_MOD_VX_H */


