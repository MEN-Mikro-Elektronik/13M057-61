/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: cmi.c
 *      Project: driver for Profibus Module M57
 *
 *      $Author: AGromann $
 *        $Date: 2014/11/05 10:51:29 $
 *    $Revision: 1.1 $
 *
 *  Description: Common memory interface for Profibus module M57
 *				 originally written by M. Boettcher, Softing
 *
 *     Required:
 *     Switches: DEBUG
 *				 NOTE: Most compiler switches are defined in MEN/typdefs.h
 *					   (operating system, byteorder etc)
 *---------------------------[ Public Functions ]----------------------------
 * cmi_init
 * cmi_read
 * cmi_write
 * cmi_get_data_descr
 * cmi_set_data_descr
 * cmi_throwaway
 * cmi_get_base_a24
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: cmi.c,v $
 * Revision 1.1  2014/11/05 10:51:29  AGromann
 * Initial Revision
 *
 * Revision 1.13  2013/05/17 13:37:54  dpfeuffer
 * R: BSOD under Windows 7 64-bit
 * M: cmi_read(): illegal cast of (u_int32)llHdl->ma from SET_WINDOW call removed
 *
 * Revision 1.12  2010/11/16 11:23:45  CKauntz
 * R:1. DP_DATA_B_READ/WRITE increases baseaddr fails
 *   2. Compiler warning for differnet types
 * M:1. Fixed by adding additional value to offset in macro
 *   2. Added cast
 *
 * Revision 1.11  2010/10/06 23:09:46  cs
 * R: OSS_IrqMaskR is callable from any context
 * M: don't query llHdl->irqFlag for status before locking interrupts
 *
 * Revision 1.10  2004/07/22 11:45:27  AGromann
 * added timeout to function irq_to_cntrl()
 * solved problem with M57 and swapped variant
 *
 * added timeout to function irq_to_cntrl()
 * solved problem with M57 and swapped variant
 * Revision 1.9  2004/05/05 08:25:15  cs
 * Minor changes for MDIS4/2004 conformity
 *   replaced functions OSS_IrqMask/OSS_IrqUnMask with OSS_IrqMaskR/OSS_IrqRestore
 *
 * Revision 1.8  2004/05/03 15:19:43  AGromann
 * fixed buffer length problem in copy_from_dpram() and copy_to_dpram()
 *
 * fixed buffer length problem in copy_from_dpram() and copy_to_dpram()
 * Revision 1.7  2001/10/25 11:17:49  kp
 * cosmetics
 *
 * Revision 1.6  2001/10/24 16:03:16  kp
 * added parameter waitAck to cmi_write
 *
 * Revision 1.5  2001/02/28 08:18:47  kp
 * fixed casting problems to DP_DATA_B_WRITE/READ
 * Bug fix: removed global variable m57_base_a24
 *
 * Revision 1.4  2000/06/28 10:02:09  gromann
 * added cast in DP_DATA_B_READ and DP_DATA_B_WRITE
 *
 * Revision 1.3  2000/06/15 14:54:17  gromann
 * cosmetics
 *
 * Revision 1.2  2000/03/09 09:58:44  Gromann
 * Changes for Win NT
 *
 * Revision 1.1  2000/02/29 12:04:58  Gromann
 * Initial Revision
 *
 * module accesses are now made with MREAD/MWRITE because of MDIS 4
 * Revision 1.5  96/05/28  16:09:32  kp
 * fixed typing mistake for d_id (INTEL hosts only).
 *
 * Revision 1.4  96/05/28  15:50:47  kp
 * changed for firmware V5.01. Merged with DOS source.
 *
 *
 * ==== OLD LOG INFO FOR OS-9 SPECIFIC PORT ===
 * Revision 1.3  95/08/10  10:46:21  kp
 * bug fix in cmi_write: set v_waiting_for_controller before issueing
 * interrupt to module.
 *
 * Revision 1.2  95/06/08  09:23:41  kp
 * 1) declare d_sema in data descr. as volatile to make locking work
 * 2) CMI timeout now 10s
 *
 * Revision 1.1  95/06/02  16:54:36  kp
 * Initial revision
 *
 * ==== OLD LOG INFO FOR DOS SPECIFIC PORT ===
 * Revision 1.1  96/01/09  10:21:33  kp
 * Initial revision
 *---------------------------------------------------------------------------
 * (c) Copyright 1996 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#include "profidp_drv_int.h"

static int m57_irq_to_mod_pending (LL_HANDLE* llHdl );
FUNCTION LOCAL USIGN32  cmi_correct_address
         (
			LL_HANDLE* llHdl,              /* low level handle */
           IN USIGN32  address             /* address to correct */
         );
static void irq_to_cntrl(LL_HANDLE* llHdl, USIGN8 irq_val );

/*
 * Conversion macro for INTEL byteordering hosts when accessing a 32bit
 * value in the DPRAM of the M57. INTEL hosts make 2 word accesses to
 * the DPRAM, so the byteordering within the words is correct, but the
 * word order must be swapped
 */
/* Twist macros included by twist.h are not equal to TWISTLONG_W */

#if (defined(_BIG_ENDIAN_) && (!defined(MAC_BYTESWAP))) || ( defined(_LITTLE_ENDIAN_) && defined(MAC_BYTESWAP) )

# define TWISTLONG_W(l) (l)


#elif (defined(_LITTLE_ENDIAN_) && (!defined(MAC_BYTESWAP))) || ( defined(_BIG_ENDIAN_) && defined(MAC_BYTESWAP) )

# define TWISTLONG_W(l) ((((l)&0xffff)<<16) + (((l)&0xffff0000)>>16))

#else
# error "please specify either _BIG_ENDIAN_ or _LITTLE_ENDIAN_"
#endif


# define DELAY(ms)			 OSS_Delay (llHdl->osHdl, ms )		/* delay(ms) 	   delay some ms */
# define TIMEOUT             PROFIDP_CMI_TIMEOUT

# define PS								  /* var for int masking */



