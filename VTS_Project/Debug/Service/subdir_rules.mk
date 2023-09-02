################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
Service/%.obj: ../Service/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1230/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/Swelam/Desktop/try_GPS/try_GPS" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295" --include_path="C:/Users/Swelam/Desktop/try_GPS/try_GPS/Service" --include_path="C:/Users/Swelam/Desktop/try_GPS/try_GPS/ECUA" --include_path="C:/ti/ccs1230/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/include" --include_path="C:/ti/tivaware_c_series_2_1_4_178" --include_path="C:/Users/20115/Desktop/GSM/GSM" --define=ccs="ccs" --define=PART_TM4C123GH6PM -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="Service/$(basename $(<F)).d_raw" --obj_directory="Service" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


