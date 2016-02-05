/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: profidp_drv.c
 *      Project: PROFIDP module driver (MDIS4)
 *
 *       Author: kp
 *        $Date: 2014/11/05 10:51:26 $
 *    $Revision: 1.1 $
 *
 *  Description: Low-level driver for PROFIDP modules
 *
 *               The M57 is a Profibus DP M-Module with
 *               interrupt capabilities.
 *
 *               The driver handles the PROFIDP Slaves as channels.
 *
 *               The direction of the channels cannot be altered.
 *
 *               The interrupt is used by the PROFIDP module to indicate that
 *               an indication or confirmation has occurred.
 *
 *
 *     Required: OSS, DESC, DBG, ID libraries
 *     Switches: _ONE_NAMESPACE_PER_DRIVER_
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: profidp_drv.c,v $
 * Revision 1.1  2014/11/05 10:51:26  AGromann
 * Initial Revision
 *
 * Revision 2.9  2010/11/16 11:23:42  CKauntz
 * R: Ported to MDIS5
 * M: a) adapted DBG prints for 64bit pointers
 *    b) adapted casts for llHdl->ma
 *
 * Revision 2.8  2010/10/06 23:05:53  cs
 * R: ported driver to current men_typs and MDIS_API
 * M: worked in MDIS4/MDIS5 porting guide version 0.9
 *
 * Revision 2.7  2005/03/24 15:10:19  AGromann
 * Fixed problem: If data read and data write functions were called in cycle
 * times longer than 1s the error FW does not respond occured during
 * M_setblock, M_getblock etc..
 * Functions PROFIDP_BLK_RCV_CON_IND_WAIT a. PROFIDP_BLK_RCV_CON_IND didn't
 * receive conformations -> in function profi_snd_req_res_usr() now only fill
 * waitForServices structure if CON is handled driver internaly,
 * now reqPending flag is cleared at ACK INT 0xf0.
 *
 * Revision 2.6  2004/08/20 15:24:05  AGromann
 * modifications for Win2k compiler
 *
 * Revision 2.5  2004/08/20 14:24:12  AGromann
 * enhanced alive checking (added fwAliveCheckWait flag)
 *
 * enhanced alive checking (added fwAliveCheckWait flag)
 * Revision 2.4  2004/07/22 11:45:24  AGromann
 * added timeout to function profi_snd_req_res_usr() if FW does not respond
 * added cyclic FW alive checking in cyclic data transfer mode
 *
 * added timeout to function profi_snd_req_res_usr() if FW does not respond
 * added cyclic FW alive checking in cyclic data transfer mode
 * Revision 2.3  2004/05/05 08:25:13  cs
 * Minor changes for MDIS4/2004 conformity
 *   replaced functions OSS_IrqMask/OSS_IrqUnMask with OSS_IrqMaskR/OSS_IrqRestore
 *   minor type casts for Win2k compiler compatibility
 *
 * Revision 2.2  2004/05/03 15:19:40  AGromann
 * cosmetics
 *
 * cosmetics
 * Revision 2.1  2003/01/07 15:50:47  agromann
 * removed problem with PROFIDP_START_STACK on fast CPUs by changing
 * diagnostic handling in cyclic mode: Now prevent DP_GET_SLAVE_DIAG.REQ
 * from being called in parallel with user requests.
 *
 * Revision 2.0  2001/10/24 16:03:12  kp
 * Bug fix: in cyclic mode, diagnostic data will be lost if more than one
 * entry in queue. Now issuing DP_GET_SLAVE_DIAG.REQ in irq or
 * profi_snd_req_res_usr().
 * - Mask interrupts in some get/setstats.
 * - removed setstat M_LL_CH_DIR
 * - removed getstat M_LL_CH_LEN
 * - new getstat PROFIDP_CH_INPUT_LEN/PROFIDP_CH_OUTPUT_LEN
 * - Getstat M_LL_CH_DIR now returns correct value
 * - Preset DIAG data in init to reflect non existent station
 * - Gave all global symbols a unique prefix
 * - reordered setstats/getstats priorities
 *
 * Revision 1.8  2001/02/28 08:18:43  kp
 * Bug fix: align max_slave_input/output_len to word boundary
 * Big data structures removed from stack. Now in LL handle
 *
 * Revision 1.7  2000/08/30 15:29:35  gromann
 * changed cast (USIGN8) to (USIGN16) for parameter offset in function calls
 * profi_set_data (...) and profi_get_data(...),
 * incresed supported channel number to 125,
 * changed error value for user buffer error in SetStat
 * PROFIDP_BLK_DATA_TRANSFER form -1 to 2.
 *
 * Revision 1.6  2000/07/28 13:33:19  gromann
 * if an CON/IND buffer overflow occurs now the current element is lost
 *
 * Revision 1.5  2000/07/18 09:12:33  gromann
 * added function PROFIDP_copy_buspar to insert alignment bytes in slave
 * parameter data
 *
 * Revision 1.4  2000/07/03 15:19:11  gromann
 * added IRQ disable option in SetStat M_MK_IRQ_ENABLE
 *
 * Revision 1.3  2000/06/15 14:54:11  gromann
 * added swapped functionality
 * changed PROFIDP_get_slave_diag, PROFIDP_data_transfer, PROFIDP_BLK_GET_SLAVE_DIAG
 * PROFIDP_CON_IND_WAIT timeout now specified in ms, req_con timeout now set by
 * PROFIDP_WAIT_TIMEOUT (msec), PROFIDP_BlockRead added user buffer check
 *
 * Revision 1.2  2000/03/09 09:58:38  Gromann
 * Changes for Win NT
 *
 * Revision 1.1  2000/02/29 12:04:55  Gromann
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000 by MEN Mikro Elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Id: profidp_drv.c,v 1.1 2014/11/05 10:51:26 AGromann Exp $";


#include "profidp_drv_int.h" /* internal Profibus header file */


/*-----------------------------------------+
|  DEFINES                                 |
+-----------------------------------------*/
/* general */
#define CH_NUMBER			(DP_MAX_NUMBER_SLAVES + 1) /* number of device channels */
#define USE_IRQ				TRUE		/* FALSE interrupt required  */
#define ADDRSPACE_COUNT		1			/* nr of required address spaces */
#define ADDRSPACE_SIZE		256			/* size of address space */
#define MOD_ID_MAGIC		0x5346      /* ID PROM magic word */
#define MOD_ID_SIZE			128			/* ID PROM size [bytes] */
#define MOD_ID				57			/* ID PROM module ID */
#define M57_ARRAY_OFFS      4           /* offset for firmware array */

/* check if byte ordering is set */
#if defined(_BIG_ENDIAN_) || defined(_LITTLE_ENDIAN_)
#else
#	error "Byte ordering is not set, please make sure that either _BIG_ENDIAN_ or _LITTLE_ENDIAN_ is defined"
#endif

#if INTEL /* Big / Little */
#define TWISTWORD_FW(w)  OSS_Swap16(w)    /* ((((w)&0xff)<<8) + (((w)&0xff00)>>8)) */
#define TWISTLONG_FW(l)  OSS_Swap32(l)    /* ((((l)&0xff)<<24) + (((l)&0xff00)<<8) + \
                     				      (((l)&0xff0000)>>8) + (((l)&0xff000000)>>24)) */
#else
#define TWISTWORD_FW(w) (w)
#define TWISTLONG_FW(l) (l)
#endif


/*-----------------------------------------+
|  PROTOTYPES                              |
+-----------------------------------------*/
static int32 PROFIDP_Init(DESC_SPEC *descSpec, OSS_HANDLE *osHdl,
					   MACCESS *ma, OSS_SEM_HANDLE *devSemHdl,
					   OSS_IRQ_HANDLE *irqHdl, LL_HANDLE **llHdlP);
static int32 PROFIDP_Exit(LL_HANDLE **llHdlP );
static int32 PROFIDP_Read(LL_HANDLE *llHdl, int32 ch, int32 *value);
static int32 PROFIDP_Write(LL_HANDLE *llHdl, int32 ch, int32 value);
static int32 PROFIDP_SetStat(LL_HANDLE *llHdl, int32 code, int32 ch, INT32_OR_64 value32_or_64);
static int32 PROFIDP_GetStat(LL_HANDLE *llHdl, int32 code, int32 ch, INT32_OR_64 *value32_or_64P);
static int32 PROFIDP_BlockRead(LL_HANDLE *llHdl, int32 ch, void *buf, int32 size,
							int32 *nbrRdBytesP);
static int32 PROFIDP_BlockWrite(LL_HANDLE *llHdl, int32 ch, void *buf, int32 size,
							 int32 *nbrWrBytesP);
static int32 PROFIDP_Irq(LL_HANDLE *llHdl );
static int32 PROFIDP_Info(int32 infoType, ... );

static char* Ident( void );
static int32 Cleanup(LL_HANDLE *llHdl, int32 retCode);

static int16 PROFIDP_Config ( LL_HANDLE* llHdl, M_SG_BLOCK *blk );


static void load_segment ( MACCESS base, u_int32 seg_offs,
                    u_int32 offset, u_int32 length, LL_HANDLE* llHdl);


static int verify_fw_segment ( MACCESS base, u_int32 seg_offs,
                         u_int32 offset, u_int32 length, LL_HANDLE* llHdl);

static T_DP_BUS_PARA_SET* dp_bincfg_get_buspar(LL_HANDLE* llHdl, u_int8* data_ptr,
                                        u_int32 data_size );

static T_DP_SLAVE_PARA_SET* dp_bincfg_get_slavepar(LL_HANDLE* llHdl, u_int8* data_ptr,
                                             u_int32 data_size, u_int8 slave_address );

static u_int32 _skip_align(LL_HANDLE* llHdl, u_int8* data_ptr );

static int16 req_con( LL_HANDLE* llHdl, u_int8 layer, u_int8 service,
                    u_int8* data_ptr, u_int16* status_ptr );

static u_int8* get_offs (LL_HANDLE* llHdl, u_int8* address, u_int8* sl_addr,
                      u_int8* i_offs, u_int8* o_offs, u_int16* sl_para_len);

static int16 dp_setstate(LL_HANDLE* llHdl, u_int8 state, u_int16* con_status );

static int16 PROFIDP_data_transfer (LL_HANDLE* llHdl, M_SG_BLOCK* blk);

static int16 PROFIDP_get_slave_diag (LL_HANDLE* llHdl, M_SG_BLOCK* blk);

static int8 waitForService_clear ( LL_HANDLE* llHdl );

static void PROFIDP_inConIndBuffer ( LL_HANDLE* llHdl, T_PROFI_SERVICE_DESCR* c_sdb, u_int8* Buffer );

static int16 PROFIDP_outConIndBuffer ( LL_HANDLE* llHdl, M_SG_BLOCK* blk );

static int16 PROFIDP_copy_buspar (u_int16 length, u_int8 *sourceP, u_int8 *destinationP);

static int16 profi_snd_req_res_usr(
	LL_HANDLE     		  *llHdl,
	T_PROFI_SERVICE_DESCR FAR *sdb_ptr,
	VOID                  FAR *data_ptr,
	PB_BOOL               driverIntCon
	);
static int32 GetChannelDir( LL_HANDLE *llHdl, int32 ch);
static void PROFIDP_sendDiagReqIrq(LL_HANDLE *llHdl);
static int32 PROFIDP_aliveCheck(LL_HANDLE *llHdl);
static void PROFIDP_IsrTask(LL_HANDLE *llHdl);

/**************************** PROFIDP_GetEntry *********************************
 *
 *  Description:  Initialize driver's jump table
 *
 *---------------------------------------------------------------------------
 *  Input......:  ---
 *  Output.....:  drvP  pointer to the initialized jump table structure
 *  Globals....:  ---
 ****************************************************************************/
#ifdef _ONE_NAMESPACE_PER_DRIVER_
    extern void LL_GetEntry( LL_ENTRY* drvP )
#else
    extern void __PROFIDP_MOD_VX_GetEntry( LL_ENTRY* drvP )
#endif
{
    drvP->init        = PROFIDP_Init;
    drvP->exit        = PROFIDP_Exit;
    drvP->read        = PROFIDP_Read;
    drvP->write       = PROFIDP_Write;
    drvP->blockRead   = PROFIDP_BlockRead;
    drvP->blockWrite  = PROFIDP_BlockWrite;
    drvP->setStat     = PROFIDP_SetStat;
    drvP->getStat     = PROFIDP_GetStat;
    drvP->irq         = PROFIDP_Irq;
    drvP->info        = PROFIDP_Info;
}