/* --- communication mode -------------------------------------------------- */
#define INIT_MODE             0x00        /* initializing  mode              */
#define CONFIG_MODE           0x05        /* configuration mode              */
#define COMM_MODE             0x08        /* communication mode              */

/* --- common memory ready masks ------------------------------------------- */
#define _H_READY_MASK_          0x1E2D4B87  /* host ready mask                 */
#define _C_READY_MASK_          0xE1D2B478  /* controller ready mask           */

/* --- IRQ-VALUE ----------------------------------------------------------- */
#define REQ_IRQ_VALUE         0xF0        /* host to CP                 */
#define ACK_IRQ_VALUE         0x0F 		  /* host to CP                 */


#define DUMMY_BASE ((u_int32)llHdl->ma)

/*
CMI Structur definition was located here
*/


FUNCTION LOCAL PB_INT16  cmi_wait_for_controller(LL_HANDLE *llHdl) /* nodoc */

/*-----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function reads the the semaphore flag in the host common blocks. The
function returns with E_OK if the semaphore flag is set to _IDLE. If the
semarphore flag is not set after CMI_TIMEOUT seconds the function returns with
E_NO_CNTRL_RES.

possible return values:

- E_OK                             cmi is initialized
- E_IF_NO_CNTRL_RES                controller does not respond

-----------------------------------------------------------------------------*/
{
	/* llHdl->cTick_cmi_wait = ACT_TICK; */

	/* --- activate controller interrupt ----------------------------------- */

	if (MREAD_D8( llHdl->ma, C_INT_ENABLE ) & 0x01) {
		irq_to_cntrl(llHdl, REQ_IRQ_VALUE);
	}
	DBGWRT_1 ((DBH," PROFIDP_cmi: H_ID = %02x \n", MREAD_D8( llHdl->ma, H_ID)));
	/* wait until request was acknowledged by M57 moduel throw interrupt */
	if ((OSS_SemWait( llHdl->osHdl, llHdl->req_con_f0_semP, 5000 )) != 0) {
		DBGWRT_ERR((DBH," *** PROFIDP_cmi: Error or Timeout REQ/CON semaphore\n"));
		return ( E_IF_NO_CNTRL_RES );
	}
    return(E_OK);
}



FUNCTION LOCAL USIGN32  cmi_correct_address
         (
			LL_HANDLE* llHdl,              /* low level handle */
           IN USIGN32  address             /* address to correct */
         ) /* nodoc */
/*-----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function computes an address of the controller.

possible return values:

- corrected address

-----------------------------------------------------------------------------*/
{
    if (address == 0) return(0);

    address = (address - llHdl->c_base_address) + llHdl->h_base_address;

    return(address);
}



FUNCTION PUBLIC PB_INT16 cmi_init
        (
			LL_HANDLE *llHdl /*,   low level handle */
          /*
          IN  USIGN32 m57_base_a08_adr,
          IN  USIGN32 m57_base_a24_adr
          */
        ) /* nodoc */

