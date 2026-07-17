BUILD_PATH=$(pwd)
HOME_PATH=$(pwd)/../../..

echo "Delete all files (*.asm *.bin *.elf) in "$BUILD_PATH" ..."
rm -f "$BUILD_PATH"/*.asm
rm -f "$BUILD_PATH"/*.bin
rm -f "$BUILD_PATH"/*.elf
rm -rf "$BUILD_PATH"/uniproton/

echo "Delete the build directories ..."
rm -rf "$BUILD_PATH"/bsp
rm -rf "$BUILD_PATH"/apps
rm -rf "$BUILD_PATH"/components

echo "Delete "$BUILD_PATH"/libs/ file ..."
rm -f "$BUILD_PATH"/../libs/*

echo "Delete "$BUILD_PATH"/include directories ..."
rm -f "$BUILD_PATH"/../include/* -rf

echo "Delete "$HOME_PATH"/build directories ..."
rm -rf "$HOME_PATH"/build/logs
rm -rf "$HOME_PATH"/build/output
rm -rf "$HOME_PATH"/build/uniproton_config/*/prt_buildef.h

echo "Delete "$HOME_PATH"/output directories ..."
rm -rf "$HOME_PATH"/output

echo "Cleanup is complete!"
