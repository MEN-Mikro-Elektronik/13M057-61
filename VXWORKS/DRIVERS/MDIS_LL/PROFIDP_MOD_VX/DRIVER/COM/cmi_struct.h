/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: cmi_struct.h
 *
 *       Author: ag
 *        $Date: 2014/11/05 10:51:40 $
 *    $Revision: 1.1 $
 *
 *  Description: Intern header file for PROFIDP driver
 *               - definition of CMI data structures and offsets
 *               
 *
 *     Switches: 
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: cmi_struct.h,v $
 * Revision 1.1  2014/11/05 10:51:40  AGromann
 * Initial Revision
 *
 * Revision 1.3  2001/02/28 08:18:56  kp
 * added newline at EOF
 *
 * Revision 1.2  2000/06/15 15:05:31  gromann
 * cosmetics
 *
 * Revision 1.1  2000/02/29 12:05:06  Gromann
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _CMI_STRUCT_H
#define _CMI_STRUCT_H

#ifndef MAC_BYTESWAP

#if !INTEL
/* --- DATA description block ---------------------------------------------- */
typedef struct T_DATA_DESCR
{
  USIGN8  d_id;               /* identifier to describe the data description */
  USIGN8  dummy;			  /* kp@men */
  VOLATILE USIGN8  d_sema_h;  /* semaphore                                   */
  VOLATILE USIGN8  d_sema_c;  /* semaphore                                   */
  USIGN16 d_data_size;        /* data block size                             */
  USIGN32 d_data_addr;        /* data block address                          */
} T_DATA_DESCR;

/* offset of values in T_DATA_DESCR for non INTEL */
#define D_ID             0
#define DUMMY            1
#define D_SEMA_H         2
#define D_SEMA_C         3
#define D_DATA_SIZE      4
#define D_DATA_ADDR      6
#define T_DATA_DESC_LEN  10


/* --- CMI descriptor block (motorola hosts) ------------------------------- */
typedef struct T_CMI_DESCRIPTOR
{
  USIGN32 h_ready_mask;       /* ready mask                                  */
  USIGN32 h_base_address;     /* base address of DPR                         */

  /* originally for type and version of host software           */
  /* I use h_id as C_TO_H_IRQ_VAL */
  USIGN8  h_id;               
  USIGN8  h_int_enable;       /* call by interrupt                           */
  USIGN8  h_address_swap_mode;/* address mode                                */
  USIGN8  h_state;            /* page addressing                             */
  USIGN32 h_param_addr;       /* parameter block address (SDB)               */
  USIGN32 h_data_addr;        /* data block address                          */
  USIGN16 h_param_size;       /* parameter block size                        */
  USIGN16 h_data_size;        /* data block size                             */
  VOLATILE USIGN8  h_sema;    /* semaphore                                   */
  USIGN8  h_ret_val;          /* return value                                */
  USIGN8  h_head;             /* reserved for a circular request block buffer*/
  USIGN8  h_tail;             /* reserved for a circular request block buffer*/
  USIGN32 h_data_descr_addr;  /* data descriptior list address (DATA_DESCR)  */

  USIGN32 c_ready_mask;       /* ready mask                                  */
  USIGN32 c_base_address;     /* base address of DPR                         */

  /* originally for type and version of controller software           */
  /* I use c_id as H_TO_C_IRQ_VAL */
  USIGN8  c_id;               /* type and version of host software           */
  USIGN8  c_int_enable;       /* call by interrupt                           */
  USIGN8  c_address_swap_mode;/* address mode                                */
  USIGN8  c_state;            /* page addressing                             */
  USIGN32 c_param_addr;       /* parameter block address (SDB)               */
  USIGN32 c_data_addr;        /* data block address                          */
  USIGN16 c_param_size;       /* parameter block size                        */
  USIGN16 c_data_size;        /* data block size                             */
  VOLATILE USIGN8  c_sema;    /* semaphore                                   */
  USIGN8  c_ret_val;          /* return value                                */
  USIGN8  c_head;             /* reserved for a circular request block buffer*/
  USIGN8  c_tail;             /* reserved for a circular request block buffer*/
  USIGN32 c_data_descr_addr;  /* data descriptior list address (DATA_DESCR)  */
} T_CMI_DESCRIPTOR;

