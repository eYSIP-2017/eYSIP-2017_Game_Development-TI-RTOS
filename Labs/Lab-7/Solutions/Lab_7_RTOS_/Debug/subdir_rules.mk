################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
EK_TM4C123GXL.obj: ../EK_TM4C123GXL.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/Users/AxRox/TIVA_Workspace/Lab_7_RTOS_" --include_path="/Users/AxRox/TIVA_Workspace/Lab_7_RTOS_" --include_path="/Users/AxRox/ti/tirtos_tivac_2_16_00_08/products/TivaWare_C_Series-2.1.1.71b" --include_path="/Users/AxRox/ti/tirtos_tivac_2_16_00_08/products/bios_6_45_01_29/packages/ti/sysbios/posix" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.1.LTS/include" --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=ccs --define=TIVAWARE -g --gcc --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="EK_TM4C123GXL.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

gameConsoleRTOS.obj: ../gameConsoleRTOS.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/Users/AxRox/TIVA_Workspace/Lab_7_RTOS_" --include_path="/Users/AxRox/TIVA_Workspace/Lab_7_RTOS_" --include_path="/Users/AxRox/ti/tirtos_tivac_2_16_00_08/products/TivaWare_C_Series-2.1.1.71b" --include_path="/Users/AxRox/ti/tirtos_tivac_2_16_00_08/products/bios_6_45_01_29/packages/ti/sysbios/posix" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.1.LTS/include" --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=ccs --define=TIVAWARE -g --gcc --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="gameConsoleRTOS.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

build-1763385263:
	@$(MAKE) -Onone -f subdir_rules.mk build-1763385263-inproc

build-1763385263-inproc: ../gameConsoleRTOS.cfg
	@echo 'Building file: $<'
	@echo 'Invoking: XDCtools'
	"/Users/AxRox/ti/xdctools_3_32_00_06_core/xs" --xdcpath="/Users/AxRox/ti/tirtos_tivac_2_16_00_08/packages;/Users/AxRox/ti/tirtos_tivac_2_16_00_08/products/tidrivers_tivac_2_16_00_08/packages;/Users/AxRox/ti/tirtos_tivac_2_16_00_08/products/bios_6_45_01_29/packages;/Users/AxRox/ti/tirtos_tivac_2_16_00_08/products/ndk_2_25_00_09/packages;/Users/AxRox/ti/tirtos_tivac_2_16_00_08/products/uia_2_00_05_50/packages;/Users/AxRox/ti/tirtos_tivac_2_16_00_08/products/ns_1_11_00_10/packages;/Applications/ti/ccsv7/ccs_base;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M4F -p ti.platforms.tiva:TM4C123GH6PM -r release -c "/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.1.LTS" --compileOptions "-mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path=\"/Users/AxRox/TIVA_Workspace/Lab_7_RTOS_\" --include_path=\"/Users/AxRox/TIVA_Workspace/Lab_7_RTOS_\" --include_path=\"/Users/AxRox/ti/tirtos_tivac_2_16_00_08/products/TivaWare_C_Series-2.1.1.71b\" --include_path=\"/Users/AxRox/ti/tirtos_tivac_2_16_00_08/products/bios_6_45_01_29/packages/ti/sysbios/posix\" --include_path=\"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.1.LTS/include\" --define=ccs=\"ccs\" --define=PART_TM4C123GH6PM --define=ccs --define=TIVAWARE -g --gcc --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi  " "$<"
	@echo 'Finished building: $<'
	@echo ' '

configPkg/linker.cmd: build-1763385263 ../gameConsoleRTOS.cfg
configPkg/compiler.opt: build-1763385263
configPkg/: build-1763385263