/******************************** PROFIDP_Init ***********************************
 *
 *  Description:  Allocate and return low-level handle, initialize hardware
 *
 *                The function initializes the device with the
 *                definitions made in the descriptor. The interrupt
 *                is disabled.
 *
 *                The following descriptor keys are used:
 *
 *                Descriptor key          Default          Range
 *                --------------------    ---------------  -------------
 *                DEBUG_LEVEL_DESC        OSS_DBG_DEFAULT  see dbg.h
 *                DEBUG_LEVEL             OSS_DBG_DEFAULT  see dbg.h
 *                ID_CHECK                0                0..1
 *
 *                max number of slaves:
 *                MAX_NUM_SLAVES          125	           0..DP_MAX_NUMBER_SLAVES
 *
 *                lowest slave address:
 *                LOWEST_SL_ADDR          out of config    0..
 *                                        file	           DP_MAX_NUMBER_SLAVES
 *
 *                max number of slave diag entries:
 *                MAX_SLAVE_DIAG_ENTRIES  125	           0..max
 *
 *                max data length of one diag entry:
 *                MAX_SLAVE_DIAG_LEN      244	           DP_MIN_SLAVE_DIAG_LEN..
 *                                                         DP_MAX_SLAVE_DIAG_DATA_LEN
 *
 *                clear outputs when entering CLEAR state:
 *                CLEAR_OUTPUTS           PB_TRUE	        PB_TRUE, PB_FALSE
 *
 *                automatically respond to M2 services:
 *                AUTO_REMOTE_SERVICE     0xe0	           see pb_dp.h
 *
 *                set cyclic data transfer state:
 *                CYCLC_DATA_TRANSFER     PB_TRUE	       PB_TRUE, PB_FALSE
 *
 *                act as responder M1 or M2
 *                MASTER_CLASS_2          PB_FALSE	       PB_TRUE, PB_FALSE
 *
 *                number of elements in CON/IND buf:
 *                CON_IND_BUF_EL          127              0..max
 *
 *---------------------------------------------------------------------------
 *  Input......:  descSpec   pointer to descriptor data
 *                osHdl      oss handle
 *                ma         hardware access handle
 *                devSemHdl  device semaphore handle
 *                irqHdl     irq handle
 *  Output.....:  llHdlP     pointer to low-level driver handle
 *                return     success (0) or error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 PROFIDP_Init(
    DESC_SPEC       *descP,
    OSS_HANDLE      *osHdl,
    MACCESS         *ma,
    OSS_SEM_HANDLE  *devSemHdl,
    OSS_IRQ_HANDLE  *irqHdl,
    LL_HANDLE       **llHdlP
)
{
    LL_HANDLE *llHdl = NULL;
    u_int32 gotsize;
    int32 error;
    u_int32 value;
	u_int32         elements_con_ind;
	u_int32         isr_task_prio;
	int16           i;
	u_int32         lowSlAddr;
	/* firmware load*/
	m57_download*  down_p;
	m57_segm*      seg_p;
	int            segcnt;
	const u_int8*  dummy;

    /*------------------------------+
    |  prepare the handle           |
    +------------------------------*/
	*llHdlP = NULL;		/* set low-level driver handle to NULL */

	/* alloc */
    if ((llHdl = (LL_HANDLE*)OSS_MemGet(
    				osHdl, sizeof(LL_HANDLE), &gotsize)) == NULL)
       return(ERR_OSS_MEM_ALLOC);

	/* clear */
    OSS_MemFill(osHdl, gotsize, (char*)llHdl, 0x00);

	/* init */
    llHdl->memAlloc   = gotsize;
    llHdl->osHdl      = osHdl;
    llHdl->irqHdl     = irqHdl;
    llHdl->ma		  = *ma;

	/* initialize pointer for CON/IND Buffer */
	llHdl->con_ind_buf = 0;

    /*------------------------------+
    |  init id function table       |
    +------------------------------*/
	/* driver's ident function */
	llHdl->idFuncTbl.idCall[0].identCall = Ident;
	/* library's ident functions */
	llHdl->idFuncTbl.idCall[1].identCall = DESC_Ident;
	llHdl->idFuncTbl.idCall[2].identCall = OSS_Ident;
	/* firmware ident function */
	llHdl->idFuncTbl.idCall[3].identCall = Firmware_Ident;
	/* terminator */
	llHdl->idFuncTbl.idCall[4].identCall = NULL;

    /*------------------------------+
    |  prepare debugging            |
    +------------------------------*/
	DBG_MYLEVEL = OSS_DBG_DEFAULT;	/* set OS specific debug level */
	DBGINIT((NULL,&DBH));

    /*------------------------------+
    |  scan descriptor              |
    +------------------------------*/
	/* prepare access */
    if ((error = DESC_Init(descP, osHdl, &llHdl->descHdl)))
		return( Cleanup(llHdl,error) );

    /* DEBUG_LEVEL_DESC */
    if ((error = DESC_GetUInt32(llHdl->descHdl, OSS_DBG_DEFAULT,
								&value, "DEBUG_LEVEL_DESC")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		return( Cleanup(llHdl,error) );

	DESC_DbgLevelSet(llHdl->descHdl, value);	/* set level */

    /* DEBUG_LEVEL */
    if ((error = DESC_GetUInt32(llHdl->descHdl, OSS_DBG_DEFAULT,
								&llHdl->dbgLevel, "DEBUG_LEVEL")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		return( Cleanup(llHdl,error) );

    DBGWRT_1((DBH, "LL - PROFIDP_Init\n"));

    /* ID_CHECK */
    if ((error = DESC_GetUInt32(llHdl->descHdl, /*TRUE*/ FALSE, /* P6 has no ID */
								&llHdl->idCheck, "ID_CHECK")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		return( Cleanup(llHdl,error) );

    /*------------------------------------+
    |  scan M57 specific descriptor keys  |
    +------------------------------------*/

    /* address assignment mode is always DP_AAM_ARRAY */
	llHdl->addrAssignMode = DP_AAM_ARRAY;

    DBGWRT_2((DBH, "LL - PROFIDP_Init: ADDR_ASSIGN_MODE = %08x\n", llHdl->addrAssignMode));


    /* max number of slaves */
    if ((error = DESC_GetUInt32(llHdl->descHdl, DP_MAX_NUMBER_SLAVES,
								&llHdl->maxNumSlaves, "MAX_NUM_SLAVES")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		return( Cleanup(llHdl,error) );

    DBGWRT_2((DBH, "LL - PROFIDP_Init: DP_MAX_NUMBER_SLAVES = %08x\n", llHdl->maxNumSlaves));

    /* lowest slave address */
    if ((error = DESC_GetUInt32(llHdl->descHdl, 0xff,
								&lowSlAddr, "LOWEST_SL_ADDR")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		return( Cleanup(llHdl,error) );
	llHdl->lowest_slave_address = (u_int8) lowSlAddr;
    DBGWRT_2((DBH, "LL - PROFIDP_Init: DP_LOWEST_SL_ADDR = %02x\n", llHdl->lowest_slave_address));


    /* max slave diagnosis entries */
    if ((error = DESC_GetUInt32(llHdl->descHdl, DP_MAX_SLAVE_DIAG_ENTRIES,
								&llHdl->maxSlaveDiagEntries, "MAX_SLAVE_DIAG_ENTRIES")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		return( Cleanup(llHdl,error) );

    DBGWRT_2((DBH, "LL - PROFIDP_Init: MAX_SLAVE_DIAG_ENTRIES = %08x\n", llHdl->maxSlaveDiagEntries));

    /* max slave diagnosis data length */
    if ((error = DESC_GetUInt32(llHdl->descHdl, DP_MAX_SLAVE_DIAG_LEN,
								&llHdl->maxSlaveDiagLen, "MAX_SLAVE_DIAG_LEN")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		return( Cleanup(llHdl,error) );

    DBGWRT_2((DBH, "LL - PROFIDP_Init: MAX_SLAVE_DIAG_LEN = %08x\n", llHdl->maxSlaveDiagLen));


    /* clear outputs when entering CLEAR state */
    if ((error = DESC_GetUInt32(llHdl->descHdl, DP_CLEAR_OUTPUTS,
								&llHdl->clearOutputs, "CLEAR_OUTPUTS")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		return( Cleanup(llHdl,error) );

    DBGWRT_2((DBH, "LL - PROFIDP_Init: CLEAR_OUTPUTS = %08x\n", llHdl->clearOutputs));


    /* automatically respond to M2 services */
    if ((error = DESC_GetUInt32(llHdl->descHdl, DP_AUTO_REMOTE_SERVICE,
								&llHdl->autoRemoteService, "AUTO_REMOTE_SERVICE")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		return( Cleanup(llHdl,error) );

    DBGWRT_2((DBH, "LL - PROFIDP_Init: AUTO_REMOTE_SERVICE = %08x\n", llHdl->autoRemoteService));


    /* cyclic data transfer */
    if ((error = DESC_GetUInt32(llHdl->descHdl, DP_CYCLC_DATA_TRANSFER,
								&llHdl->cyclicDataTransfer, "CYCLC_DATA_TRANSFER")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		return( Cleanup(llHdl,error) );

    DBGWRT_2((DBH, "LL - PROFIDP_Init: CYCLC_DATA_TRANSFER = %08x\n", llHdl->cyclicDataTransfer));

    /* use master class 2 functionality */
    if ((error = DESC_GetUInt32(llHdl->descHdl, PB_FALSE,
								&llHdl->masterClass2, "MASTER_CLASS_2")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		return( Cleanup(llHdl,error) );

    DBGWRT_2((DBH, "LL - PROFIDP_Init: MASTER_CLASS_2 = %08x\n", llHdl->masterClass2));

    /* Priority of ISR-Task */
    if ((error = DESC_GetUInt32(llHdl->descHdl, DP_ISR_TASK_PRIO,
								&isr_task_prio, "ISR_TASK_PRIO")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		return( Cleanup(llHdl,error) );
    DBGWRT_1((DBH, "LL - PROFIDP_Init: ISR_TASK_PRIO = %08x\n", isr_task_prio));

    /* size of CON/IND Buffer */
    if ((error = DESC_GetUInt32(llHdl->descHdl, DP_CON_IND_BUF_EL,
								&elements_con_ind, "CON_IND_BUF_EL")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		return( Cleanup(llHdl,error) );
    llHdl->con_ind_buf_size = elements_con_ind * CON_IND_BUF_ELEMENT_SIZE;
    DBGWRT_2((DBH, "LL - PROFIDP_Init: CON_IND_BUF_SIZE = %08x\n", llHdl->con_ind_buf_size));

	if ( (llHdl->con_ind_buf = (u_int8*) OSS_MemGet( osHdl, llHdl->con_ind_buf_size, &gotsize)) == NULL)
		return(Cleanup(llHdl, ERR_OSS_MEM_ALLOC));

    OSS_MemFill(osHdl, gotsize, (char*) llHdl->con_ind_buf, 0x00);

	llHdl->con_ind_memSize = gotsize;

	llHdl->con_ind_buf_outP = llHdl->con_ind_buf;
	llHdl->con_ind_buf_inP  = llHdl->con_ind_buf;

    /*------------------------------+
    |  check module ID              |
    +------------------------------*/
	if (llHdl->idCheck) {
		int modIdMagic = m_read( (U_INT32_OR_64) llHdl->ma, 0);
		int modId      = m_read( (U_INT32_OR_64) llHdl->ma, 1);

		if (modIdMagic != MOD_ID_MAGIC) {
			DBGWRT_ERR((DBH," *** PROFIDP_Init: illegal magic=0x%04x\n",modIdMagic));
			error = ERR_LL_ILL_ID;
			return( Cleanup(llHdl,error) );
		}
		if (modId != MOD_ID) {
			DBGWRT_ERR((DBH," *** PROFIDP_Init: illegal id=%d\n",modId));
			error = ERR_LL_ILL_ID;
			return( Cleanup(llHdl,error) );
		}
	}


    /*------------------------------+
    |  init semaphores              |
    +------------------------------*/

	/* create semaphore for REQ/CON f0 */
	if ((OSS_SemCreate( llHdl->osHdl, OSS_SEM_BIN, 0, &llHdl->req_con_f0_semP )) != 0) {
		DBGWRT_ERR((DBH," *** PROFIDP_Init: Error creating REQ/CON semaphore\n"));
		return( Cleanup(llHdl,error) );
	}

	/* create semaphore for REQ/CON 0f */
	if ((OSS_SemCreate( llHdl->osHdl, OSS_SEM_BIN, 0, &llHdl->req_con_0f_semP )) != 0) {
		DBGWRT_ERR((DBH," *** PROFIDP_Init: Error creating REQ/CON semaphore\n"));
		return( Cleanup(llHdl,error) );
	}

	/* create semaphore for CON/IND buffer */
	if ((OSS_SemCreate( llHdl->osHdl, OSS_SEM_COUNT, 0, &llHdl->con_buf_semP )) != 0) {
		DBGWRT_ERR((DBH," *** PROFIDP_Init: Error creating CON/IND buffer semaphore\n"));
		return( Cleanup(llHdl,error) );
	}

	/* create semaphore for cyclic alive check */
	if ((OSS_SemCreate( llHdl->osHdl, OSS_SEM_BIN, 1, &llHdl->fwAliveCheckSemP )) != 0) {
		DBGWRT_ERR((DBH," *** PROFIDP_Init: Error creating alive check semaphore\n"));
		return( Cleanup(llHdl,error) );
	}

	/* create semaphore for triggering ISR-Task */
	if ((OSS_SemCreate( llHdl->osHdl, OSS_SEM_BIN, 0, &llHdl->isrTaskSemP )) != 0) {
		DBGWRT_ERR((DBH," *** PROFIDP_Init: Error creating ISR-Task semaphore\n"));
		return( Cleanup(llHdl,error) );
	}

	llHdl->windowPointerSemId = semMCreate( SEM_Q_PRIORITY );
	if( llHdl->windowPointerSemId == SEM_ID_NULL ) {
		DBGWRT_ERR((DBH," *** PROFIDP_Init: Error creating window pointer semaphore\n"));
		return( Cleanup(llHdl,error) );
	}

    /*------------------------------+
    |  create ISR-Task              |
    +------------------------------*/
	llHdl->isrTaskId = taskSpawn( "tM57Isr",
								  (int) isr_task_prio,
			                      0,
			                      4096,
			                      (FUNCPTR) PROFIDP_IsrTask,
			                      (_Vx_usr_arg_t) llHdl, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
	if( llHdl->isrTaskId == TASK_ID_ERROR ) {
		error = PROFIDP_ERR_CREATING_ISR_TASK;
		return (Cleanup( llHdl, error));
	}

    /*------------------------------+
    |  init hardware                |
    +------------------------------*/

/*---------------------------------------------------------------+
|                         firmware load                          |
+---------------------------------------------------------------*/
	dummy = &dp_fw[0];
	dummy += M57_ARRAY_OFFS;
	down_p = (m57_download *) dummy;
	seg_p = &down_p->segment[0];

	if( TWISTLONG_FW(down_p->sync_code) != SYNC_CODE )
	{
		DBGWRT_ERR((DBH," *** PROFIDP_Init: Firmware is not valid\n"));
		DBGWRT_ERR((DBH," *** PROFIDP_Init: Sync_code = %08x\n", down_p->sync_code));
		DBGWRT_ERR((DBH," *** PROFIDP_Init: &dp_fw[0] = %08x\n", dp_fw));
		error = PROFIDP_ERR_VALIDATE_FW;
		return (Cleanup( llHdl, error));
	}
	/*--- count number of segments ---*/
		for(segcnt=0; seg_p->offset; seg_p++, segcnt++)
		;

	DBGWRT_2((DBH,"LL - PROFIDP_Init: %d segments\n", segcnt ));
	DBGWRT_1((DBH,"\nM57 Base Address = %08p\n", (void*) llHdl->ma ));
	/*--- reset module ---*/

	M57_RESET( llHdl->ma, 1);			/* reset module (keep reset)*/

	DBGWRT_2((DBH,"LL - PROFIDP_Init: m57 reseted\n"));

	/* init irq flag */
	llHdl->irqFlag          = 0;   /* flag indicates that irq routine is done at the moment */

	/*----------------+
    |  Load segments  |
    +----------------*/

	for( seg_p=down_p->segment; seg_p->offset; seg_p++ ){
		DBGWRT_2((DBH,"LL - PROFIDP_Init: Loading segment loadaddr=$%08x length=%08x\n",
						  TWISTLONG_FW(seg_p->loadaddr), TWISTLONG_FW(seg_p->length) ));

		load_segment( llHdl->ma, (TWISTLONG_FW (seg_p->offset)),
		              (TWISTLONG_FW (seg_p->loadaddr)), (TWISTLONG_FW (seg_p->length)), llHdl );
	}

	/*------------------+
    |  Verify segments  |
    +------------------*/

	for( seg_p=down_p->segment; seg_p->offset; seg_p++ ){
		DBGWRT_2((DBH,"LL - PROFIDP_Init: Verify segment loadaddr=$%08x length=%08x\n",
						  seg_p->loadaddr, seg_p->length ));

		if ( verify_fw_segment ( llHdl->ma, TWISTLONG_FW (seg_p->offset),
		     TWISTLONG_FW (seg_p->loadaddr), TWISTLONG_FW (seg_p->length), llHdl) < 0)
		{
			error = PROFIDP_ERR_VERIFY_FW;
			return ( Cleanup( llHdl, error) );
		 }
	}

	 M57_RESET( llHdl->ma, 0);			 /* release reset (run) */


    /*------------------------------+
    |  initialize handle variables  |
    +------------------------------*/

 	llHdl->intFlagReq       = 1;   /* int intFalg */
	llHdl->con_ind_num_el   = 0;   /* set actual number of CON/IND buffer to 0 */
	llHdl->con_ind_buf_full = 0;   /* init CON/IND buffer full flag */
	llHdl->allCh            = 0;   /* flag locks semaphore while reading/writing to/from all ch */
	llHdl->conIndTimeout    = TIMEOUT_DEFAULT; /* set default timeout value */
	llHdl->fm2EventReason   = 0;   /* initialize FMB_FM2_EVENT variable */
	/* initialize chInfo array */
	for ( i = 0; i < DP_MAX_NUMBER_SLAVES; i++) {
		llHdl->chInfo[i].num_in  = 0;
		llHdl->chInfo[i].num_out = 0;
	}

	/* initialize chDiag array */
	for ( i = 0; i < (DP_MAX_NUMBER_SLAVES + 1); i++) {
		T_DP_DIAG_DATA *diag = (T_DP_DIAG_DATA *)&llHdl->chDiag[i][0];

		diag->station_status_1 = DP_DIAG_1_STATION_NON_EXISTENT;
		diag->master_add = 0xff;
	}

	/* initialize FW alive check variables */
	llHdl->aliveState = PROFIDP_ALIVE_IDLE;
	llHdl->lastAliveCheck   = 0;
	llHdl->lastFwDiagConInd = 0;
	llHdl->fwAliveCheckWait = FALSE;

	*llHdlP = llHdl;	/* set low-level driver handle */

	return(ERR_SUCCESS);
}



/****************************** PROFIDP_Exit ********************************
 *
 *  Description:  De-initialize hardware and clean up memory
 *
 *                The function deinitializes the device by resetting the
 *                onboard Profibus controller. The interrupt is disabled.
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdlP  	pointer to low-level driver handle
 *  Output.....:  return    success (0) or error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 PROFIDP_Exit(
   LL_HANDLE    **llHdlP
)
{
    LL_HANDLE *llHdl = *llHdlP;
	int32 error = 0;

    DBGWRT_1((DBH, "LL - PROFIDP_Exit\n"));

    /*------------------------------+
    |  de-init hardware             |
    +------------------------------*/

	M57_IRQ_DISABLE( llHdl->ma );	     /* disable IRQ on module */
	M57_RESET( llHdl->ma, 1);	         /* reset module (keep reset) */

    /*------------------------------+
    |  remove semaphores            |
    +------------------------------*/

	/* remove semaphore for REQ/CON f0 */
	if ((OSS_SemRemove( llHdl->osHdl, &llHdl->req_con_f0_semP )) != 0) {
		DBGWRT_ERR((DBH," *** PROFIDP_Exit: Error removing REQ/CON semaphore\n"));
	}

	/* remove semaphore for REQ/CON 0f */
	if ((OSS_SemRemove( llHdl->osHdl, &llHdl->req_con_0f_semP )) != 0) {
		DBGWRT_ERR((DBH," *** PROFIDP_Exit: Error removing REQ/CON semaphore\n"));
	}

	/* remove semaphore for CON/IND buffer */
	if ((OSS_SemRemove( llHdl->osHdl, &llHdl->con_buf_semP )) != 0) {
		DBGWRT_ERR((DBH," *** PROFIDP_Exit: Error removing CON/IND buffer semaphore\n"));
	}

	/* remove semaphore for alive check */
	if ((OSS_SemRemove( llHdl->osHdl, &llHdl->fwAliveCheckSemP )) != 0) {
		DBGWRT_ERR((DBH," *** PROFIDP_Exit: Error removing alive check semaphore\n"));
	}

	/* delete ISR task */
	if ( taskDelete( llHdl->isrTaskId ) != 0 ) {
		DBGWRT_ERR((DBH," *** PROFIDP_Exit: Error deleting ISR-Task\n"));
	}

	/* remove semaphore for ISR-Task */
	if ((OSS_SemRemove( llHdl->osHdl, &llHdl->isrTaskSemP )) != 0) {
		DBGWRT_ERR((DBH," *** PROFIDP_Exit: Error removing alive check semaphore\n"));
	}

	/* remove semaphore for window pointer */
	if ((semDelete(llHdl->windowPointerSemId)) != 0) {
		DBGWRT_ERR((DBH," *** PROFIDP_Exit: Error removing window pointer semaphore\n"));
	}

    /*------------------------------+
    |  clean up memory              |
    +------------------------------*/
	*llHdlP = NULL;		/* set low-level driver handle to NULL */
	error = Cleanup(llHdl,error);

	return(error);
}

/****************************** PROFIDP_Read *************************************
 *
 *  Description:  Do nothing
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl    low-level handle
 *                ch       current channel
 *  Output.....:  valueP   read value
 *                return   error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 PROFIDP_Read(
    LL_HANDLE *llHdl,
    int32 ch,
    int32 *valueP
)
{

    DBGWRT_1((DBH, "LL - PROFIDP_Read: ch=%d not supported\n",ch));

	return(ERR_LL_ILL_FUNC);
}

/****************************** PROFIDP_Write ************************************
 *
 *  Description:  Do nothing
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl    low-level handle
 *                ch       current channel
 *                value    value to write
 *  Output.....:  return   error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 PROFIDP_Write(
    LL_HANDLE *llHdl,
    int32 ch,
    int32 value
)
{

    DBGWRT_1((DBH, "LL - PROFIDP_Write: ch=%d not supported\n",ch));

	return(ERR_LL_ILL_FUNC);

}

/****************************** PROFIDP_SetStat **********************************
 *
 *  Description:  Set the driver status
 *
 *                The following status codes are supported:
 *
 *          Code                        Description                 Values
 *  ---------------------------  --------------------------------- ----------
 *  M_LL_DEBUG_LEVEL             driver debug level                see dbg.h
 *  M_MK_IRQ_ENABLE              interrupt enable                  0..1
 *  M_LL_IRQ_COUNT               interrupt counter                 0..max
 *
 *  PROFIDP_SIG_ON_EVENT_SET     install signal                    UOS_SIG_USR1,
 *                                                                 UOS_SIG_USR2
 *  PROFIDP_SIG_ON_EVENT_CLR     uninstall signal                  UOS_SIG_USR1,
 *                                                                 UOS_SIG_USR2
 *  PROFIDP_BLK_CONFIG           configure PROFDP device           -
 *  PROFIDP_BLK_SEND_REQ_RES     send request or response          -
 *  PROFIDP_BLK_RVC_CON_IND      get CON/IND out of buffer         -
 *  PROFIDP_BLK_RVC_CON_IND_WAIT wait until CON/IND is available   -
 *                               in buffer and read it out
 *  PROFIDP_BLK_SET_ALL_CH       write values to all output ch.    -
 *  PROFIDP_BLK_START_STACK      start stack STOP-CLEAR-OPERATE    -
 *  PROFIDP_BLK_STOP_STACK       stop stack CLEAR-STOP             -
 *  PROFIDP_BLK_DATA_TRANSFER    do one Data Transfer cycle        -
 *  ROFIDP_SET_STACK_STOP        set stack state to STOP           non
 *  PROFIDP_SET_STACK_CLEAR      set stack state to CLEAR          non
 *  PROFIDP_SET_STACK_OPERATE    set stack state to OPERATE        non
 *  PROFIDP_WAIT_TIMEOUT         set timeout (sec.) for            0..max
 *                               PROFIDP_BLK_RVC_CON_IND_WAIT
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl           low-level handle
 *                code            status code
 *                ch              current channel
 *                value32_or_64   data or pointer to block data structure
 *                                (M_SG_BLOCK*)
 *  Output.....:  return     success (0) or error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 PROFIDP_SetStat(
    LL_HANDLE *llHdl,
    int32  code,
    int32  ch,
    INT32_OR_64 value32_or_64
)
{
	int32 error = ERR_SUCCESS;
	int32		value	= (int32)value32_or_64;	/* 32bit value     */
	INT32_OR_64	valueP 	= value32_or_64;		/* stores 32/64bit pointer */

	M_SG_BLOCK* blk = (M_SG_BLOCK*)valueP;
	int16     i;
	u_int8 state[] = { DP_ST_STOP, DP_ST_CLEAR, DP_ST_OPERATE };
	u_int16  con_status;
	int16     dp_error;


    DBGWRT_1((DBH, "LL - PROFIDP_SetStat: ch=%d code=0x%04x value=%08p\n",
			  ch, code, value32_or_64));

    switch(code) {
		/*--------------------------------------------+
		|       PROFIDP specific codes                |
		+--------------------------------------------*/

        /*--------------------------------+
        |  do one Data Transfer service   |
        +--------------------------------*/
		case PROFIDP_BLK_DATA_TRANSFER:

			/* do data transfer service */
			if ((dp_error = (int16) PROFIDP_data_transfer ( llHdl, blk )) != 0) {
				DBGWRT_ERR((DBH, " *** PROFIDP Set Stat: Error Data Transfer\n"));
				if( dp_error == 2 ){
					return (ERR_LL_USERBUF);
				}

				return (PROFIDP_ERR_DATA_TRANSFER);
			}


			break;

        /*---------------------------------------+
        |  write values to all output channels   |
        +----------------------------------------*/
		case PROFIDP_BLK_SET_ALL_CH:

			/* check buffer size */
		    DBGWRT_1((DBH, "LL - PROFIDP_BLK_SET_ALL_CH: size=%d\n", blk->size));
			if ( ((u_int32) blk->size) > ((u_int32) (llHdl->max_slave_output_len * llHdl->maxNumSlaves))) {
				DBGWRT_ERR((DBH," *** PROFIDP_SetStat: User buffer to small\n"));
				return (ERR_LL_USERBUF);
			}

			/* check if output slaves are available */
			if (llHdl->max_slave_output_len == 0) {
				DBGWRT_ERR((DBH," *** PROFIDP_SetStat: No outputs available\n"));
				return (PROFIDP_ERR_NO_OUTPUTS);
			}

			/* write user data block to output area of DPRAM */
			if ( (dp_error = (int16) profi_set_data( llHdl, ID_DP_SLAVE_IO_IMAGE, (USIGN16) DP_OUT_POINTER,
			    (u_int16) blk->size, (u_int8*) blk->data))  != E_OK) {
				DBGWRT_ERR((DBH, " *** PROFIDP_SetStat: PROFIDP_BLK_SET_ALL_CH failed error code = %04x\n",
				                 dp_error));
				return (PROFIDP_ERR_SET_DATA);

			}

			/* check if FW is still alive */
			if( PROFIDP_aliveCheck( llHdl ) ) {
				DBGWRT_ERR((DBH, " *** PROFIDP_BlockRead: FW is not alive !!\n"));
				return (PROFIDP_ERR_FW_NOT_ALIVE);
			}

			break;

       /*-------------------------------+
        |   send a request or response  |
        +-------------------------------*/
		case PROFIDP_BLK_SEND_REQ_RES:

			/* send request respons */
			dp_error = (int16) profi_snd_req_res_usr( llHdl, (T_PROFI_SERVICE_DESCR*) blk->data,
					((VOID *) ( ((u_int8*) blk->data) + sizeof (T_PROFI_SERVICE_DESCR))), 0);

			/* if an error occured return error code */
			if (dp_error != E_OK) {
				DBGWRT_ERR((DBH, " *** PROFIDP_SetStat: PROFIDP_BLK_SEND_REQ_RES failed error code = %04x\n",
				                  dp_error));
				return (PROFIDP_ERR_SEND_REQ_RES);

			}

			break;

       /*----------------------------------------+
        |   receive a confirmation or indication |
        +----------------------------------------*/
		case PROFIDP_BLK_RCV_CON_IND:

			/* check buffer size */
			if ( blk->size < CON_IND_BUF_ELEMENT_SIZE) {
				DBGWRT_ERR((DBH," *** PROFIDP_BLK_RVC_CON_IND: User buffer to small\n"));
				return (ERR_LL_USERBUF);
			}

			/* decrement sem counter if there is one element in buffer */
			if (llHdl->con_ind_num_el > 0) {
				if ((OSS_SemWait( llHdl->osHdl, llHdl->con_buf_semP, OSS_SEM_NOWAIT )) != 0) {
					DBGWRT_ERR((DBH," *** PROFIDP_SetStat: Error or Timeout waiting for CON/IND buffer semaphore\n"));
					return (PROFIDP_ERR_CON_IND_SEM);
				}
			}

			/* look for element in buffer */
			if (llHdl->con_ind_num_el == 0) {
				DBGWRT_ERR((DBH,"*** PROFIDP_BLK_RVC_CON_IND: No CON/IND available\n"));
				return (PROFIDP_ERR_NO_CON_IND);
			}

			/* read element out of buffer */
			if (PROFIDP_outConIndBuffer ( llHdl, blk ) < 0)
				return (PROFIDP_ERR_READ_CON_IND);

		break;

        /*----------------------------------------------+
        |  wait to receive a confirmation or indication |
        +----------------------------------------------*/
		case PROFIDP_BLK_RCV_CON_IND_WAIT:
			/* check buffer size */
			if ( blk->size < CON_IND_BUF_ELEMENT_SIZE) {
				DBGWRT_ERR((DBH," *** PROFIDP_BLK_RVC_CON_IND_WAIT: User buffer to small\n"));
				return (ERR_LL_USERBUF);
			}

			/* wait until element is found in buffer or timeout occurs */
			if ((OSS_SemWait( llHdl->osHdl, llHdl->con_buf_semP, (int32) (llHdl->conIndTimeout /* * 1000 */) )) != 0) {
				DBGWRT_ERR((DBH," *** PROFIDP_SetStat: Error or Timeout waiting for CON/IND buffer semaphore\n"));
				return (PROFIDP_ERR_CON_IND_SEM);
			}


			/* read element out of buffer */
			if (PROFIDP_outConIndBuffer ( llHdl, blk ) < 0)
				return (PROFIDP_ERR_READ_CON_IND);

			break;

        /*------------------------------------------+
        |   set Profibus stack from STOP to OPERATE |
        +------------------------------------------*/
		case PROFIDP_START_STACK:

			/* set protocol stack in modes STOP, CLEAR, OPERATE */
			for( i=0; i<3; i++){
				dp_error = dp_setstate( llHdl, state[i], &con_status );

				if( dp_error == -1 ){
					DBGWRT_ERR((DBH, " *** PROFIDP Set Stat: Error setting mode to %02x\n", state[i]));
					return (PROFIDP_ERR_START_STACK);

				}
				if( dp_error == 1 ){
					DBGWRT_ERR((DBH, " *** PROFIDP Set Stat: Can't set mode to %x. status=%x\n", state[i],
						   con_status));
					return (PROFIDP_ERR_START_STACK);
				}
			}

			break;

        /*------------------------------------------+
        |  set Profibus stack from OPERATE to STOP  |
        +------------------------------------------*/
		case PROFIDP_STOP_STACK:

			/* set protocol stack to CLEAR, STOP */
			for( i=1; i>=0; i--){
				dp_error = dp_setstate( llHdl, state[i], &con_status );

				if( dp_error == -1 ){
					DBGWRT_ERR((DBH, " *** PROFIDP Set Stat: Error setting mode to %02x\n",
					                 state[i]));
					return (PROFIDP_ERR_STOP_STACK);
				}
				if( dp_error == 1 ){
					DBGWRT_ERR((DBH, " *** PROFIDP Set Stat: Can't set mode to %x. status=%x\n",
					                  state[i], con_status));
					return (PROFIDP_ERR_STOP_STACK);
				}
			}

			break;

        /*-----------------------------+
        |  set Profibus stack to STOP  |
        +-----------------------------*/
		case PROFIDP_SET_STACK_STOP:

				dp_error = dp_setstate( llHdl, DP_ST_STOP, &con_status );

				if( dp_error == -1 ){
					DBGWRT_ERR((DBH, " *** PROFIDP Set Stat: Error setting mode to %02x\n",
					                  DP_ST_STOP));
					return (PROFIDP_ERR_SET_STACK);

				}
				if( dp_error == 1 ){
					DBGWRT_ERR((DBH, " *** PROFIDP Set Stat: Can't set mode to %x. status=%x\n",
					                DP_ST_STOP, con_status));
					return (PROFIDP_ERR_SET_STACK);
				}


				break;


        /*------------------------------+
        |  set Profibus stack to CLEAR  |
        +------------------------------*/
		case PROFIDP_SET_STACK_CLEAR:

				dp_error = dp_setstate( llHdl, DP_ST_CLEAR, &con_status );

				if( dp_error == -1 ){
					DBGWRT_ERR((DBH, " *** PROFIDP Set Stat: Error setting mode to %02x\n",
					                 DP_ST_CLEAR));
					return (PROFIDP_ERR_SET_STACK);

				}
				if( dp_error == 1 ){
					DBGWRT_ERR((DBH, " *** PROFIDP Set Stat: Can't set mode to %x. status=%x\n",
					                 DP_ST_CLEAR, con_status));
					return (PROFIDP_ERR_SET_STACK);
				}



				break;


        /*--------------------------------+
        |  set Profibus stack to OPERATE  |
        +--------------------------------*/
		case PROFIDP_SET_STACK_OPERATE:

				dp_error = dp_setstate( llHdl, DP_ST_OPERATE, &con_status );

				if( dp_error == -1 ){
					DBGWRT_ERR((DBH, " *** PROFIDP Set Stat: Error setting mode to %02x\n",
					                 DP_ST_OPERATE));
					return (PROFIDP_ERR_SET_STACK);

				}
				if( dp_error == 1 ){
					DBGWRT_ERR((DBH, " *** PROFIDP Set Stat: Can't set mode to %x. status=%x\n",
					                 DP_ST_OPERATE, con_status));
					return (PROFIDP_ERR_SET_STACK);
				}


				break;

        /*------------------------------------------------+
        |  set PROFIDP_BLK_RCV_CON_IND_WAIT timeout value |
        +------------------------------------------------*/
		case PROFIDP_WAIT_TIMEOUT:
			llHdl->conIndTimeout = (u_int32) value;

			break;

       /*-------------------------------+
        |   install signal              |
        +-------------------------------*/
		case PROFIDP_SIG_ON_EVENT_SET:   /* Install Signal on event */
			if (llHdl->sigHdl != NULL) {
				DBGWRT_ERR((DBH, " *** PROFIDP_SetStat: signal already installed\n"));
				return (ERR_OSS_SIG_SET);
			}

			/* install signal */
			if ( (error = OSS_SigCreate(llHdl->osHdl, value, &llHdl->sigHdl)))
				return (error);


			break;

       /*-------------------------------+
        |   uninstall signal            |
        +-------------------------------*/
		case PROFIDP_SIG_ON_EVENT_CLR:   /* remove signal */

			if (llHdl->sigHdl == NULL) {
				DBGWRT_ERR((DBH, " *** PROFIDP_SetStat: no signal installed\n"));
				return (ERR_OSS_SIG_CLR);
			}

			/* uninstall signal */
			if ( (error = OSS_SigRemove(llHdl->osHdl, &llHdl->sigHdl)))
				return (error);
			break;

		/*--------------------------------------------+
		|         load Profibus configuration         |
		+--------------------------------------------*/
		case PROFIDP_BLK_CONFIG:
			if (PROFIDP_Config ( llHdl, blk ) < 0) {
				DBGWRT_ERR((DBH, " *** PROFIDP_SetStat: PROFIDP_BLK_CONFIG failed\n"));
				return (PROFIDP_ERR_CONFIG);
			}
			break;

        /*--------------------------+
        |  debug level              |
        +--------------------------*/
        case M_LL_DEBUG_LEVEL:
            llHdl->dbgLevel = value;
            break;
        /*--------------------------+
        |  enable interrupts        |
        +--------------------------*/
        case M_MK_IRQ_ENABLE:
			switch(value) {

				case TRUE:
					M57_IRQ_ENABLE(llHdl->ma);  /* enable on module interrupts */
					/* previously interrupt was enabled in cmi_init routine */
            		break;

				case FALSE:
					M57_IRQ_DISABLE(llHdl->ma);  /* disable on module interrupts */
					break;

				default:
					error = ERR_LL_ILL_PARAM;
					break;
			}
            break;
        /*--------------------------+
        |  set irq counter          |
        +--------------------------*/
        case M_LL_IRQ_COUNT:
            llHdl->irqCount = value;
            break;


        /*--------------------------+
        |  (unknown)                |
        +--------------------------*/
        default:
            error = ERR_LL_UNK_CODE;
    }

	return(error);
}

/****************************** PROFIDP_GetStat **********************************
 *
 *  Description:  Get the driver status
 *
 *                The following status codes are supported:
 *
 *                Code                     Description                 Values
 *       -----------------------------  --------------------------  ----------
 *       M_LL_DEBUG_LEVEL               driver debug level          see dbg.h
 *       M_LL_CH_NUMBER                 number of channels          0..
 *                                                                  DP_MAX_NUMBER_SLAVES
 *       M_LL_CH_DIR                    direction of curr. chan.    M_CH_???
 *       M_LL_CH_TYP                    description of curr. chan.  M_CH_BINARY
 *       M_LL_IRQ_COUNT                 interrupt counter           0..max
 *       M_LL_ID_CHECK                  EEPROM is checked           0..1
 *       M_LL_ID_SIZE                   EEPROM size [bytes]         128
 *       M_LL_BLK_ID_DATA               EEPROM raw data             -
 *       M_MK_BLK_REV_ID                ident function table ptr    -
 *       PROFIDP_BLK_GET_ALL_CH         read all input channels     -
 *       PROFIDP_BLK_GET_DIAG           do Get_Slave_Diag service   -
 *       PROFIDP_BLK_GET_SLAVE_DIAG     get diag for current ch.
 *       PROFIDP_WAIT_TIMEOUT           get timeout (sec.) for      0..max
 *                                      PROFIDP_BLK_RVC_CON_IND_WAIT
 *       PROFIDP_NUM_CON_IND            get number of CON/IND in    0..max
 *                                      buffer
 *       PROFIDP_FM2_REASON             get reason of FMB_FM2_EVENT see below
 *          Bit  7   6   5   4   3   2   1   0
 *             +-------------------------------+
 *             |RSV|   |   |   |   |   |   |RSV|
 *             +-------------------------------+
 *                   +   +   +   +   +   +--- duplicate recognized address
 *                   |   |   |   |   +------- phys. layer is malfunctioning
 *                   |   |   |   +----------- timeout on bus detected
 *                   |   |   +--------------- no receiver synchronization
 *                   |   +------------------- station out of ring
 *                   +----------------------- new station in ring
 *
 *       PROFIDP_BUFF_OVERFLOW          get information about buffer 0 = ok,
 *                                      overflow                     1 = overflow
 *       PROFIDP_MAX_INPUT_LEN          get max slave input length   0..max
 *       PROFIDP_MAX_OUTPUT_LEN         get max slave output length  0..max
 *       PROFIDP_CH_INPUT_LEN           get slave input length       0..max
 *       PROFIDP_CH_OUTPUT_LEN          get slave output length      0..max
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl            low-level handle
 *                code             status code
 *                ch               current channel
 *                value32_or_64P   pointer to block data structure (M_SG_BLOCK*)
 *  Output.....:  value32_or_64P   data pointer or
 *                            pointer to block data structure (M_SG_BLOCK*)
 *                return     success (0) or error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 PROFIDP_GetStat(
    LL_HANDLE *llHdl,
    int32  code,
    int32  ch,
    INT32_OR_64 *value32_or_64P
)
{
	int32		*valueP		  = (int32*)value32_or_64P;		/* pointer to 32bit value  */
	INT32_OR_64	*value64P	  = value32_or_64P;		 		/* stores 32/64bit pointer  */
	M_SG_BLOCK	*blk 		  = (M_SG_BLOCK*)value32_or_64P; 	/* stores block struct pointer */

	int32 error = ERR_SUCCESS;
	int16     get_error;
    u_int16  dataSize;


    DBGWRT_1((DBH, "LL - PROFIDP_GetStat: ch=%d code=0x%04x\n",
			  ch,code));

    switch(code)
    {
  /*--------------------------------------+
  |       PROFIDP specific codes          |
  +---------------------------------------*/

        /*-------------------------------+
        |   read data from all channels  |
        +-------------------------------*/
        case PROFIDP_BLK_GET_ALL_CH:

		    DBGWRT_1((DBH, "LL - PROFIDP_BLK_GET_ALL_CH: size = %d \n", blk->size));

			dataSize = (u_int16) blk->size;

			/* check if input slaves are available */
			if (llHdl->max_slave_input_len == 0) {
				DBGWRT_ERR((DBH," *** PROFIDP_GetStat: No inputs available\n"));
				return (PROFIDP_ERR_NO_INPUTS);
			}

			/* read input data from input area of DPRAM */
			if (profi_get_data( llHdl, ID_DP_SLAVE_IO_IMAGE, (USIGN16) 0, &dataSize,
			                    (u_int8*) blk->data)  != E_OK) { /* inputs have offset 0 */
				DBGWRT_ERR((DBH, " *** PROFIDP_GetStat: PROFIDP_BLK_SET_DATA failed\n"));
				return (PROFIDP_ERR_GET_DATA);
			}
			/* return number of read bytes done by blk->size */
			blk->size = (int32) dataSize;

			/* check if FW is still alive */
			if( PROFIDP_aliveCheck( llHdl ) ) {
				DBGWRT_ERR((DBH, " *** PROFIDP_BlockRead: FW is not alive !!\n"));
				return (PROFIDP_ERR_FW_NOT_ALIVE);
			}

            break;


        /*-------------------------------+
        |   do diag request service      |
        +-------------------------------*/
		case PROFIDP_BLK_GET_DIAG:

			get_error = PROFIDP_get_slave_diag ( llHdl, blk );
			if( get_error == -1 ){
				DBGWRT_ERR((DBH, " *** PROFIDP Set Stat: Error get diag\n"));
				error = PROFIDP_ERR_REQ_CON_TIMEOUT;
			}
			break;

        /*-------------------------------------------+
        | read diag entry for channel out of buffer  |
        +-------------------------------------------*/
		case PROFIDP_BLK_GET_SLAVE_DIAG:

			DBGWRT_1((DBH, "LL - PROFIDP_get_diag_array: Get diag entrie of ch %08x\n", ch));

			if ( blk->size < sizeof(T_DP_DIAG_DATA) ) {
				DBGWRT_ERR((DBH, " *** PROFIDP_GetStat: Error get slave diag\n"));
				return (ERR_LL_USERBUF);
			}

			if( ERROR == semTake( llHdl->windowPointerSemId, WAIT_FOREVER ) ) {
				DBGWRT_ERR((DBH," *** PROFIDP_aliveCheck: Error taking window pointer semaphore\n"));
			}

			OSS_MemCopy(llHdl->osHdl, blk->size, (char*) &llHdl->chDiag[ch][0], (char*) blk->data);

			if( ERROR == semGive( llHdl->windowPointerSemId ) ) {
				DBGWRT_ERR((DBH," *** PROFIDP_aliveCheck: Error giving window pointer semaphore\n"));
			}


			break;

        /*------------------------------------------------+
        |  get PROFIDP_BLK_RCV_CON_IND_WAIT timeout value |
        +------------------------------------------------*/
		case PROFIDP_WAIT_TIMEOUT:
			*valueP = llHdl->conIndTimeout;

			break;

        /*------------------------------------------------+
        |  get number of elements in CON/IND buffer       |
        +------------------------------------------------*/
		case PROFIDP_NUM_CON_IND:
			*valueP = (int32) llHdl->con_ind_num_el;

			break;

        /*------------------------------------------------+
        |  get current reason of FM2 EVENT                |
        +------------------------------------------------*/
		case PROFIDP_FM2_REASON:
			if( ERROR == semTake( llHdl->windowPointerSemId, WAIT_FOREVER ) ) {
				DBGWRT_ERR((DBH," *** PROFIDP_aliveCheck: Error taking window pointer semaphore\n"));
			}

			*valueP = (int32) llHdl->fm2EventReason;
			llHdl->fm2EventReason = 0;

			if( ERROR == semGive( llHdl->windowPointerSemId ) ) {
				DBGWRT_ERR((DBH," *** PROFIDP_aliveCheck: Error giving window pointer semaphore\n"));
			}

			break;

        /*------------------------------------------------+
        |  get CON/IND buffer overflow flag               |
        +------------------------------------------------*/
		case PROFIDP_BUFF_OVERFLOW:
			*valueP = (int32) llHdl->con_ind_buf_full;
			break;

        /*------------------------------------------------+
        |  get max input length                           |
        +------------------------------------------------*/
		case PROFIDP_MAX_INPUT_LEN:
			*valueP = (int32) llHdl->max_slave_input_len;
			break;


        /*------------------------------------------------+
        |  get max output length                          |
        +------------------------------------------------*/
		case PROFIDP_MAX_OUTPUT_LEN:
			*valueP = (int32) llHdl->max_slave_output_len;
			break;


        /*------------------------------------------------+
        |  get channel input length                       |
        +------------------------------------------------*/
		case PROFIDP_CH_INPUT_LEN:
			*valueP = (int32) llHdl->chInfo[ch].num_in;
			break;


        /*------------------------------------------------+
        |  get channel output length                      |
        +------------------------------------------------*/
		case PROFIDP_CH_OUTPUT_LEN:
			*valueP = (int32) llHdl->chInfo[ch].num_out;
			break;

        /*--------------------------+
        |  debug level              |
        +--------------------------*/
        case M_LL_DEBUG_LEVEL:
            *valueP = llHdl->dbgLevel;
            break;
        /*--------------------------+
        |  number of channels       |
        +--------------------------*/
        case M_LL_CH_NUMBER:
            *valueP = CH_NUMBER;
            break;
        /*--------------------------+
        |  channel direction        |
        +--------------------------*/
        case M_LL_CH_DIR:
            *valueP = GetChannelDir( llHdl, ch );
            break;
        /*--------------------------+
        |  channel type info        |
        +--------------------------*/
        case M_LL_CH_TYP:
            *valueP = M_CH_BINARY;
            break;
        /*--------------------------+
        |  irq counter              |
        +--------------------------*/
        case M_LL_IRQ_COUNT:
            *valueP = llHdl->irqCount;
            break;
        /*--------------------------+
        |  ID PROM check enabled    |
        +--------------------------*/
        case M_LL_ID_CHECK:
            *valueP = llHdl->idCheck;
            break;
        /*--------------------------+
        |   ID PROM size            |
        +--------------------------*/
        case M_LL_ID_SIZE:
            *valueP = MOD_ID_SIZE;
            break;
        /*--------------------------+
        |   ID PROM data            |
        +--------------------------*/
        case M_LL_BLK_ID_DATA:
		{
			u_int32 n;
			u_int16 *dataP = (u_int16*)blk->data;

			if (blk->size < MOD_ID_SIZE)		/* check buf size */
				return(ERR_LL_USERBUF);

			for (n=0; n<MOD_ID_SIZE/2; n++)		/* read MOD_ID_SIZE/2 words */
				*dataP++ = (u_int16)m_read( (U_INT32_OR_64) llHdl->ma, (u_int8) n);

			break;
		}
        /*--------------------------+
        |   ident table pointer     |
        |   (treat as non-block!)   |
        +--------------------------*/
        case M_MK_BLK_REV_ID:
           *value64P = (INT32_OR_64)&llHdl->idFuncTbl;
           break;

        /*--------------------------+
        |  (unknown)                |
        +--------------------------*/
        default:
            error = ERR_LL_UNK_CODE;
    }

	return(error);
}

/***************************** PROFIDP_BlockRead ***************************
 *
 *  Description:  Read data from a Profibus channel (Slave)
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl        low-level handle
 *                ch           current channel
 *                buf          data buffer
 *                size         data buffer size
 *  Output.....:  nbrRdBytesP  number of read bytes
 *                return       success (0) or error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 PROFIDP_BlockRead(
     LL_HANDLE *llHdl,
     int32     ch,
     void      *buf,
     int32     size,
     int32     *nbrRdBytesP
)
{
	u_int16 dataSize;

    DBGWRT_1((DBH, "LL - PROFIDP_BlockRead: ch=%d, size=%d\n",ch,size));

	/* check if reading from channel is allowed */
	if ( !llHdl->chInfo[ch].num_in || (ch < llHdl->lowest_slave_address) ) {
		DBGWRT_ERR((DBH, " *** PROFIDP_BlockRead: current channel = %03d is no input channel", ch));
		return (ERR_LL_ILL_CHAN);
	}

	dataSize = (u_int16) llHdl->chInfo[ch].num_in;

	/* check if user buffer is to small */
	if ( size < dataSize ) {
		DBGWRT_ERR((DBH, " *** PROFIDP_BlockRead: User buffer is to small"));
		return (ERR_LL_USERBUF);
	}

	/* read data for slave out of DPRAM */
	if (profi_get_data( llHdl, ID_DP_SLAVE_IO_IMAGE, (USIGN16) DP_ARRAY_OFFSET_IN (ch), &dataSize, buf)
	    != E_OK) {
		DBGWRT_ERR((DBH, " *** PROFIDP_BlockRead: PROFIDP_BLK_SET_DATA failed\n"));
		return (PROFIDP_ERR_GET_DATA);
	}
	/* return number of read bytes */
	*nbrRdBytesP = dataSize;

	/* check if FW is still alive */
	if( PROFIDP_aliveCheck( llHdl ) ) {
		DBGWRT_ERR((DBH, " *** PROFIDP_BlockRead: FW is not alive !!\n"));
		return (PROFIDP_ERR_FW_NOT_ALIVE);
	}

	return(ERR_SUCCESS);
}

/**************************** PROFIDP_BlockWrite ***************************
 *
 *  Description:  Write data to a Profibus channel (Slave)
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl        low-level handle
 *                ch           current channel
 *                buf          data buffer
 *                size         data buffer size
 *  Output.....:  nbrWrBytesP  number of written bytes
 *                return       success (0) or error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 PROFIDP_BlockWrite(
     LL_HANDLE *llHdl,
     int32     ch,
     void      *buf,
     int32     size,
     int32     *nbrWrBytesP
)
{
	u_int16 dataSize;

    DBGWRT_1((DBH, "LL - PROFIDP_BlockWrite: ch=%d, size=%d\n",ch,llHdl->chInfo[ch].num_out));

	/* check if writing from channel is allowed */
	if ( !llHdl->chInfo[ch].num_out || (ch < llHdl->lowest_slave_address) ) {
		DBGWRT_ERR((DBH, " *** PROFIDP_BlockWrite: current channel = %03d is no output channel"
		                 " lowest slave adderss = %03d", ch, llHdl->lowest_slave_address));
		return (ERR_LL_ILL_CHAN);
	}


	dataSize = (u_int16) llHdl->chInfo[ch].num_out;

	/* check if user buffer is to small */
	if ( size < dataSize ) {
		DBGWRT_ERR((DBH, " *** PROFIDP_BlockWrite: User buffer is to small"));
		return (ERR_LL_USERBUF);
	}

	/* write output data for slave to DPRAM */
	if (profi_set_data( llHdl, ID_DP_SLAVE_IO_IMAGE, (USIGN16) DP_ARRAY_OFFSET_OUT (ch),
	    dataSize, buf)  != E_OK) {
			DBGWRT_ERR((DBH, " *** PROFIDP_BlockWrite: PROFIDP_BLK_SET_DATA failed\n"));
			return (PROFIDP_ERR_SET_DATA);
	}


	/* return number of written bytes */
	*nbrWrBytesP = dataSize;

	/* check if FW is still alive */
	if( PROFIDP_aliveCheck( llHdl ) ) {
		DBGWRT_ERR((DBH, " *** PROFIDP_BlockRead: FW is not alive !!\n"));
		return (PROFIDP_ERR_FW_NOT_ALIVE);
	}

	return(ERR_SUCCESS);
}

/**************************** PROFIDP_Irq **********************************
 *
 *  Description:  Interrupt service routine
 *
 *                The interrupt is triggered to inform the host on a
 *                confirmation or indication.
 *
 *                If the driver can detect the interrupt's cause it returns
 *                LL_IRQ_DEVICE or LL_IRQ_DEV_NOT, otherwise LL_IRQ_UNKNOWN.
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl    low-level handle
 *  Output.....:  return   LL_IRQ_DEVICE	irq caused by device
 *                         LL_IRQ_DEV_NOT   irq not caused by device
 *                         LL_IRQ_UNKNOWN   unknown
 *  Globals....:  ---
 ****************************************************************************/
static int32 PROFIDP_Irq(
   LL_HANDLE *llHdl
)
{
	IDBGWRT_2((DBH, " >>> PROFIDP_Irq: Signal ISR-Task semaphore <<<\n"));
	if ((OSS_SemSignal( llHdl->osHdl, llHdl->isrTaskSemP )) != 0) {
		DBGWRT_ERR((DBH," *** PROFIDP_Irq: Error signaling ISR-Task semaphore\n"));
	}

	/* notify Profibus module that int was received */
	M57_IRQ_ACK(llHdl->ma);	/* release interrupt */

	return (LL_IRQ_UNKNOWN);
}

/**************************** PROFIDP_IsrTask **********************************
 *
 *  Description:  Interrupt service task
 *
 *                This routine is triggered by the ISR using a semaphore
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl    low-level handle
 *  Output.....:  return   void
 *  Globals....:  ---
 ****************************************************************************/
static void PROFIDP_IsrTask(LL_HANDLE *llHdl)
{
	u_int16 con_size;
	T_PROFI_SERVICE_DESCR      c_sdb;
    T_DP_GET_SLAVE_DIAG_CON*   diagStruct;
    T_DP_DIAG_DATA*            diagData;
    T_FMB_FM2_EVENT_IND*       fm2;
	u_int8  irqVal;

	while( 1 ) {

		OSS_SemWait( llHdl->osHdl, llHdl->isrTaskSemP, OSS_SEM_WAITFOREVER );
		IDBGWRT_1((DBH, " >>> !!! PROFIDP_IrqTask <<<: ma = 0x%08x \n", llHdl->ma));

		/* wait for window pointer to get free */
		if( ERROR == semTake( llHdl->windowPointerSemId, WAIT_FOREVER ) ) {
			DBGWRT_ERR((DBH," >>> PROFIDP_IrqTask: Error taking window pointer semaphore\n"));
		}


		irqVal = (u_int8) DP_READ_INT8( llHdl->ma, COFF(H_ID));

		llHdl->irqFlag = 1;

		switch ( irqVal ) {

			case 0xf0:

				if( llHdl->getSlaveDiagReqWaitAck  ){
					/* ack to request sent by the irq routine */
					DBGWRT_3((DBH," got Ack for DP_GET_SLAVE_DIAG req\n"));
					MWRITE_D8( llHdl->ma, H_SEMA, _IDLE);
					/* clear flag getSlaveDiagReqWaitAck */
					llHdl->getSlaveDiagReqWaitAck = FALSE;
				}
				else {
					if ((OSS_SemSignal( llHdl->osHdl, llHdl->req_con_f0_semP )) != 0) {
						DBGWRT_ERR((DBH," *** PROFIDP_IrqTask: Error signaling REQ/CON semaphore\n"));
					}
				}
				llHdl->reqPending = FALSE;

				break;


			case 0x0f:

				con_size = sizeof(llHdl->con_buf); /* sizeof(llHdl->req_con_buf); */
				IDBGWRT_1((DBH, " >>> Entering profi_rcv_con_ind\n"));

				/* receive CON/IND which caused IRQ */
				if( profi_rcv_con_ind( llHdl, &c_sdb, llHdl->con_buf, &con_size )
					!= CON_IND_RECEIVED){

					IDBGWRT_ERR((DBH, " >>> PROFIDP_IrqTask: Error receiving CON / IND\n"));
					IDBGWRT_1((DBH, " >>> PROFIDP_IrqTask: c_sdb.service = %02x c_sdb.primitive = %02x "
									 "c_sdb.layer = %02x \n", c_sdb.service , c_sdb.primitive, c_sdb.layer));
				}
				IDBGWRT_3((DBH, " >>> PROFIDP_IrqTask: c_sdb.service = %02x c_sdb.primitive = %02x "
								 "c_sdb.layer = %02x\n",
								  c_sdb.service , c_sdb.primitive, c_sdb.layer));
				/* check if driver is internally waiting for CON/IND */
				if(	(llHdl->waitForService.layer == c_sdb.layer && llHdl->waitForService.service == c_sdb.service &&
				 llHdl->waitForService.primitive == c_sdb.primitive) /* || llHdl->conIndWaitFalg == 1 */ ) {

					OSS_MemCopy(llHdl->osHdl, sizeof (llHdl->req_con_buf), (char*) llHdl->con_buf,
								(char*) (llHdl->req_con_buf));

					llHdl->waitForService.invoke_id = c_sdb.invoke_id;
					llHdl->waitForService.result    = c_sdb.result;
					llHdl->waitForService.comm_ref  = c_sdb.comm_ref;

					/* signal semaphore */
					if ((OSS_SemSignal( llHdl->osHdl, llHdl->req_con_0f_semP )) != 0) {
						DBGWRT_ERR((DBH," *** PROFIDP_Init: Error signaling REQ/CON semaphore\n"));
					}

					/* send diag request immediately after pendig user request */
					if (llHdl->getSlaveDiagReqDelayed) {
						PROFIDP_sendDiagReqIrq(llHdl);
						llHdl->getSlaveDiagReqDelayed = FALSE;
					}
				}

				else {

					if ( !llHdl->cyclicDataTransfer ) {

						/* save CON/IND in buffer */
						PROFIDP_inConIndBuffer ( llHdl, &c_sdb, llHdl->con_buf );

						/* give semaphore for CON/IND buffer if no overflow has occured */
						if (llHdl->con_ind_buf_full != 0x01) {
							if ((OSS_SemSignal( llHdl->osHdl, llHdl->con_buf_semP )) != 0) {
								DBGWRT_ERR((DBH," *** PROFIDP_Init: Error signaling CON/IND buffer semaphore\n"));
							}
						}

						/* send signal if installed */
						if (llHdl->sigHdl != NULL) {
							OSS_SigSend(llHdl->osHdl, llHdl->sigHdl);

							IDBGWRT_1((DBH, " >>> PROFIDP_IrqTask: Signal sent\n"));
						}
					}
					else {

						if ( c_sdb.service == DP_GET_SLAVE_DIAG &&
							 (c_sdb.primitive == IND || c_sdb.primitive == CON )){
							int16 diagEntries;

							/*
							 * slave diagnostic info update
							 */
							diagStruct = (T_DP_GET_SLAVE_DIAG_CON*) llHdl->con_buf;
							diagData   = (T_DP_DIAG_DATA*) (diagStruct + 1);

							diagEntries = TWISTWORD(diagStruct->diag_entries);

							IDBGWRT_2((DBH, " >>> PROFIDP_IrqTask: Got slave DIAG addr=0x%02x status=0x%02x 0x%02x 0x%02x"
									   " mstaddr=0x%02x ident=0x%04x rem entries=%d\n",
									  diagStruct->rem_add,
									  diagData->station_status_1,
									  diagData->station_status_2,
									  diagData->station_status_3,
									  diagData->master_add,
									  TWISTWORD(diagData->ident_number),
									  diagEntries));

							if ( diagStruct->rem_add < (DP_MAX_NUMBER_SLAVES + 1)
								 && !diagStruct->status ) {
								OSS_MemCopy(llHdl->osHdl, TWISTWORD(diagStruct->diag_data_len), (char*) diagData,
											(char*) (&llHdl->chDiag[diagStruct->rem_add][0]));

								/* send signal for diag updated if installed */
								if ( llHdl->sigHdl != NULL  ) {
									OSS_SigSend(llHdl->osHdl, llHdl->sigHdl);

									IDBGWRT_1((DBH, ">>> PROFIDP_IrqTask: Signal sent\n"));

								}
							}
							else {
								IDBGWRT_ERR((DBH, " >>> *** PROFIDP_IrqTask: Diag IND rem_add or status mismatch "
										   "rem_add=%02x status=%x\n",
										diagStruct->rem_add, diagStruct->status ));
							}

							/* clear flag getSlaveDiagReqWaitCon */
							llHdl->getSlaveDiagReqWaitCon = FALSE;
							/* clear flag fwAliveCheckWait */
							llHdl->fwAliveCheckWait = FALSE;

							/*
							 * If more diags pending, trigger a DP_GET_SLAVE_DIAG REQ so
							 * that FW sends us the next one
							 * Note: diagEntries is set to -1 in case of DIAG overflow
							 */
							if( diagEntries != 0){

								/*
								 * Before attempting to send the request, check if
								 * there is already a request pending from
								 * the main routines
								 */
								if( llHdl->reqPending ){
									llHdl->getSlaveDiagReqDelayed = TRUE;
									IDBGWRT_2((DBH," Already a req. pending...\n"));
								}
								else {
									/* send DP_GET_SLAVE_DIAG req */
									PROFIDP_sendDiagReqIrq(llHdl);

								}
							}
						}

						else {
							/* save FMB FM2 EVENT indication reason in handle, if cyclic data transfer is
							   selected */
							if ( c_sdb.service == FMB_FM2_EVENT && c_sdb.primitive == IND &&
								 c_sdb.layer == FMB_USR && llHdl->cyclicDataTransfer) {
									fm2 = (T_FMB_FM2_EVENT_IND*) llHdl->con_buf;
									DBGWRT_2((DBH,"FMB FM2 Event IND 0x%04x\n", TWISTWORD(fm2->reason)));
									if (TWISTWORD(fm2->reason) < 7)
										llHdl->fm2EventReason |= (1 << (TWISTWORD(fm2->reason)));

									/* send signal if installed */
									if (llHdl->sigHdl != NULL ) {
										OSS_SigSend(llHdl->osHdl, llHdl->sigHdl);

										IDBGWRT_1((DBH, ">>> PROFIDP_IrqTask: Signal sent\n"));
									}

							 }

							 /* save unknown CON/IND ? */
						}
					}
				}

				break;

			default:

				DBGWRT_ERR((DBH, " >>> PROFIDP_IrqTask: unknown IRQ \n"));
				break;
		}
		llHdl->irqFlag = 0;

		IDBGWRT_1((DBH, " >>> PROFIDP_IrqTask: >>> END IRQ <<<\n\n\n"));

		if ( irqVal == 0x0f || irqVal == 0xf0 ) {
			llHdl->irqCount++;  /* increase IRQ counter */
			/* notify Profibus module that int was received */
			DP_WRITE_INT8(llHdl->ma, COFF(H_ID), 0);
			/* restore window pointer */
			DP_SET_WINDOW(llHdl->ma, llHdl->current_wptr);
		}

		if( ERROR == semGive( llHdl->windowPointerSemId ) ) {
			DBGWRT_ERR((DBH," >>> PROFIDP_IrqTask: Error giving window pointer semaphore\n"));
		}

		/* taskDelay( 1000 ); */
	} /* while */
}

/****************************** PROFIDP_Info ************************************
 *
 *  Description:  Get information about hardware and driver requirements
 *
 *                The following info codes are supported:
 *
 *                Code                      Description
 *                ------------------------  -----------------------------
 *                LL_INFO_HW_CHARACTER      hardware characteristics
 *                LL_INFO_ADDRSPACE_COUNT   nr of required address spaces
 *                LL_INFO_ADDRSPACE         address space information
 *                LL_INFO_IRQ               interrupt required
 *                LL_INFO_LOCKMODE          process lock mode required
 *
 *                The LL_INFO_HW_CHARACTER code returns all address and
 *                data modes (ORed) which are supported by the hardware
 *                (MDIS_MAxx, MDIS_MDxx).
 *
 *                The LL_INFO_ADDRSPACE_COUNT code returns the number
 *                of address spaces used by the driver.
 *
 *                The LL_INFO_ADDRSPACE code returns information about one
 *                specific address space (MDIS_MAxx, MDIS_MDxx). The returned
 *                data mode represents the widest hardware access used by
 *                the driver.
 *
 *                The LL_INFO_IRQ code returns whether the driver supports an
 *                interrupt routine (TRUE or FALSE).
 *
 *                The LL_INFO_LOCKMODE code returns which process locking
 *                mode the driver needs (LL_LOCK_xxx).
 *
 *---------------------------------------------------------------------------
 *  Input......:  infoType	   info code
 *                ...          argument(s)
 *  Output.....:  return       success (0) or error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 PROFIDP_Info(
   int32  infoType,
   ...
)
{
    int32   error = ERR_SUCCESS;
    va_list argptr;

    va_start(argptr, infoType );

    switch(infoType) {
		/*-------------------------------+
        |  hardware characteristics      |
        |  (all addr/data modes ORed)    |
        +-------------------------------*/
        case LL_INFO_HW_CHARACTER:
		{
			u_int32 *addrModeP = va_arg(argptr, u_int32*);
			u_int32 *dataModeP = va_arg(argptr, u_int32*);

			*addrModeP = MDIS_MA08;
			*dataModeP = MDIS_MD08 | MDIS_MD16;
			break;
	    }
		/*-------------------------------+
        |  nr of required address spaces |
        |  (total spaces used)           |
        +-------------------------------*/
        case LL_INFO_ADDRSPACE_COUNT:
		{
			u_int32 *nbrOfAddrSpaceP = va_arg(argptr, u_int32*);

			*nbrOfAddrSpaceP = ADDRSPACE_COUNT;
			break;
	    }
		/*-------------------------------+
        |  address space type            |
        |  (widest used data mode)       |
        +-------------------------------*/
        case LL_INFO_ADDRSPACE:
		{
			u_int32 addrSpaceIndex = va_arg(argptr, u_int32);
			u_int32 *addrModeP = va_arg(argptr, u_int32*);
			u_int32 *dataModeP = va_arg(argptr, u_int32*);
			u_int32 *addrSizeP = va_arg(argptr, u_int32*);

			if (addrSpaceIndex >= ADDRSPACE_COUNT)
				error = ERR_LL_ILL_PARAM;
			else {
				*addrModeP = MDIS_MA08;
				*dataModeP = MDIS_MD16;
				*addrSizeP = ADDRSPACE_SIZE;
			}

			break;
	    }
		/*-------------------------------+
        |   interrupt required           |
        +-------------------------------*/
        case LL_INFO_IRQ:
		{
			u_int32 *useIrqP = va_arg(argptr, u_int32*);

			*useIrqP = USE_IRQ;
			break;
	    }
		/*-------------------------------+
        |   process lock mode            |
        +-------------------------------*/
        case LL_INFO_LOCKMODE:
		{
			u_int32 *lockModeP = va_arg(argptr, u_int32*);

			*lockModeP = LL_LOCK_CALL;
			break;
	    }
		/*-------------------------------+
        |   (unknown)                    |
        +-------------------------------*/
        default:
          error = ERR_LL_ILL_PARAM;
    }

    va_end(argptr);
    return(error);
}

/*******************************  Ident  ************************************
 *
 *  Description:  Return ident string
 *
 *---------------------------------------------------------------------------
 *  Input......:  -
 *  Output.....:  return  pointer to ident string
 *  Globals....:  -
 ****************************************************************************/
static char* Ident( void )	/* nodoc */
{
    return( "PROFIDP - PROFIDP low level driver: $Id: profidp_drv.c,v 1.1 2014/11/05 10:51:26 AGromann Exp $" );
}

/********************************* Cleanup **********************************
 *
 *  Description: Close all handles, free memory and return error code
 *		         NOTE: The low-level handle is invalid after this function is
 *                     called.
 *
 *---------------------------------------------------------------------------
 *  Input......: llHdl		low-level handle
 *               retCode    return value
 *  Output.....: return	    retCode
 *  Globals....: -
 ****************************************************************************/
static int32 Cleanup(
   LL_HANDLE    *llHdl,
   int32        retCode		/* nodoc */
)
{
    /*------------------------------+
    |  close handles                |
    +------------------------------*/
	/* clean up desc */
	if (llHdl->descHdl)
		DESC_Exit(&llHdl->descHdl);

	/* clean up debug */
	DBGEXIT((&DBH));

    /*------------------------------+
    |  free memory                  |
    +------------------------------*/

	/* free CON/IND Buffer */
    if (llHdl->con_ind_buf)
    	OSS_MemFree(llHdl->osHdl, llHdl->con_ind_buf, llHdl->con_ind_memSize);


    /* free my handle */
    OSS_MemFree(llHdl->osHdl, (int8*)llHdl, llHdl->memAlloc);

    /*------------------------------+
    |  return error code            |
    +------------------------------*/
	return(retCode);
}


/***************************** PROFIDP_Config ******************************
 *
 *  Description: Configure Profibus
 *
 *---------------------------------------------------------------------------
 *  Input......: llHdl		low-level handle
 *               blk        pointer to M_SG_BLOCK structure
 *  Output.....: return	    retCode (-1 Error 0 Success)
 *  Globals....: -
 ****************************************************************************/

static int16 PROFIDP_Config ( LL_HANDLE* llHdl, M_SG_BLOCK *blk ) /* nodoc */
{

	T_FMB_SET_CONFIGURATION_REQ  *fmbSetConfig;
	T_DP_INIT_MASTER_REQ        *dpInitMaster;
	u_int8                      i;
	u_int8*                     dummy_p;
	int32                       retVal;
	T_DP_BUS_PARA_SET*          buspar;
	T_DP_SLAVE_PARA_SET*        slavepar;
	u_int16                     con_status;
	USIGN16                     max_bus_para_len;
	USIGN16                     max_slave_para_len;
	STRINGV*					master_class2_name_adr;
	u_int16                     sl_para_len;
	u_int8                      sl_addr;
	u_int8*                     next_sl_add;
	u_int8                      sl_count;
	u_int8                      num_in;
	u_int8                      num_out;
	T_DP_DOWNLOAD_REQ*          dl;  /* bus para set */
	u_int8                      slave; /* slave para set */
	u_int16                     new_data_len; /* buffer for new download data len */


	DBGWRT_1((DBH, "LL - PROFIDP_Config: Start Firmware \n"));

	/* set autostart address to firmware start address */
	DP_WRITE_INT32(llHdl->ma, (u_int32) DP_AUTO_START_ADDR_POINTER, (u_int32) DP_AUTO_START_ADDR);

	/* Initialize CMI interface */
	DBGWRT_1((DBH, "LL - PROFIDP_Config: Start CMI-Initialization\n"));
	retVal = (int16) profi_init(llHdl);

	switch (retVal) {
		case E_IF_NO_CNTRL_RES:
			DBGWRT_1((DBH, "LL - PROFIDP_Config: No controller respons\n"));
			return -1;

		case E_OK:
			DBGWRT_1((DBH, "LL - PROFIDP_Config: CMI configuration ok!\n"));

		break;

		default:
			DBGWRT_1((DBH, "LL - PROFIDP_Config: Unknown ERROR occured\n"));
			return -1;
	}

	max_bus_para_len = TWISTWORD(*(u_int16*)blk->data); /* TWISTWORD(*(u_int16*)DP_konfig); */
	DBGWRT_1((DBH, "LL - PROFIDP_Config: max_bus_para_len = %04x\n", max_bus_para_len));

	/* extract master default address from config data (fdl_add) */
	llHdl->master_default_address = *((u_int8*) blk->data + 2);   /* DP_konfig[3]; */
	DBGWRT_1((DBH, "LL - PROFIDP_Config: master_default_address = %02x\n", llHdl->master_default_address));

	/* extract master_class2_name from config data */
	master_class2_name_adr = (STRINGV*) (((u_int8*) blk->data) + 34);

	/* extract slave affecting data out of config data */
	next_sl_add = (((u_int8*) blk->data) + 0x42);  /* (DP_konfig +0x42); */
	DBGWRT_2((DBH, "LL - PROFIDP_Config get offs: next_sl_add (rel. to array start) = %04x\n",
    	            (next_sl_add - ((u_int8*) blk->data))));

	sl_count = 0;
	llHdl->max_slave_input_len = 0;
	llHdl->max_slave_output_len = 0;
	max_slave_para_len = 0;
	num_in = 0;
	num_out = 0;

	do {

		next_sl_add = get_offs(llHdl, next_sl_add, &sl_addr, &num_in, &num_out, &sl_para_len);

		/* get lowest slave address */
		if (sl_count == 0 && llHdl->lowest_slave_address == 0xff)
			llHdl->lowest_slave_address = sl_addr;

		/* fill chInfo sturcture for current slave */
		llHdl->chInfo[sl_addr].num_in = num_in;
		llHdl->chInfo[sl_addr].num_out = num_out;


		/* get max slave input length */
		if ( num_in > llHdl->max_slave_input_len)
			llHdl->max_slave_input_len = num_in;

		/* get max slave output length */
		if ( num_out > llHdl->max_slave_output_len)
			llHdl->max_slave_output_len = num_out;

		/* get max slave parameter length */
		if (max_slave_para_len < sl_para_len)
			max_slave_para_len = sl_para_len;

		DBGWRT_2((DBH, "LL - PROFIDP_Config: max_slave parameter lenght = %08x\n",
		               max_slave_para_len));

		DBGWRT_2((DBH, "LL - PROFIDP_Config: max_slave input lenght = %08x\n",
		               llHdl->max_slave_input_len));
		DBGWRT_2((DBH, "LL - PROFIDP_Config: max_slave output lenght = %08x\n",
		               llHdl->max_slave_output_len));

		DBGWRT_2((DBH, "LL - PROFIDP_Config: get offs: sl_addr = %02x num_out = %08x"
	    	           " num_in = %08x sl_data_len %04x\n",
	        	       sl_addr, num_out, num_in, sl_para_len));
		DBGWRT_2((DBH, "LL - PROFIDP_Config get offs: next_sl_add (rel. to array start) = %04x\n",
	    	            (next_sl_add - ((u_int8*) blk->data))));
		sl_count++;
	} while (next_sl_add < ( (u_int8*) (((u_int8*) blk->data) + blk->size) ));

	DBGWRT_2((DBH, "LL - PROFIDP_Config get offs: sl_count = %02x\n", sl_count));

	/* clear memory-space for fmbSetConfig */
	fmbSetConfig = (T_FMB_SET_CONFIGURATION_REQ *)llHdl->req_con_buf;
	dummy_p = (u_int8*) fmbSetConfig;
	for (i = 0; i < sizeof (*fmbSetConfig); i++) {
		*dummy_p = 0;
		dummy_p++;
	}

	max_slave_para_len += 4;  /* add possible alignment bytes which are added */
	                          /* by function PROFIDP_copy_buspar */
	if (max_slave_para_len > DP_MAX_DOWNLOAD_DATA_LEN)
		max_slave_para_len = DP_MAX_DOWNLOAD_DATA_LEN;

	/* kp: max_slave_input_len/max_slave_output length must be word aligned */

	if( llHdl->max_slave_output_len & 1 )
		llHdl->max_slave_output_len++;
	if( llHdl->max_slave_input_len & 1 )
		llHdl->max_slave_input_len++;


	/* assigning values form configuration file array */
	fmbSetConfig->dp_active 			    	= PB_TRUE;
	fmbSetConfig->fdl_evt_receiver 			= TWISTWORD(FMB_USR);
	fmbSetConfig->data_buffer_length 		= TWISTWORD(0xff);
	fmbSetConfig->dp.max_number_slaves 		= (u_int8) llHdl->maxNumSlaves;
	fmbSetConfig->dp.max_slave_output_len 	= llHdl->max_slave_output_len;
	fmbSetConfig->dp.max_slave_input_len 	= llHdl->max_slave_input_len;
	fmbSetConfig->dp.max_slave_diag_entries  = (u_int8) llHdl->maxSlaveDiagEntries;
	fmbSetConfig->dp.max_slave_diag_len  	= (u_int8) llHdl->maxSlaveDiagLen;
	fmbSetConfig->dp.max_bus_para_len 		= TWISTWORD(max_bus_para_len);
	fmbSetConfig->dp.max_slave_para_len  	= TWISTWORD(max_slave_para_len);

    DBGWRT_2((DBH, "LL - PROFIDP_SetStat: Issue FMB set configuration request\n"));
	if ( (req_con ( llHdl, FMB, FMB_SET_CONFIGURATION, (u_int8*) fmbSetConfig, &con_status))) {
	    DBGWRT_ERR((DBH, " *** LL - PROFIDP_SetStat: req_con Error\n"));
		goto abort;
	}

	/* Issue Init master */
	dpInitMaster = (T_DP_INIT_MASTER_REQ *)llHdl->req_con_buf;

	dpInitMaster->master_default_address = llHdl->master_default_address;
	dpInitMaster->master_class2 			= (PB_BOOL) llHdl->masterClass2;
	OSS_MemCopy(llHdl->osHdl, sizeof(dpInitMaster->master_class2_name),
	     master_class2_name_adr, dpInitMaster->master_class2_name);
	dpInitMaster->lowest_slave_address 	= llHdl->lowest_slave_address;
	dpInitMaster->slave_io_address_mode 	= (u_int8) llHdl->addrAssignMode; /*slave_io_address_mode;*/
	dpInitMaster->clear_outputs 			= (PB_BOOL) llHdl->clearOutputs;
	dpInitMaster->auto_remote_services 	= (u_int8) llHdl->autoRemoteService;
	dpInitMaster->cyclic_data_transfer 	= (PB_BOOL) llHdl->cyclicDataTransfer;

    DBGWRT_2((DBH, "LL - PROFIDP_SetStat: Issue Init master request sizeof:\n"));
	if( (req_con( llHdl, DP, DP_INIT_MASTER,(u_int8 *)dpInitMaster,
						&con_status )))													{
	    DBGWRT_ERR((DBH, " *** LL - PROFIDP_SetStat: req_con Error\n"));
		goto abort;
	}

	/*------------------------+
    |  Load Busparameter Set  |
    +------------------------*/

	dl = (T_DP_DOWNLOAD_REQ *) llHdl->req_con_buf;
	/*--- get ptr to busparameter struct ---*/
	if((buspar = dp_bincfg_get_buspar(llHdl, (u_int8*) blk->data, (u_int32) blk->size ))==NULL){
		DBGWRT_ERR((DBH, "Can't find BUS PARAMETER struct!\n"));
		goto abort;
	}
	dl->data_len = buspar->bus_para_len;
	dl->rem_add  = llHdl->master_default_address; /* 0 */
	dl->area_code = DP_AREA_BUS_PARAM;
	dl->add_offset = TWISTWORD(0);


	OSS_MemCopy( llHdl->osHdl, TWISTWORD(dl->data_len), (char*) buspar, (char*)(dl+1));

	DBGWRT_2((DBH, "LL - PROFIDP_SetStat: Loading bus parameter\n"));

	if( (req_con(llHdl, DP, DP_DOWNLOAD_LOC, (u_int8 *)dl,
						  &con_status )))
		goto abort;


	/*-------------------------------+
    |  Load all slave parametersets  |
    +-------------------------------*/


	for( slave=0; slave<127; slave++ ){

		/*--- get ptr to slaveparameter struct ---*/
		if((slavepar = dp_bincfg_get_slavepar( llHdl, (u_int8*) blk->data, (u_int32) blk->size, slave ))){

			dl->data_len = slavepar->slave_para_len;
			dl->rem_add  = llHdl->master_default_address; /* 0 */
			dl->area_code = slave;
			dl->add_offset = TWISTWORD(0);

			new_data_len = (u_int16) PROFIDP_copy_buspar((u_int16) TWISTWORD(dl->data_len), (u_int8*) slavepar, (u_int8*)(dl+1));
			dl->data_len = TWISTWORD(new_data_len);
			DBGWRT_2((DBH, "Loading slave parameter set %d\n", slave));

			if( (req_con( llHdl/*mode*/, DP, DP_DOWNLOAD_LOC, (u_int8 *)dl,
					  &con_status )))
				goto abort;
		}
	}


	return (0);		/* configuration done and ok */
	abort:
	    DBGWRT_ERR((DBH, " *** LL - PROFIDP_SetStat: req_con Error doing abort condition\n"));
		return (-1);


}

/************************* dp_bincfg_get_buspar ****************************
 *
 *  Description: Get pointer to Bus-Parameter data out of configuration array
 *
 *---------------------------------------------------------------------------
 *  Input......: llHdl		low-level handle
 *               data_ptr   pointer to configuration data array
 *				 data_size  size of configuration data array
 *
 *  Output.....: return	    data_ptr if OK or NULL if data not valid
 *  Globals....: -
 ****************************************************************************/

static T_DP_BUS_PARA_SET *dp_bincfg_get_buspar(LL_HANDLE* llHdl, u_int8* data_ptr, u_int32 data_size ) /* nodoc */

{

	if( data_size >= _skip_align( llHdl, data_ptr ))
		return (T_DP_BUS_PARA_SET*) data_ptr;
	else
		return NULL;
}

/************************ dp_bincfg_get_slavepar ****************************
 *
 *  Description: Get pointer to Slave-Parameter data out of configuration array
 *
 *---------------------------------------------------------------------------
 *  Input......: llHdl		    low-level handle
 *               data_ptr       pointer to configuration data array
 *				 data_size      size of configuration data array
 *               slave_address  address of slave
 *
 *  Output.....: return	    data_ptr if OK or NULL if data not valid
 *  Globals....: -
 ****************************************************************************/

static T_DP_SLAVE_PARA_SET *
 dp_bincfg_get_slavepar(LL_HANDLE* llHdl, u_int8* data_ptr, u_int32 data_size, u_int8 slave_address ) /* nodoc */
{
	u_int32 len,addr;

	len = _skip_align( llHdl, data_ptr );        /* skip busparameters */
	data_ptr += len;
	data_size -= len;

	while( data_size > 4 ){
		addr = *data_ptr++;			              /* slave address */
		data_size--;
		len = _skip_align( llHdl, data_ptr );     /* length of parameter set */

		if( addr == slave_address ){
			if( data_size >= len )
				return (T_DP_SLAVE_PARA_SET*) data_ptr;
			else
				return NULL;
		}
		data_ptr += len;
		data_size -= len;
	}
	return NULL;
}

/***************************** _sikip_align *********************************
 *
 *  Description: Return size of following data structure
 *
 *---------------------------------------------------------------------------
 *  Input......: llHdl		low-level handle
 *               data_ptr   pointer to configuration data array
 *
 *  Output.....: return	    size (word aligned)
 *  Globals....: -
 ****************************************************************************/

static u_int32 _skip_align(LL_HANDLE* llHdl, u_int8* data_ptr ) /* nodoc */

{
	u_int32 size;
	size = TWISTWORD( *((u_int16*) data_ptr));
	return size;
}


/****************************** load_segment *******************************
 *
 *  Description: Load a firmware segment
 *
 *---------------------------------------------------------------------------
 *  Input......: base       base address of memory window
 *               seg_off    offset in firmware array
 *               offset     load address in DRAM
 *               length     data length of segment
 *               llHdl      low-level handle
 *
 *  Output.....: -
 *  Globals....: -
 ****************************************************************************/

static void load_segment ( MACCESS base, u_int32 seg_offs,
                    u_int32 offset, u_int32 length, LL_HANDLE* llHdl ) /* nodoc */
{
	u_int16 *p = (u_int16*) &dp_fw[(seg_offs + M57_ARRAY_OFFS)];

	DBGWRT_2((DBH,"\nseg_offs = %08x, base = %08p, offset = %08x, length = %08x, *p = %04x\n",
	        seg_offs, (void*)base, offset, length, *p));
	SET_WINDOW( base, offset );

	while( length > 0 ){
		DP_DATA_W_WRITE( base, TWISTWORD(*p) );
		p++;
		length -= 2;
	}
}


/***************************** verify_segment *******************************
 *
 *  Description: Verify a firmware segment
 *
 *---------------------------------------------------------------------------
 *  Input......: base       base address of memory window
 *               seg_off    offset in firmware array
 *               offset     load address in DRAM
 *               length     data length of segment
 *               llHdl      low-level handle
 *
 *  Output.....: -
 *  Globals....: -
 ****************************************************************************/

static int verify_fw_segment ( MACCESS base, u_int32 seg_offs,
                         u_int32 offset, u_int32 length, LL_HANDLE* llHdl) /* nodoc */
{
	u_int16 rval;
	u_int16 *p = (u_int16 *) &dp_fw[(seg_offs + M57_ARRAY_OFFS)];

	SET_WINDOW( base, offset );

	while( length > 0 ){

		rval = DP_DATA_W_READ( base );

		if( rval != TWISTWORD(*p)){
			DBGWRT_ERR((DBH,"*** PROFIDP_Init: Error verifying firmware!\n"));
			DBGWRT_ERR((DBH,"*** PROFIDP_Init: Error at address offset"
			                " $%x should be $%04x, is $%04x\n",
			                offset, *p, rval ));
			return (-1);
		}
		p++;
		offset += 2;
		length -= 2;
	}
return (0);

}

/******************************* req_con ************************************
 *
 *  Description:  Send a request to protocol stack and wait
 *                for its confirmation
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl		  low-level handle
 *				  layer		  destination layer id
 *				  service: 	  service to execute
 *				  data_ptr:   service specific data
 *				  status_ptr: pointer to u_int16 to store the status code
 *							  returned with the confirmation
 *
 *  Output.....:  0: ok
 *				  1: Unsuccessful status in CON, see *status_ptr
 *				  -1: Fatal error executing request (errno set)
 *  Globals....:  profibus_fd, special_conind
 ****************************************************************************/

static int16 req_con( LL_HANDLE* llHdl, u_int8 layer, u_int8 service,
                    u_int8* data_ptr, u_int16* status_ptr ) /* nodoc */
{
	T_PROFI_SERVICE_DESCR psd;
	u_int16 error;

	psd.comm_ref = TWISTWORD(0);
	psd.layer = layer;
	psd.service = service;
	psd.primitive = REQ;

	/*---------------------------------+
    |  Send Request to protocol stack  |
    +---------------------------------*/
	DBGWRT_2((DBH, "LL - PROFIDP_req_con\n"));

	if( (error = profi_snd_req_res_usr( llHdl, &psd, data_ptr, 1 ))){
		DBGWRT_ERR((DBH," *** profi_snd_req_res_usr: ERROR error code = %04xh\n", error ));
		return -1;
	}

	/* wait for confiramtion */
	if ((OSS_SemWait( llHdl->osHdl, llHdl->req_con_0f_semP, 5000 )) != 0) {
		DBGWRT_ERR((DBH," *** PROFIDP_req_con: Error or Timeout REQ/CON semaphore\n"));
		return ( -1 );
	}

	if( llHdl->waitForService.result == NEG ){
		*status_ptr = (u_int16)((llHdl->req_con_buf[0] << 8) + llHdl->req_con_buf[1]);

		if( *status_ptr ){
				DBGWRT_ERR((DBH," *** Bad status in confirmation: %04x\n", *status_ptr ));
			}
		/* clear wait for service structure */
		waitForService_clear ( llHdl );
		return *status_ptr ? 1 : 0;
	}
	/* clear wait for service structure */
	waitForService_clear ( llHdl );
	return (0); /* POS */

}

/****************************** dp_setstate **********************************
 *
 *  Description:  Set state of protocol stack (OFFLINE, STOP, CLEAR, OPERATE)
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl:	low-level handle
 *
 *				  state:
 *						DP_ST_xxx
 *
 *				  con_status:
 *							Status returned with unsuccessful confirmation
 *							only valid when return value == 1
 *  Output.....:  0 = everything ok
 *				  1 = unsuccessful confirmation received.
 *					  Status code in con_status
 *				 -1 = fatal error, errno set.
 *  Globals....:
 ****************************************************************************/
static int16 dp_setstate( LL_HANDLE* llHdl, u_int8 state, u_int16* con_status ) /* nodoc */
{
	T_DP_ACT_PARAM_REQ ap;
	int16 error;

	ap.rem_add = llHdl->master_default_address; /* 1 */
	ap.area_code = DP_AREA_SET_MODE;
	ap.activate = state;

    DBGWRT_2((DBH, "LL - PROFIDP_SetStat: Set state to %02x\n", state ));

	error=req_con( llHdl, DP, DP_ACT_PARAM_LOC, (u_int8 *)&ap,
					 con_status );
	return error;
}

/************************* PROFIDP_data_transfer **************************
 *
 *  Description:  Perform Profibus Data Transfer Service
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl   low-level handle
 *				  blk     pointer to data structure M_SG_BLOCK
 *
 *
 *  Output.....:  0: ok
 *
 *				  -1: Fatal error executing request
 *  Globals....:  -
 ****************************************************************************/

static int16 PROFIDP_data_transfer (LL_HANDLE* llHdl, M_SG_BLOCK* blk) /* nodoc */
{

	u_int16   status_ptr;
	int16      retVal;
	u_int16   len;
	T_DP_DATA_TRANSFER_CON*  dataTrans;


	len = sizeof (T_DP_DATA_TRANSFER_CON);

	if ( blk->size < len ) {
		DBGWRT_ERR((DBH,"*** PROFIDP_data_transfer: Error user buffer to small\n"));
		return (2);
	}


	if ( (retVal = req_con( llHdl, DP, DP_DATA_TRANSFER,
    	                NULL, &status_ptr )) != 0)           {

		DBGWRT_ERR((DBH,"*** PROFIDP_data_transfer: Error req_con error code = %04x\n",
		                retVal));
		return (retVal);
	}

	dataTrans = (T_DP_DATA_TRANSFER_CON*) llHdl->req_con_buf;

	DBGWRT_2((DBH, "LL - PROFIDP_data transfer: status = %04x diag_entries = %04x\n",
	                dataTrans->status, dataTrans->diag_entries ));

	OSS_MemCopy(llHdl->osHdl, (u_int32) blk->size, (char*) llHdl->req_con_buf, (char*) blk->data);


	return ( 0 );
}


/************************ PROFIDP_get_slave_diag ***************************
 *
 *  Description:  Perform Get_Slave_Diag service
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl   low-level handle
 *				  blk     pointer to data structure
 *
 *
 *  Output.....:  0: ok
 *
 *				  -1: Fatal error executing request
 *  Globals....:  -
 ****************************************************************************/

static int16 PROFIDP_get_slave_diag (LL_HANDLE* llHdl, M_SG_BLOCK* blk) /* nodoc */
{
	u_int16   status_ptr;
	int16      retVal;
	u_int16   len;
	T_DP_GET_SLAVE_DIAG_CON*  diag;

	len = sizeof (T_DP_GET_SLAVE_DIAG_CON);

	if ( blk->size < len ) {
		DBGWRT_ERR((DBH,"*** PROFIDP_get_slave_diag: Error user buffer to small\n"));
		return (-1);
	}

	DBGWRT_3((DBH," PROFIDP_get_slave_diag: %d, %d, %d \n",llHdl->reqPending, llHdl->getSlaveDiagReqDelayed, llHdl->getSlaveDiagReqWaitCon ));


	if ( (retVal = req_con( llHdl, /* int mode, */ DP, DP_GET_SLAVE_DIAG,
    	                NULL, &status_ptr )) != 0)           {

		DBGWRT_ERR((DBH,"*** PROFIDP_get_salve_diag: Error req_con\n"));
		return (retVal);
	}

	DBGWRT_3((DBH," PROFIDP_get_slave_diag: %d, %d, %d \n",llHdl->reqPending, llHdl->getSlaveDiagReqDelayed, llHdl->getSlaveDiagReqWaitCon ));

	diag = (T_DP_GET_SLAVE_DIAG_CON*) llHdl->req_con_buf;

	DBGWRT_2((DBH, "LL - PROFIDP_data transfer: status = %04x diag_entries = %04x\n",
	                diag->status, diag->diag_entries ));


	OSS_MemCopy(llHdl->osHdl, (u_int32) blk->size, (char*) llHdl->req_con_buf, (char*) blk->data);

	return ( 1 );
}

/************************* PROFIDP_inConIndBuffer **************************
 *
 *  Description:  Write one confirmation or indication into buffer
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl   low-level handle
 *				  c_sdp   pointer to T_PROFI_SERVICE_DESCR
 *				  Buffer  pointer to CON/IND specific data
 *
 *  Output.....:  -
 *
 *  Globals....:  -
 ****************************************************************************/

static void PROFIDP_inConIndBuffer ( LL_HANDLE* llHdl, T_PROFI_SERVICE_DESCR* c_sdb,
                                     u_int8* Buffer ) /* nodoc */

{
	IDBGWRT_1((DBH, ">>> PROFIDP_Irq: save CON/IND in buffer\n"));

	/* save CON / IND in buffer if there is enough space */
	if ( llHdl->con_ind_num_el * CON_IND_BUF_ELEMENT_SIZE >= llHdl->con_ind_buf_size ) {

		/* buffer overflow */
		llHdl->con_ind_buf_full = 0x01;
		return;
	}

	llHdl->con_ind_num_el++;
	llHdl->con_ind_buf_full = 0;

	/* store service descriptor block */
	OSS_MemCopy(llHdl->osHdl, sizeof (T_PROFI_SERVICE_DESCR), (char*) c_sdb,
	   	        (char*) (llHdl->con_ind_buf_inP));

	/* store service data */
	OSS_MemCopy(llHdl->osHdl, DP_MAX_TELEGRAM_LEN, (char*) Buffer,
	   	       (char*) (llHdl->con_ind_buf_inP + sizeof (T_PROFI_SERVICE_DESCR)));

	llHdl->con_ind_buf_inP += CON_IND_BUF_ELEMENT_SIZE;

	if ( llHdl->con_ind_buf_inP >= ((u_int8*)(llHdl->con_ind_buf + llHdl->con_ind_buf_size)))
		llHdl->con_ind_buf_inP = llHdl->con_ind_buf;


}

/************************ PROFIDP_outConIndBuffer **************************
 *
 *  Description:  Read one confirmation or indication out of buffer
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl   low-level handle
 *				  blk     pointer to data structure
 *
 *  Output.....:  number of elements remaining in buffer
 *				  -1: Error
 *  Globals....:  -
 ****************************************************************************/
static int16 PROFIDP_outConIndBuffer ( LL_HANDLE* llHdl, M_SG_BLOCK* blk ) /* nodoc */
{
	/* if user buffer is to short or no element is in buffer return -1 */
	if ( blk->size < CON_IND_BUF_ELEMENT_SIZE) {
		DBGWRT_ERR((DBH,"*** PROFIDP_outConIndBuffer: block size to small\n"));
		return -1;

	}

	if ( llHdl->con_ind_num_el == 0 ) {
		DBGWRT_ERR((DBH,"*** PROFIDP_outConIndBuffer: no element to read\n"));
		return -1;
	}

	DBGWRT_2((DBH,"\nnumber of Elements in buffer = %04x", llHdl->con_ind_num_el));

	/* read element from buffer */
	OSS_MemCopy(llHdl->osHdl, CON_IND_BUF_ELEMENT_SIZE, (char*) llHdl->con_ind_buf_outP,
			     ((char*) blk->data) );
	llHdl->con_ind_num_el--;
	llHdl->con_ind_buf_outP += CON_IND_BUF_ELEMENT_SIZE;

	if ( llHdl->con_ind_buf_outP >= ((u_int8*)(llHdl->con_ind_buf + llHdl->con_ind_buf_size)))
		llHdl->con_ind_buf_outP = llHdl->con_ind_buf;

	if ( llHdl->con_ind_buf_full == 1 ) {
		DBGWRT_ERR((DBH,"*** PROFIDP_outConIndBuffer: buffer overflow occured\n"));
		return -1;
	}

	return ( llHdl->con_ind_num_el );
}



/************************** waitForService_clear ***************************
 *
 *  Description:  Clear wait for service structure
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl   low-level handle
 *
 *
 *  Output.....:  0: ok
 *
 *  Globals....:  -
 ****************************************************************************/
static int8 waitForService_clear ( LL_HANDLE* llHdl ) /* nodoc */
{
	/* clear wait for Service structure */
	llHdl->waitForService.comm_ref  = 0xffff;
	llHdl->waitForService.layer     = 0xff;
	llHdl->waitForService.service   = 0xff;
	llHdl->waitForService.primitive = 0xff;
	llHdl->waitForService.invoke_id = (PB_INT8) 0xff;
	llHdl->waitForService.result    = (PB_INT16) 0xffff;

	return (0);
}

/*************************** get_offs **************************************
 *
 *  Description: Get IN/OUTput offset of specific slave
 *
 *---------------------------------------------------------------------------
 *  Input......: llHdl		  low-level handle
 *               address      pointer to slave parameter length of specific slave
 *
 *  Output.....: sl_addr      slave address
 *               num_inputs   number of input bytes for slave
 *               num_outputs  number of output bytes for slave
 *               sl_para_len  length of slave parameter set
 *               return       memory address of next slave parameter set
 *
 *  Globals....: -
 ****************************************************************************/
static u_int8* get_offs (LL_HANDLE* llHdl, u_int8* address, u_int8* sl_addr,
                      u_int8* num_inputs, u_int8* num_outputs, u_int16* sl_para_len) /* nodoc */
{
	u_int8* start_addr;
	u_int16 size;

	start_addr = address;

	*sl_addr = *address;
	address++;
	*sl_para_len = TWISTWORD(*((u_int16*) address));  /* read slave parameter data length */
	DBGWRT_2((DBH, "PROFIDP_CONFIG get_offs: sl_para_len = %04x, address = %08p\n", *sl_para_len,
	                address));

	address += 16;   /* jump to prm_data */
	size = TWISTWORD(*((u_int16*) address)); /* read prm_data size */
	DBGWRT_2((DBH, "PROFIDP_CONFIG get_offs: prm_data size = %04x\n", size));

	address += size;              /* jump to cfg_data */

	size = TWISTWORD(*((u_int16*) address)); /* read cfg_data size */
	DBGWRT_2((DBH, "PROFIDP_CONFIG get_offs: cfg_data size = %04x\n", size));

	address += size;              /* jump to aat_data, size was already twisted !*/

	size = TWISTWORD(*((u_int16*) address)); /* read aat_data size */

	address += 2;    /* jump over aat_data size */

	*num_inputs = *address;      /* read number of inputs */
	address++;
	DBGWRT_2((DBH, "PROFIDP_CONFIG get_offs: %04x Inputs\n", *num_inputs));

	*num_outputs = *address;     /* read number of outputs */
	address++;
	DBGWRT_2((DBH, "PROFIDP_CONFIG get_offs: %04x Outputs\n", *num_outputs));

	return (start_addr + *sl_para_len + 1 /* jump over slave address*/);

}

/*************************** PROFIDP_copy_buspar ***************************
 *
 *  Description: Get IN/OUTput offset of specific slave
 *
 *---------------------------------------------------------------------------
 *  Input......: length       length of slave param data
 *               sourceP      pointer to slave param data to be copied
 *               destinationP pointer to mem area where data should be copied
 *
 *  Output.....: return       new length of slave param data (alignmet)
 *
 *  Globals....: -
 ****************************************************************************/

static int16 PROFIDP_copy_buspar (u_int16 length, u_int8 *sourceP, u_int8 *destinationP) /* nodoc */
{
	int16 i, j, intLen, alignCount, intLenBuf;
	u_int8 *startDestPoint;

	startDestPoint = destinationP;
	alignCount = 0;

	/* copy first part of T_DP_SLAVE_PARA_SET */
	for (i = 1; i <= 16; i++) {

		*destinationP++ = *sourceP++;

	}
	i--; /* correct value of i, because i is now used as actual length value */
	/* copy and add alignment if necessary */
	/* the following structures are copied here: */
	/* T_DP_PRM_DATA, T_DP_CFG_DATA, T_DP_AAT_DATA, T_DP_USER_DATA */
	while(i < length) {

		intLenBuf = (u_int16) *(u_int16*) sourceP;
		intLen = TWISTWORD(intLenBuf);

		for (j = 1; j <= intLen; j++) {

			*destinationP++ = *sourceP++;
			i++;
		}
		/* add alignment */
		if (intLen & 0x1) {
			*destinationP++ = (u_int8) 0x00;
			alignCount++;
		}
	}

	/* save new data length in T_DP_SLAVE_PARA_SET structure */
	* (u_int16*) startDestPoint = TWISTWORD((u_int16) (length + alignCount));
	return (length + alignCount);

}



/********************************* profi_snd_req_res_usr ********************
 *
 *  Description: Send request to firmware/interact with irq DIAG handler
 *
 *	This routine effectively calls pci.c:profi_snd_req_res().
 *  It cannot be called from the interrupt routine
 *
 *  It records the pending request as long as it hasn't been acknowledged
 *  by the firmware. (uses llHdl->reqPending)
 *
 *  When the irq routine attempts to send a DP_GET_SLAVE_DIAG request
 *  it checks the reqPending flag first.
 *
 *  If it is clear, the DP_GET_SLAVE_DIAG request is send immediately by
 *  the IRQ routine. It can happen, that the main routine attempts
 *  to send another request while we're waiting for the ACK for
 *  DP_GET_SLAVE_DIAG. profi_snd_req_res_usr() therefore checks the
 *  getSlaveDiagReqWaitCon flag before sending a new request.
 *
 *  If it is set, the irq does not send the request. Instead, it sets
 *  the flag getSlaveDiagReqDelayed in llHdl. After the service sent
 *  by profi_snd_req_res_usr has terminated the DP_GET_SLAVE_DIAG request
 *  is then sent by profi_snd_req_res_usr().
 *
 *---------------------------------------------------------------------------
 *  Input......: llHdl			low level handle
 *				 sdp_ptr		service descriptor pointer
 *				 data_ptr		service specific data
 *				 driverIntCon	confirmatin of req is handled driver internal
 *  Output.....: returns:		error code
 *  Globals....: -
 ****************************************************************************/
static int16 profi_snd_req_res_usr(
	LL_HANDLE     		  *llHdl,
	T_PROFI_SERVICE_DESCR FAR *sdb_ptr,
	VOID                  FAR *data_ptr,
	PB_BOOL               driverIntCon
	) /* nodoc */
{
	int16 dpError;
	u_int32 startTick;
	u_int8 rsp_layer = (sdb_ptr->layer)==DP ? DP_USR : FMB_USR;


	DBGWRT_3((DBH,"profi_snd_req_res_usr: Layer = 0x%02x\n", sdb_ptr->layer ));
	DBGWRT_3((DBH,"profi_snd_req_res_usr: Service = 0x%02x\n", sdb_ptr->service ));
	DBGWRT_3((DBH,"profi_snd_req_res_usr: Primitive = 0x%02x\n", sdb_ptr->primitive ));

	/* check if irq routine is waiting for ACK from controller */
	startTick = ACT_TICK;
	while( llHdl->getSlaveDiagReqWaitCon || llHdl->reqPending ){
		DBGWRT_2((DBH,"profi_snd_req_res_usr: wait for getSlaveDiagReqWaitCon=%d or reqPending=%d\n",
		          llHdl->getSlaveDiagReqWaitCon, llHdl->reqPending ));
		if ( (int32) (ACT_TICK - startTick) >= ((int32) (PROFIDP_CMI_TIMEOUT * TICK_RATE)) ) {
			DBGWRT_2((DBH,"profi_snd_req_res_usr: wait for getSlaveDiagReqWaitCon TIMEOUT !\n"));
			return( E_IF_NO_CNTRL_RES );
		}
		OSS_Delay( llHdl->osHdl, 1 );
	}

	llHdl->reqPending = TRUE;	/* flag request is pending */

	if( driverIntCon ) {
		/* set up wait for service structure */
		llHdl->waitForService.layer     = rsp_layer;
		llHdl->waitForService.service   = sdb_ptr->service;
		llHdl->waitForService.primitive = CON;
	}
	/* send request to firmware, wait for ACK */
	dpError = profi_snd_req_res( llHdl, sdb_ptr, data_ptr, TRUE );

	return dpError;
}

/********************************* GetChannelDir *****************************
 *
 *  Description: Determine channel direction depending on
 *			     number of inputs/outputs
 *
 *---------------------------------------------------------------------------
 *  Input......: llHdl			low level handle
 *				 ch				channel number
 *  Output.....: returns:		M_CH_IN/M_CH_OUT/M_CH_INOUT
 *  Globals....: -
 ****************************************************************************/
static int32 GetChannelDir( LL_HANDLE *llHdl, int32 ch)	/* nodoc */
{
	if( llHdl->chInfo[ch].num_out && llHdl->chInfo[ch].num_in )
		return M_CH_INOUT;

	if( llHdl->chInfo[ch].num_out )
		return M_CH_OUT;

	return M_CH_IN;
}

/***************************** PROFIDP_sendDiagReqIrq ************************
 *
 *  Description: Set up DP_GET_SLAVE_DIAG request
 *               ACK and CON handling is completely done form ISR, so this
 *               function does not wait for any ACK or CON and returns
 *               immediately.
 *			     This functin can be called from ISR !
 *
 *---------------------------------------------------------------------------
 *  Input......: llHdl			low level handle
 *
 *  Output.....: returns:		-
 *  Globals....: -
 ****************************************************************************/
static void PROFIDP_sendDiagReqIrq(LL_HANDLE *llHdl) /* nodoc */
{
	int32 error;
	T_PROFI_SERVICE_DESCR diagPsd;


	IDBGWRT_2((DBH," Request next diag...\n"));
	diagPsd.comm_ref	= 0;
	diagPsd.layer 		= DP;
	diagPsd.service 	= DP_GET_SLAVE_DIAG;
	diagPsd.primitive	= REQ;

	/*
	 * set flag getSlaveDiagReqWaitAck
	 * this flag is reset when ACK-INT 0xf0 occurs
	 */
	llHdl->getSlaveDiagReqWaitAck = TRUE;

	/*
	 * set flag getSlaveDiagReqWaitCon
	 * this flag is reset when CON-INT 0x0f occurs
	 */
	llHdl->getSlaveDiagReqWaitCon = TRUE;

	/*
	 * Start sending DP_GET_SLAVE_DIAG req
	 * Do not wait for acknowledge from f/w
	 */
	if( (error = profi_snd_req_res( llHdl, &diagPsd, NULL, FALSE ))){
		IDBGWRT_ERR((DBH,"*** PROFIDP_Irq: error 0x%x sending DP_GET_SLAVE_DIAG req!\n",
					 error ));
	}
}

/***************************** PROFIDP_aliveCheck ****************************
 *
 *  Description: In cyclic data transfer mode this functin checks if the
 *               PROFIDP-FW is still alive. This is done by sending a
 *               get slave diag request to FW and checking if a confirmation
 *               occurs for this service.
 *
 *---------------------------------------------------------------------------
 *  Input......: llHdl			low level handle
 *
 *  Output.....: returns:		alive status 0: FW alive
 *                                          -1: FW does not respond
 *  Globals....: -
 ****************************************************************************/
static int32 PROFIDP_aliveCheck(LL_HANDLE *llHdl) /* nodoc */
{
	int32          error            = 0;

	/* alive check is only done in cyclic mode */
	if( llHdl->cyclicDataTransfer ) {

		switch( llHdl->aliveState ) {

			case PROFIDP_ALIVE_IDLE:
				if( (((u_int32) ACT_TICK) - llHdl->lastAliveCheck) > ((u_int32) PROFIDP_ALIVE_CHECK_CYCLE) ) {
					/* lock semaphore */
					if( (OSS_SemWait( llHdl->osHdl, llHdl->fwAliveCheckSemP, OSS_SEM_NOWAIT ))
					     == 0 ) {
						DBGWRT_3((DBH," PROFIDP_aliveCheck: %d, %d, %d \n",llHdl->reqPending, llHdl->getSlaveDiagReqDelayed, llHdl->getSlaveDiagReqWaitCon ));
						/* lock interrupts */
						if( ERROR == semTake( llHdl->windowPointerSemId, WAIT_FOREVER ) ) {
							DBGWRT_ERR((DBH," *** PROFIDP_aliveCheck: Error taking window pointer semaphore\n"));
						}

						if( !llHdl->reqPending && !llHdl->getSlaveDiagReqDelayed &&
						    !llHdl->getSlaveDiagReqWaitCon ) {
							DBGWRT_3((DBH," PROFIDP_aliveCheck: Send alive REQ \n" ));
							/* send diag request for alive checking */
							PROFIDP_sendDiagReqIrq( llHdl );
							llHdl->lastFwDiagConInd = ACT_TICK;
							llHdl->aliveState = PROFIDP_ALIVE_WAIT_CON;
							/* set flag fwAliveCheckWait */
							llHdl->fwAliveCheckWait = TRUE;

						}
						/* enable interrupts */
						if( ERROR == semGive( llHdl->windowPointerSemId ) ) {
							DBGWRT_ERR((DBH," *** PROFIDP_aliveCheck: Error giving window pointer semaphore\n"));
						}


						/* release semaphore */
						if ((OSS_SemSignal( llHdl->osHdl, llHdl->fwAliveCheckSemP )) != 0) {
							DBGWRT_ERR((DBH," *** PROFIDP_aliveCheck: Error signaling "
											"alive semaphore\n"));
						}

					}
				}
				break;

			case PROFIDP_ALIVE_WAIT_CON:
				if( llHdl->fwAliveCheckWait == FALSE ) {
					llHdl->aliveState = PROFIDP_ALIVE_IDLE;
					llHdl->lastAliveCheck = ACT_TICK;
					DBGWRT_3((DBH," PROFIDP_aliveCheck: Received alive CON \n" ));
				}
				else {
					if( (((u_int32) ACT_TICK) - llHdl->lastFwDiagConInd) > ((u_int32) PROFIDP_ALIVE_TIMEOUT )) {
						error = -1;
					}
				}

				break;

		} /* switch */
		DBGWRT_2((DBH," PROFIDP_aliveCheck: lastFwDiagConInd = %ld, "
		              "lastAliveCheck = %ld, aliveState = %d\n",
		              llHdl->lastFwDiagConInd, llHdl->lastAliveCheck, llHdl->aliveState ));

	} /* if */

	return error;
}





