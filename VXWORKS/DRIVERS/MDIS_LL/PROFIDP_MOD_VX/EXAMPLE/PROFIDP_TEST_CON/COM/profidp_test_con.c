/****************************************************************************
 ************                                                    ************
 ************                PROFIDP_TEST_CON                    ************
 ************                                                    ************
 ****************************************************************************
 *
 *       Author: ag
 *        $Date: 2014/11/05 10:52:15 $
 *    $Revision: 1.1 $
 *
 *  Description: Test program for the PROFIDP driver
 *
 *               Write values to slave with controlled data transfer
 *
 *     Required: libraries: mdis_api
 *     Switches: VXWORKS
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: profidp_test_con.c,v $
 * Revision 1.1  2014/11/05 10:52:15  AGromann
 * Initial Revision
 *
 * Revision 1.13  2011/11/14 11:16:29  agromann
 * R: Error during setstat PROFIDP_SET_STACK_OPERATE under Linux (User
 * signal was recognized as deadly signal).
 * M: Now call setstat PROFIDP_SIG_ON_EVENT_SET after PROFIDP_SET_STACK_OPERATE
 * and before PROFIDP_STOP_STACK.
 *
 * Revision 1.12  2010/11/16 11:23:55  CKauntz
 * R: Compiler warning for while 1 and wrong data type
 * M: a) Replaced while 1 by variable wait4ever
 *    b) Added cast
 *
 * Revision 1.11  2010/10/07 00:49:03  cs
 * R: not compilable for MDIS5
 * M: move include of usr_oss.h after include of mdis_api.h
 *
 * Revision 1.10  2010/10/06 23:20:27  cs
 * R: ported driver to current men_typs and MDIS_API
 * M: worked in MDIS4/MDIS5 porting guide version 0.9
 *
 * Revision 1.9  2005/03/24 15:10:22  AGromann
 * clear profidp_test_con_event which was set because of manual
 * DP_DATA_TRANSFER REQ + CON
 *
 * Revision 1.8  2004/05/05 08:25:35  cs
 * changed functions to static, only prototypes
 *
 * Revision 1.7  2000/08/30 15:29:38  gromann
 * cosmetics
 *
 * Revision 1.6  2000/07/28 13:33:23  gromann
 * cosmetics
 *
 * Revision 1.5  2000/07/18 09:12:39  gromann
 * cosmetics
 *
 * Revision 1.4  2000/07/03 15:19:23  gromann
 * removed SetStats to enable/disable interrupt
 *
 * Revision 1.3  2000/06/15 15:08:19  gromann
 * cosmetics
 *
 * Revision 1.2  2000/03/09 09:59:14  Gromann
 * Changes for Win NT
 *
 * Revision 1.1  2000/02/29 12:05:25  Gromann
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Id: profidp_test_con.c,v 1.1 2014/11/05 10:52:15 AGromann Exp $";

#include <stdio.h>
#include <string.h>
#include <MEN/men_typs.h>
#include <MEN/mdis_api.h>
#include <MEN/mdis_err.h>
#include <MEN/usr_oss.h>
#include <MEN/profidp_mod_vx_drv.h>

# include "dp_config_test_con.h"

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
static MDIS_PATH path;
static u_int8 profidp_test_con_event = 0;

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
static int _profidp_test_con( char *devName, int32 ch );
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
		printf("Syntax: profidp_test_con <device> \n");
		printf("Function: PROFIDP test for writing a value to a slave in controlled mode\n");
		printf("Options:\n");
		printf("    device       device name\n");
		printf("\n");
		return(1);
	}

	ch = 0; /* Write to channel 2 */

	printf("\nPROFIDP-Test program:\n Device name: %s\n Cannel: %01ld\n", argv[1], ch);

	_profidp_test_con( argv[1],  ch);
	return (0);
}

/**************************** _profidp_test_con ***************************
 *
 *  Description:  Opens the device and writes values to a channel.
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  devName device name in the system e.g. "/m57/0"
 *                ch      channel to read form
 *  Output.....:  return  0 => Ok or 1 => Error
 *
 *  Globals....:  ---
 *
 ****************************************************************************/
