#***************************  M a k e f i l e  *******************************
#
#         Author: kp
#          $Date: 2014/11/05 10:52:25 $
#      $Revision: 1.1 $
#
#    Description: Makefile definitions for the PROFIDP test program
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: program.mak,v $
#   Revision 1.1  2014/11/05 10:52:25  AGromann
#   Initial Revision
#
#   Revision 1.1  2000/06/15 15:08:29  gromann
#   Initial Revision
#
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=profidp_mod_vx_test_endpruf

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/mdis_api$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/usr_oss$(LIB_SUFFIX)

MAK_INCL=$(MEN_INC_DIR)/profidp_mod_vx_drv.h	\
         $(MEN_INC_DIR)/men_typs.h	\
         $(MEN_INC_DIR)/mdis_api.h	\
         $(MEN_INC_DIR)/mdis_err.h	\
         $(MEN_INC_DIR)/usr_oss.h   \
         $(MEN_INC_DIR)/PROFIDP_MOD_VX/pb_type.h    \
         $(MEN_INC_DIR)/PROFIDP_MOD_VX/pb_conf.h    \
         $(MEN_INC_DIR)/PROFIDP_MOD_VX/pb_dp.h      \
         $(MEN_INC_DIR)/PROFIDP_MOD_VX/pb_err.h     \
         $(MEN_INC_DIR)/PROFIDP_MOD_VX/pb_fmb.h     \
         $(MEN_INC_DIR)/PROFIDP_MOD_VX/pb_if.h      \
         $(MEN_INC_DIR)/PROFIDP_MOD_VX/pb_usr_twist.h  \
         $(MEN_MOD_DIR)/dp_config_endpruf.h


MAK_INP1=profidp_test_endpruf$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)
