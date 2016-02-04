/*********************  P r o g r a m  -  M o d u l e ***********************
 *  
 *         Name: dpgdl.c
 *      Project: SPF driver for Profibus Module M57
 *
 *      $Author: AGromann $
 *        $Date: 2014/11/05 10:51:31 $
 *    $Revision: 1.1 $
 *
 *  Description: Profibus DP-Service-Specific-Functions  
 *				 they return the length length of the Request- or Response-Data.
 *
 *                      
 *     Required:  
 *     Switches: 
 *
 *---------------------------[ Public Functions ]----------------------------
 *  
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: dpgdl.c,v $
 * Revision 1.1  2014/11/05 10:51:31  AGromann
 * Initial Revision
 *
 * Revision 1.3  2011/10/04 14:17:15  agromann
 * R: Problems with Data Exchange Mode on X86-CPUs
 * M: Fixed byte swap problem in dpgdl.c.
 *
 * Revision 1.2  2001/10/24 16:03:18  kp
 * only include profidp_drv_int.h
 *
 * Revision 1.1  2000/02/29 12:05:00  Gromann
 * Initial Revision
 *
 * adapted for MDIS 4 
 * Revision 1.2  96/05/28  15:53:20  kp
 * adapted for firmware 5.01
 * 
 * Revision 1.1  95/06/02  16:54:49  kp
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

FILE_NAME               DPGDL.C

PROJECT_NAME            PROFIBUS

MODULE                  DPGDL

COMPONENT_LIBRARY       PCIL.LIB PCIM.LIB PCIS.LIB
COMPONENT_DLL           PCIL.DLL PCIM.DLL PCIS.DLL

AUTHOR                  SOFTING GmbH

VERSION                 1.00

DATE                    24.04.95

STATUS                  finished

FUNCTIONAL_MODULE_DESCRIPTION

This modul contains DP-Service-Specific-Functions which return the length
length of the Request- or Response-Datas.

PVCS_SECTION

$Workfile$

$Author: AGromann $

$Archive$

$Revision: 1.1 $

$Date: 2014/11/05 10:51:31 $

$Modtime$


CHANGE_NOTES

Date      Name      Change
------------------------------------------------------------------------------


RELATED_DOCUMENTS
=============================================================================*/

#include "profidp_drv_int.h"


#define swap_16_intel_motorola(x) TWISTWORD(x)

FUNCTION_DECLARATIONS

/*
#include "pci.ext"
*/

LOCAL_DEFINES


LOCAL_TYPEDEFS


EXPORT_DATA


IMPORT_DATA


LOCAL_DATA



