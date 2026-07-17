# Uniproton 多核SMP使用指南

## 1 整体方案

在多核场景下，可通过SMP机制支持UniProton的多核运行，使同一OS实例的多个任务同时在不同核上同时处理。以上特性可显著提升系统的实时性能与并行性能，应用场景广泛。

## 2 开发环境说明

- 开发平台：armv8
- 芯片型号：hi3095
- OS版本信息：UniProton master
- 集成开发环境：UniProton-docker

## 3 多核SMP部署

### 3.1 SMP镜像编译

1) 以hi3095平台为例，修改开关配置文件build/uniproton_config/config_armv8_kp920_lite/defconfig：
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

2) 修改配置文件demos/hi3095/config/prt_config.h
```
/* 实时系统实际运行的核数，单位：个 */
#define OS_SYS_CORE_RUN_NUM                             2
/* 最大可支持的核数，单位：个 */
#define OS_SYS_CORE_MAX_NUM                             4
/* 主核ID */
#define OS_SYS_CORE_PRIMARY                             2
```
以上配置项设置了SMP运行的主核为CPU 2，系统的模块注册和初始化操作都是在主核上完成。除主核外，配置了从核CPU 3上的系统任务运行。

3) 可通过以下命令，在demos/hi3095/build目录下编译SMP测试用例，生成用例镜像UniProton_SMP_test.elf。
```
sh build_app.sh UniProton_SMP_test
```

### 3.2 SMP镜像部署运行

1) 以hi3095为例，CPU 3在欧拉侧已预留，只需要将CPU 2下线即可：
```
echo 0 > /sys/bus/cpu/devices/cpu2/online
```

2) 设置mica conf文件：
```
[Mica]
Name=uniproton-hi3095-smp
CPU=2
ClientPath=/mcs/UniProton_SMP_test.elf
AutoBoot=no
```

3) 基于上述conf文件配置，将UniProton_SMP_test.elf使用mica拉起，成功后进入tty界面，便可查看多核任务打印情况。

### 3.3 SMP多核任务设置

在以上测试用例中，实现了SMP多核任务的设置。与单核任务创建不同，SMP多核在使用PRT_TaskCreate接口创建任务后，还需要调用PRT_TaskCoreBind接口将任务与指定从核绑定，以实现多核任务同时运行。相关功能测试代码在demos/hi3095/apps/openamp/main.c中的smp_test函数中实现。
