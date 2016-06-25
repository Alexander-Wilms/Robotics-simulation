ProgramFile
! cRobot 'RV6'
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
PTP             -0.5000     0.7000     0.600     0.0000   180.0000     0.0000
LIN             -0.5000     0.7000     0.5000     0.0000   180.0000     0.0000
LIN             -0.5000     1.3000     0.500     0.0000   180.0000     0.0000
LIN             0.5000     1.3000     0.500     0.0000   180.0000     0.0000
LIN             0.5000     0.7000     0.500     0.0000   180.0000     0.0000
LIN             -0.5000     0.7000     0.5000     0.0000   180.0000     0.0000
PTP             -0.2500     1.0000     0.8500     0.0000   180.0000     0.0000
LIN             -0.2500     1.0000     0.7500     0.0000   180.0000     0.0000
ERC GRAB BODY ZYL1
PTP             -0.2500     1.0000     1.00     0.0000   180.0000     0.0000
PTP             0     1.0000     1.10     0.0000   180.0000     0.0000
LIN             0     1.0000     1.00     0.0000   180.0000     0.0000
ERC RELEASE BODY ZYL1
PTP             0.2500     1.0000     0.8500     0.0000   180.0000     0.0000
LIN             0.2500     1.0000     0.7500     0.0000   180.0000     0.0000
ERC GRAB BODY ZYL3
LIN             0.25000     0.70     1     0.0000   180.0000     0.0000
LIN             -0.25000     0.70     1     0.0000   180.0000     0.0000
PTP             -0.2500     1.0000     0.8500     0.0000   180.0000     0.0000
PTP             -0.2500     1.0000     0.7500     0.0000   180.0000     0.0000
ERC RELEASE BODY ZYL3
PTP             0     1.0000     1.00     0.0000   180.0000     0.0000
ERC GRAB BODY ZYL1
PTP             0.2500     1.0000     0.8500     0.0000   180.0000     0.0000
LIN             0.2500     1.0000     0.7500     0.0000   180.0000     0.0000
ERC RELEASE BODY ZYL1
call Circle()
EndFct
!
Fct Circle()
LIN_REL          0.0500     0.0000     0.0000     0.0000     0.0000     0.0000
VIA_POS_REL      -0.0500     -0.0500     0.0000     0.0000     0.0000     0.0000
CIRC_REL         -0.1000    0.000     0.0000     0.0000     0.0000     0.0000
VIA_POS_REL      0.0500     0.0500     0.0000     0.0000     0.0000     0.0000
CIRC_REL         0.1000    0.000     0.0000     0.0000     0.0000     0.0000
!
EndFct

