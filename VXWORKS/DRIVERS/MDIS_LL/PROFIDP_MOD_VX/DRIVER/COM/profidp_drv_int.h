/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: profidp_drv_int.h
 *
 *       Author: ag
 *        $Date: 2014/11/05 10:51:36 $
 *    $Revision: 1.1 $
 *
 *  Description: Intern header file for PROFIDP driver
 *               - PROFIDP structures and definitions
 *
 *
 *     Switches: _ONE_NAMESPACE_PER_DRIVER_
 *               _LL_DRV_
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: profidp_drv_int.h,v $
 * Revision 1.1  2014/11/05 10:51:36  AGromann
 * Initial Revision
 *
 * Revision 2.7  2010/11/16 11:23:47  CKauntz
 * R: Compiler warning for differnet types
 * M: Added cast
 *
 * Revision 2.6  2010/10/06 23:10:08  cs
 * R:1. driver was not compilable for MPC85XX
 *   2. OSS_IrqMaskR is callable from any context
 * M:1. adapted calls to MACCESS macros to consider current macro definitios
 *   2. don't query llHdl->irqFlag for status before locking interrupts
 *
 * Revision 2.5  2004/08/20 14:24:14  AGromann
 * added fwAliveCheckWait flag
 *
 * Revision 2.4  2004/07/22 11:45:29  AGromann
 * updated for FW alive checking
 *
 * Revision 2.3  2004/05/05 08:25:17  cs
 * Minor changes for MDIS4/2004 conformity
 *   removed defines NO_IRQS, ALLOW_IRQS, now functions placed in code
 *
 * Revision 2.2  2003/01/07 15:50:55  agromann
 * cosmetics
 *
 * Revision 2.1  2001/10/25 11:17:51  kp
 * changed order of include files (vxW compile)
 *
 * Revision 2.0  2001/10/24 16:03:26  kp
 * added some llHdl fields for PROFIDP driver 2.0
 * added macro for unique symbol names
 *
 * Revision 1.5  2001/02/28 08:18:49  kp
 * added con_buf to LL handle struct
 *
 * Revision 1.4  2001/01/22 08:58:02  kp
 * stdio.h no longer included (not allowed for LL drivers!)
 *
 * Revision 1.3  2000/06/15 15:05:29  gromann
 * added swapped functionality
 *
 * Revision 1.2  2000/03/09 09:58:50  Gromann
 * Changes for Win NT
 *
 * Revision 1.1  2000/02/29 12:05:02  Gromann
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _PROFIDP_DRV_INT_H
#define _PROFIDP_DRV_INT_H

#ifdef __cplusplus
      extern "C" {
#endif


#define _NO_LL_HANDLE		/* ll_defs.h: don't define LL_HANDLE struct */

#ifdef PROFIDP_SW				/* swapped variant */
#	define MAC_MEM_MAPPED
#	define ID_SW
#endif


#include <MEN/men_typs.h>   /* system dependent definitions   */
#include <MEN/maccess.h>    /* hw access macros and types     */
#include <MEN/dbg.h>        /* debug functions                */
#include <MEN/oss.h>        /* oss functions                  */
#include <MEN/desc.h>       /* descriptor functions           */
#include <MEN/modcom.h>     /* ID PROM functions              */
#include <MEN/mdis_api.h>   /* MDIS global defs               */
#include <MEN/mdis_com.h>   /* MDIS common defs               */
#include <MEN/mdis_err.h>   /* MDIS error codes               */
#include <MEN/ll_defs.h>    /* low-level driver definitions   */
#include <MEN/PROFIDP_MOD_VX/profidp_byte_ord.h>
#include "m57.h"            /* M57 hardware access macros <MEN/m57.h> changed to "m57.h"    */
#include <MEN/PROFIDP_MOD_VX/twist.h>           /* needs #define INTEL (#define BIG/LITTLE)*/


# include <MEN/PROFIDP_MOD_VX/keywords.h>
# include <MEN/PROFIDP_MOD_VX/pb_type.h>
# include <MEN/PROFIDP_MOD_VX/pb_conf.h>
# include <MEN/PROFIDP_MOD_VX/pb_dp.h>
# include <MEN/PROFIDP_MOD_VX/pb_err.h>
# include <MEN/PROFIDP_MOD_VX/pb_fmb.h>
# include <MEN/PROFIDP_MOD_VX/pb_if.h>


#include "cmi_struct.h"

#include <taskLib.h>
#include <semLib.h>

/*-----------------------------------------+
|  DEFINES                                 |
+-----------------------------------------*/
/* general */

#define TIMEOUT_DEFAULT     5000        /* set timeout default to 5 sec. */

#define PROFIDP_CMI_TIMEOUT 5           /* timeout for cmi routines in seconds ! */

/* debug settings */
#define DBG_MYLEVEL			llHdl->dbgLevel
#define DBH					llHdl->dbgHdl

/* register offsets */
#define DP_AUTO_START_ADDR_POINTER   0x400L     /* Address of auto start window in DPRAM */
#define DP_AUTO_START_ADDR           0x18000L   /* Auto start address = address of firmware */

#define COMM_OFF	  0x8000			/* offset of CMI-descr. in DPRAM */
                                        /* CMI descriptor offset in DPRAM */
#define COFF(cmi_var) \
  (cmi_var + COMM_OFF)

#define DP_ARRAY_OFFSET_IN(ch)    ((ch - llHdl->lowest_slave_address) * llHdl->max_slave_input_len)

#define DP_OUT_POINTER            (llHdl->maxNumSlaves * llHdl->max_slave_input_len)

#define DP_ARRAY_OFFSET_OUT(ch)   (DP_OUT_POINTER + \
                                     ((ch - llHdl->lowest_slave_address) * llHdl->max_slave_output_len))


