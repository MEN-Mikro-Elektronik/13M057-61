#***************************  M a k e f i l e  *******************************
#
#         Author: ag
#          $Date: 2014/11/05 10:52:21 $
#      $Revision: 1.1 $
#
#    Description: Makefile definitions for the PROFIDP tool program
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: program.mak,v $
#   Revision 1.1  2014/11/05 10:52:21  AGromann
#   Initial Revision
#
#   Revision 1.1  2000/08/30 15:29:43  gromann
#   Initial Revision
#
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=profidp_mod_vx_tool

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
         $(MEN_INC_DIR)/PROFIDP_MOD_VX/pb_usr_twist.h \


MAK_INP1=profidp_tool$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

