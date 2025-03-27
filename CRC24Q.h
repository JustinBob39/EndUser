#ifndef CRC24_H
#define CRC24_H

#define LO(x)	(unsigned char)((x) & 0xff)
#define MID(x)	(unsigned char)(((x) >> 8) & 0xff)
#define HI(x)	(unsigned char)(((x) >> 16) & 0xff)

class CRC24Q {
    static const unsigned int crc24q[256];

public:
    static unsigned int crc24q_hash(const unsigned char *data, int len);

    static bool crc24q_check(const unsigned char *data, int len);
};

#endif //CRC24_H
