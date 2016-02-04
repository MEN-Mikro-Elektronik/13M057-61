/*********************  P r o g r a m  -  M o d u l e ***********************
 *  
 *         Name: pci.c
 *      Project: SPF driver for Profibus Module M57
 *
 *      $Author: AGromann $
 *        $Date: 2014/11/05 10:51:28 $
 *    $Revision: 1.1 $
 *
 *  Description: Profibus service call interface (driver internal)
 *                      
 *     Required:  
 *     Switches: 
 *
 *---------------------------[ Public Functions ]----------------------------
 *  profi_init
 *  profi_snd_req_res
 *  profi_rcv_con_ind
 *  profi_set_data
 *  profi_get_data
 *  
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: pci.c,v $
 * Revision 1.1  2014/11/05 10:51:28  AGromann
 * Initial Revision
 *
 * Revision 1.5  2001/10/24 16:03:14  kp
 * added parameter waitAck to profi_snd_req_res
 *
 * Revision 1.4  2001/02/28 08:18:45  kp
 * profi_set_data/profi_get_data: retry cmi function only if semaphore busy
 *
 * Revision 1.3  2000/06/15 14:54:14  gromann
 * cosmetics
 *
 * Revision 1.2  2000/03/09 09:58:41  Gromann
 * Changes for Win NT
 *
 * Revision 1.1  2000/02/29 12:04:56  Gromann
 * Initial Revision
 *
 * added llHdl to function prototypes
 * Revision 1.2  96/05/28  15:55:27  kp
 * added profi_rcv_con_ind_wait() (just for polling OS).
 * added FMB layer handler.
 * 
 * Revision 1.1  95/06/02  16:55:10  kp
 * Initial revision
 * 
 *---------------------------------------------------------------------------
 * (c) Copyright 1995 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/

/*****************************************************************************
*                                                                            *
*                               SOFTING GmbH                                 *
*                            Dingolfinger Str. 2                             *
*                             D-81673 Muenchen                               *
*                        Phone: (++49)-(0)89-413004-0                        *
*                        Fax:   (++49)-(0)89-491018                          *
*                                                                            *
*                        Copyright (C) SOFTING GmbH 1995                     *
*                             All Rights Reserved                            *
*                                                                            *
******************************************************************************

FILE_NAME               PCI.C

PROJECT_NAME            PROFIBUS

MODULE                  PCI   

COMPONENT_LIBRARY       PCIL.LIB PCIM.LIB PCIS.LIB
COMPONENT_DLL           PCIL.DLL PCIM.DLL PCIS.DLL

AUTHOR                  SOFTING GmbH

VERSION                 5.00

DATE                    24.04.95

STATUS                  finished

FUNCTIONAL_MODULE_DESCRIPTION


PROFIBUS-Communication Interface
--------------------------------

The communication between host and controller is performed
using some common memory. Softing has specified the data structures and
the synchronization mechanism to be used as "Common Memory Interface"
(CMI). The driver at the host should provide the functions described
below in order to offer a standardized procedural interface to the
communication.

PVCS_SECTION

$Workfile$

$Author: AGromann $

$Archive$

$Revision: 1.1 $

$Date: 2014/11/05 10:51:28 $

$Modtime$


CHANGE_NOTES

Date      Name      Change
------------------------------------------------------------------------------


RELATED_DOCUMENTS
=============================================================================*/
#include "profidp_drv_int.h"
#include <MEN/PROFIDP_MOD_VX/keywords.h>

INCLUDES
#include "profidp_drv_int.h"

GLOBAL_DEFINES

LOCAL_DEFINES

EXPORT_TYPEDEFS

LOCAL_TYPEDEFS


FUNCTION_DECLARATIONS
/*
#include "pci.ext"
*/

EXPORT_DATA

IMPORT_DATA

LOCAL_DATA



FUNCTION GLOBAL PB_INT16 CALL_CONV profi_init
        (
			LL_HANDLE  *llHdl
          /*
          IN  USIGN32  dpr_address_a08,
          IN  USIGN32  dpr_address_a24
          */
        ) /* nodoc */

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
This function is used to download the firmware and to initialize the
Host-Interface and Controller with default values.

INPUT:  dpr_address_a08         -> host base address of Dual-Port-Ram (A08)
        dpr_address_a24			-> host base address of Dual-Port-Ram (A24)

