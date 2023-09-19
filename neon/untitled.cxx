/*
 * untitled.cxx
 * 
 * Copyright 2023  <yxs@raspberrypi>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <iostream>
#include <arm_neon.h>
#include <cstdint>
#include <cstring>
#include <cstdio>

uint8_t* neon_SHA256(const char* str, long long length, uint8_t* sha256) {
    // 初始化哈希值
    uint32_t hash[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };

    // 数据填充
    const int block_size = 64;
    const int max_blocks = 16;  // 最大块数
    uint8_t padded_data[block_size * max_blocks];
    std::memset(padded_data, 0, sizeof(padded_data));
    std::memcpy(padded_data, str, length);

    // 计算填充后的总位数
    uint64_t total_bits = length * 8;
    int padding_bytes = block_size - (length % block_size);
    if (padding_bytes < 8) {
        padding_bytes += block_size;
    }
    total_bits += padding_bytes * 8;

    // 在填充数据的末尾添加一个 1 比特位
    padded_data[length] = 0x80;

    // 在填充数据的末尾添加消息长度（以比特位表示）
    for (int i = 0; i < 8; i++) {
        padded_data[block_size * max_blocks - 8 + i] = (total_bits >> (56 - i * 8)) & 0xFF;
    }

    // 处理数据块和更新哈希值
    uint32_t w[64];
    for (int i = 0; i < max_blocks; i++) {
        const uint8_t* block = padded_data + i * block_size;

        // 将数据块转换为 uint32x4x4_t 类型
        uint32x4x4_t data;
        data.val[0] = vld1q_u32(reinterpret_cast<const uint32_t*>(block));
        data.val[1] = vld1q_u32(reinterpret_cast<const uint32_t*>(block + 16));
        data.val[2] = vld1q_u32(reinterpret_cast<const uint32_t*>(block + 32));
        data.val[3] = vld1q_u32(reinterpret_cast<const uint32_t*>(block + 48));

        // 将数据块的字节顺序转换为大端顺序
        data.val[0] = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(data.val[0])));
        data.val[1] = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(data.val[1])));
        data.val[2] = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(data.val[2])));
        data.val[3] = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(data.val[3])));

        // TODO: 使用 NEON 指令进行数据块的处理和哈希值的更新
        

        // 示例：将处理后的哈希值拷贝到输出数组
        std::memcpy(sha256, hash, 32);
    }

    return sha256;
}
int main(int argc, char **argv)
{
	
	return 0;
}