/* offsets of values in T_CMI_DESCRIPTOR for use with MREAD / MWRITE for non INTEL */
#define  H_READY_MASK            0
#define  H_BASE_ADDRESS          4
#define  H_ID                    8
#define  H_INT_ENABLE            9
#define  H_ADDRESS_SWAP_MODE     10
#define  H_STATE                 11
#define  H_PARAM_ADDR            12
#define  H_DATA_ADDR             16
#define  H_PARAM_SIZE            20
#define  H_DATA_SIZE             22
#define  H_SEMA                  24
#define  H_RET_VAL               25
#define  H_HEAD                  26
#define  H_TAIL                  27
#define  H_DATA_DESCR_ADDR       28

#define  C_READY_MASK            32
#define  C_BASE_ADDRESS          36
#define  C_ID                    40
#define  C_INT_ENABLE            41
#define  C_ADDRESS_SWAP_MODE     42
#define  C_STATE                 43
#define  C_PARAM_ADDR            44
#define  C_DATA_ADDR             48
#define  C_PARAM_SIZE            52
#define  C_DATA_SIZE             54
#define  C_SEMA                  56
#define  C_RET_VAL               57
#define  C_HEAD                  58
#define  C_TAIL                  59
#define  C_DATA_DESCR_ADDR       60


#else

/* --- DATA description block ---------------------------------------------- */
typedef struct T_DATA_DESCR
{
  USIGN8  dummy;			  /* kp@men */
  USIGN8  d_id;               /* identifier to describe the data description */
  /*VOLATILE*/ USIGN8  d_sema_c;  /* semaphore                                   */
  /*VOLATILE*/ USIGN8  d_sema_h;  /* semaphore                                   */
  USIGN16 d_data_size;        /* data block size                             */
  USIGN32 d_data_addr;        /* data block address                          */
} T_DATA_DESCR;

/* offset of values in T_DATA_DESCR for INTEL */
#define DUMMY            0
#define D_ID             1
#define D_SEMA_H         2
#define D_SEMA_C         3
#define D_DATA_SIZE      4
#define D_DATA_ADDR      6
#define T_DATA_DESC_LEN  10

/* --- CMI descriptor block (intel hosts) ---------------------------------- */
typedef struct T_CMI_DESCRIPTOR
{
  USIGN32 h_ready_mask;       /* ready mask                                  */
  USIGN32 h_base_address;     /* base address of DPR                         */

  /* originally for type and version of host software           */
  /* I use h_id as C_TO_H_IRQ_VAL */
  USIGN8  h_int_enable;       /* call by interrupt                           */
  USIGN8  h_id;               
  USIGN8  h_state;            /* page addressing                             */
  USIGN8  h_address_swap_mode;/* address mode                                */
  USIGN32 h_param_addr;       /* parameter block address (SDB)               */
  USIGN32 h_data_addr;        /* data block address                          */
  USIGN16 h_param_size;       /* parameter block size                        */
  USIGN16 h_data_size;        /* data block size                             */
  USIGN8  h_ret_val;          /* return value                                */
  VOLATILE USIGN8  h_sema;    /* semaphore                                   */
  USIGN8  h_tail;             /* reserved for a circular request block buffer*/
  USIGN8  h_head;             /* reserved for a circular request block buffer*/
  USIGN32 h_data_descr_addr;  /* data descriptior list address (DATA_DESCR)  */

  USIGN32 c_ready_mask;       /* ready mask                                  */
  USIGN32 c_base_address;     /* base address of DPR                         */

  /* originally for type and version of controller software           */
  /* I use c_id as H_TO_C_IRQ_VAL */
  USIGN8  c_int_enable;       /* call by interrupt                           */
  USIGN8  c_id;               /* type and version of host software           */
  USIGN8  c_state;            /* page addressing                             */
  USIGN8  c_address_swap_mode;/* address mode                                */
  USIGN32 c_param_addr;       /* parameter block address (SDB)               */
  USIGN32 c_data_addr;        /* data block address                          */
  USIGN16 c_param_size;       /* parameter block size                        */
  USIGN16 c_data_size;        /* data block size                             */
  USIGN8  c_ret_val;          /* return value                                */
  VOLATILE USIGN8 c_sema;     /* semaphore                                   */
  USIGN8  c_tail;             /* reserved for a circular request block buffer*/
  USIGN8  c_head;             /* reserved for a circular request block buffer*/
  USIGN32 c_data_descr_addr;  /* data descriptior list address (DATA_DESCR)  */
} T_CMI_DESCRIPTOR;