static int _profidp_test_con( char *devName, int32 ch )
{
	int32	/*path,*/ chan;
	u_int32 va;
	char	*device;
	M_SG_BLOCK blk;
	u_int8          conBuf[DP_MAX_TELEGRAM_LEN];
	u_int8          dataBuf[DP_MAX_TELEGRAM_LEN + sizeof(T_PROFI_SERVICE_DESCR)];
	int16           numDiag;
    T_DP_GET_SLAVE_DIAG_CON*  diagP;
	T_DP_DATA_TRANSFER_CON*   dataTrans;
	T_DP_DIAG_DATA*           diagData;
	u_int8     outBuf1[] = {0x0f, 0x0f, 0x0f, 0x0f};
	u_int8     outBuf2[] = {0xf0, 0xf0, 0xf0, 0xf0};
	u_int8 Puffer[CON_IND_BUF_ELEMENT_SIZE];

	T_PROFI_SERVICE_DESCR*  sdb;
	T_PROFI_SERVICE_DESCR   t_sdb;
	T_FMB_FM2_EVENT_IND*    fm2;
	int32                   maxInOut;
    int8                    wait4ever = 1;


	device = devName;
	chan = ch;
	va = 0;


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

	/* dummy initialization of blk */
	blk.data = (void *) DP_config_test_con_mod_vx;

	printf("\nSize of DP_config_test_con = %04lx", sizeof(DP_config_test_con_mod_vx));

	blk.size = sizeof (DP_config_test_con_mod_vx);


	/*--------------------+
    |  config             |
    +--------------------*/
	printf("\nconfiguration - M_setstat\n");

	/* channel number */
	if ((M_setstat(path, M_MK_CH_CURRENT, 2)) < 0) {
		PrintError("setstat M_MK_CH_CURRENT");
		goto abort;
	}


	/* configurate profibus module */
	printf("\nStart Profibus configuration\n");
    if ((M_setstat(path, PROFIDP_BLK_CONFIG, (int32) &blk)) < 0) {
    	PrintError("setstat PROFIDP_BLK_CONFIG");
		goto abort;
	}

	/* get max slave input length */
	printf("\nGet max slave input length\n");
    if ((M_getstat(path, PROFIDP_MAX_INPUT_LEN, &maxInOut)) < 0) {
    	PrintError("getstat PROFIDP_MAX_INPUT_LEN");
		goto abort;
	}
	printf("\nMax slave input length = %08lx\n", maxInOut);

	/* get max slave output length */
	printf("\nGet max slave output length\n");
    if ((M_getstat(path, PROFIDP_MAX_OUTPUT_LEN, &maxInOut)) < 0) {
    	PrintError("getstat PROFIDP_MAX_OUTPUT_LEN");
		goto abort;
	}
	printf("\nMax slave output length = %08lx\n", maxInOut);


      /* set protocol stack to STOP */
	printf("\nSet protocol stack to STOP\n");
    if ((M_setstat(path, PROFIDP_SET_STACK_STOP, 0)) < 0) {
    	PrintError("setstat PROFIDP_SET_STACK_STOP");
		goto abort;
	}


	/* set protocol stack to CLEAR */
	printf("\nSet protocol stack to CLEAR\n");
    if ((M_setstat(path, PROFIDP_SET_STACK_CLEAR, 0)) < 0) {
    	PrintError("setstat PROFIDP_SET_STACK_CLEAR");
		goto abort;
	}

	/* set protocol stack to OPERATE */
	printf("\nSet protocol stack to OPERATE\n");
    if ((M_setstat(path, PROFIDP_SET_STACK_OPERATE, 0)) < 0) {
    	PrintError("setstat PROFIDP_SET_STACK_OPERATE");
		goto abort;
	}

	/* install Signal */
	if ((M_setstat(path, PROFIDP_SIG_ON_EVENT_SET, UOS_SIG_USR1)) < 0) {
		PrintError("setstat PROFIDP_SIG_ON_EVENT");
		goto abort;
	}

	blk.data = (void *) conBuf;
	blk.size = sizeof (conBuf);

    if ((M_setblock(path, (u_int8*) outBuf2, (int32) sizeof(outBuf2))) < 0) {
   	PrintError("setblock");
	goto abort;
	}


	while ( UOS_KeyPressed() == -1 ) {

		printf("\nPress any key to stop");

	 	/* initialization of blk structure */
		blk.data = (void *) conBuf;
		blk.size = sizeof (conBuf);

		/* do data transfer using PROFIDP_BLK_DATA_TRANSFER */
		printf("\nStart Profibus data transfer\n");
		if ((M_setstat(path, PROFIDP_BLK_DATA_TRANSFER, (int32) &blk)) < 0) {
			PrintError("setstat PROFIDP_BLK_DATA_TRANSFER");
			goto abort;
		}

		dataTrans = (T_DP_DATA_TRANSFER_CON*) conBuf;
		numDiag = (int16) PB_TWISTWORD(dataTrans->diag_entries);

		diagP = (T_DP_GET_SLAVE_DIAG_CON*) conBuf;
		diagData = (T_DP_DIAG_DATA*) (diagP + 1);

		/* get diag if new diag is available */
		if ( numDiag != 0) {
			printf("\n %04x Diag entries available", numDiag);
				do {
				printf("\nGet diag data\n");
			    if ((M_getstat(path, PROFIDP_BLK_GET_DIAG, (int32*) &blk)) < 0) {
					PrintError("setstat PROFIDP_BLK_GET_DIAG");
					goto abort;
				}

				printf("\nDiag from Station %02x status = %04x", diagP->rem_add, PB_TWISTWORD(diagP->status));
				printf("\n  station_status_1 = %02x\n  station_status_2 = %02x\n  station_status_3 = %02x"
		   			   "\n  master_add = %02x\n  ident_number = %04x\n", diagData->station_status_1,
		    		   diagData->station_status_2, diagData->station_status_3, diagData->master_add,
		    		   PB_TWISTWORD(diagData->ident_number));

				if ( diagData->station_status_1 )
					printf("\nSlave indicates ERROR");


				} while (PB_TWISTWORD(diagP->diag_entries) > 0);
		}

		if (profidp_test_con_event == 1) {

			/* initialization of blk structure */
			blk.data = (void*) Puffer;
			blk.size = sizeof(Puffer);

			printf("\nRead asynchronous CON/IND\n");
			if ((M_setstat(path, PROFIDP_BLK_RCV_CON_IND, (int32) &blk)) < 0) {
				PrintError("setstat PROFIDP_BLK_RCV_CON_IND");
				profidp_test_con_event = 0;
				/* if no CON/IND is found in buffer don't read it */
				goto ahead;
			}

			sdb = (T_PROFI_SERVICE_DESCR*) Puffer;
			fm2 = (T_FMB_FM2_EVENT_IND*) (Puffer + sizeof (T_PROFI_SERVICE_DESCR));
			printf("\nAsynch. CON/IND:\n sdb->layer     = %02x\n sdb->service   = %02x\n sdb->primitive = %02x",
			       sdb->layer, sdb->service, sdb->primitive);
			printf("\nReason = %04x", PB_TWISTWORD(fm2->reason));

			profidp_test_con_event = 0;
		}

		ahead:
		/* initialization of blk structure */
		blk.data = (void*) outBuf1;
		blk.size = sizeof(outBuf1);

		/* set all channels to outBuf1*/
		printf("\nProfi set all channels\n");
		if ((M_setstat(path, PROFIDP_BLK_SET_ALL_CH, (int32) &blk)) < 0) {
    		PrintError("setstat PROFIDP_BLK_SET_ALL_CH");
			goto abort;
		}

		UOS_Delay( 1000 );

		/* do data transfer useing PROFIDP_BLK_SEND_REQ_RES and PROFIDP_BLK_RCV_CON_IND_WAIT */
		t_sdb.comm_ref  = PB_TWISTWORD(0);
		t_sdb.layer     = DP;
		t_sdb.service   = DP_DATA_TRANSFER;
		t_sdb.primitive = REQ;

		blk.data = (void*) &t_sdb;
		blk.size = sizeof(T_PROFI_SERVICE_DESCR);

		printf("\nSend request\n");
		/* send request */
	    if ((M_setstat(path, PROFIDP_BLK_SEND_REQ_RES, (int32) &blk)) < 0) {
			PrintError("setstat PROFIDP_BLK_SEND_REQ_RES");
			goto abort;
		}

   		/* initialization of blk structure */
		blk.data = (void *) dataBuf;
		blk.size = sizeof (dataBuf);


		/* wait for confirmation */
		if ((M_setstat(path, PROFIDP_BLK_RCV_CON_IND_WAIT, (int32) &blk)) < 0) {
  	 		PrintError("setstat PROFIDP_BLK_RVC_CON_IND_WAIT");
			goto abort;
		}

		sdb = (T_PROFI_SERVICE_DESCR*) dataBuf;
		printf("\nSingle Data Transfer:\n sdb->layer     = %02x\n sdb->service   = %02x\n sdb->primitive = %02x",
		       sdb->layer, sdb->service, sdb->primitive);
		dataTrans = (T_DP_DATA_TRANSFER_CON*) (dataBuf + sizeof(T_PROFI_SERVICE_DESCR));
		printf("\nSingle dataTrans: %04x Diag entries available", PB_TWISTWORD(dataTrans->diag_entries));

		/* clear profidp_test_con_event which was set because of */
		/* manual request                                        */
		profidp_test_con_event = 0;

		UOS_Delay( 1000 );

		/* initialization of blk structure */
		blk.data = (void *) conBuf;
		blk.size = sizeof (conBuf);

		/* set channel number 2 to outBuf2 */
		if ((M_setblock(path, (u_int8*) outBuf2, (int32) sizeof(outBuf2))) < 0) {
 	  		PrintError("setblock");
			goto abort;
		}

	}

	printf("\n Press ESC to close");
	while ( wait4ever ) {
		if( UOS_KeyPressed() == 27 /*ESC*/ )
			break;
	}

	/* remove signal */
	if ((M_setstat(path, PROFIDP_SIG_ON_EVENT_CLR, UOS_SIG_USR1)) < 0) {
		PrintError("setstat PROFIDP_SIG_ON_EVENT");
		goto abort;
	}

    /* stop protocol stack */
	printf("\nStop Profibus protocol stack\n");
    if ((M_setstat(path, PROFIDP_STOP_STACK, 0)) < 0) {
    	PrintError("setstat PROFIDP_STOP_STACK");
		goto abort;
	}


	/*--------------------+
    |  close path         |
    +--------------------*/

	/* Uninstall signal */
	UOS_SigExit();

	printf("\nM_close\n");

	/* close path */
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
	profidp_test_con_event = 1;

}








