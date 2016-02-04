/****************************************************************************
 ************                                                    ************
 ************             PROFIDP_TEST_ENDPRUF                   ************
 ************                                                    ************
 ****************************************************************************
 *  
 *       Author: ag
 *        $Date: 2014/11/05 10:52:23 $
 *    $Revision: 1.1 $
 *
 *  Description: Test program for the PROFIDP driver
 *               Wirte values to slave with cyclic data transfer mode
 *               For final inspection purpose
 *                      
 *     Required: libraries: mdis_api
 *     Switches: VXWORKS
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: profidp_test_endpruf.c,v $
 * Revision 1.1  2014/11/05 10:52:23  AGromann
 * Initial Revision
 *
 * Revision 1.3  2010/11/16 11:24:00  CKauntz
 * R:1. Ported to MDIS5
 *   2. Wrong calling convention for SigHandler
 * M:1. added support for 64 bit
 *   2. Fixed calling convention
 *
 * Revision 1.2  2000/07/03 15:19:27  gromann
 * removed SetStat to enable interrupt
 *
 * Revision 1.1  2000/06/15 15:08:27  gromann
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/
 
static const char RCSid[]="$Id: profidp_test_endpruf.c,v 1.1 2014/11/05 10:52:23 AGromann Exp $";

#include <stdio.h>
#include <string.h>
#include <MEN/men_typs.h>
#include <MEN/usr_oss.h>
#include <MEN/mdis_api.h>
#include <MEN/mdis_err.h>
#include <MEN/profidp_mod_vx_drv.h>

# include "dp_config_endpruf.h"

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
MDIS_PATH path;
int16      profidp_mod_vx_test_endpruf_event = 0;

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
static int _profidp_test_endpruf( char *devName, int32 ch );
static void PrintError(char *info);
static void __MAPILIB SigHandler(u_int32 sigCode);
static void EventHandler(void);


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
	u_int32 ch;
	
	if (argc < 2 || strcmp(argv[1],"-?")==0) {
		printf("Syntax: profidp_test <device> \n");
		printf("Function: PROFIDP test for writing a value to a channel in cyclic mode\n");
		printf("Options:\n");
		printf("    device       device name\n");
		printf("\n");
		return(1);
	}
	
	ch = 2; /* write to channel 2 */

	printf("\nPROFIDP-Test program:\n Device name: %s\n Cannel: %01ld\n", argv[1], ch);

	_profidp_test_endpruf( argv[1],  ch);
	return (0);
}

