MK_OBJS+=lm3s6918.o interrupts.o

ifneq ($(CONFIG_TI_CCS5),)
  CFLAGS+=-mv7M3 --code_state=16 --define=PART_LM3S6918
  LDFLAGS+=-mv7M3
  LINKER_SCRIPT:=$(d)/lm3s6918.cmd
  # Currently require pre-compiled version of driverlib for ccs5
  LIBS+=-l"$(TI_ARM_MCU_BSP_DIR)/driverlib/ccs-cm3/Debug/driverlib-cm3.lib"
endif

ifneq ($(CONFIG_GCC),)
  BSP_SRCS:=systick.c sysctl.c gpio.c interrupt.c uart.c ssi.c cpu.c pwm.c \
    $(if $(CONFIG_STELLARIS_ETH),ethernet.c)

  MK_LINKS:=$(addprefix $(TI_ARM_MCU_BSP_DIR)/driverlib/,$(BSP_SRCS))

  MK_OBJS+=entry_gcc.o $(BSP_SRCS:%.c=%.o)

  LINKER_SCRIPT:=$(BUILD)/$(d)/lm3s6918.ld
  CFLAGS+=-mthumb -mcpu=cortex-m3 -O3 \
    -ffunction-sections -fdata-sections -DPART_LM3S6918 \
    -I$(TI_ARM_MCU_BSP_DIR) -Dgcc -fsingle-precision-constant
  LDFLAGS+=--entry reset_isr --gc-sections --nostdlib
  LIBS+=-L$(call get_libc_dir) -lm -lc -L$(call get_libgcc_dir) -lgcc
endif
