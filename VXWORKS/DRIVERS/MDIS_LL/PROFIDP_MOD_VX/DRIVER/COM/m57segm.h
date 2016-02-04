/***********************  I n c l u d e  -  F i l e  ************************
 *  
 *         Name: m57segm.h
 *
 *      $Author: AGromann $
 *        $Date: 2014/11/05 10:51:45 $
 *    $Revision: 1.1 $
 * 
 *  Description: Layout of firmware loadfile for M57 module
 *                      
 *     Switches: 
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: m57segm.h,v $
 * Revision 1.1  2014/11/05 10:51:45  AGromann
 * Initial Revision
 *
 * Revision 1.2  2001/02/28 08:18:58  kp
 * added newline at EOF
 *
 * Revision 1.1  2000/02/29 12:05:13  Gromann
 * Initial Revision
 *
 * Revision 1.1  1995/06/02 17:02:50  kp
 * Initial revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1995 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/

#ifndef _M57SEGM_H
#define _M57SEGM_H

/*-----------------------------------------+
| Segment descriptor for M57 download file |
+-----------------------------------------*/
typedef struct {
	u_int32		offset;			/* offset in download file */
	u_int32		length;			/* number of bytes in segment */
	u_int32		loadaddr;		/* destination load address in M57 RAM */
} m57_segm;

/*-----------------------+
| Start of download file |
+-----------------------*/
typedef struct {
	u_int32		sync_code;
#define SYNC_CODE 0xdeadface
	m57_segm	segment[1];		/* segments (more than one possible) */
} m57_download;

#endif /* _M57SEGM_H */