Possible return values:
- E_OK                             -> nterface is initialized
- E_IF_NO_CNTRL_RES                -> controller does not respond
- E_IF_INIT_INVALID_PARAMETER      -> invalid initialize parameter
-----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

	return cmi_init(llHdl/*, dpr_address_a08, dpr_address_a24*/);
}



FUNCTION GLOBAL PB_INT16 CALL_CONV profi_end(VOID) /* nodoc */

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
This function is used to exit the Host-Interface and exit the Controller.

Possible return values:
- E_OK 
-----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

return(E_OK);
}


FUNCTION GLOBAL PB_INT16 CALL_CONV profi_snd_req_res
         (
			             LL_HANDLE     *llHdl,
          IN T_PROFI_SERVICE_DESCR FAR *sdb_ptr,
          IN VOID                  FAR *data_ptr,
          IN PB_BOOL                   waitForAck
         ) /* nodoc */

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
This function is used to send a service request or a service response
to the communication.


INPUT:  sdb_ptr                  -> pointer to SERVICE-DESCRIPTION-BLOCK
        data_ptr                 -> pointer to service specific data
        waitForAck               -> 1=wait for ACK from controller
		                            0=return immediately

Possible return values:

- E_OK                           -> no error occured

- E_IF_FATAL_CMI_ERROR           -> unrecoverable error in CMI
- E_IF_FATAL_LAYER2_ERROR        -> unrecoverable error in LAYER2
- E_IF_FATAL_LAYER7_ERROR        -> unrecoverable error in LAYER7
- E_IF_FATAL_OS_ERROR            -> unrecoverable error in OS

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
{
LOCAL_VARIABLES

PB_INT16     ret_val;               /* return value   */
PB_INT16     data_len;              /* size of datas  */

FUNCTION_BODY


ret_val  = E_OK;
data_len = 0;

DBGWRT_2((DBH, "LL - profi_snd_req_res\n"));

/* --- check communication reference --------------------------------------- */
if (sdb_ptr->comm_ref > MAX_COMREF) return(E_IF_INVALID_COMM_REF);

/* --- check Service-Primitive --------------------------------------------- */
if ((sdb_ptr->primitive == IND) || (sdb_ptr->primitive == CON))
{
  return (E_IF_INVALID_PRIMITIVE);
}

/* --- set the result parameter to POS if a request is send ---------------- */
if (sdb_ptr->primitive == REQ) sdb_ptr->result = POS;


/* --- get service specific data length ------------------------------------ */
switch(sdb_ptr->layer)
{
#if 0
  case FMS:
       ret_val = fmsgdl_get_data_len(sdb_ptr->result,
                                     sdb_ptr->service,
                                     sdb_ptr->primitive,
                                     data_ptr,
                                     &data_len
                                    );
       break;

  case FM7:
       ret_val = fm7gdl_get_data_len(sdb_ptr->result,
                                     sdb_ptr->service,
                                     sdb_ptr->primitive,
                           (USIGN8*) data_ptr,
                                     &data_len
                                    );
       break;

  case FDL:
       ret_val = fdlgdl_get_data_len(sdb_ptr->result,
                                     sdb_ptr->service,
                                     sdb_ptr->primitive,
                                     data_ptr,
                                     &data_len
                                    );
#endif
  case DP:
       ret_val = dpgdl_get_data_len(sdb_ptr->result,
                                    sdb_ptr->service,
                                    sdb_ptr->primitive,
                          (USIGN8*) data_ptr,
                                    &data_len
                                   );
       break;

  case FMB:
       ret_val = fmbgdl_get_data_len(sdb_ptr->result,
                                     sdb_ptr->service,
                                     sdb_ptr->primitive,
                           (USIGN8*) data_ptr,
                                     &data_len
                                    );
	   break;
  default:
       ret_val = E_IF_INVALID_LAYER;
       break;
}

if (ret_val == E_OK)
{
   /* --- transfer service description block and data block to CMI --------- */
   return(cmi_write (llHdl,sdb_ptr,data_ptr,data_len,waitForAck));
}

return(ret_val);
}



FUNCTION GLOBAL PB_INT16 CALL_CONV profi_rcv_con_ind
         (
			                LL_HANDLE      *llHdl,
          OUT   T_PROFI_SERVICE_DESCR  FAR *sdb_ptr,
          INOUT VOID                   FAR *data_ptr,
          INOUT USIGN16                FAR *data_len
         ) /* nodoc */

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