/*-----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
This function initializes the the host cmi driver

possible return values:

- E_OK                             cmi is initialized
- E_IF_NO_CNTRL_RES                controller does not respond
- E_IF_INVALID_CNTRL_TYPE_VERSION  invalid controller type or software version
-----------------------------------------------------------------------------*/
{
	u_int32  save;
	/*--- init global controller base address ---*/

	/*--- reset+start module ---*/

	M57_RESET(llHdl->ma,1);
	DELAY(50);
	M57_RESET(llHdl->ma,0);
	/* Controller needs 16us to start up */
	DELAY(50);

	DBGWRT_2((DBH, "CMI init: base_a08=%lx\n", (void*)llHdl->ma));

	/* --- set CMI host base address --------------------------------------- */
	SET_WINDOW(llHdl->ma,COMM_OFF);	
    
	llHdl->Wcmi = (T_CMI_DESCRIPTOR *)(llHdl->ma);

	/* --- intialize CMI host and controller ready mask -------------------- */
	PB_MWRITE_D32( llHdl->ma, H_READY_MASK, 0);
	PB_MWRITE_D32( llHdl->ma, C_READY_MASK, 0);

	/* --- intialize CMI host and controller return value ------------------ */
	MWRITE_D8( llHdl->ma, H_RET_VAL, 0);
	MWRITE_D8( llHdl->ma, C_RET_VAL, 0);

	/* --- set the cmi host descriptor block parameters -------------------- */
	PB_MWRITE_D32( llHdl->ma, H_READY_MASK, TWISTLONG_W(_H_READY_MASK_));

	PB_MWRITE_D32( llHdl->ma, H_BASE_ADDRESS, DUMMY_BASE+COMM_OFF);
	llHdl->h_base_address = DUMMY_BASE+COMM_OFF;

	MWRITE_D8( llHdl->ma, H_ID, 0x00);

	MWRITE_D8( llHdl->ma, H_INT_ENABLE, REQ_ACK_BY_IR);

	MWRITE_D8( llHdl->ma, H_ADDRESS_SWAP_MODE, ABS_32BIT_MOTOROLA);		/* not used */
	MWRITE_D8( llHdl->ma, H_STATE, INIT_MODE);
	MWRITE_D8( llHdl->ma, H_SEMA, _IDLE);

	/* --- get the start time for check timeout ---------------------------- */
	llHdl->cTick_cmi_init = ACT_TICK;


	DBGWRT_2((DBH, "CMI init: wait controller ready\n"));
	/* --- wait until controller ready mask is set ------------------------- */

	DBGWRT_2((DBH, "CMI init: C_READY_MASK=%08lx TWISTLONG=%08x\n", PB_MREAD_D32( llHdl->ma, C_READY_MASK),
	               TWISTLONG_W(_C_READY_MASK_)));
	while (PB_MREAD_D32( llHdl->ma, C_READY_MASK) != TWISTLONG_W(_C_READY_MASK_)){

		/* --- set host ready mask again because controller initializes
		   the ready mask ---*/
/* >> */PB_MWRITE_D32( llHdl->ma, H_READY_MASK, TWISTLONG_W(_H_READY_MASK_));
		DELAY(10);

		/* --- check time out -> controller does not respond --------------- */


		if (MREAD_D8( llHdl->ma, C_RET_VAL) != E_OK)  return(E_IF_NO_CNTRL_RES);
		if ( (int32) (ACT_TICK - llHdl->cTick_cmi_init) >= ((int32) (TIMEOUT * TICK_RATE)) )
			return(E_IF_NO_CNTRL_RES);
	}

	/* --- set host ready mask again --------------------------------------- */
	PB_MWRITE_D32( llHdl->ma, H_READY_MASK, TWISTLONG_W(_H_READY_MASK_));


	/* --- determine host param_addr and data_addr ------------------------- */
	DBGWRT_2((DBH, "CMI init: bparam=%lx data=%lx data_descr=%lx\n",
		  TWISTLONG_W(PB_MREAD_D32( llHdl->ma, H_PARAM_ADDR)),
		  TWISTLONG_W(PB_MREAD_D32( llHdl->ma, H_DATA_ADDR)),
		  TWISTLONG_W(PB_MREAD_D32( llHdl->ma, H_DATA_DESCR_ADDR))));

	llHdl->c_base_address = TWISTLONG_W(PB_MREAD_D32( llHdl->ma, C_BASE_ADDRESS));

	save = cmi_correct_address (llHdl, TWISTLONG_W(PB_MREAD_D32( llHdl->ma, H_PARAM_ADDR)));
	PB_MWRITE_D32( llHdl->ma, H_PARAM_ADDR, TWISTLONG_W(save));

	save = cmi_correct_address(llHdl, TWISTLONG_W(PB_MREAD_D32( llHdl->ma, H_DATA_ADDR)));
	PB_MWRITE_D32( llHdl->ma, H_DATA_ADDR, TWISTLONG_W(save));

	save = cmi_correct_address(llHdl, TWISTLONG_W(PB_MREAD_D32( llHdl->ma, H_DATA_DESCR_ADDR)));
	PB_MWRITE_D32( llHdl->ma, H_DATA_DESCR_ADDR, TWISTLONG_W(save));

	DBGWRT_2((DBH, "CMI init: cparm:   param=%lx data=%lx data_descr=%lx\n",
		  TWISTLONG_W(PB_MREAD_D32( llHdl->ma, H_PARAM_ADDR)),
		  TWISTLONG_W(PB_MREAD_D32( llHdl->ma, H_DATA_ADDR)),
		  TWISTLONG_W(PB_MREAD_D32( llHdl->ma, H_DATA_DESCR_ADDR))));

	llHdl->h_serv_descr       = (T_PROFI_SERVICE_DESCR FAR *)
		((U_INT32_OR_64)(TWISTLONG_W(PB_MREAD_D32( llHdl->ma, H_PARAM_ADDR))));
	llHdl->h_data_ptr         = (VOID                  FAR *)
		((U_INT32_OR_64)(TWISTLONG_W(PB_MREAD_D32( llHdl->ma, H_DATA_ADDR))));
	llHdl->h_data_descr_list  = (T_DATA_DESCR          FAR *)
		((U_INT32_OR_64)(TWISTLONG_W(PB_MREAD_D32( llHdl->ma, H_DATA_DESCR_ADDR))));

	llHdl->param_block_size   = MREAD_D16( llHdl->ma, H_PARAM_SIZE);
	llHdl->data_block_size    = MREAD_D16( llHdl->ma, H_DATA_SIZE);
	DBGWRT_2((DBH, "CMI init: param_bs=%x data_bs=%x\n", llHdl->param_block_size,
		   llHdl->data_block_size));

	/* --- determine controller param_addr and data_addr ------------------- */
	llHdl->c_serv_descr   = (T_PROFI_SERVICE_DESCR FAR *)
		(cmi_correct_address (llHdl, TWISTLONG_W(PB_MREAD_D32( llHdl->ma, C_PARAM_ADDR))));

	llHdl->c_data_ptr     = (VOID FAR *)
		(cmi_correct_address (llHdl, TWISTLONG_W(PB_MREAD_D32( llHdl->ma, C_DATA_ADDR))));

	DBGWRT_2((DBH, "CMI init: C_SDB=%lx C_DATA_BLOCK=%lx\n",
		  TWISTLONG_W(PB_MREAD_D32( llHdl->ma, C_PARAM_ADDR)), TWISTLONG_W(PB_MREAD_D32( llHdl->ma, C_DATA_ADDR))));

	MWRITE_D8( llHdl->ma, H_RET_VAL, E_OK);

	MWRITE_D8( llHdl->ma, H_STATE, CONFIG_MODE);


	llHdl->cTick_cmi_init = ACT_TICK;
	DBGWRT_2((DBH, "CMI init: wait for controller config mode\n"));
	while (MREAD_D8( llHdl->ma, C_STATE) != CONFIG_MODE){
		/* --- wait until controller state is 'CONFIG_MODE' ---------------- */
		DELAY(10);
		if (MREAD_D8( llHdl->ma, C_RET_VAL) != E_OK) return(MREAD_D8( llHdl->ma, C_RET_VAL));
		if ( (int32) (ACT_TICK - llHdl->cTick_cmi_init) >= ((int32) (TIMEOUT * TICK_RATE)) )
			return(E_IF_NO_CNTRL_RES);

	}

	if ( 0x0 != MREAD_D8( llHdl->ma, C_ID ) ) {
		DBGWRT_2((DBH, "CMI init: C_ID was not = 0, set C_ID = 0 ...\n"));
		MWRITE_D8( llHdl->ma, C_ID, 0);
	}

	/* int enable is made via setstat in profidp_drv.c */
	/* M57_IRQ_ENABLE(llHdl->ma);  enable interrupts */

	DBGWRT_2((DBH, "CMI init: exit ok\n"));
	return(E_OK);
}



FUNCTION PUBLIC PB_INT16 cmi_read
         (
								LL_HANDLE	*llHdl,  /* low level handle */
          IN    T_PROFI_SERVICE_DESCR FAR   *sdb_ptr,
          IN    VOID                  FAR   *data_ptr,
          INOUT USIGN16               FAR   *data_len
         ) /* nodoc */

