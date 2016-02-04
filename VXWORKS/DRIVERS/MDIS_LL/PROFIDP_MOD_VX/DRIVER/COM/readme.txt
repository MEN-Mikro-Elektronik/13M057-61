This Driver differs from the description in the PROFIDP User Manual
Edition E1 in the following point:

SetStat functions PROFIDP_BLK_RCV_CON_IND and 
PROFIDP_BLK_RCV_CON_IND_WAIT:

If an overflow of CON/IND buffer occurs the CURRENT CON/IND 
element is lost.
(Previously the "oldest" element was overwritten.)


