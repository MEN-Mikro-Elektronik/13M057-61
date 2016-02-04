/****************************************************************************
 ************                                                    ************
 ************                   PROFIDP_SIMP                     ************
 ************                                                    ************
 ****************************************************************************
 *
 *       Author: ag
 *        $Date: 2014/11/05 10:52:06 $
 *    $Revision: 1.1 $
 *
 *  Description: Simple example program for the PROFIDP driver
 *
 *               Write a value to a channel (with cyclic data transfer mode)
 *
 *     Required: libraries: mdis_api
 *     Switches: VXWORKS
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: profidp_simp.c,v $
 * Revision 1.1  2014/11/05 10:52:06  AGromann
 * Initial Revision
 *
 * Revision 1.6  2010/11/16 11:23:51  CKauntz
 * R: M_setstat value not 64 bit compatible
 * M: Added cast for SetStat value
 *
 * Revision 1.5  2010/10/07 00:48:58  cs
 * R: not compilable for MDIS5
 * M: move include of usr_oss.h after include of mdis_api.h
 *
 * Revision 1.4  2010/10/06 23:19:47  cs
 * R: ported driver to current men_typs and MDIS_API
 * M: worked in MDIS4/MDIS5 porting guide version 0.9
 *
 * Revision 1.3  2000/06/15 15:08:06  gromann
 * cosmetics
 *
 * Revision 1.2  2000/03/09 09:59:04  Gromann
 * Changes for Win NT
 *
 * Revision 1.1  2000/02/29 12:05:19  Gromann
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Id: profidp_simp.c,v 1.1 2014/11/05 10:52:06 AGromann Exp $";

#include <stdio.h>
#include <string.h>
#include <MEN/men_typs.h>
#include <MEN/mdis_api.h>
#include <MEN/usr_oss.h>
#include <MEN/profidp_mod_vx_drv.h>

# include "dp_config_simp.h"

# include <MEN/PROFIDP_MOD_VX/pb_type.h>
# include <MEN/PROFIDP_MOD_VX/pb_conf.h>
# include <MEN/PROFIDP_MOD_VX/pb_dp.h>
# include <MEN/PROFIDP_MOD_VX/pb_err.h>
# include <MEN/PROFIDP_MOD_VX/pb_fmb.h>
# include <MEN/PROFIDP_MOD_VX/pb_if.h>
# include <MEN/PROFIDP_MOD_VX/pb_usr_twist.h>


/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
#if defined(_BIG_ENDIAN_) || defined(_LITTLE_ENDIAN_)
#else
#	error "Byte ordering is not set, please make sure that either _BIG_ENDIAN_ or _LITTLE_ENDIAN_ is defined"
#endif

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   EXTERNALS                           |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
static int _profidp_simp( char *devName, int32 ch );
static void PrintError(char *info);


/********************************* main *************************************
 *
 *  Description: Program main function
 *
 *---------------------------------------------------------------------------
 *  Input......: argc,argv	argument counter, data ..
 *  Output.....: return	    success (0) or error (1)
 *  Globals....: -
 ****************************************************************************/

int main(int argc, char *argv[])
{
	int32	chan;
	char	*device;

	if (argc < 2 || strcmp(argv[1],"-?")==0) {
		printf("Syntax: profidp_simp <device> \n");
		printf("Function: PROFIDP example for writing a value to a channel\n");
		printf("Options:\n");
		printf("    device       device name\n");
		printf("\n");
		return(1);

	}

	device = argv[1];

	chan = 2; /* write to channel 2 */

	printf("\nPROFIDP-Test program:\n Device name: %s\n Cannel: %01ld\n", argv[1], chan);

	_profidp_simp( argv[1],  chan);


	return(0);
}


/******************************* _profidp_simple ****************************
 *
 *  Description:  Opens the device and cyclically writes data to a Profibus
 *                slave.
 *
 *---------------------------------------------------------------------------
 *  Input......:  devName device name in the system e.g. "/m57/0"
 *                ch      channel to wirte to
 *  Output.....:  return  0 => Ok or 1 => Error
 *
 *  Globals....:  ---
 *
 ****************************************************************************/