- E_IF_FATAL_CMI_ERROR    -> unrecoverable error in CMI
- E_IF_FATAL_LAYER2_ERROR -> unrecoverable error in LAYER2
- E_IF_FATAL_LAYER7_ERROR -> unrecoverable error in LAYER7
- E_IF_FATAL_OS_ERROR     -> unrecoverable error in OS


- E_IF_INVALID_DATA_SIZE  -> size of data block provided not sufficient
- E_IF_CMI_ERROR          -> error occured in CMI
- E_IF_INVALID_CMI_CALL   -> invalid CMI call

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

PB_INT16 ret_val = E_OK;           /* return value */


FUNCTION_BODY


/* --- read Service-Description-Block and Data-Block from CMI ------------- */
DBGWRT_2((DBH, "Entering cmi_read sdb_ptr = %08x, data_ptr = %08x, data_len = %08x\n",
       sdb_ptr, data_ptr, *data_len));
if ((ret_val = cmi_read (llHdl, sdb_ptr,data_ptr,data_len)) != CON_IND_RECEIVED)
{
	DBGWRT_ERR((DBH, "profi_rcv_con_ind %x\n", ret_val ));
	return(ret_val);
}

#if 0
/* - if service-identifier equal FM7_RESET, reset the Controller-Firmware - */
if ((sdb_ptr->layer     == FM7_USR  ) &&
    (sdb_ptr->primitive == CON      ) &&
    (sdb_ptr->service   == FM7_RESET)    )
{
  if ((ret_val = cmi_init((T_PROFI_INIT FAR*) &init_parameter)) != E_OK) return(ret_val);
}
#endif

	DBGWRT_2((DBH, "profi_rcv_con_ind cir %x\n", CON_IND_RECEIVED ));
	return(CON_IND_RECEIVED);
}

extern u_int8 loop3;

FUNCTION GLOBAL PB_INT16 CALL_CONV profi_rcv_con_ind_wait
         (
          					LL_HANDLE      *llHdl,
          OUT   T_PROFI_SERVICE_DESCR  FAR *sdb_ptr,
          INOUT VOID                   FAR *data_ptr,
          INOUT USIGN16                FAR *data_len
         )
{
	PB_INT16 ret_val;
	u_int16 i;
	
	i = 0;

	while( (ret_val = profi_rcv_con_ind(llHdl, sdb_ptr,data_ptr,data_len)) == 
		  NO_CON_IND_RECEIVED) {
		  	i++;
			/* ; */
	}
	
	return ret_val;
}

FUNCTION GLOBAL PB_INT16 CALL_CONV profi_set_data
        (
		   LL_HANDLE   *llHdl,
          IN  USIGN8   data_id,      /* id of data           */
          IN  USIGN16  offset,       /* offset in data descr */
          IN  USIGN16  data_size,    /* size of data         */
          IN  VOID FAR *data_ptr     /* pointer to data      */
        ) /* nodoc */

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
{
LOCAL_VARIABLES
	PB_INT16 err;

FUNCTION_BODY
    llHdl->cTick_pb_set_get_data = ACT_TICK;
	
    while ((err = cmi_set_data_descr(llHdl,data_id,offset,data_size,data_ptr)) 
		   == E_IF_SERVICE_CONSTR_CONFLICT) {
		/* retry while semaphore busy */
		if ( (int32) (ACT_TICK - llHdl->cTick_pb_set_get_data) >= ((int32) (5 * TICK_RATE)) ) {
			return(E_IF_SERVICE_CONSTR_CONFLICT);
		}
	}


	return(err);
}


FUNCTION GLOBAL PB_INT16 CALL_CONV profi_get_data
        (
			  LL_HANDLE       *llHdl,
          IN     USIGN8       data_id,      /* id of data              */
          IN     USIGN16      offset,       /* offset in data descr    */
          INOUT  USIGN16  FAR *data_size,   /* pointer to size of data */
          INOUT  VOID     FAR *data_ptr     /* pointer to data         */
        ) /* nodoc */

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
{
LOCAL_VARIABLES
	PB_INT16 err;

FUNCTION_BODY

    llHdl->cTick_pb_set_get_data = ACT_TICK;

    while ((err = cmi_get_data_descr(llHdl,data_id,offset,data_size,data_ptr))
		   == E_IF_SERVICE_CONSTR_CONFLICT) {
		
        if ( (int32) (ACT_TICK - llHdl->cTick_pb_set_get_data) >= ((int32) (5 * TICK_RATE)) ) {
           return(E_IF_SERVICE_CONSTR_CONFLICT);
	    }
    }

	return(err);
}

