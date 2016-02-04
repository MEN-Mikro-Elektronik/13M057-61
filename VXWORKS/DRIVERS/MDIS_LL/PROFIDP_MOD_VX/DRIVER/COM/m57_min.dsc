#************************** MDIS4 device descriptor *************************
#
# m57_min.dsc: Descriptor for M57_MOD_VX
# Automatically generated by mdiswiz 2.01.024-vxworks-4.2 Build Nov 28 2013 16:08:14 from 13m05761.xml
# 2014-11-05
#
#****************************************************************************

m57_1 {

    # ------------------------------------------------------------------------
    #        general parameters (don't modify)
    # ------------------------------------------------------------------------
    DESC_TYPE = U_INT32 0x1
    HW_TYPE = STRING PROFIDP_MOD_VX

    # ------------------------------------------------------------------------
    #        reference to base board
    # ------------------------------------------------------------------------
    BOARD_NAME = STRING a201_1
    DEVICE_SLOT = U_INT32 0x0

    # ------------------------------------------------------------------------
    #        device parameters
    # ------------------------------------------------------------------------

    # enable irq after init
    IRQ_ENABLE = U_INT32 1

    # Define wether M-Module ID-PROM is checked
    ID_CHECK = U_INT32 1
}
# EOF