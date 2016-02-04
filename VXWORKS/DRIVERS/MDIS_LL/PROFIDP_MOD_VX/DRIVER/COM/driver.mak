#***************************  M a k e f i l e  *******************************
#
#         Author: kp
#          $Date: 2014/11/05 10:51:46 $
#      $Revision: 1.1 $
#
#    Description: Makefile definitions for the PROFIDP driver
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: driver.mak,v $
#   Revision 1.1  2014/11/05 10:51:46  AGromann
#   Initial Revision
#
#   Revision 1.4  2004/05/05 08:25:19  cs
#   removed needless include mbuf.h
#
#   Revision 1.3  2000/06/15 15:07:33  gromann
#   added driver specific header files
#
#   Revision 1.2  2000/03/09 09:59:01  Gromann
#   Changes for Win NT
#
#   Revision 1.1  2000/02/29 12:05:15  Gromann
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=profidp_mod_vx

MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/desc$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/mbuf$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/oss$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/id$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/dbg$(LIB_SUFFIX)	\


MAK_INCL=$(MEN_INC_DIR)/profidp_mod_vx_drv.h	\
         $(MEN_INC_DIR)/men_typs.h	\
         $(MEN_INC_DIR)/oss.h		\
         $(MEN_INC_DIR)/mdis_err.h	\
         $(MEN_INC_DIR)/maccess.h	\
         $(MEN_INC_DIR)/desc.h		\
         $(MEN_INC_DIR)/mdis_api.h	\
         $(MEN_INC_DIR)/mdis_com.h	\
         $(MEN_INC_DIR)/modcom.h	\
         $(MEN_INC_DIR)/ll_defs.h	\
         $(MEN_INC_DIR)/ll_entry.h	\
         $(MEN_INC_DIR)/dbg.h		\
         $(MEN_MOD_DIR)/m57.h	    \
         $(MEN_MOD_DIR)/cmi.h	    \
         $(MEN_MOD_DIR)/cmi_struct.h       \
         $(MEN_MOD_DIR)/m57_firm.h	       \
         $(MEN_MOD_DIR)/m57segm.h	       \
         $(MEN_MOD_DIR)/pci.h	           \
         $(MEN_MOD_DIR)/profidp_drv_int.h  \
         $(MEN_INC_DIR)/PROFIDP_MOD_VX/keywords.h \
         $(MEN_INC_DIR)/PROFIDP_MOD_VX/pb_conf.h  \
         $(MEN_INC_DIR)/PROFIDP_MOD_VX/pb_dp.h    \
         $(MEN_INC_DIR)/PROFIDP_MOD_VX/pb_err.h   \
         $(MEN_INC_DIR)/PROFIDP_MOD_VX/pb_fmb.h   \
         $(MEN_INC_DIR)/PROFIDP_MOD_VX/pb_if.h    \
         $(MEN_INC_DIR)/PROFIDP_MOD_VX/pb_type.h  \
         $(MEN_INC_DIR)/PROFIDP_MOD_VX/pb_usr_twist.h     \
         $(MEN_INC_DIR)/PROFIDP_MOD_VX/profidp_byte_ord.h \
         $(MEN_INC_DIR)/PROFIDP_MOD_VX/twist.h            \


MAK_INP1=profidp_drv$(INP_SUFFIX)
MAK_INP2=fmbgdl$(INP_SUFFIX)
MAK_INP3=dpgdl$(INP_SUFFIX)
MAK_INP4=pci$(INP_SUFFIX)
MAK_INP5=cmi$(INP_SUFFIX)
MAK_INP6=m57_firm$(INP_SUFFIX)


MAK_INP=$(MAK_INP1) \
        $(MAK_INP2) \
        $(MAK_INP3) \
        $(MAK_INP4) \
        $(MAK_INP5) \
        $(MAK_INP6) 
