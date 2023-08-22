project_name="Mobile_Car_G474"

# rm -rf cmake-build-debug
mkdir -p cmake-build-debug
cd cmake-build-debug

cmake ..

make
export openocd_scripts=$OPENOCD_HOME/openocd/scripts


openocd \
    -f $openocd_scripts/interface/stlink.cfg \
    -f $openocd_scripts/target/stm32g4x.cfg \
    -c init \
    -c "reset halt; wait_halt; flash write_image erase ${project_name}.bin 0x08000000;" \
    -c "reset run; shutdown"