/*-----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function reads the SERVICE_DESCRIPTION_BLOCK and the SERVICE_DATA_BLOCK
from the common memory if the controller semaphore is set.

possible return values:
- CON_IND_RECEIVED        -> a confirmation or indication is received
- NO_CON_IND_RECEIVED     -> no confirmation or indication is received

- E_IF_INVALID_DATA_SIZE  -> not enough memory to transfer the service specific
                             datas from CMI data block to user data block
- E_IF_xxxxxxxxxxxxx      -> controller interface errors
-----------------------------------------------------------------------------*/
{
	LOCAL_VARIABLES

	PB_INT16 ret_val = E_OK;            /* return value */

	FUNCTION_BODY

	SET_WINDOW(llHdl->ma,COMM_OFF); /* set window to CMI descriptor */


	if (MREAD_D8( llHdl->ma, C_SEMA) == _IDLE){
		DBGWRT_2((DBH, "cmi_read: no conind ret_val = %04x\n", ret_val ));
		return(NO_CON_IND_RECEIVED);
	}

	if (MREAD_D8( llHdl->ma, C_RET_VAL) != E_OK){
		ret_val = (PB_INT16) MREAD_D8( llHdl->ma, C_RET_VAL);

		DBGWRT_2((DBH, "cmi_read: rv=%x\n", ret_val ));
		MWRITE_D8( llHdl->ma, C_RET_VAL, E_OK);
		MWRITE_D8( llHdl->ma, C_SEMA, _IDLE);

		/* --- activate controller interrupt ------------------------------- */

		if (MREAD_D8( llHdl->ma, C_INT_ENABLE) & 0x02) irq_to_cntrl(llHdl, ACK_IRQ_VALUE);

		return(PB_ERR(ret_val));
	}

	/* --- copy controller service description block ----------------------- */
	copy_from_dpram( llHdl, (USIGN8 FAR *) sdb_ptr,
				  (USIGN8 FAR *) llHdl->c_serv_descr,
				  sizeof(T_PROFI_SERVICE_DESCR)
				  );

	SET_WINDOW(llHdl->ma,COMM_OFF); /* set window to CMI descriptor */

	/* --- copy controller service data block ----------------------------- */

	if (MREAD_D16( llHdl->ma, C_DATA_SIZE) <= *data_len){
		copy_from_dpram( llHdl, (USIGN8 FAR  *) data_ptr,
						(USIGN8 FAR  *) llHdl->c_data_ptr,
						MREAD_D16( llHdl->ma, C_DATA_SIZE)
						);
		SET_WINDOW(llHdl->ma,COMM_OFF); /* set window to CMI descriptor*/
		*data_len = MREAD_D16( llHdl->ma, C_DATA_SIZE);

		/* --- set controller semaphore return to _IDLE -------------------- */

		MWRITE_D8( llHdl->ma, C_SEMA, _IDLE);


		/* --- activate controller interrupt ------------------------------- */
		if (MREAD_D8( llHdl->ma, C_INT_ENABLE) & 0x02) irq_to_cntrl(llHdl, ACK_IRQ_VALUE);

		DBGWRT_2((DBH, "cmi_read: conind %x\n", CON_IND_RECEIVED ));
		return(CON_IND_RECEIVED);
	}
	else {


		*data_len = MREAD_D16( llHdl->ma, C_DATA_SIZE);

		/* CHANGED BY KP: do not set Sema to IDLE so that the host can
		   try it with a larger buffer! */
 		/*!!!Wcmi->c_sema = _IDLE;*/

		/* --- activate controller interrupt ------------------------------- */
		/*if (Wcmi->c_int_enable & 0x02) irq_to_cntrl(ACK_IRQ_VALUE);*/

		return(E_IF_INVALID_DATA_SIZE);
	}
}



FUNCTION PUBLIC PB_INT16 cmi_write
         (
							  LL_HANDLE	  *llHdl,   /* low level handle */
          IN T_PROFI_SERVICE_DESCR  FAR   *sdb_ptr,
          IN VOID                   FAR   *data_ptr,
		  IN USIGN16                      data_len,
		  IN PB_BOOL					  waitForAck /* 1=wait for ACK from controller
													    0=return immediately*/
         ) /* nodoc */

