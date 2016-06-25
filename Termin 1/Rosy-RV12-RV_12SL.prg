ProgramFile
! cRobot 'RV_12SL'
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
VIA_POS         -0.8918    -0.0858     0.9020   123.6276   -25.4628    51.2218
CIRC             0.3716     0.8152     0.9020    56.2468  -154.1452    19.0171
PTP              0.1186     0.3914     1.3055   -44.8395   -32.6817    77.4476
! 
EndFct