/* offsets of values in T_CMI_DESCRIPTOR for use with MREAD / MWRITE for INTEL */
#define  H_READY_MASK            0
#define  H_BASE_ADDRESS          4
#define  H_INT_ENABLE            8
#define  H_ID                    9
#define  H_STATE                 10
#define  H_ADDRESS_SWAP_MODE     11
#define  H_PARAM_ADDR            12
#define  H_DATA_ADDR             16
#define  H_PARAM_SIZE            20
#define  H_DATA_SIZE             22
#define  H_RET_VAL               24
#define  H_SEMA                  25
#define  H_TAIL                  26
#define  H_HEAD                  27
#define  H_DATA_DESCR_ADDR       28

#define  C_READY_MASK            32
#define  C_BASE_ADDRESS          36
#define  C_INT_ENABLE            40
#define  C_ID                    41
#define  C_STATE                 42
#define  C_ADDRESS_SWAP_MODE     43
#define  C_PARAM_ADDR            44
#define  C_DATA_ADDR             48
#define  C_PARAM_SIZE            52
#define  C_DATA_SIZE             54
#define  C_RET_VAL               56
#define  C_SEMA                  57
#define  C_TAIL                  58
#define  C_HEAD                  59
#define  C_DATA_DESCR_ADDR       60


#endif /* !INTEL */

#else /* MAC_BYTESWAP */
/* MAC_BYTESWAP twists accesses now motorola byteordering must be used for intel systems */
#if !INTEL
/* --- DATA description block (intel hosts)----------------------------------- */
typedef struct T_DATA_DESCR
{
  USIGN8  dummy;			  /* kp@men */
  USIGN8  d_id;               /* identifier to describe the data description */
  /*VOLATILE*/ USIGN8  d_sema_c;  /* semaphore                                   */
  /*VOLATILE*/ USIGN8  d_sema_h;  /* semaphore                                   */
  USIGN16 d_data_size;        /* data block size                             */
  USIGN32 d_data_addr;        /* data block address                          */
} T_DATA_DESCR;

/* offset of values in T_DATA_DESCR for INTEL */
#define DUMMY            0
#define D_ID             1
#define D_SEMA_H         2
#define D_SEMA_C         3
#define D_DATA_SIZE      4
#define D_DATA_ADDR      6
#define T_DATA_DESC_LEN  10