#define ACT_TICK 			 (OSS_TickGet (llHdl->osHdl)) /* / OSS_TickRateGet (llHdl->osHdl)) */
							/* get current system tick converted to sec */
#define TICK_RATE			 OSS_TickRateGet (llHdl->osHdl)

#define PROFIDP_ALIVE_CHECK_CYCLE  (2 * TICK_RATE)  /* check every 2s if FW is alive */
#define PROFIDP_ALIVE_TIMEOUT      (1 * TICK_RATE)  /* FW alive timeout 1s */
#define PROFIDP_ALIVE_IDLE         0  /* wait for next FW alive check cycle */
#define PROFIDP_ALIVE_WAIT_CON     1  /* wait for CON of FW alive request (get diag req) */

/* replace D32 accesses with two D16 accesses */
#if (defined(_BIG_ENDIAN_) && (!defined(MAC_BYTESWAP))) || ( defined(_LITTLE_ENDIAN_) && defined(MAC_BYTESWAP) )

# define PB_MWRITE_D32( ma, offset, value ) \
	(MWRITE_D16 (ma, offset, (u_int16) ((value)>>16))), \
	(MWRITE_D16 (ma, offset + 2, (u_int16) value))
# define PB_MREAD_D32( ma, offset ) \
	(((MREAD_D16( ma, offset )) << 16) + MREAD_D16( ma, offset + 2 ))

#elif (defined(_LITTLE_ENDIAN_) && (!defined(MAC_BYTESWAP))) || ( defined(_BIG_ENDIAN_) && defined(MAC_BYTESWAP) )

# define PB_MWRITE_D32( ma, offset, value ) \
	(MWRITE_D16 (ma, offset + 2, (u_int16) ((value)>>16))), \
	(MWRITE_D16 (ma, offset, (u_int16) value))
# define PB_MREAD_D32( ma, offset ) \
	((((u_int32)MREAD_D16( ma, offset + 2 )) << 16) + (u_int32)MREAD_D16( ma, offset ))

#else
# error "please specify either _BIG_ENDIAN_ or _LITTLE_ENDIAN_"
#endif


/* --- semaphore ----------------------------------------------------------- */
#define _IDLE                 0x0         /* semaphor idle                   */
#define _BUSY                 0x1         /* semaphor busy                   */

/*--- States for dp_setstate() ---*/
#define DP_ST_OFFLINE			0x00
#define DP_ST_STOP				0x40
#define DP_ST_CLEAR				0x80
#define DP_ST_OPERATE			0xc0



/* this macro makes shure that the window-pointer can be resotored by IRQ-routine */
# define SET_WINDOW(base,x)	 \
	{   \
		if( llHdl->isrTaskId == taskIdSelf() ) { \
			DP_SET_WINDOW(base,x); \
		} \
		else { \
			DP_SET_WINDOW(base,x); \
			llHdl->current_wptr = x; \
		} \
	}

/*---- defs to make unique names for global symbols ----*/

