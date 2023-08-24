import struct
import time
import serial
import serial.tools.list_ports
import numpy as np


def send(x: float, y: float, z: float):
    # float 转化为 小端4 byte
    x_bytes = struct.pack('b', int(x * 100))
    y_bytes = struct.pack('b', int(y * 100))
    z_bytes = struct.pack('f', float(z))
    # print("x =", x, ":", " ".join(["%02X" % d for d in x_bytes]))
    # print("y =", y, ":", " ".join(["%02X" % d for d in y_bytes]))
    # print("z =", z, ":", " ".join(["%02X" % d for d in z_bytes]))
    data_send = [0x00] * 18
    data_send[0] = 0xAA  # header 1
    data_send[1] = 0x55  # header 2
    data_send[2] = 0x11  # 数据类型标记
    data_send[3] = 0x00  # 保留
    data_send[4] = x_bytes[0]
    data_send[5] = y_bytes[0]
    data_send[6] = z_bytes[0]
    data_send[7] = z_bytes[1]
    data_send[8] = z_bytes[2]
    data_send[9] = z_bytes[3]
    data_send[-2] = sum(data_send[2:-2]) & 0xFF  # checksum
    data_send[-1] = 0xFF  # tail
    return data_send


data_send = send(0.1, 0, 0)
data_send_hex = " ".join(["%02X" % d for d in data_send])
print(data_send_hex)

data_send = send(0, 0.1, 0)
data_send_hex = " ".join(["%02X" % d for d in data_send])
print(data_send_hex)

data_send = send(0, 0, 0.1)
data_send_hex = " ".join(["%02X" % d for d in data_send])
print(data_send_hex)

# exit()

# 获取所有串口设备实例。
# 如果没找到串口设备，则输出：“无串口设备。”
# 如果找到串口设备，则依次输出每个设备对应的串口号和描述信息。
# ports_list = list(serial.tools.list_ports.comports())
# if len(ports_list) <= 0:
#     print("无串口设备。")
# else:
#     print("可用的串口设备如下：")
#     for comport in ports_list:
#         print(list(comport)[0], list(comport)[1])
"""
cu.usbmodem11303
cu.usbserial-1110
tty.usbmodem11303
tty.usbserial-1110
"""
ser_base = "/dev/cu.usbserial"

ser = None
for comport in list(serial.tools.list_ports.comports()):
    port = list(comport)[0]
    print(ser_base, port)
    if len(ser_base) < len(port):
        if ser_base == port[:len(ser_base)]:
            print("find port:", port)
            # ser = serial.Serial(
            #     port,
            #     baudrate=460800,
            #     bytesize=serial.EIGHTBITS,  # 数据位
            #     parity=serial.PARITY_NONE,  # 奇偶校验
            #     stopbits=serial.STOPBITS_ONE,  # 停止位
            #     timeout=0.5  # 读超时设置
            # )
            serR = serial.Serial(
                port,
                baudrate=460800,
                bytesize=serial.EIGHTBITS,  # 数据位
                parity=serial.PARITY_NONE,  # 奇偶校验
                stopbits=serial.STOPBITS_ONE,  # 停止位
                timeout=0.5  # 读超时设置
            )
if serR is None:
    print("no port found")
    exit(0)

data_receive = [0] * 20
cnt_ = 0
state = 0  # 收到 AA 时，state=1，收到 55 时，state=2
last_time = time.time()
while True:

    # 获取当前时间
    now_time = time.time()
    now_time_str = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(now_time))

    # # 接受串口数据
    # data = ser.read(1)
    # if data is not None and len(data) > 0:
    #     u8_data = struct.unpack('B', data)[0]
    #     # print(f"[{now_time_str}]", hex(u8_data), u8_data)
    #     if state == 0 and u8_data == 0xAA:
    #         state = 1
    #     elif state == 1 and u8_data == 0x55:
    #         state = 2
    #     elif state == 2:
    #         if cnt_ >= len(data_receive):
    #             cnt_ = 0
    #         data_receive[cnt_] = u8_data
    #         cnt_ += 1
    #         if u8_data == 0xFF:
    #             checksum_indx = cnt_ - 2
    #             checksum = 0
    #             # for i in range(checksum_indx):
    #             #     checksum += data_receive[i]
    #             # if checksum == data_receive[checksum_indx]:
    #             data_receive_hex = [hex(0xAA), hex(0x55)
    #                                 ] + [hex(i) for i in data_receive[:cnt_]]
    #             print(f"[{now_time_str}][收到]",
    #                   " ".join(["%02X" % d for d in data_receive_hex]))
    #             state = 0
    #             cnt_ = 0
    #     else:
    #         state = 0
    #         cnt_ = 0

    # 发送串口数据
    if now_time - last_time > 1:
        last_time = now_time
        u8_data_send = struct.pack('B' * len(data_send), *data_send)
        serR.write(u8_data_send)
        print(f"[{now_time_str}][发送]",
              " ".join(["%02X" % d for d in u8_data_send]),
              )
        print(f"[{now_time_str}][D_R ]",
              " ".join(["%02X" % d for d in data_receive]))
        print()
ser.close()
serR.close()