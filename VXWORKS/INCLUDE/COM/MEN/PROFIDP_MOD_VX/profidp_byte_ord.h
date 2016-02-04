/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: profidp_byte_ord.h
 *
 *       Author: ag
 *        $Date: 2014/11/05 10:52:43 $
 *    $Revision: 1.1 $
 *
 *  Description: set byte ordering big/little endian for PROFIDP driver
 *
 *     Switches: 
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: profidp_byte_ord.h,v $
 * Revision 1.1  2014/11/05 10:52:43  AGromann
 * Initial Revision
 *
 * Revision 1.2  2001/02/28 08:19:06  kp
 * added newline at EOF
 *
 * Revision 1.1  2000/02/29 12:05:45  Gromann
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _PRORFIDP_BYTE_ORD_H
#define _PRORFIDP_BYTE_ORD_H

#include <MEN/men_typs.h>   /* system dependent definitions   */
/* #include <MEN/maccess.h>     hw access macros and types     */
#include <MEN/dbg.h>        /* debug functions                */

#include <MEN/oss.h>

#ifdef _BIG_ENDIAN_
# define INTEL 0

#endif

#ifdef _LITTLE_ENDIAN_

/* #else */
# define INTEL 1
#endif 

#endif /* _PRORFIDP_BYTE_ORD_H */
