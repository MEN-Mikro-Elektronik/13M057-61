/****************************************************************************
 ************                                                    ************
 ************                   PROFIDP_TOOL                     ************
 ************                                                    ************
 ****************************************************************************
 *
 *       Author: ag
 *        $Date: 2014/11/05 10:52:20 $
 *    $Revision: 1.1 $
 *
 *  Description: Test tool for the PROFIDP driver.
 *               This program must be used in cyclic data transfer mode.
 *				 I.e. descriptor key CYCLC_DATA_TRANSFER must be TRUE.
 *
 *               The program initializes the profibus with the configuration
 *				 data that is read from a file.
 *
 *				 When nothing is entered by the user, this program displays
 *				 the current slave's diagnostic status and input data
 *				 periodically.
 *
 * 				 The user can however enter the following commands:
 *				   h: display this help
 *				   s: change current slave
 *				   w: modify slave outputs
 *				   e: exit
 *
 *				 Any bus events (FM2) are displayed as well.
 *
 *     Required: libraries: mdis_api, usr_oss
 *     Switches:
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: profidp_tool.c,v $
 * Revision 1.1  2014/11/05 10:52:20  AGromann
 * Initial Revision
 *
 * Revision 2.4  2010/11/16 11:23:58  CKauntz
 * R:1. Compiler warning for while 1 and wrong data type
 *   2. Expression only true for excat one value
 * M:1.a) Replaced while 1 by variable wait4ever
 *     b) Added cast
 *   2. Fixed expression
 *
 * Revision 2.3  2010/10/07 00:49:05  cs
 * R: not compilable for MDIS5
 * M: move include of usr_oss.h after include of mdis_api.h
 *
 * Revision 2.2  2010/10/06 23:19:56  cs
 * R:1. ported driver to current men_typs and MDIS_API
 *   2. sscanf is deprecated in common MDIS code
 * M:1. worked in MDIS4/MDIS5 porting guide version 0.9
 *   2. replace sscanf with strtol
 *
 * Revision 2.1  2001/10/25 11:17:56  kp
 * Adapted to run under VxWorks
 * UOS_KeyPressed() has problems under VxWorks...
 *
 * Revision 2.0  2001/10/24 16:03:33  kp
 * Completely rewritten
 *
 * Revision 1.2  2001/02/28 08:19:04  kp
 * include <stdlib.h>
 *
 * Revision 1.1  2000/08/30 15:29:41  gromann
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2001 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Id: profidp_tool.c,v 1.1 2014/11/05 10:52:20 AGromann Exp $";

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <MEN/men_typs.h>
#include <MEN/mdis_api.h>
#include <MEN/mdis_err.h>
#include <MEN/usr_oss.h>
#include <MEN/profidp_mod_vx_drv.h>

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

#define CHK(expression) \
 if( !(expression)) {\
	 printf("*** Error during: %s\nfile %s\nline %d\n", \
      #expression,__FILE__,__LINE__);\
      printf("\n%s\n",M_errstring(UOS_ErrnoGet()));\
     goto abort;\
 }

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
static int _profidp_tool( char *devName, char *fileName );
static void PrintHelp( void );
static void ChangeSlave( MDIS_PATH path, int32 slave );
static void DisplaySlave( MDIS_PATH path, int32 slave );
static void InputSlaveNum( int32 *slaveP );
static u_int8 GetHexByte(u_int8 def);
static void ChangeSlaveOutputs(MDIS_PATH path, int32 slave );
static u_int8 *ReadConfigFile( char *fileName, u_int32 *fileSizeP );
static long GetFileSize( FILE *fp );
static void DisplayBusEvents( MDIS_PATH path );

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

	if (argc < 3 || strcmp(argv[1],"-?")==0) {
		printf("Syntax: profidp_tool <device> <file> \n");
		printf("Function: Interactive tool for PROFIDP driver\n");
		printf("Options:\n");
		printf("    device       device name\n");
		printf("    file         file name of binary configuration file\n");
		printf("\n");
		return(1);
	}

	printf("\nPROFIDP-Tool program:\n Device name: %s\n Configuration file name: %s\n", argv[1], argv[2]);

	_profidp_tool( argv[1], argv[2]);
	return (0);
}

/******************************* _profidp_tool *****************************
 *
 *  Description:  Opens the device and allows to read and modify
 *                each Pofibus channel
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  devName device name in the system e.g. "/m57/0"
 *                file    name of binary configuration file
 *  Output.....:  return  0 => Ok or 1 => Error
 *
 *  Globals....:  ---
 *
 ****************************************************************************/
