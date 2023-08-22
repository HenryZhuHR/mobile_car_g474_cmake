project_name="mobile_robot_control"

export openocd_scripts=$OPENOCD_HOME/openocd/scripts


arm-none-eabi-gdb

openocd \
    -f $openocd_scripts/interface/stlink.cfg \
    -f $openocd_scripts/target/stm32g4x.cfg