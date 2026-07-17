###################### build boards ######################
export TOOLCHAIN_PATH=/opt/buildtools/gcc-arm-none-eabi-10-2020-q4-major
export TOOLCHAIN_PREFIX=arm-none-eabi

SCRIPT_DIR=$(dirname $(realpath $0))
BOARD_PATH=$(basename $(dirname $SCRIPT_DIR))
BOARD_PATH=$(basename $(dirname $(dirname $SCRIPT_DIR)))/$BOARD_PATH
export BOARD_PATH

export CPU_TYPE=d9_secure
export APP_NAME=uniproton

# export ALL="task-switch task-preempt semaphore-shuffle interrupt-latency deadlock-break message-latency"
export ALL=$APP_NAME

cp ../config/defconfig ../../../build/uniproton_config/config_cortex_r5_$CPU_TYPE
sh ./build_static.sh $CPU_TYPE $BOARD_PATH

cmake -S .. -B $APP_NAME \
        -DAPP:STRING=$APP_NAME \
        -DTOOLCHAIN_PATH:STRING=$TOOLCHAIN_PATH \
        -DCPU_TYPE:STRING=$CPU_TYPE \
        -DTOOLCHAIN_PREFIX:STRING=$TOOLCHAIN_PREFIX \

pushd $APP_NAME && {
    make $APP_NAME || exit 1
} && popd

if [ -f $APP_NAME/$APP_NAME ]; then
    cp "$APP_NAME/$APP_NAME" "$SCRIPT_DIR/$APP_NAME.elf"
    $TOOLCHAIN_PATH/bin/$TOOLCHAIN_PREFIX-objcopy -O binary "$SCRIPT_DIR/$APP_NAME.elf" "$SCRIPT_DIR/$APP_NAME.bin"
    $TOOLCHAIN_PATH/bin/$TOOLCHAIN_PREFIX-objdump -D "$SCRIPT_DIR/$APP_NAME.elf" > "$SCRIPT_DIR/$APP_NAME.asm"
fi
