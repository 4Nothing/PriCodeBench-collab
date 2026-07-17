coredump
===============================

===============================
1. 配置选项
===============================

目前支持armv8,armv7-r两种架构，需在bsp的deconfig文件中开启 ``CONFIG_OS_OPTION_COREDUMP``

===============================
2. 裸板下coredump
===============================

程序崩溃后会打印coredump的日志格式如下：

.. code-block:: 

    #CD:BEGIN#
    #CD:5a4501000500050003000000
    #CD:4102004400
    #CD:0000000035041400350414000000000000000000450514004405140073010060
    #CD:e8161b00c0491b0099b214000606060605050505040404040303030302020202
    #CD:01010101
    ...
    #CD:END#

需将这段日志保存到coredump.log中，后续会使用到

===============================
1. coredump脚本使用方法
===============================

在UniProton的src/component/coredump/scripts目录下的coredump.sh脚本用于gdb调试coredump，用法如下

直接命令行调试：

.. code-block::

    ./coredump.sh <coredump_elf_file> <coredump_log_file>


gdb远程调试：

.. code-block::

    ./coredump.sh <coredump_elf_file> <coredump_log_file> <port_num>

coredump_elf_file：
    需要调试的elf文件，通常为编译后的uniproton.elf文件

coredump_log_file
    需要调试的coredump文件，通常为手动保存coredump.log

port_num：
    可选参数，远程调试时的端口号，gdb远程调试需指定某一端口，IP为本机IP
    不指定端口号时，即为直接命令行调试

===============================
1. coredump举例
===============================

将coredump.log文件拷贝到本地，运行coredump.sh脚本进行调试

直接命令行调试：

.. code-block:: 

    $ ./coredump.sh uniproton.elf coredump.log
    gdb server start..
    GNU gdb (Ubuntu 12.1-0ubuntu1~22.04.2) 12.1
    Copyright (C) 2022 Free Software Foundation, Inc.
    License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
    This is free software: you are free to change and redistribute it.
    There is NO WARRANTY, to the extent permitted by law.
    Type "show copying" and "show warranty" for details.
    This GDB was configured as "x86_64-linux-gnu".
    Type "show configuration" for configuration details.
    For bug reporting instructions, please see:
    <https://www.gnu.org/software/gdb/bugs/>.
    Find the GDB manual and other documentation resources online at:
        <http://www.gnu.org/software/gdb/documentation/>.

    For help, type "help".
    Type "apropos word" to search for commands related to "word"...
    Reading symbols from uniproton.elf...
    warning: File "/home/user/resp/uniproton/scripts/coredump/.gdbinit" auto-loading has been declined by your `auto-load safe-path' set to "$debugdir:$datadir/auto-load".
    To enable execution of this file add
            add-auto-load-safe-path /home/user/resp/uniproton/scripts/coredump/.gdbinit
    line to your configuration file "/home/user/.config/gdb/gdbinit".
    To completely disable this security protection add
            set auto-load safe-path /
    --Type <RET> for more, q to quit, c to continue without paging--
    line to your configuration file "/home/user/.config/gdb/gdbinit".
    For more information about this security protection see the
    "Auto-loading safe path" section in the GDB manual.  E.g., run from the shell:
            info "(gdb)Auto-loading safe path"
    timer_demo () at /home/user/resp/uniproton/demos/common/demo_timer.c:31
    31          int c = *(int*)(0);
    (gdb) bt
    #0  timer_demo () at /home/user/resp/uniproton/demos/common/demo_timer.c:31
    #1  0x00000000c000e274 in apps_entry () at /home/user/resp/uniproton/bsp/phytium/gsk-e2000q/apps/main.c:71
    #2  0x00000000c00219b0 in osmain_entry (param1=0, param2=0, param3=0, param4=0) at /home/user/resp/uniproton/bsp/phytium/gsk-e2000q/src/osmain.c:38
    #3  0x00000000c0090948 in OsTskEntry ()
    #4  0x00000000c0090928 in ?? ()
    Backtrace stopped: not enough registers or memory available to unwind further
    (gdb) 

IDE远程调试：

.. code-block:: 

    $ ./coredump.sh uniproton.elf coredump.log 3456                                                                                                   [6:38:34]
    gdb server start.., wait for connect on port: 3456

然后在IDE或者gdb客户端中进行连接：

.. code-block:: 

    $ gdb-multiarch uniproton.elf                                                                                           [6:03:46]
    GNU gdb (Ubuntu 12.1-0ubuntu1~22.04.2) 12.1
    Copyright (C) 2022 Free Software Foundation, Inc.
    License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
    This is free software: you are free to change and redistribute it.
    There is NO WARRANTY, to the extent permitted by law.
    Type "show copying" and "show warranty" for details.
    This GDB was configured as "x86_64-linux-gnu".
    Type "show configuration" for configuration details.
    For bug reporting instructions, please see:
    <https://www.gnu.org/software/gdb/bugs/>.
    Find the GDB manual and other documentation resources online at:
        <http://www.gnu.org/software/gdb/documentation/>.

    For help, type "help".
    Type "apropos word" to search for commands related to "word"...
    Reading symbols from uniproton.elf...
    (gdb) target remote :3456
    Remote debugging using :3456
    timer_demo () at /home/user/resp/uniproton/demos/common/demo_timer.c:31
    31          int c = *(int*)(0);
    (gdb) bt
    #0  timer_demo () at /home/user/resp/uniproton/demos/common/demo_timer.c:31
    #1  0x00000000c000e274 in apps_entry () at /home/user/resp/uniproton/bsp/phytium/gsk-e2000q/apps/main.c:71
    #2  0x00000000c00219b0 in osmain_entry (param1=0, param2=0, param3=0, param4=0)
        at /home/user/resp/uniproton/bsp/phytium/gsk-e2000q/src/osmain.c:38
    #3  0x00000000c0090948 in OsTskEntry ()
    #4  0x00000000c0090928 in ?? ()
    Backtrace stopped: not enough registers or memory available to unwind further
    (gdb)
