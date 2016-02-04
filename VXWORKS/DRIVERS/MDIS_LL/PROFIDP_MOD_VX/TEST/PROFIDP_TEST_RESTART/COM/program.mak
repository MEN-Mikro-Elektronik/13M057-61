#***************************  M a k e f i l e  *******************************
#
#         Author: ag
#          $Date: 2014/11/05 10:52:29 $
#      $Revision: 1.1 $
#
#    Description: Makefile definitions for the PROFIDP test program
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: program.mak,v $
#   Revision 1.1  2014/11/05 10:52:29  AGromann
#   Initial Revision
#
#   Revision 1.1  2003/01/08 15:37:15  agromann
#   Initial Revision
#
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=profidp_mod_vx_test_restart

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/mdis_api$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/usr_oss$(LIB_SUFFIX)


MAK_INCL=$(MEN_INC_DIR)/profidp_mod_vx_drv.h	\
         $(MEN_INC_DIR)/men_typs.h	\
         $(MEN_INC_DIR)/mdis_api.h	\
         $(MEN_INC_DIR)/usr_oss.h   \
         $(MEN_INC_DIR)/PROFIDP_MOD_VX/pb_type.h    \
         $(MEN_INC_DIR)/PROFIDP_MOD_VX/pb_conf.h    \
         $(MEN_INC_DIR)/PROFIDP_MOD_VX/pb_dp.h      \
         $(MEN_INC_DIR)/PROFIDP_MOD_VX/pb_err.h     \
         $(MEN_INC_DIR)/PROFIDP_MOD_VX/pb_fmb.h     \
         $(MEN_INC_DIR)/PROFIDP_MOD_VX/pb_if.h      \
         $(MEN_INC_DIR)/PROFIDP_MOD_VX/pb_usr_twist.h \
         $(MEN_MOD_DIR)/dp_config_test_restart.h  


MAK_INP1=profidp_test_restart$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