static int _profidp_tool( char *devName, char *fileName )
{
	u_int8 			*cfgData;
	u_int32 		cfgDataSize;
	MDIS_PATH 		path=-1;
	M_SG_BLOCK      blk;
	int32			currentSlave;
	int32			abort = FALSE;
	int32			rv = 0;
	int				userInput;

	/*-----------------------------+
	|  Read config data from file  |
	+-----------------------------*/
	if( (cfgData = ReadConfigFile( fileName, &cfgDataSize )) == NULL )
		return 1;


	/*------------------------------+
	|  Open path to PROFIDP device  |
	+------------------------------*/
	CHK( (path = M_open( devName )) >= 0 );

	/*------------------------------+
	|  Configure & start profibus   |
	+------------------------------*/

	blk.data = (void *)cfgData;
	blk.size = cfgDataSize;

	CHK( M_setstat(path, PROFIDP_BLK_CONFIG, (int32) &blk) == 0 );

	CHK( M_setstat(path, PROFIDP_START_STACK, 0) == 0 );

	/*-------------+
	|  Main loop   |
	+-------------*/
	PrintHelp();

	currentSlave = 2;
	ChangeSlave( path, currentSlave );

	while( abort == FALSE ){

		/* show inputs & diag of current slave */
		DisplaySlave( path, currentSlave );

		/* display bus status if changed */
		DisplayBusEvents( path );

		/* check for user input */
#ifdef VXWORKS
		/*
		 * Special handling in VXWORKS: UOS_KeyPressed()
		 * does not work as expected. So user has to enter <CR>
		 * first, then the selection
		 */
		userInput = -1;
		if( UOS_KeyPressed() == '\n' ){
			PrintHelp();
			printf("Select Command: ");
			fflush(stdout);
			userInput = UOS_KeyWait();
		}
#else
		userInput = UOS_KeyPressed();
#endif
		switch( userInput ){
		case 'e':
			/* exit */
			abort = TRUE;
			printf("\nEXITING...\n");
			break;

		case 's':
			/* new slave number */
			InputSlaveNum( &currentSlave );
			ChangeSlave( path, currentSlave );
			break;

		case 'w':
			/* modify outputs */
			ChangeSlaveOutputs( path, currentSlave );

		case 'h':
			PrintHelp();
		}
		UOS_Delay(300);
	}

 end:

	if( path >=0 )
		M_close( path );
	if( cfgData )
		free( cfgData );

	return rv;

 abort:
	rv = 1;
	goto end;
}

static void PrintHelp( void )
{
	printf("\n***** PROFIDP TEST TOOL ******\n"
		   "Commands:\n"
		   " h: display this help\n"
		   " s: change current slave\n"
		   " w: modify slave outputs\n"
		   " e: exit\n\n"
		);
}

static void ChangeSlave( MDIS_PATH path, int32 slave )
{
	int32 inLen, outLen;

	printf("\nCurrent slave is now 0x%02lx: ", slave );

	CHK( M_setstat( path, M_MK_CH_CURRENT, slave ) == 0 );
	CHK( M_getstat( path, PROFIDP_CH_INPUT_LEN, &inLen ) == 0 );
	CHK( M_getstat( path, PROFIDP_CH_OUTPUT_LEN, &outLen ) == 0 );

	printf(" 0x%lx input bytes, 0x%lx output bytes\n",
		   inLen, outLen );
 abort:
	return;
}

static void DisplaySlave( MDIS_PATH path, int32 slave )
{
	u_int8 buf[DP_MAX_TELEGRAM_LEN];
	int32 inLen;
	M_SG_BLOCK      blk;
	T_DP_DIAG_DATA* diag = (T_DP_DIAG_DATA*)buf;
	int i;

	printf(" Slave 0x%02lx: ", slave);

	/* Diag data */
	blk.data = (void *)diag;
	blk.size = sizeof(T_DP_DIAG_DATA);

	CHK( M_getstat(path, PROFIDP_BLK_GET_SLAVE_DIAG, (int32*)&blk ) == 0 );
	printf(" DIAG: status=%02x/%02x/%02x ID=%04x mst=%02x  ",
		   diag->station_status_1,
		   diag->station_status_2,
		   diag->station_status_3,
		   diag->ident_number,
		   diag->master_add );

	/* Read slave inputs */
	CHK( M_getstat( path, PROFIDP_CH_INPUT_LEN, &inLen ) == 0 );
	if( inLen > 0 ){

		printf("INPUTS: ");
		CHK( M_getblock( path, buf, sizeof(buf) ) == inLen );

		for( i=0; i<inLen; i++ ){
			printf("%02x ", buf[i] );
		}
	}
	printf("\n");
 abort:
	return;
}

