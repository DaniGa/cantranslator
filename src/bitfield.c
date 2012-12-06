#include "bitfield.h"

/**
 * Find the ending bit of a bitfield within the final byte.
 *
 * Returns: a bit position from 0 to 7.
 */
int findEndBit(int startBit, int numBits) {
    int endBit = (startBit + numBits) % 8;
    return endBit == 0 ? 8 : endBit;
}

uint64_t bitmask(int numBits) {
    return (0x1 << numBits) - 1;
}

uint64_t reverseBitmaskVariableLength(int numBits, int totalLength) {
    uint64_t mask = bitmask(numBits);
    return mask << (totalLength - numBits);
}

uint64_t reverseBitmask(int numBits) {
    return reverseBitmaskVariableLength(numBits, 64);
}


int startingByte(int startBit) {
    return startBit / 8;
}

int endingByte(int startBit, int numBits) {
    return (startBit + numBits - 1) / 8;
}

uint64_t getBitField(uint64_t data, int startBit, int numBits) {
    int startByte = startingByte(startBit);
    int endByte = endingByte(startBit, numBits);

    uint8_t* bytes = (uint8_t*)&data;
    uint64_t ret = bytes[startByte];
    if(startByte != endByte) {
        // The lowest byte address contains the most significant bit.
        int i;
        for(i = startByte + 1; i <= endByte; i++) {
            ret = ret << 8;
            ret = ret | bytes[i];
        }
    }

    ret >>= 8 - findEndBit(startBit, numBits);
    return ret & bitmask(numBits);
}

/**
 * TODO it would be nice to have a warning if you call with this a value that
 * won't fit in the number of bits you've specified it should use.
 */
void setBitField(uint64_t* data, uint64_t value, int startBit, int numBits) {
    int shiftDistance = 64 - startBit - numBits;
    value <<= shiftDistance;
    *data &= ~(bitmask(numBits) << shiftDistance);
    *data |= value;
}

uint8_t nthByte(uint64_t source, int byteNum) {
    return (source >> (sizeof(uint64_t) -
                ((byteNum + 1) * sizeof(uint8_t)))) & 0xFF;
}
