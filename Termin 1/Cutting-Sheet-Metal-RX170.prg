ProgramFile
! Below section is called once at t=0
! Add Initialization commands here
! 
EndInit
! 
! Below section is called at t>0
! Add new ERPL / ERCL commands here
! 
SPEED_PTP_OV   80.0000
SPEED_CP_OV    80.0000
SPEED_ORI_OV   80.0000
ACCEL_PTP_OV   100.0000
ACCEL_CP_OV    100.0000
ACCEL_ORI_OV   100.0000
OV_PRO         100.0000
ERC NO_DECEL OFF
ZONE             0.0000
! 
! 
call MyMoveFct()
! 
EndProgramFile

Fct MyMoveFct()
Home HOME_1
PTP              1.5656     0.0728     0.5668     0.0000   -10.0000    90.0000
PTP             -0.1274     1.5621     0.5668     9.9940     0.3472  -178.0304
LIN             -0.0575    -0.4380     0.9147     6.9958  -179.7562    -1.9857
VIA_POS         -0.1706    -0.4380     1.9066     6.9959  -179.7564    -1.9866
CIRC             0.1846    -0.4379     1.2404     6.9960  -179.7564    -1.9865
! 
EndFct