/*-----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
This function writes the SERVICE_DESRIPTION_BLOCK and the SERVICE_DATA_BLOCK
to the common memory.

possible return values:
- E_OK                    -> no error occured
- E_IF_INVALID_CMI_CALL   -> invalid CMI call
- E_IF_INVALID_DATA_SIZE  -> not enough cmi data block memory
- E_IF_NO_CNTRL_RES       -> controller does not respond
- E_IF_xxxxxxxxxxxxx      -> controller interface errors
-----------------------------------------------------------------------------*/
{
	LOCAL_VARIABLES

	PB_INT16 ret_val;

	FUNCTION_BODY

	DBGWRT_2((DBH, "cmi_write "));
	/* --- copy service description parameter block from host to CMI ------- */
	if (sdb_ptr != NULL){
		copy_to_dpram( llHdl,
					  (USIGN8 FAR *) llHdl->h_serv_descr,
					  (USIGN8 FAR *) sdb_ptr,
					  sizeof(T_PROFI_SERVICE_DESCR)
					  );

		SET_WINDOW(llHdl->ma,COMM_OFF); /* set window to CMI descriptor*/
		MWRITE_D16( llHdl->ma, H_PARAM_SIZE, (USIGN16) sizeof(T_PROFI_SERVICE_DESCR));
	}
	else {
		return(E_IF_INVALID_CMI_CALL);
	}


	/* --- copy data block from host to CMI -------------------------------- */
	if (data_len > llHdl->data_block_size) return(E_IF_INVALID_DATA_SIZE);


	if (data_ptr != NULL){
		/*
		printf("\nEntering copy to dpram");
		*/
		copy_to_dpram(llHdl,
					  (USIGN8 FAR*) llHdl->h_data_ptr,
					  (USIGN8 FAR*) data_ptr,
					  data_len
					  );

		SET_WINDOW(llHdl->ma,COMM_OFF); /* set window to CMI descriptor*/
		MWRITE_D16( llHdl->ma, H_DATA_SIZE, data_len);
	}
	else {
		SET_WINDOW( llHdl->ma,COMM_OFF); /* set window to CMI descriptor*/
		MWRITE_D16( llHdl->ma, H_DATA_SIZE, 0);
	}

	/* --- set host semaphore to _BUSY ------------------------------------- */
	MWRITE_D8( llHdl->ma, H_SEMA, _BUSY);

	/* --- activate controller interrupt and
	  wait until the REQ int occurs (semaphore in host cmi common block is set to _IDLE) ---
	 */

	if( waitForAck ){
		if ((ret_val = cmi_wait_for_controller(llHdl)) != E_OK){
			MWRITE_D8( llHdl->ma, H_SEMA, _IDLE);
			return(ret_val);
		}
	}
	else {
		/* activate controller interrupt */
		if (MREAD_D8( llHdl->ma, C_INT_ENABLE ) & 0x01) {
			irq_to_cntrl(llHdl, REQ_IRQ_VALUE);
		}
		return E_OK;
	}

	/* --- set semaphore in host cmi common block again to _IDLE ----------- */
	MWRITE_D8( llHdl->ma, H_SEMA, _IDLE);

	/* --- return the result of controller --------------------------------- */
	DBGWRT_1((DBH," ret_val = %04x \n", ret_val));
	ret_val = (PB_INT16) MREAD_D8( llHdl->ma, H_RET_VAL);
	DBGWRT_1((DBH," H_RET_VAL = %04x \n", ret_val));

	ret_val = (PB_INT16) MREAD_D8( llHdl->ma, H_RET_VAL);


	if( ret_val != E_OK )
		ret_val = PB_ERR(ret_val); /* PB_ERR does nothing  make a 057:000 error number */

	MWRITE_D8( llHdl->ma, H_RET_VAL, E_OK);
	return(ret_val);
}


FUNCTION PUBLIC PB_INT16 cmi_get_data_descr
        (
			LL_HANDLE*      llHdl,              /* low level handle */
          IN    USIGN8      data_descr_id, 		/* id of data description   */
          IN    USIGN16     offset,             /* offset in data description*/
          OUT   USIGN16 FAR *data_descr_size,   /* pointer to size of data
												   description */
          INOUT VOID    FAR *data_descr_ptr     /* pointer to data
												   description         */
        ) /* nodoc */

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
This function is used to read  datas from CMI data description area

possible return values:
- E_OK                          -> OK
- E_IF_SERVICE_CONSTR_CONFLICT  -> service not executable at time
- E_IF_SERVICE_NOT_SUPPORTED    -> service not supported

----------------------------------------------------------------------------*/
{
	LOCAL_VARIABLES

	PB_INT16       i=0;                       /* loop variable */
	USIGN8         id;
	USIGN8  FAR *h_data_descr_ptr;
	USIGN8 Wh_data_descr_list =
		(USIGN8) (((USIGN32)llHdl->h_data_descr_list)&0x7f);

	FUNCTION_BODY

	/*------------------------------------------------------------------+
    |  Warning: works only if the entire array is smaller than the 128  |
    |  byte window and does not cross a 128 byte boundary               |
    +------------------------------------------------------------------*/
	SET_WINDOW(llHdl->ma, ((USIGN32)llHdl->h_data_descr_list - DUMMY_BASE));

	DBGWRT_2((DBH, "cmi_get_data_descr: id=%d off=%x size=%x ",
		   data_descr_id, offset, *data_descr_size));

	while( (id = MREAD_D8( llHdl->ma, Wh_data_descr_list + D_ID + (i * T_DATA_DESC_LEN))) != 0){
		if (id == data_descr_id){

			MWRITE_D8( llHdl->ma, Wh_data_descr_list + D_SEMA_H + (i * T_DATA_DESC_LEN), 0xaa);

			/*--- check controller semaphore ---*/
			if (MREAD_D8(llHdl->ma, Wh_data_descr_list + D_SEMA_C + (i * T_DATA_DESC_LEN))){
				MWRITE_D8( llHdl->ma, Wh_data_descr_list + D_SEMA_H + (i * T_DATA_DESC_LEN), 0x00);
				return(E_IF_SERVICE_CONSTR_CONFLICT);
			}

			/* assign data pointer */
			h_data_descr_ptr = (USIGN8 FAR*)
				cmi_correct_address(llHdl, TWISTLONG_W(
										 PB_MREAD_D32(llHdl->ma, Wh_data_descr_list + D_DATA_ADDR +
										                       (i * T_DATA_DESC_LEN))));

			/* check datasize, if size to long limit it to max length */
			if (*data_descr_size > (MREAD_D16(llHdl->ma, Wh_data_descr_list + D_DATA_SIZE +
			                                 (i * T_DATA_DESC_LEN))) - offset){
				*data_descr_size = MREAD_D16(llHdl->ma, Wh_data_descr_list + D_DATA_SIZE +
			                                 (i * T_DATA_DESC_LEN));
			}

			DBGWRT_2((DBH, "idx=%d ptr=%lx\n", i, h_data_descr_ptr));
			copy_from_dpram(llHdl, (USIGN8 FAR*) data_descr_ptr,
							(USIGN8 FAR*) &h_data_descr_ptr[offset],
							*data_descr_size
							);

			SET_WINDOW(llHdl->ma,
/*!!added llHdl-> */   ((USIGN32)llHdl->h_data_descr_list - DUMMY_BASE));
			/* lock semaphore while reading from all channels */
			if (!llHdl->allCh)
				MWRITE_D8( llHdl->ma, Wh_data_descr_list + D_SEMA_H + (i * T_DATA_DESC_LEN), 0x00);

			return(E_OK);
		}
		i++;
	}
	return(E_IF_SERVICE_NOT_SUPPORTED);
}


