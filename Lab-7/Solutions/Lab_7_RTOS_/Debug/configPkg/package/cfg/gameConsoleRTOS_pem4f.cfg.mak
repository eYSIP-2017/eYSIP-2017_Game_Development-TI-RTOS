# invoke SourceDir generated makefile for gameConsoleRTOS.pem4f
gameConsoleRTOS.pem4f: .libraries,gameConsoleRTOS.pem4f
.libraries,gameConsoleRTOS.pem4f: package/cfg/gameConsoleRTOS_pem4f.xdl
	$(MAKE) -f /Users/AxRox/TIVA_Workspace/Lab_7_RTOS_/src/makefile.libs

clean::
	$(MAKE) -f /Users/AxRox/TIVA_Workspace/Lab_7_RTOS_/src/makefile.libs clean

