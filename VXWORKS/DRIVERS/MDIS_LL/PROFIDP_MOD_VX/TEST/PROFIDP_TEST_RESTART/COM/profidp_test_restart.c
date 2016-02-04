/****************************************************************************
 ************                                                    ************
 ************                   PROFIDP_TEST_RESTART             ************
 ************                                                    ************
 ****************************************************************************
 *  
 *       Author: ag
 *        $Date: 2014/11/05 10:52:26 $
 *    $Revision: 1.1 $
 *
 *  Description: Test program to restart PROFI-Bus Stack cyclically
 *
 *                      
 *     Required: libraries: mdis_api
 *     Switches: VXWORKS
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: profidp_test_restart.c,v $
 * Revision 1.1  2014/11/05 10:52:26  AGromann
 * Initial Revision
 *
 * Revision 1.2  2010/11/16 11:24:02  CKauntz
 * R: Ported to MDIS5
 * M: added support for 64 bit
 *
 * Revision 1.1  2003/01/08 15:37:12  agromann
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/
 
static const char RCSid[]="$Id: profidp_test_restart.c,v 1.1 2014/11/05 10:52:26 AGromann Exp $";

#include <stdio.h>
#include <string.h>
#include <MEN/men_typs.h>
#include <MEN/usr_oss.h>
#include <MEN/mdis_api.h>
#include <MEN/profidp_mod_vx_drv.h>

# include "dp_config_test_restart.h"

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
static int _profidp_test_restart( char *devName, int32 ch );
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
		printf("Syntax: profidp_test_restart <device> \n");
		printf("Function: Test program to restart PROFI-Bus Stack cyclically\n");
		printf("Options:\n");
		printf("    device       device name\n");
		printf("\n");
		return(1);

	}
	
	device = argv[1];

	chan = 2; /* write to channel 2 */

	printf("\nPROFIDP-Test program:\n Device name: %s\n Cannel: %01ld\n", argv[1], chan);

	_profidp_test_restart( argv[1],  chan);


	return(0);
}


/******************************* _profidp_test_restart ************************
 *
 *  Description:  Opens the device and cyclically restarts stack
 *
 *---------------------------------------------------------------------------
 *  Input......:  devName device name in the system e.g. "/m57/0"
 *                ch      channel to wirte to
 *  Output.....:  return  0 => Ok or 1 => Error
 *
 *  Globals....:  ---
 *
 ****************************************************************************/
static int _profidp_test_restart( char *devName, int32 ch )
{
	MDIS_PATH       path;
    int32	        chan; 
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

	printf("\nPress any key to stop");

	count++;

	printf("\nM_open count = %ld", count);
	if ((path = M_open(device)) < 0) {
		PrintError("open");
		return(1);
	}

	/* initialize blk with config data */
	/* DP_config_simp_mod_vx[] is defined in dp_config_simp.h */
	blk.data = (void *) DP_config_test_restart_mod_vx; 
	blk.size = sizeof (DP_config_test_restart_mod_vx);

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
	if ((M_setstat(path, PROFIDP_BLK_CONFIG, (int32) &blk)) < 0) {
		PrintError("setstat PROFIDP_BLK_CONFIG");
		goto abort;
	}

	while ( UOS_KeyPressed() == -1 ) {

		/* Start Profibus protocol stack */
		printf("\nStart Profibus protocol stack\n");
    	if ((M_setstat(path, PROFIDP_START_STACK, 0)) < 0) {
    		PrintError("setstat PROFIDP_START_STACK");
			goto abort;
		}
	}	/* while */
		/* get FMB_FM2_EVENT reason */
		printf("\nGet FMB_FM2_EVENT reason\n");
		if ((M_getstat(path, PROFIDP_FM2_REASON, &fmbReason)) < 0) {
    		PrintError("setstat PROFIDP_FM2_REASON");
			goto abort;
		}
		printf("\nFMB_FM2_EVENT reason = %08lx\n", fmbReason);

		/* channel number */
		if ((M_setstat(path, M_MK_CH_CURRENT, 2)) < 0) {
			PrintError("setstat M_MK_CH_CURRENT");
			goto abort;
		}
	
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
		printf("\n STATION 2:\n  station_status_1 = %02x\n  station_status_2 = %02x\n"
			   "  station_status_3 = %02x\n  master_add = %02x\n  ident_number = %04x\n",
			   diag->station_status_1,  diag->station_status_2, diag->station_status_3,
			   diag->master_add, PB_TWISTWORD(diag->ident_number));



		/* channel number */
		if ((M_setstat(path, M_MK_CH_CURRENT, 3)) < 0) {
			PrintError("setstat M_MK_CH_CURRENT");
			goto abort;
		}
	
		/* write data to slave */
		if ((M_setblock(path, (u_int8*) outBuf1, (int32) 1)) < 0) {
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
		printf("\nSTATION 3:\n  station_status_1 = %02x\n  station_status_2 = %02x\n"
			   "  station_status_3 = %02x\n  master_add = %02x\n  ident_number = %04x\n",
			   diag->station_status_1,  diag->station_status_2, diag->station_status_3,
			   diag->master_add, PB_TWISTWORD(diag->ident_number));

		UOS_Delay(100); /* Delay for 100 ms */



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