FUNCTION PUBLIC PB_INT16 cmi_set_data_descr
        (
			LL_HANDLE* llHdl,              /* low level handle */
          IN  USIGN8   data_descr_id,      /* id of data description      */
          IN  USIGN16  offset,             /* offset in data description  */
          IN  USIGN16  data_descr_size,    /* size of data description    */
          IN  VOID FAR *data_descr_ptr     /* pointer to data description */
        ) /* nodoc */

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function is used to write datas to CMI data description area

possible return values:
- E_OK                          -> OK
- E_IF_SERVICE_CONSTR_CONFLICT  -> service not executable at time
- E_IF_SERVICE_NOT_SUPPORTED    -> service not supported
- E_IF_INVALID_DATA_SIZE        -> invalid user data size

----------------------------------------------------------------------------*/
{
	LOCAL_VARIABLES

	PB_INT16   i=0;     /* loop variable */
	USIGN8    id;
	USIGN8  FAR *h_data_descr_ptr;
	/*T_DATA_DESCR FAR*/USIGN8 Wh_data_descr_list =
		(USIGN8)(/*((u_int32) llHdl->ma)+*/(((USIGN32)llHdl->h_data_descr_list)&0x7f));

	FUNCTION_BODY



	/*------------------------------------------------------------------+
    |  Warning: works only if the entire array is smaller than the 128  |
    |  byte window and does not cross a 128 byte boundary               |
    +------------------------------------------------------------------*/
	SET_WINDOW(llHdl->ma, ((USIGN32)llHdl->h_data_descr_list - DUMMY_BASE));

	DBGWRT_2((DBH, "cmi_set_data_descr: id=%d off=%x size=%x ",
		   data_descr_id, offset, data_descr_size));

	DBGWRT_2((DBH, "cmi_set_data_descr: MREAD offset=%x ", (Wh_data_descr_list + D_ID + (i * T_DATA_DESC_LEN))));

	while( (id = MREAD_D8( llHdl->ma, (Wh_data_descr_list + D_ID + (i * T_DATA_DESC_LEN)))) != 0){

		DBGWRT_2((DBH, "\ncmi_set_data_descr: id=%02x\n", id));

		if ( id == data_descr_id){

			if ((data_descr_size + offset) >  MREAD_D16(llHdl->ma, Wh_data_descr_list + D_DATA_SIZE +
			                                 (i * T_DATA_DESC_LEN)))
				return(E_IF_INVALID_DATA_SIZE);

			MWRITE_D8( llHdl->ma, Wh_data_descr_list + D_SEMA_H + (i * T_DATA_DESC_LEN), 0xaa);

			/*--- check controller semaphore ---*/
			if (MREAD_D8(llHdl->ma, Wh_data_descr_list + D_SEMA_C + (i * T_DATA_DESC_LEN))){
				DBGWRT_2((DBH, "cmi_set_data_descr: D_SEMA_C = %02x\n", MREAD_D8(llHdl->ma, Wh_data_descr_list + D_SEMA_C + (i * T_DATA_DESC_LEN))));
				MWRITE_D8( llHdl->ma, Wh_data_descr_list + D_SEMA_H + (i * T_DATA_DESC_LEN), 0x00);
				DBGWRT_ERR((DBH, "\n *** cmi_set_data_descr: can't get controller semaphore\n"));
				return(E_IF_SERVICE_CONSTR_CONFLICT);
			}

			h_data_descr_ptr = (USIGN8 FAR*)
				cmi_correct_address(llHdl, TWISTLONG_W(
										PB_MREAD_D32(llHdl->ma, (Wh_data_descr_list + D_DATA_ADDR +
										                       (i * T_DATA_DESC_LEN)))));
			/*---------------------------------*/
			DBGWRT_2((DBH, ">*>*>cmi_set_data_descr: %08x", TWISTLONG_W(
										PB_MREAD_D32(llHdl->ma, (Wh_data_descr_list + D_DATA_ADDR +
										                       (i * T_DATA_DESC_LEN))))));

#if 0
			DBGWRT_2((DBH, ">*>*>cmi_set_data_descr: %08x", TWISTLONG_W(
										MREAD_D32(llHdl->ma, (Wh_data_descr_list + D_DATA_ADDR +
										                       (i * T_DATA_DESC_LEN))))));
#endif /* if 0 */
			/*---------------------------------*/
			DBGWRT_2((DBH, "cmi_set_data_descr: idx=%d ptr=%lx\n", i, h_data_descr_ptr));

			copy_to_dpram(llHdl,
						  (USIGN8 FAR*) &h_data_descr_ptr[offset],
						  (USIGN8 FAR*) data_descr_ptr,
						  data_descr_size
						  );

			SET_WINDOW(llHdl->ma,
						  ((USIGN32)llHdl->h_data_descr_list - DUMMY_BASE));
			/* lock semaphore while wirting to all channels */
			if (!llHdl->allCh)
				MWRITE_D8( llHdl->ma, Wh_data_descr_list + D_SEMA_H + (i * T_DATA_DESC_LEN), 0x00);
			return(E_OK);
		}
		i++;
	}
	return(E_IF_SERVICE_NOT_SUPPORTED);
}



#if INTEL   /* Big / Little */
# define FROM_DPRAM (tmp=*dpdata,TWISTWORD(tmp))
#else
# define FROM_DPRAM *dpdata
#endif