static int _profidp_simp( char *devName, int32 ch )
{
	MDIS_PATH       path;
	int32			chan;
	u_int32         va;
	char            *device;
	M_SG_BLOCK      blk;
	u_int8          conBuf[DP_MAX_TELEGRAM_LEN];
	T_DP_DIAG_DATA* diag;
	u_int8          outBuf1[] = {0x0f, 0x0f, 0x0f, 0x0f};
	int32           fmbReason;
	u_int32         count = 0;

	device = devName;
	chan = ch;
	va = 0;

	/*--------------------+
    |  open path          |
    +--------------------*/
	while ( UOS_KeyPressed() == -1 ) {

		printf("\nPress any key to stop");

		count++;

		printf("\nM_open count = %ld", count);
		if ((path = M_open(device)) < 0) {
			PrintError("open");
			return(1);
		}

		/* initialize blk with config data */
		/* DP_config_simp_mod_vx[] is defined in dp_config_simp.h */
		blk.data = (void *) DP_config_simp_mod_vx;
		blk.size = sizeof (DP_config_simp_mod_vx);

	/*--------------------+
    |  config             |
    +--------------------*/
		printf("\nconfiguration - M_setstat\n");

		printf("\nSet channel number to write to\n");
		/* channel number */
		if ((M_setstat(path, M_MK_CH_CURRENT, chan)) < 0) {
			PrintError("setstat M_MK_CH_CURRENT");
			goto abort;
		}

		/* configure profibus */
		printf("\nConfigure Profibus \n");
    	if ((M_setstat(path, PROFIDP_BLK_CONFIG, (INT32_OR_64) &blk)) < 0) {
    		PrintError("setstat PROFIDP_BLK_CONFIG");
			goto abort;
		}

		/* Start Profibus protocol stack */
		printf("\nStart Profibus protocol stack\n");
    	if ((M_setstat(path, PROFIDP_START_STACK, 0)) < 0) {
    		PrintError("setstat PROFIDP_START_STACK");
			goto abort;
		}

		/* get FMB_FM2_EVENT reason */
		printf("\nGet FMB_FM2_EVENT reason\n");
		if ((M_getstat(path, PROFIDP_FM2_REASON, &fmbReason)) < 0) {
    		PrintError("setstat PROFIDP_FM2_REASON");
			goto abort;
		}
		printf("\nFMB_FM2_EVENT reason = %08lx\n", fmbReason);

		/* write data to slave */
		if ((M_setblock(path, (u_int8*) outBuf1, (int32) sizeof(outBuf1))) < 0) {
    		PrintError("setstat PROFIDP_BLK_SET_DATA");
			goto abort;
		}

		/* get diag information */
		blk.data = (void *) conBuf;
		blk.size = sizeof (conBuf);

		printf("\nGet Slave Diag\n");
		if ((M_getstat(path, PROFIDP_BLK_GET_SLAVE_DIAG, (int32*) &blk)) < 0) {
			PrintError("setstat PROFIDP_GET_SLAVE_DIAG");
			goto abort;
		}

		diag = (T_DP_DIAG_DATA*) conBuf;
		printf("\n  station_status_1 = %02x\n  station_status_2 = %02x\n"
			   "  station_status_3 = %02x\n  master_add = %02x\n  ident_number = %04x\n",
			   diag->station_status_1,  diag->station_status_2, diag->station_status_3,
			   diag->master_add, PB_TWISTWORD(diag->ident_number));

		printf("\nStop Profibus protocol stack\n");
		if ((M_setstat(path, PROFIDP_STOP_STACK, 0)) < 0) {
    		PrintError("setstat PROFIDP_BLK_STOP_STACK");
			goto abort;
		}

	/*--------------------+
    |  close path         |
    +--------------------*/

		printf("\nM_close\n");

		if ( M_close (path) < 0) {
			PrintError("close");
			goto abort;
		}

	}
	return(0);

	/*--------------------+
    |  cleanup            |
    +--------------------*/
	abort:

	if (M_close(path) < 0)
		PrintError("close");

	return(1);
}



/********************************* PrintError *******************************
 *
 *  Description: Print MDIS error message
 *
 *---------------------------------------------------------------------------
 *  Input......: info	info string
 *  Output.....: -
 *  Globals....: -
 ****************************************************************************/
static void PrintError(char *info)
{
	printf("*** can't %s: %s\n", info, M_errstring(UOS_ErrnoGet()));
}


