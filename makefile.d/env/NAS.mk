NAS_ADDR := //100.69.175.93/Public
NAS_FLAGS := -o username=robo,password=robo,iocharset=utf8,file_mode=0777,dir_mode=077

NAS := /mnt/RoboNAS
NAS_FLASH := $(NAS_PATH)/flash
NAS_FLASH_PATH := $(NAS_FLASH)/korobo-2024-C
NAS_FLASH_MDC := $(NAS_FLASH)/mdc

NAS_MOUNT_FLAG := $(NAS_PATH)/aquota.group