/* static */ void copy_from_dpram
            (
			 LL_HANDLE  *llHdl, /* low level handle */
			 USIGN8 FAR *dst,	/* pointer to host memory */
			 USIGN8 FAR *src,	/* pointer to dpram */
			 USIGN16 len		/* number of bytes to copy (even or odd) */
			) /* nodoc */
{
	USIGN16 *dstw;
/*	USIGN16 *dpdata = (USIGN16 *)(((u_int32) llHdl->ma)+0x80); */
	USIGN16 t;
	SEM_INFO info;
	USIGN8   semWasNotAlreadyClaimed = 0;
#if 0 /* INTEL */
	USIGN16 tmp;
#endif
	PS;							/* var to hold processor status mask */

	/* check window pointer semaphore, because cmi_irq would destroy window ptr */
	if( OK == semInfoGet( llHdl->windowPointerSemId, &info ) ) {
		if( info.state.owner != taskIdSelf() ) {
			if( ERROR == semTake( llHdl->windowPointerSemId, WAIT_FOREVER ) ) {
				DBGWRT_ERR((DBH," *** copy_from_dpram: Error taking window pointer semaphore\n"));
			}
			semWasNotAlreadyClaimed = ~0;
			DBGWRT_1((DBH, "copy_from_dpram: SemOwner (!=) = 0x%lx 0x%lx\n",(USIGN32) info.state.owner, taskIdSelf() ));
		}
		else {
			DBGWRT_1((DBH, "copy_from_dpram: SemOwner (==) = 0x%lx 0x%lx\n",(USIGN32) info.state.owner, taskIdSelf() ));
		}
	}
	else {
		DBGWRT_ERR((DBH," *** copy_from_dpram: Error getting window pointer semaphore info\n"));
	}

	/*--- set window pointer to start address in dpram ---*/
	DP_SET_WINDOW(llHdl->ma, ((USIGN32)src - DUMMY_BASE));

	DBGWRT_2((DBH, "copy_from_dpram winptr=%lx len=%x\n",
		   (USIGN32)src-DUMMY_BASE, len ));

	if( ((USIGN32)src) & 0x1 ){
		/*--- dpram address odd ---*/
#if (defined(_LITTLE_ENDIAN_) && (!defined(MAC_BYTESWAP))) || \
( defined(_BIG_ENDIAN_) && defined(MAC_BYTESWAP) )
		/* *dst++ = *(USIGN8 *)dpdata; */
		*dst++ = DP_DATA_B_READ(llHdl->ma, 0);

#elif (defined(_BIG_ENDIAN_) && (!defined(MAC_BYTESWAP))) || \
( defined(_LITTLE_ENDIAN_) && defined(MAC_BYTESWAP) )

		*dst++ = DP_DATA_B_READ( llHdl->ma, 1 );
		/* *dst++ = *(((USIGN8 *)dpdata)+1); */

#else
# error "please specify either _BIG_ENDIAN_ or _LITTLE_ENDIAN_"
#endif
		len--;
	}
	dstw = (USIGN16 *)dst;

	while( len > 16 ){
		/* autoincrement window ptr */
		t = DP_DATA_W_READ( llHdl->ma );
		*dstw = TWISTWORD( t );
		dstw++;
		t = DP_DATA_W_READ( llHdl->ma );
		*dstw = TWISTWORD( t );
		dstw++;
		t = DP_DATA_W_READ( llHdl->ma );
		*dstw = TWISTWORD( t );
		dstw++;
		t = DP_DATA_W_READ( llHdl->ma );
		*dstw = TWISTWORD( t );
		dstw++;
		t = DP_DATA_W_READ( llHdl->ma );
		*dstw = TWISTWORD( t );
		dstw++;
		t = DP_DATA_W_READ( llHdl->ma );
		*dstw = TWISTWORD( t );
		dstw++;
		t = DP_DATA_W_READ( llHdl->ma );
		*dstw = TWISTWORD( t );
		dstw++;
		t = DP_DATA_W_READ( llHdl->ma );
		*dstw = TWISTWORD( t );
		dstw++;
		len -= 16;
	}
	while( len > 1 ){
		t = DP_DATA_W_READ( llHdl->ma );
		*dstw = TWISTWORD( t ); /* autoincrement window ptr */
		dstw++;
		len -= 2;
	}
	if( len ){					/* one byte remaining? */
		dst = (USIGN8 *)dstw;

#if (defined(_BIG_ENDIAN_) && (!defined(MAC_BYTESWAP))) || \
( defined(_LITTLE_ENDIAN_) && defined(MAC_BYTESWAP) )
		/* *dst++ = *(USIGN8 *)dpdata; */
		*dst++ = DP_DATA_B_READ( llHdl->ma, 0 );
#elif (defined(_LITTLE_ENDIAN_) && (!defined(MAC_BYTESWAP))) || \
( defined(_BIG_ENDIAN_) && defined(MAC_BYTESWAP) )

		/* *dst++ = *(((USIGN8 *)dpdata)+1); */
		*dst++ = DP_DATA_B_READ( llHdl->ma, 1 );
#else
# error "please specify either _BIG_ENDIAN_ or _LITTLE_ENDIAN_"
#endif
	}

	if( semWasNotAlreadyClaimed ) {
		if( ERROR == semGive( llHdl->windowPointerSemId ) ) {
			DBGWRT_ERR((DBH," *** copy_from_dpram: Error giving window pointer semaphore\n"));
		}
	}

}

#if INTEL /* Big / Little */
# define TO_DPRAM	TWISTWORD(*srcw);srcw++
#else
# define TO_DPRAM	*srcw++
#endif

