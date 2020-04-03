#undef DEBUG

#include "iBUS.h"

extern unsigned int channels[iBUSNumberOfSignals];
extern unsigned int numberOfSamplesOK;
extern unsigned int numberOfSamplesNOK;
extern unsigned int numberOfBadSignal;
extern unsigned int crcMismatches;

void setup()
{
    Serial.begin(115200);
}

static unsigned long scheduler = 0;
static unsigned long elapsedProcessingTime = 0;

void loop()
{
    unsigned long timestamp = millis();

    unsigned long elapsedTimestamp1 = micros();
    int rc = iBUS();
    if (rc == 0)
        numberOfSamplesOK++;
    else if (rc == -1)
        numberOfSamplesNOK++;
    else
        numberOfBadSignal++;
    unsigned long elapsedTimestamp2 = micros();
    elapsedProcessingTime += elapsedTimestamp2 - elapsedTimestamp1;

    if (timestamp > scheduler)
    {
        scheduler = timestamp + 1000;
        Serial.print("Elapsed(us): ");
        Serial.println(elapsedProcessingTime);
        Serial.print("Samples(OK/NOK/BAD): ");
        Serial.print(numberOfSamplesOK);
        Serial.print("/");
        Serial.print(numberOfSamplesNOK);
        Serial.print("/");
        Serial.println(numberOfBadSignal);
        Serial.print("CRC mismatch: ");
        Serial.println(crcMismatches);

        elapsedProcessingTime = 0;
        numberOfSamplesOK = 0;
        numberOfSamplesNOK = 0;
        for (byte signalNumber = 0; signalNumber < iBUSNumberOfSignals; signalNumber++)
        {
            Serial.print("CH");
            Serial.print(signalNumber + 1);
            Serial.print(": ");
            Serial.println(channels[signalNumber]);
        }
        Serial.println();
    }

    delay(25);
}