/* cmi.c */
#define cmi_get_data_descr	PROFIDP_GLOBNAME(PROFIDP_VARIANT,cmi_get_data_descr)
#define cmi_init			PROFIDP_GLOBNAME(PROFIDP_VARIANT,cmi_init)
#define cmi_read			PROFIDP_GLOBNAME(PROFIDP_VARIANT,cmi_read)
#define cmi_set_data_descr	PROFIDP_GLOBNAME(PROFIDP_VARIANT,cmi_set_data_descr)
#define cmi_write			PROFIDP_GLOBNAME(PROFIDP_VARIANT,cmi_write)
#define copy_from_dpram		PROFIDP_GLOBNAME(PROFIDP_VARIANT,copy_from_dpram)
#define copy_to_dpram		PROFIDP_GLOBNAME(PROFIDP_VARIANT,copy_to_dpram)

/* dpgdl.c */
#define dpgdl_get_data_len	PROFIDP_GLOBNAME(PROFIDP_VARIANT,dpgdl_get_data_len)

/* fmbgdl.c */
#define fmbgdl_get_data_len	PROFIDP_GLOBNAME(PROFIDP_VARIANT,fmbgdl_get_data_len)

/* m57_firm.c */
#define Firmware_Ident		PROFIDP_GLOBNAME(PROFIDP_VARIANT,Firmware_Ident)
#define dp_fw				PROFIDP_GLOBNAME(PROFIDP_VARIANT,dp_fw)

/* pci.c */
#define profi_end			PROFIDP_GLOBNAME(PROFIDP_VARIANT,profi_end)
#define profi_get_data		PROFIDP_GLOBNAME(PROFIDP_VARIANT,profi_get_data)
#define profi_init			PROFIDP_GLOBNAME(PROFIDP_VARIANT,profi_init)
#define profi_rcv_con_ind	PROFIDP_GLOBNAME(PROFIDP_VARIANT,profi_rcv_con_ind)
#define profi_rcv_con_ind_wait	PROFIDP_GLOBNAME(PROFIDP_VARIANT,profi_rcv_con_ind_wait)
#define profi_set_data		PROFIDP_GLOBNAME(PROFIDP_VARIANT,profi_set_data)
#define profi_snd_req_res	PROFIDP_GLOBNAME(PROFIDP_VARIANT,profi_snd_req_res)

/*-----------------------------------------+
|  TYPEDEFS                                |
+-----------------------------------------*/

/* channel info function */
typedef struct {
	u_int8 num_in;
	u_int8 num_out;
	} CH_INFO;

