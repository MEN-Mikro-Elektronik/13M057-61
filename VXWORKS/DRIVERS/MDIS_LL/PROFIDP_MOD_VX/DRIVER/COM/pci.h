/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: pci.h
 *
 *       Author: ag
 *        $Date: 2014/11/05 10:51:37 $
 *    $Revision: 1.1 $
 *
 *  Description: Intern header file for PROFIDP driver
 *               - prototypes for pci.c
 *               
 *
 *     Switches: 
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: pci.h,v $
 * Revision 1.1  2014/11/05 10:51:37  AGromann
 * Initial Revision
 *
 * Revision 1.3  2001/02/28 08:18:51  kp
 * added newline at EOF
 *
 * Revision 1.2  2000/03/09 09:58:53  Gromann
 * Changes for Win NT
 *
 * Revision 1.1  2000/02/29 12:05:04  Gromann
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _PCI_H
#define _PCI_H

FUNCTION GLOBAL PB_INT16 CALL_CONV profi_init
        (
			LL_HANDLE  *llHdl
          /*
          IN  USIGN32  dpr_address_a08,
          IN  USIGN32  dpr_address_a24
          */
        );
        
FUNCTION GLOBAL PB_INT16 CALL_CONV profi_end(VOID);


FUNCTION GLOBAL PB_INT16 CALL_CONV profi_snd_req_res
         (
			             LL_HANDLE     *llHdl,
          IN T_PROFI_SERVICE_DESCR FAR *sdb_ptr,
          IN VOID                  FAR *data_ptr,
          IN PB_BOOL                   dummy
         );

FUNCTION GLOBAL PB_INT16 CALL_CONV profi_rcv_con_ind
         (
			                LL_HANDLE      *llHdl,
          OUT   T_PROFI_SERVICE_DESCR  FAR *sdb_ptr,
          INOUT VOID                   FAR *data_ptr,
          INOUT USIGN16                FAR *data_len
         );
         
FUNCTION GLOBAL PB_INT16 CALL_CONV profi_rcv_con_ind_wait
         (
          					LL_HANDLE      *llHdl,
          OUT   T_PROFI_SERVICE_DESCR  FAR *sdb_ptr,
          INOUT VOID                   FAR *data_ptr,
          INOUT USIGN16                FAR *data_len
         );
         
FUNCTION GLOBAL PB_INT16 CALL_CONV profi_set_data
        (
		   LL_HANDLE   *llHdl,
          IN  USIGN8   data_id,      /* id of data           */
          IN  USIGN16  offset,       /* offset in data descr */
          IN  USIGN16  data_size,    /* size of data         */
          IN  VOID FAR *data_ptr     /* pointer to data      */
        );
        
FUNCTION GLOBAL PB_INT16 CALL_CONV profi_get_data
        (
			  LL_HANDLE       *llHdl,
          IN     USIGN8       data_id,      /* id of data              */
          IN     USIGN16      offset,       /* offset in data descr    */
          INOUT  USIGN16  FAR *data_size,   /* pointer to size of data */
          INOUT  VOID     FAR *data_ptr     /* pointer to data         */
        );
        
#endif /* _PCI_H */
