import struct

float_list = [
    0x0c,
    0x2f,
    0xb6,
    0x42,
]
assert len(float_list) == 4

# 将 4 byte 转化为 float
float_num = struct.unpack("f", struct.pack("B" * 4, *float_list))[0]
print(float_num)