/* low-level handle */
typedef struct {
	/* general */
    int32           memAlloc;		   /* size allocated for the handle */
    OSS_HANDLE      *osHdl;            /* oss handle */
    OSS_IRQ_HANDLE  *irqHdl;           /* irq handle */
    DESC_HANDLE     *descHdl;          /* desc handle */
    MACCESS         ma;                /* hw access handle */
	MDIS_IDENT_FUNCT_TBL idFuncTbl;	   /* id function table */
	OSS_SEM_HANDLE* req_con_f0_semP;   /* pointer to semaphore handle */
	OSS_SEM_HANDLE* req_con_0f_semP;   /* pointer to semaphore handle */
	OSS_SEM_HANDLE* con_buf_semP;      /* pointer to semaphore handle */
	OSS_SEM_HANDLE* fwAliveCheckSemP;  /* pointer to semaphore handle */
	/* debug */
    u_int32           dbgLevel;		/* debug level */
	DBG_HANDLE        *dbgHdl;      /* debug handle */
	/* misc */
    u_int32           irqCount;        /* interrupt counter */
    u_int32           idCheck;		   /* id check enabled */
	/* signal */
	OSS_SIG_HANDLE*    sigHdl;         /* signal handle */
	T_CMI_DESCRIPTOR* cmi;             /* pointer to structure CMI_DESCRIPTOR in DPRAM */
	/* M57 spec dsecriptor-key entries */
	u_int32			  addrAssignMode;   /* adress assignment mode */
	u_int32			  maxNumSlaves;     /* max number of slaves */
	u_int32			  maxSlaveDiagEntries; /* max number of slave diag entries */
	u_int32			  maxSlaveDiagLen;     /* max data length of one diag entry */
	u_int32			  clearOutputs;        /* clear outputs when entering CLEAR state */
	u_int32			  autoRemoteService;   /* automaticaly respond to M2 services */
	u_int32			  cyclicDataTransfer;   /* set cyclic data transfer state */
	u_int32           masterClass2;         /* use master class 2 functionality */
	/* static variables from cmi.c */
	T_CMI_DESCRIPTOR      *Wcmi;
	u_int32			      c_base_address;
	u_int32			      h_base_address;
	T_PROFI_SERVICE_DESCR *h_serv_descr;
	T_PROFI_SERVICE_DESCR *c_serv_descr;
	T_DATA_DESCR          *h_data_descr_list;
	VOID                  *h_data_ptr;
	VOID                  *c_data_ptr;
	USIGN16               data_block_size;
	USIGN16               param_block_size;
	USIGN32				  current_wptr; /* current 128 byte dpram window  */
	u_int8                req_con_buf[DP_MAX_TELEGRAM_LEN];	 /* buffer for REQ/CON data */
	u_int8                con_buf[DP_MAX_TELEGRAM_LEN];	 /* buffer for CON data */
	u_int32               cTick_cmi_init;
	u_int32               cTick_irq_to;
	u_int32               cTick_pb_set_get_data;
	u_int8                intFlagReq;       /* Interrupt flag to ack requests. Set to 0 if IRQ occured */
	T_PROFI_SERVICE_DESCR waitForService;
	u_int8                diagBuf[DP_MAX_NUMBER_SLAVES] [DP_MAX_TELEGRAM_LEN]; /*slave diag buffer */
	/* CON/IND buffer */
	u_int8*               con_ind_buf;
	u_int32               con_ind_buf_size;   /* buffer size of CON/IND buffer */
	u_int8*               con_ind_buf_inP;    /* in pointer of CON/IND buffer */
	u_int8*               con_ind_buf_outP;
	u_int8                con_ind_num_el;
	u_int8                con_ind_buf_full;   /* flag is set to 0x01 if buffer is full */
	u_int32               con_ind_memSize;    /* returned mem size of OSS_Memget */
	CH_INFO               chInfo[DP_MAX_NUMBER_SLAVES]; /* info structure for channels channel = slave address */
	u_int8                max_slave_output_len;    /* max slave output len */
	u_int8                max_slave_input_len;     /* max slave input len */
	u_int8                lowest_slave_address;    /* lowest slave address */
	u_int8                chDiag[(DP_MAX_NUMBER_SLAVES + 1)][DP_MAX_TELEGRAM_LEN]; /* diag entries for each channel */
	u_int8                irqFlag;          /* flag is set while doing int routine */
	u_int8                reqPending;       /* flag is set while waiting for ACK from firmware
											   for user requests */
	u_int8				  getSlaveDiagReqDelayed; /* irq routine sets this flag when DP_GET_SLAVE_DIAG req delayed */
	u_int8				  getSlaveDiagReqWaitAck; /* this flag is sets while waiting for DP_GET_SLAVE_DIAG
												     req ACK */
	u_int8				  getSlaveDiagReqWaitCon; /* this flag is sets while waiting for DP_GET_SLAVE_DIAG
												     req CON */
	u_int8                allCh;            /* flag to hold DPRAM semaphor */
	u_int8				  _pad[3];
	u_int32               conIndTimeout;    /* timeout for PROFIDP_BLK_RCV_CON_IND_WAIT */
	u_int8                master_default_address; /* address of profibus master */
	u_int8                fm2EventReason;   /* actual fm2 event reason */
	u_int8                aliveState;       /* alive state for FW alive checking */
	u_int32               lastAliveCheck;   /* tick value of last alive check */
	u_int32               lastFwDiagConInd; /* tick value of last alive message */
	u_int8				  fwAliveCheckWait; /* wait for FW alive confirmation */
	OSS_SEM_HANDLE*  	  isrTaskSemP;
	TASK_ID               isrTaskId;
	SEM_ID                windowPointerSemId;

	} LL_HANDLE;

#include <MEN/ll_entry.h>      /* low-level driver jump table  */
#include <MEN/profidp_mod_vx_drv.h>   /* PROFIDP driver header file */
#include "m57segm.h"        /* segment structure for firmware */
#include "m57_firm.h"       /* firmware data array            */

/* include files which need LL_HANDLE */
#include "cmi.h"               /* CMI sructure and prototypes for cmi.c */
#include "pci.h"               /* prototypes for pci.h */

/*-----------------------------------------+
|  PROTOTYPES                              |
+-----------------------------------------*/


#ifdef __cplusplus
      }
#endif

#endif /* _PROFIDP_DRV_INT_H */