/* --- CMI descriptor block (intel hosts) ---------------------------------- */
typedef struct T_CMI_DESCRIPTOR
{
  USIGN32 h_ready_mask;       /* ready mask                                  */
  USIGN32 h_base_address;     /* base address of DPR                         */

  /* originally for type and version of host software           */
  /* I use h_id as C_TO_H_IRQ_VAL */
  USIGN8  h_int_enable;       /* call by interrupt                           */
  USIGN8  h_id;               
  USIGN8  h_state;            /* page addressing                             */
  USIGN8  h_address_swap_mode;/* address mode                                */
  USIGN32 h_param_addr;       /* parameter block address (SDB)               */
  USIGN32 h_data_addr;        /* data block address                          */
  USIGN16 h_param_size;       /* parameter block size                        */
  USIGN16 h_data_size;        /* data block size                             */
  USIGN8  h_ret_val;          /* return value                                */
  VOLATILE USIGN8  h_sema;    /* semaphore                                   */
  USIGN8  h_tail;             /* reserved for a circular request block buffer*/
  USIGN8  h_head;             /* reserved for a circular request block buffer*/
  USIGN32 h_data_descr_addr;  /* data descriptior list address (DATA_DESCR)  */

  USIGN32 c_ready_mask;       /* ready mask                                  */
  USIGN32 c_base_address;     /* base address of DPR                         */

  /* originally for type and version of controller software           */
  /* I use c_id as H_TO_C_IRQ_VAL */
  USIGN8  c_int_enable;       /* call by interrupt                           */
  USIGN8  c_id;               /* type and version of host software           */
  USIGN8  c_state;            /* page addressing                             */
  USIGN8  c_address_swap_mode;/* address mode                                */
  USIGN32 c_param_addr;       /* parameter block address (SDB)               */
  USIGN32 c_data_addr;        /* data block address                          */
  USIGN16 c_param_size;       /* parameter block size                        */
  USIGN16 c_data_size;        /* data block size                             */
  USIGN8  c_ret_val;          /* return value                                */
  VOLATILE USIGN8 c_sema;     /* semaphore                                   */
  USIGN8  c_tail;             /* reserved for a circular request block buffer*/
  USIGN8  c_head;             /* reserved for a circular request block buffer*/
  USIGN32 c_data_descr_addr;  /* data descriptior list address (DATA_DESCR)  */
} T_CMI_DESCRIPTOR;

/* offsets of values in T_CMI_DESCRIPTOR for use with MREAD / MWRITE for INTEL */
#define  H_READY_MASK            0
#define  H_BASE_ADDRESS          4
#define  H_INT_ENABLE            8
#define  H_ID                    9
#define  H_STATE                 10
#define  H_ADDRESS_SWAP_MODE     11
#define  H_PARAM_ADDR            12
#define  H_DATA_ADDR             16
#define  H_PARAM_SIZE            20
#define  H_DATA_SIZE             22
#define  H_RET_VAL               24
#define  H_SEMA                  25
#define  H_TAIL                  26
#define  H_HEAD                  27
#define  H_DATA_DESCR_ADDR       28

#define  C_READY_MASK            32
#define  C_BASE_ADDRESS          36
#define  C_INT_ENABLE            40
#define  C_ID                    41
#define  C_STATE                 42
#define  C_ADDRESS_SWAP_MODE     43
#define  C_PARAM_ADDR            44
#define  C_DATA_ADDR             48
#define  C_PARAM_SIZE            52
#define  C_DATA_SIZE             54
#define  C_RET_VAL               56
#define  C_SEMA                  57
#define  C_TAIL                  58
#define  C_HEAD                  59
#define  C_DATA_DESCR_ADDR       60

#else
/* --- DATA description block (motorola hosts)------------------------------ */
typedef struct T_DATA_DESCR
{
  USIGN8  d_id;               /* identifier to describe the data description */
  USIGN8  dummy;			  /* kp@men */
  VOLATILE USIGN8  d_sema_h;  /* semaphore                                   */
  VOLATILE USIGN8  d_sema_c;  /* semaphore                                   */
  USIGN16 d_data_size;        /* data block size                             */
  USIGN32 d_data_addr;        /* data block address                          */
} T_DATA_DESCR;

/* offset of values in T_DATA_DESCR for non INTEL */
#define D_ID             0
#define DUMMY            1
#define D_SEMA_H         2
#define D_SEMA_C         3
#define D_DATA_SIZE      4
#define D_DATA_ADDR      6
#define T_DATA_DESC_LEN  10


