# user code

USER_SRC_DIR = . #user_src
USER_BUILD_DIR = ../user_build
USER_C_FILES = $(wildcard $(USER_SRC_DIR)/*.c)
USER_ASM_FILES = $(wildcard $(USER_SRC_DIR)/*.S)
USER_OBJ_FILES = $(USER_C_FILES:$(USER_SRC_DIR)/%.c=$(USER_BUILD_DIR)/%_c.o)
USER_OBJ_FILES += $(USER_ASM_FILES:$(USER_SRC_DIR)/%.S=$(USER_BUILD_DIR)/%_s.o)

$(USER_BUILD_DIR):
	mkdir $(USER_BUILD_DIR)

$(USER_BUILD_DIR)/%_c.o: $(USER_SRC_DIR)/%.c
	$(ARMGNU)-gcc $(CFLAGS) $(COPS) -MMD -c $< -o $@

$(USER_BUILD_DIR)/%_s.o: $(USER_SRC_DIR)/%.S
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@

user.img: user_build $(USER_OBJ_FILES)
	$(ARMGNU)-ld -T $(USER_SRC_DIR)/linker.ld -o user.elf $(USER_OBJ_FILES)
	$(ARMGNU)-objcopy user.elf -O binary user.img

user: user.img

clean:
	rm -rf $(USER_BUILD_DIR)
