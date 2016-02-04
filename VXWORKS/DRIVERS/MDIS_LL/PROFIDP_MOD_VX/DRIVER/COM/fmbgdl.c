/*****************************************************************************
*                                                                            *
*                               SOFTING GmbH                                 *
*                            Dingolfinger Str. 2                             *
*                             D-81673 Muenchen                               *
*                        Phone: (++49)-(0)89-413004-0                        *
*                        Fax:   (++49)-(0)89-491018                          *
*                                                                            *
*                     Copyright (C) SOFTING GmbH 1995,1996                   *
*                             All Rights Reserved                            *
*                                                                            *
******************************************************************************

FILE_NAME               FMBGDL.C

PROJECT_NAME            PROFIBUS

MODULE                  FMBGDL  

COMPONENT_LIBRARY       M_PAPI_L.LIB M_PAPI_M.LIB M_PAPI_S.LIB

AUTHOR                  SOFTING GmbH

VERSION                 5.00
                        5.01

DATE                    24.11.95
                        31.01.96

STATUS                  finished

FUNCTIONAL_MODULE_DESCRIPTION

This modul contains the Fieldbus Basic Management function which returns the 
length of the request datas.


PVCS_SECTION

$Workfile:   fmbgdl.c  $

$Author: AGromann $

$Archive:   P:\profi.vcs\pbv50\papi\src\cp5412a2\fmbgdl.c_v  $

$Revision: 1.1 $

$Date: 2014/11/05 10:51:32 $

$Modtime:   24 Oct 1995 15:56:02  $


CHANGE_NOTES

Date      Name      Change
------------------------------------------------------------------------------
16.01.96  Boe       function: fmbgdl_get_data_len()
                    - add service 'FMB_SET_BUSPARAMETER

RELATED_DOCUMENTS
=============================================================================*/
#include "profidp_drv_int.h"



GLOBAL_DEFINES

LOCAL_DEFINES

EXPORT_TYPEDEFS

LOCAL_TYPEDEFS

FUNCTION_DECLARATIONS

EXPORT_DATA

IMPORT_DATA

LOCAL_DATA



FUNCTION PUBLIC PB_INT16 fmbgdl_get_data_len 
          (
            IN  PB_INT16      result,          /* Service-Result       */
            IN  USIGN8     service,            /* Service              */
            IN  USIGN8     primitive,          /* Service-Primitive    */
            IN  USIGN8 FAR *data_ptr,          /* pointer to data      */
            OUT PB_INT16      *data_len        /* length of data       */
          ) /* nodoc */

/*-----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function is used to return the data length of FMB-SERVICES

possible return values:
- Data-length

-----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

PB_INT16 ret_val = E_OK;
    
FUNCTION_BODY

*data_len = 0;

switch (service)
{    
   case FMB_SET_CONFIGURATION:
        *data_len = sizeof(T_FMB_SET_CONFIGURATION_REQ);
        break;

   case FMB_SET_BUSPARAMETER:
        *data_len = sizeof(T_FMB_SET_BUSPARAMETER_REQ);
        break;

   case FMB_EXIT:              
        *data_len = 0;
        break;
        
   default: 
        ret_val = E_IF_INVALID_SERVICE; 
        break;
}
return(ret_val);
}   