/* --- CMI descriptor block (motorola hosts) ------------------------------- */
typedef struct T_CMI_DESCRIPTOR
{
  USIGN32 h_ready_mask;       /* ready mask                                  */
  USIGN32 h_base_address;     /* base address of DPR                         */

  /* originally for type and version of host software           */
  /* I use h_id as C_TO_H_IRQ_VAL */
  USIGN8  h_id;               
  USIGN8  h_int_enable;       /* call by interrupt                           */
  USIGN8  h_address_swap_mode;/* address mode                                */
  USIGN8  h_state;            /* page addressing                             */
  USIGN32 h_param_addr;       /* parameter block address (SDB)               */
  USIGN32 h_data_addr;        /* data block address                          */
  USIGN16 h_param_size;       /* parameter block size                        */
  USIGN16 h_data_size;        /* data block size                             */
  VOLATILE USIGN8  h_sema;    /* semaphore                                   */
  USIGN8  h_ret_val;          /* return value                                */
  USIGN8  h_head;             /* reserved for a circular request block buffer*/
  USIGN8  h_tail;             /* reserved for a circular request block buffer*/
  USIGN32 h_data_descr_addr;  /* data descriptior list address (DATA_DESCR)  */

  USIGN32 c_ready_mask;       /* ready mask                                  */
  USIGN32 c_base_address;     /* base address of DPR                         */

  /* originally for type and version of controller software           */
  /* I use c_id as H_TO_C_IRQ_VAL */
  USIGN8  c_id;               /* type and version of host software           */
  USIGN8  c_int_enable;       /* call by interrupt                           */
  USIGN8  c_address_swap_mode;/* address mode                                */
  USIGN8  c_state;            /* page addressing                             */
  USIGN32 c_param_addr;       /* parameter block address (SDB)               */
  USIGN32 c_data_addr;        /* data block address                          */
  USIGN16 c_param_size;       /* parameter block size                        */
  USIGN16 c_data_size;        /* data block size                             */
  VOLATILE USIGN8  c_sema;    /* semaphore                                   */
  USIGN8  c_ret_val;          /* return value                                */
  USIGN8  c_head;             /* reserved for a circular request block buffer*/
  USIGN8  c_tail;             /* reserved for a circular request block buffer*/
  USIGN32 c_data_descr_addr;  /* data descriptior list address (DATA_DESCR)  */
} T_CMI_DESCRIPTOR;

/* offsets of values in T_CMI_DESCRIPTOR for use with MREAD / MWRITE for non INTEL */
#define  H_READY_MASK            0
#define  H_BASE_ADDRESS          4
#define  H_ID                    8
#define  H_INT_ENABLE            9
#define  H_ADDRESS_SWAP_MODE     10
#define  H_STATE                 11
#define  H_PARAM_ADDR            12
#define  H_DATA_ADDR             16
#define  H_PARAM_SIZE            20
#define  H_DATA_SIZE             22
#define  H_SEMA                  24
#define  H_RET_VAL               25
#define  H_HEAD                  26
#define  H_TAIL                  27
#define  H_DATA_DESCR_ADDR       28

#define  C_READY_MASK            32
#define  C_BASE_ADDRESS          36
#define  C_ID                    40
#define  C_INT_ENABLE            41
#define  C_ADDRESS_SWAP_MODE     42
#define  C_STATE                 43
#define  C_PARAM_ADDR            44
#define  C_DATA_ADDR             48
#define  C_PARAM_SIZE            52
#define  C_DATA_SIZE             54
#define  C_SEMA                  56
#define  C_RET_VAL               57
#define  C_HEAD                  58
#define  C_TAIL                  59
#define  C_DATA_DESCR_ADDR       60

#endif /* if INTEL */

#endif /* MAC_BYTESWAP */

#endif /* _CMI_STRUCT_H */
