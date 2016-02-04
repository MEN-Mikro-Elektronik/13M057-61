/***********************  I n c l u d e  -  F i l e  ************************
 *  
 *         Name: m57_firm.h
 *
 *       Author: ag
 *        $Date: 2014/11/05 10:51:43 $
 *    $Revision: 1.1 $
 * 
 *  Description: Prototypes for Firmware data array and ident function
 *                      
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: m57_firm.h,v $
 * Revision 1.1  2014/11/05 10:51:43  AGromann
 * Initial Revision
 *
 * Revision 1.3  2001/10/24 16:03:30  kp
 * changed PROFIDP_Firmware_Ident to Firmware_Ident
 *
 * Revision 1.2  2000/03/09 09:58:58  Gromann
 * Changes for Win NT
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/

#ifndef _M57_FIRM_H
#define _M57_FIRM_H

#ifdef __cplusplus
	extern "C" {
#endif

/*--------------------------------------+
|   EXTERNALS                           |
+--------------------------------------*/
extern const u_int8 dp_fw[];

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
char* Firmware_Ident(void);


#ifdef __cplusplus
	}
#endif

#endif	/* _M57_FIRM_H */

