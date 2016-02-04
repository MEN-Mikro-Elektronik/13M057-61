/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: pb_usr_twist.h
 *
 *       Author: ag
 *        $Date: 2014/11/05 10:52:46 $
 *    $Revision: 1.1 $
 *
 *  Description: set byte ordering big/little endian for PROFIDP driver
 *               in user functions.
 *
 *     Switches: 
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: pb_usr_twist.h,v $
 * Revision 1.1  2014/11/05 10:52:46  AGromann
 * Initial Revision
 *
 * Revision 1.1  2000/03/09 09:59:26  Gromann
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/


#ifdef _LITTLE_ENDIAN_ /* Big / Little ??*/
#define PB_TWISTWORD(w) ((((w)&0xff)<<8) + (((w)&0xff00)>>8))
#define PB_TWISTLONG(l) ((((l)&0xff)<<24) + (((l)&0xff00)<<8) + \
                     (((l)&0xff0000)>>8) + (((l)&0xff000000)>>24))
#else
#define PB_TWISTWORD(w) (w)
#define PB_TWISTLONG(l) (l)
#endif