/* static */ void copy_to_dpram
            (
			LL_HANDLE   *llHdl,   /* low level handle */
			 USIGN8 FAR *dst,	/* pointer to dpram */
			 USIGN8 FAR *src,	/* pointer to host memory */
			 USIGN16 len		/* number of bytes to copy (even or odd) */
			) /* nodoc */
{
	USIGN16 *srcw;
	SEM_INFO info;
	USIGN8   semWasNotAlreadyClaimed = 0;

/*
	USIGN16 *dpdata = (USIGN16 *)(((u_int32) llHdl->ma)+0x80);
	u_int32 destination;
*/
	PS;

	/* check window pointer semaphore, because cmi_irq would destroy window ptr */
	if( OK == semInfoGet( llHdl->windowPointerSemId, &info ) ) {
		if( info.state.owner != taskIdSelf() ) {
			if( ERROR == semTake( llHdl->windowPointerSemId, WAIT_FOREVER ) ) {
				DBGWRT_ERR((DBH," *** copy_to_dpram: Error taking window pointer semaphore\n"));
			}
			semWasNotAlreadyClaimed = ~0;
			DBGWRT_1((DBH, "copy_from_dpram: SemOwner (!=) = 0x%lx 0x%lx\n",(USIGN32) info.state.owner, taskIdSelf() ));
		}
		else {
			DBGWRT_1((DBH, "copy_from_dpram: SemOwner (==) = 0x%lx 0x%lx\n",(USIGN32) info.state.owner, taskIdSelf() ));
		}

	}
	else {
		DBGWRT_ERR((DBH," *** copy_to_dpram: Error getting window pointer semaphore info\n"));
	}


	/*--- set window pointer to start address in dpram ---*/
	DP_SET_WINDOW(llHdl->ma, ((USIGN32)dst - DUMMY_BASE));
	DBGWRT_2((DBH, "copy_to_dpram winptr=%lx len=%x\n",
		   (USIGN32)dst-DUMMY_BASE, len ));

	if( ((USIGN32)dst) & 0x1 ){

#if (defined(_LITTLE_ENDIAN_) && (!defined(MAC_BYTESWAP))) || \
( defined(_BIG_ENDIAN_) && defined(MAC_BYTESWAP) )

/*		*(USIGN8 *)dpdata = *src++; */
		DP_DATA_B_WRITE( llHdl->ma, *src++, 0 );

#elif (defined(_BIG_ENDIAN_) && (!defined(MAC_BYTESWAP))) || \
( defined(_LITTLE_ENDIAN_) && defined(MAC_BYTESWAP) )

/*		*(((USIGN8 *)dpdata)+1) = *src++; */
		DP_DATA_B_WRITE( llHdl->ma, *src++, 1);

#else
# error "please specify either _BIG_ENDIAN_ or _LITTLE_ENDIAN_"
#endif
		len--;
	}
	srcw = (USIGN16 *)src;

	while( len > 16 ){
		/* autoincrement window ptr */
		DP_DATA_W_WRITE( llHdl->ma, TWISTWORD(*srcw) );
		srcw++;
		DP_DATA_W_WRITE( llHdl->ma, TWISTWORD(*srcw) );
		srcw++;
		DP_DATA_W_WRITE( llHdl->ma, TWISTWORD(*srcw) );
		srcw++;
		DP_DATA_W_WRITE( llHdl->ma, TWISTWORD(*srcw) );
		srcw++;
		DP_DATA_W_WRITE( llHdl->ma, TWISTWORD(*srcw) );
		srcw++;
		DP_DATA_W_WRITE( llHdl->ma, TWISTWORD(*srcw) );
		srcw++;
		DP_DATA_W_WRITE( llHdl->ma, TWISTWORD(*srcw) );
		srcw++;
		DP_DATA_W_WRITE( llHdl->ma, TWISTWORD(*srcw) );
		srcw++;

		len -= 16;
	}
	while( len > 1 ){
		/* autoincrement window ptr */
		DP_DATA_W_WRITE( llHdl->ma, TWISTWORD(*srcw) );
		srcw++;
		len -= 2;
	}
	if( len ){					/* one byte remaining? */
		src = (USIGN8 *)srcw;

#if (defined(_BIG_ENDIAN_) && (!defined(MAC_BYTESWAP))) || \
( defined(_LITTLE_ENDIAN_) && defined(MAC_BYTESWAP) )

/*		*(USIGN8 *)dpdata = *src++; */
		DP_DATA_B_WRITE( llHdl->ma, *src++, 0 );

#elif (defined(_LITTLE_ENDIAN_) && (!defined(MAC_BYTESWAP))) || \
( defined(_BIG_ENDIAN_) && defined(MAC_BYTESWAP) )

/*		*(((USIGN8 *)dpdata)+1) = *src++; */
		DBGWRT_2((DBH, "\n>*>*>copy_to_dpram: only one byte\n" ));
		DP_DATA_B_WRITE( llHdl->ma, *src++, 1 );

#else
# error "please specify either _BIG_ENDIAN_ or _LITTLE_ENDIAN_"
#endif

	}

	if( semWasNotAlreadyClaimed ) {
		if( ERROR == semGive( llHdl->windowPointerSemId ) ) {
			DBGWRT_ERR((DBH," *** copy_to_dpram: Error giving window pointer semaphore\n"));
		}
	}


}

static void irq_to_cntrl( LL_HANDLE* llHdl, USIGN8 irq_val ) /* nodoc */
{
	llHdl->cTick_irq_to = ACT_TICK;

	while( m57_irq_to_mod_pending ( llHdl )/* M57_IRQ_TO_MOD_PENDING((u_int32) llHdl->ma ) */ ) {
		DBGWRT_2((DBH, " *** irq_to_cntrl: irq was pending to module\n"));

		if ( llHdl->isrTaskId == taskIdSelf() /* llHdl->irqFlag */)
			break;
		if ( (int32) (ACT_TICK - llHdl->cTick_irq_to) >= ((int32) (TIMEOUT * TICK_RATE)) )	{
			break;
			/* return */

		}
 	}

	llHdl->cTick_irq_to = ACT_TICK;

	while ( 0x0 != MREAD_D8( llHdl->ma, C_ID ) ) {
	    DBGWRT_2((DBH, "irq_to_cntrl: C_ID was not = 0\n"));
		if ( (int32) (ACT_TICK - llHdl->cTick_irq_to) >= ((int32) (TIMEOUT * TICK_RATE)) )	{
		    DBGWRT_2((DBH, "irq_to_cntrl: TIMEOUT !!!\n"));
			break;

		}

	}

    DBGWRT_2((DBH, "irq_to_cntrl\n"));
	MWRITE_D8( llHdl->ma, C_ID, irq_val);

	M57_IRQ_TO_MOD(llHdl->ma);  /* issue interrupt to module */
}


static int m57_irq_to_mod_pending (LL_HANDLE* llHdl ) /* nodoc */
{
	u_int8 conVal;

	conVal = MREAD_D8(llHdl->ma, CNTR_REG);
	conVal &= 0x10;
	return (!conVal);
}






