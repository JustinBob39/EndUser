#include "test.h"

#include <string>
#include <cassert>
#include <iostream>

#include "CRC24Q.h"
#include "Deserializer.h"

void CRC24QTest() {
    // https://emn178.github.io/online-tools/crc/
    // choose model CRC-24/LTE-A
    unsigned char str[14] = "Hello World";
    str[11] = 0x12;
    str[12] = 0x93;
    str[13] = 0x7c;
    const bool result = CRC24Q::crc24q_check(str, 14);
    assert(result);
}

void DeserializerSingleTest() {
    const char *frame =
            "\xD3\x00\x23\x00\xA0\x13\x02\x10\x00\x00\x13\x02\x10\x00\x00\x60"
            "\x73\x9F\x8E\x08\xC0\x84\x00\x00\x18\x1C\xE7\xE3\xA3\x30\x21\x00"
            "\x00\x06\x07\x39\xF8\xF0\xDD\xA4\x68";
    constexpr int frame_size = 41;
    Deserializer deserializer = {};
    const auto entries = deserializer.transform(frame, frame_size);
    for (const auto &entry: entries) {
        std::cout << entry.to_string() << std::endl;
    }
}

void DeserializerGroupTest() {
    const char *frame =
            "\xD3\x00\x65\x00\xB0\x13\x02\x10\x00\x00\x13\x02\x10\x00\x00\x60"
            "\x73\x9F\x8E\x08\xC0\x84\x00\x00\x18\x1C\xE7\xE3\xA3\x30\x21\x00"
            "\x00\x06\x07\x39\xF8\xF0\x11\x81\x08\x00\x00\x09\x81\x08\x00\x00"
            "\x30\x39\xCF\xC7\x04\x60\x42\x00\x00\x0C\x0E\x73\xF1\xD1\x98\x10"
            "\x80\x00\x03\x03\x9C\xFC\x78\x0C\xC0\x84\x00\x00\x04\xC0\x84\x00"
            "\x00\x18\x1C\xE7\xE3\x82\x30\x21\x00\x00\x06\x07\x39\xF8\xE8\xCC"
            "\x08\x40\x00\x01\x81\xCE\x7E\x3C\x95\x9E\x11";
    constexpr int frame_size = 107;
    Deserializer deserializer = {};
    const auto entries = deserializer.transform(frame, frame_size);
    for (const auto &entry: entries) {
        std::cout << entry.to_string() << std::endl;
    }
}


int main() {
    CRC24QTest();
    DeserializerSingleTest();
    DeserializerGroupTest();
    return 0;
}
