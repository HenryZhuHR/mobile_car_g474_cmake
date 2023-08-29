project_name="Mobile_Car_G474"


rm -rf build

if [ ! -d build ]; then
    mkdir -p build
fi
cd build


CMAKE_ARGS=""
NUM_CORES=`nproc --all`

if [ "$(uname)" = "Darwin" ]; then
    NUM_CORES=`sysctl -n hw.ncpu`
fi

if [ "$(expr substr $(uname -s) 1 5)" == "MINGW" ]; then
    cmake .. -G "MinGW Makefiles"
else
    cmake .. $CMAKE_ARGS 
fi

make -j$NUM_CORES


export openocd_scripts=$OPENOCD_HOME/openocd/scripts


openocd \
    -f $openocd_scripts/interface/stlink.cfg \
    -f $openocd_scripts/target/stm32g4x.cfg \
    -c init \
    -c "reset halt; wait_halt; flash write_image erase ${project_name}.bin 0x08000000;" \
    -c "reset run; shutdown"
