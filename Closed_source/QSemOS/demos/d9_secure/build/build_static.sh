git clone https://gitee.com/openeuler/libboundscheck.git

cp libboundscheck/include/* ../../../platform/libboundscheck/include
cp libboundscheck/src/* ../../../platform/libboundscheck/src
rm -rf libboundscheck

CPU_TYPE=$1
BOARD_PATH=$2

pushd ./../../../
python build.py $CPU_TYPE

INC_DIR=$BOARD_PATH/include
LIB_DIR=$BOARD_PATH/libs

[ -d $LIB_DIR ] || mkdir $LIB_DIR
cp output/UniProton/lib/$CPU_TYPE/*.a $LIB_DIR
cp output/libboundscheck/lib/$CPU_TYPE/*.lib $LIB_DIR

[ -d $INC_DIR ] || mkdir $INC_DIR
cp output/libboundscheck/lib/$CPU_TYPE/*.h $INC_DIR
cp -r src/include/uapi/* $INC_DIR
cp build/uniproton_config/config_cortex_r5_$CPU_TYPE/prt_buildef.h $INC_DIR/

popd
