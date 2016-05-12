APPNAME = helloworld
TOOLCHAIN = /home/lime-lab+sam/Documents/buildroot-2016.02/output/host/usr/bin
ROOTFS_DIR = /home/lime-lab+sam/Documents/buildroot-2016.02/rootfs

#CROSS_COMPILER = $(TOOLCHAIN)/bin/arm-fsl-linux-gnueabiCC
CROSS_COMPILER = $(TOOLCHAIN)/arm-buildroot-linux-gnueabi-
CC = $(CROSS_COMPILER)gcc
DEL_FILE = rm -rf
CP_FILE = cp -rf
TARGET_PATH_LIB = $(ROOTFS_DIR)/usr/lib
TARGET_PATH_INCLUDE = $(ROOTFS_DIR)/usr/include
CFLAGS = -DLINUX -DUSE_SOC_MX6 -Wall -O2 -fsigned-char -march=armv7-a -mfpu=neon -mfloat-abi=softfp -DEGL_API_FB -DGPU_TYPE_VIV -DGL_GLEXT_PROTOTYPES -DENABLE_GPU_RENDER_20 I--library-path../include -I$(TARGET_PATH_INCLUDE)
LFLAGS = -Wl,--library-path=$(TARGET_PATH_LIB),-rpath-link=$(TARGET_PATH_LIB) -lm -lglib-2.0 -lOpenCL -lCLC -ldl -lpthread
OBJECTS = $(APPNAME).o
first: all
all: $(APPNAME)
$(APPNAME): $(OBJECTS)
	$(CC) $(LFLAGS) -o $(APPNAME) $(OBJECTS)
$(APPNAME).o: $(APPNAME).c
	$(CC) $(CFLAGS) -c $(APPNAME).c
clean:
	$(DEL_FILE) $(APPNAME) 