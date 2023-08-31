import struct
import time
from typing import List
import serial
import serial.tools.list_ports
import numpy as np


"""
ls /dev/tty* /dev/cu.* 
cu.usbmodem11303
cu.usbserial-1110
tty.usbmodem11303
tty.usbserial-1110
"""


class Gyroscope:
    def __init__(self):
        self.x = 0
        self.y = 0
        self.z = 0


class Acceleration:
    def __init__(self):
        self.x = 0
        self.y = 0
        self.z = 0


class Attitude:
    def __init__(self):
        self.roll = 0
        self.pitch = 0
        self.yaw = 0


class Imu6Axis:
    def __init__(self):
        self.gyroscope = Gyroscope()
        self.acceleration = Acceleration()
        self.attitude = Attitude()


def decode_imu(frame: List[int]):
    imu_id = struct.unpack("I", struct.pack("B" * 4, *frame[3:7]))[0]
    data = Imu6Axis()
    data.gyroscope.x = struct.unpack("h", struct.pack("B" * 2, *frame[7:9]))[0]
    data.gyroscope.y = struct.unpack("h", struct.pack("B" * 2, *frame[9:11]))[0]
    data.gyroscope.z = struct.unpack("h", struct.pack("B" * 2, *frame[11:13]))[0]
    data.acceleration.x = struct.unpack("h", struct.pack("B" * 2, *frame[13:15]))[0]
    data.acceleration.y = struct.unpack("h", struct.pack("B" * 2, *frame[15:17]))[0]
    data.acceleration.z = struct.unpack("h", struct.pack("B" * 2, *frame[17:19]))[0]
    data.attitude.roll = struct.unpack("f", struct.pack("B" * 4, *frame[19:23]))[0]
    data.attitude.pitch = struct.unpack("f", struct.pack("B" * 4, *frame[23:27]))[0]
    data.attitude.yaw = struct.unpack("f", struct.pack("B" * 4, *frame[27:31]))[0]
    data_str = " ".join(
        [
            "gyro:[%6s,%6s,%6s]"
            % (data.gyroscope.x, data.gyroscope.y, data.gyroscope.z),
            "acce:[%6s,%6s,%6s]"
            % (data.acceleration.x, data.acceleration.y, data.acceleration.z),
            "atti:[%6s,%6s,%6s]"
            % (
                "%.1f" % data.attitude.roll,
                "%.1f" % data.attitude.pitch,
                "%.1f" % data.attitude.yaw,
            ),
        ]
    )
    return data, data_str


SERIAL_DATA_LEN_MAP = (
    (0x81, "imu", 33, decode_imu),
    # (0x82, "odometer", 21),
)


def main():
    serial_port = "/dev/tty.usbmodem11103"
    baudrate = 115200
    ser = serial.Serial(
        serial_port,
        baudrate=baudrate,
        bytesize=serial.EIGHTBITS,  # 数据位
        parity=serial.PARITY_NONE,  # 奇偶校验
        stopbits=serial.STOPBITS_ONE,  # 停止位
        timeout=10 / 1000,  # 读超时设置
    )

    if ser is None:
        print("no port found")
        return

    try:
        header = [0xAA, 0x55]
        tail = 0xFF
        MAX_BUFFER_LEN = 40
        rbuffer = [0] * MAX_BUFFER_LEN
        rcnt_ = 0
        rstate = 0  # 收到 AA 时，state=1，收到 55 时，state=2
        frame_len = 0
        data_type = ""

        while True:
            serial_buffer = ser.readline()

            if serial_buffer is not None and len(serial_buffer) > 0:
                # print(serial_buffer)
                for u8_data in serial_buffer:
                    if rcnt_ >= MAX_BUFFER_LEN:
                        rcnt_ = 0
                        rstate = 0

                    if rstate == 0 and u8_data == header[rstate]:
                        rbuffer[rcnt_] = u8_data
                        rcnt_ += 1
                        rstate += 1
                    elif rstate == 1 and u8_data == header[rstate]:
                        rbuffer[rcnt_] = u8_data
                        rcnt_ += 1
                        rstate += 1
                    elif rstate == 2:
                        rbuffer[rcnt_] = u8_data
                        rcnt_ += 1
                        rstate += 1
                        frame_len = 0
                        for i in range(len(SERIAL_DATA_LEN_MAP)):
                            if u8_data == SERIAL_DATA_LEN_MAP[i][0]:
                                rstate = 3
                                frame_len = SERIAL_DATA_LEN_MAP[i][2]
                                data_type = SERIAL_DATA_LEN_MAP[i][1]
                                dec_func = SERIAL_DATA_LEN_MAP[i][3]
                                break
                        if frame_len == 0:
                            rstate = 0
                            rcnt_ = 0
                    elif rstate == 3:
                        rbuffer[rcnt_] = u8_data
                        rcnt_ += 1
                        if u8_data == tail and rcnt_ == frame_len:
                            data_frame = rbuffer[:rcnt_]

                            # checksum_indx = cnt_ - 2
                            # checksum = 0
                            # for i in range(2, checksum_indx):
                            #     checksum += buffer[i]
                            # checksum = checksum & 0xFF
                            # if checksum == buffer[checksum_indx]:
                            data_frame = rbuffer[:rcnt_]
                            data, data_str = dec_func(data_frame)
                            now_time_str = time.strftime(
                                "%Y-%m-%d %H:%M:%S", time.localtime(time.time())
                            )
                            print(
                                f"[{now_time_str}][收] [{rcnt_}/{frame_len}] ({data_type})",
                                # " ".join(["%02X" % d for d in data_frame]),
                                f"({int(data.attitude.yaw)})",  # data_str,
                                f"{data_str}"
                            )
                            rstate = 0
                            rcnt_ = 0
                            # ser.flushInput()# 每次读取完数据后清空串口缓存区
                    else:
                        rstate = 0
                        rcnt_ = 0

    except KeyboardInterrupt:
        print("KeyboardInterrupt")

    finally:
        ser.close()


if __name__ == "__main__":
    main()
