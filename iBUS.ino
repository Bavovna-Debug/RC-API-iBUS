#include "iBUS.h"

static byte iBUSRowData[iBUSPayloadLength];
unsigned int channels[iBUSNumberOfSignals];
unsigned int numberOfSamplesOK;
unsigned int numberOfSamplesNOK;
unsigned int numberOfBadSignal;
unsigned int crcMismatches;

void setupiBUS()
{
    numberOfSamplesOK = 0;
    numberOfSamplesNOK = 0;
    numberOfBadSignal = 0;
    crcMismatches = 0;
}

int iBUS()
{
    while (Serial.available() < iBUSPayloadLength);
    Serial.readBytes(iBUSRowData, iBUSPayloadLength);
    while (Serial.available() > 0) Serial.read();

    if ((iBUSRowData[0] != 0x20) || (iBUSRowData[1] != 0x40))
    {
        return -1;
    }

    for (byte signalNumber = 0; signalNumber < iBUSNumberOfSignals; signalNumber++)
    {
        unsigned int byteoffset = (signalNumber * 2) + sizeof(uint16_t);
        int channelValue = (iBUSRowData[byteoffset + 1] << 8) | iBUSRowData[byteoffset];
        if ((channelValue < 1000) || (channelValue > 2000))
        {
            return -2;
        }
        channels[signalNumber] = channelValue - 1000;
    }

    uint16_t crcProvided;
    uint16_t crcExpected;

    crcProvided = (iBUSRowData[iBUSPayloadLength - 1] << 8) | iBUSRowData[iBUSPayloadLength - 2];

    crcExpected = 0;
    for (byte offset = 0; offset < (iBUSPayloadLength - sizeof(uint16_t)); offset++)
    {
        crcExpected += iBUSRowData[offset];
    }
    crcExpected = 0xFFFF - crcExpected;

    if (crcProvided != crcExpected)
    {
        crcMismatches++;
    }

    return 0;
}
