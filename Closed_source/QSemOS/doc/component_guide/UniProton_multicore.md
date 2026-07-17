# Uniproton 多核与多实例部署使用指南

## 1 整体方案

在多核场景下，可通过SMP机制支持UniProton的多核运行，使同一OS实例的多个任务同时在不同核上同时处理。混合关键性部署机制和资源表升级方案则可支持多个OS实例的同时启动与运行。以上特性可显著提升系统的实时性能与并行性能，应用场景广泛。

## 2 开发环境说明

- 开发平台：armv8
- 芯片型号：kp920_lite
- OS版本信息：UniProton master
- 集成开发环境：UniProton-docker

## 3 多核SMP部署

### 3.1 SMP镜像编译

1) 以kp920_lite平台为例，修改开关配置文件build/uniproton_config/config_armv8_kp920_lite/defconfig：
```
CONFIG_OS_MAX_CORE_NUM=4  // 配置为最大可支持的核数
CONFIG_OS_OPTION_CPU64=y  // 是否为64位操作系统
CONFIG_OS_OPTION_SMP=y  // 是否启用SMP功能，开启
CONFIG_OS_OPTION_HWI_AFFINITY=y  // 是否启用中断绑核功能，开启
CONFIG_INTERNAL_OS_SPIN_LOCK=y  // 是否启用自旋锁功能，开启
CONFIG_OS_OPTION_TASK_AFFINITY_STATIC=y  // 是否支持任务亲和性静态特性，开启
# CONFIG_INTERNAL_OS_SCHEDULE_SINGLE_CORE_BY_CCODE is not set  // 内部函数宏，单核使用C语言实现调度部分时使用，关闭
```
通过对以上开关进行配置，SMP的主要功能便可支持。

2) 修改配置文件demos/kp920_lite/config/prt_config.h
```
/* 实时系统实际运行的核数，单位：个 */
#define OS_SYS_CORE_RUN_NUM                             2
/* 最大可支持的核数，单位：个 */
#define OS_SYS_CORE_MAX_NUM                             4
/* 主核ID */
#define OS_SYS_CORE_PRIMARY                             2
```
以上配置项设置了SMP运行的主核为CPU 2，系统的模块注册和初始化操作都是在主核上完成。除主核外，配置了从核CPU 3上的系统任务运行。

3) 可通过以下命令，在demos/kp920_lite/build目录下编译SMP测试用例，生成用例镜像UniProton_SMP_test.elf。
```
sh build_app.sh UniProton_SMP_test
```

### 3.2 SMP镜像部署运行

1) 以kp920_lite为例，CPU 3在欧拉侧已预留，只需要将CPU 2下线即可：
```
echo 0 > /sys/bus/cpu/devices/cpu2/online
```

2) 设置mica conf文件：
```
[Mica]
Name=uniproton-kp920l-smp
CPU=2
ClientPath=/mcs/UniProton_SMP_test.elf
AutoBoot=no
```

3) 基于上述conf文件配置，将UniProton_SMP_test.elf使用mica拉起，成功后进入tty界面，便可查看多核任务打印情况。

### 3.3 SMP多核任务设置

在以上测试用例中，实现了SMP多核任务的设置。与单核任务创建不同，SMP多核在使用PRT_TaskCreate接口创建任务后，还需要调用PRT_TaskCoreBind接口将任务与指定核绑定，以实现多核任务同时运行。

## 4 多实例部署

多实例部署主要分为多实例镜像编译，多实例镜像配置文件设置以及多实例镜像拉起几个步骤：

### 4.1 多实例镜像编译

1) 在demos/kp920_lite/build目录下，执行以下编译命令，生成kp920_lite.elf镜像作为第一个实例。
```
sh build_app.sh
```

2) 修改demos/kp920_lite/bsp/kp920_lite/cpu_config.h，为第二个UniProton实例预留起始地址：
```
#define MMU_IMAGE_ADDR             0x62000000ULL
```
还需同步修改demos/kp920_lite/build/kp920_lite_rsc.ld：
```
MEMORY
{
    IMU_SRAM (rwx) : ORIGIN = 0x62000000, LENGTH = 0x800000
    MMU_MEM (rwx) : ORIGIN = 0x62800000, LENGTH = 0x800000
}
```
第一个实例在默认预留的CPU 3上运行，因此第二个实例需要在CPU 2上运行，修改prt_config.h：
```
#define OS_SYS_CORE_PRIMARY                             2
```
执行步骤1中编译命令，生成kp920_lite.elf，并将其重命名为kp920_lite_bak.elf作为第二个实例。

### 4.2 多实例镜像配置文件设置

1) 镜像kp920_lite.elf对应的mica conf文件设置如下：
```
[Mica]
Name=uniproton-kp920l
CPU=3
ClientPath=/mcs/kp920_lite.elf
AutoBoot=no
```

2) kp920_lite_bak.elf对应conf设置为：
```
[Mica]
Name=uniproton-kp920l-bak
CPU=2
ClientPath=/mcs/kp920_lite_bak.elf
AutoBoot=no
```

### 4.3 多实例镜像启动

1) 与SMP多核运行相同，需先将CPU 2手动下线。

2) 基于4.2中配置的conf文件，分别对两个镜像执行实例创建与实例启动，拉起完成后，通过mica status查看实例运行情况，并分别进入对应的tty终端，查看shell界面打印是否正常。

## 5 SMP多核与多实例共同部署

SMP多核与多实例部署可同时设置，其中，UniProton_SMP_test.elf仍运行在CPU 2、CPU 3上，kp920_lite_bak.elf则运行在CPU 1上，在编译kp920_lite_bak.elf时需修改prt_config.h，将OS_SYS_CORE_PRIMARY改为1，同时修改对应conf文件，将其中配置CPU值设置为1。部署完成后，可同时启动SMP多核镜像和多实例镜像，并查看各实例运行情况。