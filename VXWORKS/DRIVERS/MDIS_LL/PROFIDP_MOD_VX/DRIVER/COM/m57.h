/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: m57.h
 *
 *      $Author: AGromann $
 *        $Date: 2014/11/05 10:51:42 $
 *    $Revision: 1.1 $
 *
 *  Description: Register macros (seen from host side) of the M57 module
 *
 *     Switches:
 *				 VOLATILE - should be set to "volatile" if ANSI-C compiler
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: m57.h,v $
 * Revision 1.1  2014/11/05 10:51:42  AGromann
 * Initial Revision
 *
 * Revision 1.6  2010/11/16 11:23:49  CKauntz
 * R:1. DP_DATA_B_READ/WRITE macro modified for use of additional offset
 *   2. Compiler warning for M57_IRQ_DISABLE, M57_RESET
 * M:1. Fixed by adding additional value to offset in macro
 *   2. Added braces
 *
 * Revision 1.5  2010/10/06 23:10:38  cs
 * R: driver was not compilable for MPC85XX
 * M: adapted calls to MACCESS macros to consider current macro definitios
 *
 * Revision 1.4  2000/06/28 10:02:36  gromann
 * added cast in DP_READ_INT8 and DP_WRITE_INT8
 *
 * Revision 1.3  2000/06/16 13:28:46  gromann
 * adapted DP_SET_WINDOW and DP_WRITE_INT32 for use with oss.h version 1.24
 *
 * Revision 1.2  2000/06/15 15:05:34  gromann
 * added swaped functionality
 *
 * Revision 1.1  2000/02/29 12:05:08  Gromann
 * Initial Revision
 *
 * module accesses are now made with MREAD/MWRITE because of MDIS4
 * Revision 2.1  1998/01/07 17:34:48  kp
 * Replaced all far statements with FAR (due to change in typdefs.h)
 * (RCS to CVS mega-checkin)
 *
 * Revision 2.0  1996/05/28 15:48:25  kp
 * adapted to INTEL byteordering.
 *
 * Revision 1.1  96/01/09  10:24:16  kp
 * Initial revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1995 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

/*
   M57 register layout (motorola byteordering view)

   Address  Size		Access		Function
   -------  ---------- 	-------		--------
   00..7f   Byte/Word	Read/Write	128 Byte Window in Dual Ported RAM

   80       Word		Read/Write	Data access Data in dual ported RAM
                                    increment window pointer by 2

   a0       Long        Write		Window pointer

   fe		Byte		Write		Bit 4: issue IRQ to M57
                                    Bit 5: extended mode 0=A20 1=A16
                                    Bit 6: reset/enable pending IRQ from M57
   ff		Byte		Write		M-Module ID EEPROM
                                    CS resets 331 on M57

   In A024 mode, any address of the DPRAM can be directly accessed
*/

#ifndef MAC_BYTESWAP

#if INTEL /* Big / Little */
# define CNTR_REG	0xff
# define ID_REG		0xfe
#else /* INTEL */
# define CNTR_REG	0xfe
# define ID_REG		0xff
#endif /* INTEL */

#else /* MAC_BYTESWAP */

#if INTEL /* Big / Little */
# define CNTR_REG	0xfe
# define ID_REG		0xff
#else /* INTEL */
# define CNTR_REG	0xff
# define ID_REG		0xfe
#endif /* INTEL */

#endif /* MAC_BYTESWAP */


/*---------------------------------------------------------------------+
| Macros for A08 access mode                                           |
+---------------------------------------------------------------------*/
/*----------------------------------------------------------------------
 * DP_SET_WINDOW
 * set the window pointer
 *  base_a08     - base address of M57 in A08 mode
 *  window_start - M57 local start address of window
 */

#define DP_SET_WINDOW( base_a08, window_start )\
		(MWRITE_D16( base_a08, 0xa0, ((window_start>>16) & 0xffff ) )),\
		(MWRITE_D16( base_a08, 0xa2, (window_start & 0xffff) ))
  

/*----------------------------------------------------------------------
 * DP_DATA and access macros
 * access (read or write) <type> at <window_ptr>
 * type must be a byte or word
 */

/* ----------------------- changed ------------------------------------*/
/* because of MDIS4 accesses only with MREAD / MWRITE */

#define DP_DATA_B_READ( base_a08, adOffset ) \
		(MREAD_D8( base_a08, (0x80 + adOffset) ))

#define DP_DATA_W_READ( base_a08 ) \
		(MREAD_D16( base_a08, 0x80 ))



#define DP_DATA_B_WRITE( base_a08, val, adOffset ) \
		(MWRITE_D8( base_a08, (0x80 + adOffset), val ))

#define DP_DATA_W_WRITE( base_a08, val ) \
		(MWRITE_D16( base_a08, 0x80, val ))

#define DP_READ_INT8(base_a08,offs) \
		(DP_SET_WINDOW(base_a08,offs), \
         (DP_DATA_B_READ( base_a08, ((offs&0x1) ? 1 : 0) ) ))

#define DP_WRITE_INT8(base_a08,offs,data)\
		{	DP_SET_WINDOW(base_a08,offs); \
			DP_DATA_B_WRITE( base_a08, data, ((offs & 0x1) ? 1 : 0) );	\
 		}

#define DP_WRITE_INT16(base_a08,offs,data)\
		{ DP_SET_WINDOW(base_a08,offs); DP_DATA_W_WRITE (base_a08, data); }

#define DP_WRITE_INT32(base_a08,offs,data)\
		{ \
			DP_WRITE_INT16(base_a08,offs, ((data>>16) & 0xffff));\
			DP_DATA_W_WRITE(base_a08, (data & 0xffff));\
		}

/*----------------------------------------------------------------------
 * M57_IRQ_TO_MOD
 * Issue interrupt to M57
 */
#define M57_IRQ_TO_MOD( base_a08 ) \
	{ \
		u_int8 tmp2 = MREAD_D8 ( base_a08, CNTR_REG ); \
		tmp2 |= 0x10; \
		MWRITE_D8 ( base_a08, CNTR_REG, tmp2); \
	}


/*--- check if interrupt pending in module ---*/
#define M57_IRQ_TO_MOD_PENDING( base_a08 ) \
		!(MREAD_D8 ( base_a08, CNTR_REG)  & 0x10)

/*----------------------------------------------------------------------
 * M57_IRQ_ENABLE, M57_IRQ_DISABLE, M57_IRQ_ACK
 * Enable/Disable/Acknowledge interrupt from M57
 */
#define M57_IRQ_ENABLE( base_a08 ) \
	{ \
		u_int8 tmp = MREAD_D8( base_a08, CNTR_REG );\
		tmp &= ~0x10;\
		MWRITE_D8( base_a08, CNTR_REG, (tmp | 0x40) );\
	}

#define M57_IRQ_DISABLE( base_a08 )                                 \
    {                                                               \
	MWRITE_D8( base_a08, CNTR_REG,                                  \
        MREAD_D8( base_a08, CNTR_REG ) & (u_int8)(~(0x40|0x10)) );  \
    }

#define M57_IRQ_ACK( base_a08 ) \
	M57_IRQ_DISABLE(base_a08); M57_IRQ_ENABLE(base_a08)


/*----------------------------------------------------------------------
 * M57_RESET
 * reset module (0=Release Reset, 1=Perform Reset)
 */
#define M57_RESET( base_a08, on ) {                        \
    MWRITE_D8( base_a08, ID_REG, (on ? 0x04 : 0x00) );     \
    }






