# invoke SourceDir generated makefile for VM_RTOS.pem4f
VM_RTOS.pem4f: .libraries,VM_RTOS.pem4f
.libraries,VM_RTOS.pem4f: package/cfg/VM_RTOS_pem4f.xdl
	$(MAKE) -f /Users/AxRox/TIVA_Workspace/Vending_Machine_Final/src/makefile.libs

clean::
	$(MAKE) -f /Users/AxRox/TIVA_Workspace/Vending_Machine_Final/src/makefile.libs clean

