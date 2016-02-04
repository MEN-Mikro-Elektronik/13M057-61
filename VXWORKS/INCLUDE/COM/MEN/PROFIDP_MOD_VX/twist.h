/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: twist.h
 *
 *       Author: ag
 *        $Date: 2014/11/05 10:52:45 $
 *    $Revision: 1.1 $
 *
 *  Description: Intern header file for PROFIDP driver
 *               - word swap routines for Big/Little Endian problem
 *
 *     Switches: 
 *               
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: twist.h,v $
 * Revision 1.1  2014/11/05 10:52:45  AGromann
 * Initial Revision
 *
 * Revision 1.2  2000/06/15 15:08:36  gromann
 * cosmetics
 *
 * Revision 1.1  2000/02/29 12:05:46  Gromann
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _TWIST_H
#define _TWIST_H


#if INTEL /* Big / Little */
#define TWISTWORD(w)  OSS_Swap16(w)    /* ((((w)&0xff)<<8) + (((w)&0xff00)>>8)) */
#define TWISTLONG(l)  OSS_Swap32(l)    /* ((((l)&0xff)<<24) + (((l)&0xff00)<<8) + \     
                     				      (((l)&0xff0000)>>8) + (((l)&0xff000000)>>24)) */
#else
#define TWISTWORD(w) (w)
#define TWISTLONG(l) (l)
#endif

#endif /* _TWIST_H */

