uint8_t dataPin = 2;

#define DATA_LENGTH 59

// Data for tests:
// bool data[DATA_LENGTH] = { 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0 };
// Should output 9:31 23.12.18

void setup() {
    Serial.begin(9600);
    pinMode(dataPin, INPUT);
}

void loop() {
    Serial.println("Waiting for minute start");
    bool data[DATA_LENGTH];
    bool syncSucc = syncTime(data);
    if (!syncSucc) {
        Serial.println("Time sync error");
        return;
    }
    Serial.print("Got time:");
    printData(data);
    Serial.println();
    bool chkSucc = checkTime(data);
    if (!chkSucc) {
        Serial.println("Parity error");
        return;
    }
    convertTime(data);
}

// Print the data array
void printData(bool * data) {
    for (uint8_t i = 0; i < DATA_LENGTH; i++) {
        Serial.print(data[i]);
    }
}

int getTime() {

}

bool syncTime(bool * data) {
    bool waszero = false;
    // Wait for 2 long pauses -> start of minute
    while (true) {
        unsigned long pulseLength = pulseIn(dataPin, HIGH) / 1000;
        if (pulseLength == 0) {
            if (waszero) {
                Serial.println("Minute detected");
                break;
            }
            waszero = true;
        } else {
            waszero = false;
        }
    }
    emptyData(data);
    uint8_t index = 0;
    // Fill in time data
    while (true) {
        unsigned long pulseLength = pulseIn(dataPin, HIGH) / 1000;
        if (pulseLength != 0) {
            if (pulseLength < 80 || pulseLength > 280) {
                Serial.print("Error: Pulse: ");
                Serial.println(pulseLength);
                return false;
            }
            // 100ms pulse -> zero, 200ms pulse -> true
            data[index] = pulseLength > 150 ? true : false;
            index++;
            // Return when data for one minute has been filled
            if (index > 58) return true;
        }
    }
}

// Zero the data array
void emptyData(bool * data) {
    for (uint8_t i = 0; i < DATA_LENGTH; i++) {
        data[i] = 0;
    }
}

// Check parity
bool checkTime(bool * data) {
    bool minpar = checkParity(data, 21, 27);
    bool hrpar = checkParity(data, 29, 34);
    bool dpar = checkParity(data, 36, 57);

    if (minpar && hrpar && dpar) return true;
    return false;
}

// Check the even parity from index "from" to index "to", the parity is in "to" + 1
bool checkParity(bool * data, uint8_t from, uint8_t to) {
    uint8_t count = 0;
    for (uint8_t i = from; i <= to; i++) {
        if (data[i] == 1) count++;
    }
    bool isEven = count % 2 == 0;
    if (!data[to + 1] == isEven) return true;
    return false;
}

void convertTime(bool * data) {
    uint8_t minute = BCDtoInt(data, 21, 24, 25, 27);
    uint8_t hour = BCDtoInt(data, 29, 32, 33, 34);
    uint8_t day = BCDtoInt(data, 36, 39, 40, 41);
    uint8_t month = BCDtoInt(data, 45, 48, 49, 49);
    uint8_t year = BCDtoInt(data, 50, 53, 54, 57);
    Serial.print(hour);
    Serial.print(":");
    Serial.print(minute);
    Serial.print(" ");
    Serial.print(day);
    Serial.print(".");
    Serial.print(month);
    Serial.print(".");
    Serial.println(year);
}

// Convert a bcd number to an integer where
// the ones-place starts at "onesFrom" and ends as "onesTo"
// and the tens-place starts at "tensFrom" and ends at "tensTo"
uint8_t BCDtoInt(bool * data, uint8_t onesFrom, uint8_t onesTo, uint8_t tensFrom, uint8_t tensTo) {
    uint8_t value = 0;
    for (uint8_t i = onesFrom; i <= onesTo; i++) {
        uint8_t amount = pow2(i - onesFrom);
        value += data[i] * amount;
    }
    for (uint8_t i = tensFrom; i <= tensTo; i++) {
        uint8_t amount = pow2(i - tensFrom) * 10;
        value += data[i] * amount;
    }
    return value;
}

// Calculate 2 ^ e (e = Exponent)
int pow2(int e) {
    if (e == 0) return 1;
    return 2 * pow2(e - 1);
}