FUNCTION PUBLIC PB_INT16 dpgdl_get_data_len
  (
  IN    PB_INT16         result,
  IN    USIGN8        service,
  IN    USIGN8        primitive,
  IN    USIGN8    FAR *data_ptr,
  OUT   PB_INT16     FAR *data_len
  ) /* nodoc */
  /*------------------------------------------------------------------------*/
  /* FUNCTIONAL_DESCRIPTION                                                 */
  /*------------------------------------------------------------------------*/
  /* - returns the data length of any called PROFIBUS DP service            */
  /*------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

  USIGN16   service_data_len;
  

FUNCTION_BODY

  switch (service)
  {
    case DP_DATA_TRANSFER  : *data_len = 0;                                break;
    case DP_GLOBAL_CONTROL : *data_len = sizeof (T_DP_GLOBAL_CONTROL_REQ); break;
    case DP_ACT_PARAM_LOC  : *data_len = sizeof (T_DP_ACT_PARAM_REQ);      break;
    case DP_INIT_MASTER    : *data_len = sizeof (T_DP_INIT_MASTER_REQ);    break;
    case DP_UPLOAD_LOC     : *data_len = sizeof (T_DP_UPLOAD_REQ);         break;
    
    case DP_DOWNLOAD_LOC   : 
    {
	  service_data_len = TWISTWORD(((T_DP_DOWNLOAD_REQ FAR*) (data_ptr))->data_len);
      if (service_data_len > DP_MAX_DOWNLOAD_DATA_LEN)
      {
        return (E_IF_INVALID_DATA_SIZE);
      }
      
      *data_len = sizeof (T_DP_DOWNLOAD_REQ) + service_data_len;
      break;
    }
    
    case DP_START_SEQ_LOC  : *data_len = sizeof (T_DP_START_SEQ_REQ);      break;
    case DP_END_SEQ_LOC    : *data_len = sizeof (T_DP_END_SEQ_REQ);        break;
    case DP_GET_SLAVE_DIAG : *data_len = 0;                                break;
    
    case DP_SET_PRM        :
    case DP_SET_PRM_LOC    :
    {
      service_data_len = swap_16_intel_motorola (((T_DP_SET_PRM_REQ FAR*) (data_ptr))->prm_data.prm_data_len);
      
      if (service_data_len > DP_MAX_PRM_DATA_LEN)
      {
        return (E_IF_INVALID_DATA_SIZE);
      }
      
      *data_len = sizeof (T_DP_SET_PRM_REQ) - sizeof (T_DP_PRM_DATA) + service_data_len;
      break;
    }

    case DP_CHK_CFG        :
    {
      service_data_len = swap_16_intel_motorola (((T_DP_CHK_CFG_REQ FAR*) (data_ptr))->cfg_data.cfg_data_len);

      if (service_data_len > DP_MAX_CFG_DATA_LEN)
      {
        return (E_IF_INVALID_DATA_SIZE);
      }
      
      *data_len = sizeof (T_DP_CHK_CFG_REQ) - sizeof (T_DP_CFG_DATA) + service_data_len;
      break;
    }

    case DP_GET_CFG        : *data_len = sizeof (T_DP_GET_CFG_REQ);        break;
    case DP_SLAVE_DIAG     : *data_len = sizeof (T_DP_SLAVE_DIAG_REQ);     break;
    case DP_RD_INP         : *data_len = sizeof (T_DP_RD_INP_REQ);         break;
    case DP_RD_OUTP        : *data_len = sizeof (T_DP_RD_OUTP_REQ);        break;
    
    case DP_DATA_EXCHANGE  :
    {
      service_data_len = swap_16_intel_motorola (((T_DP_DATA_EXCHANGE_REQ FAR*) (data_ptr))->outp_data_len);
      
      if (service_data_len > DP_MAX_OUTPUT_DATA_LEN)
      {
        return (E_IF_INVALID_DATA_SIZE);
      }
      
      *data_len = sizeof (T_DP_DATA_EXCHANGE_REQ) + service_data_len;
      break;
    }

    case DP_SET_SLAVE_ADD  :
    {
      service_data_len = swap_16_intel_motorola (((T_DP_SET_SLAVE_ADD_REQ FAR*) (data_ptr))->rem_slave_data_len);
      
      if (service_data_len > DP_MAX_REM_SLAVE_DATA_LEN)
      {
        return (E_IF_INVALID_DATA_SIZE);
      }
      
      *data_len = sizeof (T_DP_SET_SLAVE_ADD_REQ) + service_data_len;
      break;
    }

    case DP_DOWNLOAD       : return (E_IF_SERVICE_NOT_SUPPORTED);
    case DP_UPLOAD         : return (E_IF_SERVICE_NOT_SUPPORTED);
    case DP_START_SEQ      : return (E_IF_SERVICE_NOT_SUPPORTED);
    case DP_END_SEQ        : return (E_IF_SERVICE_NOT_SUPPORTED);
    case DP_ACT_PARAM      : return (E_IF_SERVICE_NOT_SUPPORTED);
    case DP_GET_MASTER_DIAG: return (E_IF_SERVICE_NOT_SUPPORTED);
    case DP_ACT_PARA_BRCT  : return (E_IF_SERVICE_NOT_SUPPORTED);

    default: return (E_IF_INVALID_SERVICE);
  } /* switch serivce */

  return (E_OK);
} /* dp_get_data_len */
