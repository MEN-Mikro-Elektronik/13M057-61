/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: cmi.h
 *
 *       Author: ag
 *        $Date: 2014/11/05 10:51:39 $
 *    $Revision: 1.1 $
 *
 *  Description: Intern header file for PROFIDP driver
 *               - prototypes for cmi.c
 *               
 *
 *     Switches: 
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: cmi.h,v $
 * Revision 1.1  2014/11/05 10:51:39  AGromann
 * Initial Revision
 *
 * Revision 1.5  2001/10/25 11:17:53  kp
 * cosmetics
 *
 * Revision 1.4  2001/10/24 16:03:28  kp
 * removed unused protos
 *
 * Revision 1.3  2001/02/28 08:18:53  kp
 * added newline at EOF
 *
 * Revision 1.2  2000/03/09 09:58:55  Gromann
 * Changes for Win NT
 *
 * Revision 1.1  2000/02/29 12:05:05  Gromann
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _CMI_H
#define _CMI_H


extern void copy_to_dpram
            ( 
			 LL_HANDLE  *llHdl, /* low level handle */
			 USIGN8 FAR *dst,	/* pointer to dpram (word aligned!) */
			 USIGN8 FAR *src,	/* pointer to host memory */
			 USIGN16 len		/* number of bytes to copy (even or odd) */
			);
/*-------------------------------------------------------------------------*/

extern void copy_from_dpram
            ( 
			LL_HANDLE* llHdl,   /* low level handle */
			 USIGN8 FAR *dst,	/* pointer to host memory */
			 USIGN8 FAR *src,	/* pointer to dpram (word aligned!) */
			 USIGN16 len		/* number of bytes to copy (even or odd) */
			);


/*--- prototypes ---*/

         
FUNCTION extern PUBLIC PB_INT16 cmi_init
        (
			LL_HANDLE *llHdl  /* low level handle */
          /*
          IN  USIGN32 m57_base_a08_adr,
          IN  USIGN32 m57_base_a24_adr
          */
        );


FUNCTION extern PUBLIC PB_INT16 cmi_read
         (
								LL_HANDLE	*llHdl,  /* low level handle */
          IN    T_PROFI_SERVICE_DESCR FAR   *sdb_ptr,
          IN    VOID                  FAR   *data_ptr,
          INOUT USIGN16               FAR   *data_len
         );
         
FUNCTION extern PUBLIC PB_INT16 cmi_write
         (
							  LL_HANDLE	  *llHdl,   /* low level handle */
          IN T_PROFI_SERVICE_DESCR  FAR   *sdb_ptr,
          IN VOID                   FAR   *data_ptr,
		  IN USIGN16                      data_len,
		  IN PB_BOOL					  waitForAck					  
         );
         
FUNCTION extern PUBLIC PB_INT16 cmi_get_data_descr
        (
			LL_HANDLE*      llHdl,              /* low level handle */
          IN    USIGN8      data_descr_id, 		/* id of data description   */
          IN    USIGN16     offset,             /* offset in data description*/
          OUT   USIGN16 FAR *data_descr_size,   /* pointer to size of data 
												   description */
          INOUT VOID    FAR *data_descr_ptr     /* pointer to data 
												   description         */
        );
        
FUNCTION extern PUBLIC PB_INT16 cmi_set_data_descr
        (
			LL_HANDLE* llHdl,              /* low level handle */
          IN  USIGN8   data_descr_id,      /* id of data description      */
          IN  USIGN16  offset,             /* offset in data description  */
          IN  USIGN16  data_descr_size,    /* size of data description    */
          IN  VOID FAR *data_descr_ptr     /* pointer to data description */
        );


FUNCTION extern PB_INT16 dpgdl_get_data_len
  (
  IN    PB_INT16     result,
  IN    USIGN8    service,
  IN    USIGN8    primitive,
  IN    USIGN8    FAR *data_ptr,
  OUT   PB_INT16     FAR *data_len
  )
  /*------------------------------------------------------------------------*/
  /* FUNCTIONAL_DESCRIPTION                                                 */
  /*------------------------------------------------------------------------*/
  /* - returns the data length of any called PROFIBUS DP service            */
  /*------------------------------------------------------------------------*/
;



/*****************************************************************************/
/* public function declarations from file:     fmbgdl.c                      */
/*****************************************************************************/

FUNCTION extern PB_INT16 fmbgdl_get_data_len 
         (
            IN  PB_INT16      result,           /* Service-Result       */
            IN  USIGN8     service,          /* Service              */
            IN  USIGN8     primitive,        /* Service-Primitive    */
            IN  USIGN8 FAR *data_ptr,        /* pointer to data      */
            OUT PB_INT16      *data_len         /* length of data       */
         )

/*-----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

this function is used to return the data length of FMG-SERVICES

possible return values:
- Data-length

-----------------------------------------------------------------------------*/
;


#endif /* _CMI_H */