/******************************* _profidp_test *********************************
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
static int _profidp_test_endpruf( char *devName, int32 ch )
{
	int32	/*path,*/ chan, fmbReason; 
	u_int32 va;
	char	*device;
	M_SG_BLOCK blk;
	u_int8          conBuf[DP_MAX_TELEGRAM_LEN];	
	T_DP_DIAG_DATA*           diag;
	u_int8     outBuf1[] = {0x0f, 0x0f, 0x0f, 0x0f};
	u_int8     outBuf2[] = {0xf0, 0xf0, 0xf0, 0xf0};
	int32      maxInOut;

	
	device = devName;
	chan = ch;
	va = 0;

	/* Init signal */
	UOS_SigInit(SigHandler);
	UOS_SigInstall(UOS_SIG_USR1);
	

	/*--------------------+
    |  open path          |
    +--------------------*/
	printf("\nM_open");
	if ((path = M_open(device)) < 0) {
		PrintError("open");
		return(1);
	}

	/* install Signal Remove signal + UOS Funktionen ?? */
	if ((M_setstat(path, PROFIDP_SIG_ON_EVENT_SET, UOS_SIG_USR1)) < 0) {
		PrintError("setstat PROFIDP_SIG_ON_EVENT");
		goto abort;
	}
	

	/* initialization of blk structure */
	blk.data = (void *) DP_config_test_endpruf_mod_vx;

	printf("\nSize of DP_config_test = %08lx", sizeof(DP_config_test_endpruf_mod_vx));

	blk.size = sizeof (DP_config_test_endpruf_mod_vx);


	/*--------------------+
    |  config             |
    +--------------------*/
	printf("\nconfiguration - M_setstat\n");
	
	/* channel number */
	if ((M_setstat(path, M_MK_CH_CURRENT, 2)) < 0) {
		PrintError("setstat M_MK_CH_CURRENT");
		goto abort;
	}
	
	/* configurate profibus */
	printf("\nStart Profibus coniguration\n");
    if ((M_setstat(path, PROFIDP_BLK_CONFIG, (int32) &blk)) < 0) {
    	PrintError("setstat PROFIDP_BLK_CONFIG");
		goto abort;
	}

	/* get max slave input length */
	printf("\nGet max slave input length\n");
    if ((M_getstat(path, PROFIDP_MAX_INPUT_LEN, &maxInOut)) < 0) {
    	PrintError("setstat PROFIDP_MAX_INPUT_LEN");
		goto abort;
	}
	printf("\nMax slave input length = %08lx\n", maxInOut);

	/* get max slave output length */
	printf("\nGet max slave output length\n");
    if ((M_getstat(path, PROFIDP_MAX_OUTPUT_LEN, &maxInOut)) < 0) {
    	PrintError("setstat PROFIDP_MAX_OUTPUT_LEN");
		goto abort;
	}
	printf("\nMax slave output length = %08lx\n", maxInOut);

	/* Start Profibus protocol stack */
	printf("\nStart Profibus protocol stack\n");
    if ((M_setstat(path, PROFIDP_START_STACK, 0)) < 0) {
    	PrintError("setstat PROFIDP_START_STATCK");
		goto abort;
	}

	UOS_Delay( 500 );
	
	/* initialization of blk structure */
	blk.data = (void *) conBuf;
	blk.size = sizeof (conBuf);
	
	printf("\nPress any key to stop\n\n");

	while ( UOS_KeyPressed() == -1 ) {
		/* If signal was sent call functions to determin cause of signal */
		if (profidp_mod_vx_test_endpruf_event != 0) {
			/* get FMB_FM2_EVENT reason */
			if ((M_getstat(path, PROFIDP_FM2_REASON, &fmbReason)) < 0) {
    			PrintError("setstat PROFIDP_FM2_REASON");
				goto abort;
			}
			/* get slave diag for current channel and save them in conBuf */
			if ((M_getstat(path, PROFIDP_BLK_GET_SLAVE_DIAG, (int32*) &blk)) < 0) {
				PrintError("getstat PROFIDP_GET_SLAVE_DIAG");
				goto abort;				
			}
		diag = (T_DP_DIAG_DATA*) conBuf;

        if ( diag->station_status_1 && !(diag->station_status_1 == 1) )
            printf("                    \rSlave indicates error\r");
            
        if ( diag->station_status_1 == 1 )
            printf("                    \rSlave not pressent\r");
        
        if ( diag->station_status_2 == 0xc && diag->station_status_1 == 0 )
            printf("                    \rDatatransfer ok\r");

		profidp_mod_vx_test_endpruf_event = 0;
		
		}
		
		/* write outBuf1 to slave */
		if ((M_setblock(path, (u_int8*) outBuf1, (int32) sizeof(outBuf1))) < 0) {
			PrintError("setstat PROFIDP_BLK_SET_DATA");
			goto abort;
		}

		/* Wait for 1 sec. */
		UOS_Delay( 1000 );

		/* write outBuf2 to slave */
    	if ((M_setblock(path, (u_int8*) outBuf2, (int32) sizeof(outBuf2))) < 0) {
    		PrintError("setstat PROFIDP_BLK_SET_DATA");
			goto abort;
		}
	
		/* Wait for 1 sec. */
		UOS_Delay( 1000 );
	}

	printf("\nStop Profibus protocol stack\n");
    /* stop protocol stack */
    if ((M_setstat(path, PROFIDP_STOP_STACK, 0)) < 0) {
    	PrintError("setstat PROFIDP_BLK_STOP_STACK");
		goto abort;
	}
	
	/* remove signal */
	if ((M_setstat(path, PROFIDP_SIG_ON_EVENT_CLR, UOS_SIG_USR1)) < 0) {
		PrintError("setstat PROFIDP_SIG_ON_EVENT");
		goto abort;
	}
	
	/* Uninstall signal */
	UOS_SigExit();
	

	/*--------------------+
    |  close path         |
    +--------------------*/
	
	printf("\nM_close\n\nTest finished without error\n");
	
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



/********************************* SigHandler *******************************
 *
 *  Description: Handle Signals send from M57-Module driver
 *			   
 *---------------------------------------------------------------------------
 *  Input......: sigCode	info string
 *  Output.....: -
 *  Globals....: -
 ****************************************************************************/

static void __MAPILIB SigHandler(u_int32 sigCode)
{
	
	switch(sigCode) {
		case UOS_SIG_USR1: EventHandler();   /* Handle event form Module */
		break;
		
		default:
			printf(" *** SigHandler: Wrong signal code occured \n");
			break;
			
	}

}

/********************************* EventHandler *******************************
 *
 *  Description: Handle Irq Signal UOS_SIG_USR1
 *			   
 *---------------------------------------------------------------------------
 *  Input......: -	  
 *  Output.....: -
 *  Globals....: -
 ****************************************************************************/
static void EventHandler(void)

{
	
	/* Signal event to main routine */
	profidp_mod_vx_test_endpruf_event = 1;
	
}