static void DisplayBusEvents( MDIS_PATH path )
{
	int32 busEvents;
	CHK( M_getstat( path, PROFIDP_FM2_REASON, &busEvents ) == 0 );

	if( busEvents ){
		printf("Bus events: ");

		if( busEvents & 0x2 ) printf( " DUPADR");
		if( busEvents & 0x4 ) printf( " L2BAD");
		if( busEvents & 0x8 ) printf( " TOUT");
		if( busEvents & 0x10 ) printf( " NOSYNC");
		if( busEvents & 0x20 ) printf( " STOUT");
		if( busEvents & 0x40 ) printf( " STNEW");
		printf("\n");

	}
 abort:
	return;
}

static void InputSlaveNum( int32 *slaveP )
{
	u_int8 byte;

	printf("\nInput new slave number (2 hex digits):");
	fflush(stdout);
	byte = GetHexByte((u_int8)(*slaveP));
	*slaveP = byte;

	return;
}

static u_int8 GetHexByte(u_int8 def)
{
	char buf[20];
	int val;
	char*errstr=NULL;
    u_int8  wait4ever = 1;

	while( wait4ever ){
		printf("[%02x]: ", def );
		fgets( buf, sizeof(buf), stdin );

		if( buf[0] == '\n' )
			return def;

		val = strtol(buf, &errstr, 16);
		if( val >=0  && val < 0x100)
			return ((u_int8)val);
		printf("*** Illegal hex value\n");
	}
    return def;
}

static void ChangeSlaveOutputs(MDIS_PATH path, int32 slave )
{
	int32 outLen, i;
	static u_int8 buf[DP_MAX_TELEGRAM_LEN];

	CHK( M_getstat( path, PROFIDP_CH_OUTPUT_LEN, &outLen ) == 0 );

	if( outLen == 0 ){
		printf("\n*** Slave has no outputs\n");
		return;
	}

	/* let user modify the outputs */
	for( i=0; i<outLen; i++ ){
		printf("\nOutput Byte 0x%02lx: ", i);
		fflush(stdout);

		buf[i] = GetHexByte( buf[i] );
	}

	CHK( M_setblock( path, buf, outLen ) == outLen );
	printf("outputs updated.\n");
 abort:
	return;
}

/********************************* ReadConfigFile ****************************
 *
 *  Description: Allocate buffer for Config Data and read in file
 *
 *
 *---------------------------------------------------------------------------
 *  Input......: fileName		name of config file
 *  Output.....: returns:		ptr to config data or NULL if error
 *				 *fileSizeP		length of config data
 *  Globals....: -
 ****************************************************************************/
static u_int8 *ReadConfigFile( char *fileName, u_int32 *fileSizeP )
{
	FILE *fp = fopen(fileName, "rb");
	long size;
	u_int8 *mem;

	if (fp == NULL) {
		fprintf( stderr, "Can't open %s\n", fileName );
		return NULL;
	}

	/* determine file size */
	size = *fileSizeP = GetFileSize(fp);

	/* allocate buffer */
	mem = (u_int8 *)malloc( size );

	if( mem == NULL ){
		fprintf( stderr, "Can't alloc buffer for %s\n", fileName );
		fclose(fp);
		return NULL;
	}

	/* read data */
	if ( fread (mem, sizeof(u_int8), size, fp) != size) {
		fprintf( stderr, "Error reading %s\n", fileName );
		fclose(fp);
		free( mem );
		return NULL;
	}
	fclose(fp);
	return mem;
}

static long GetFileSize( FILE *fp )
{
	long size;

	fseek( fp, 0, SEEK_END);	/* seek to eof */
	size = ftell( fp );
	fseek( fp, 0, SEEK_SET);	/* seek to begin */
	return size;
}

