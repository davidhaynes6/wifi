#pragma once
#include <iostream>

using std::cout;
using std::endl;

// Constants
const int NUM_CLIENTS = 5;
const int MAX_BACKOFF = 10;
const int MAX_RETRIES = 3;
const int TIMEOUT_DURATION = 5; // in seconds

class AccessPoint {
private:
    bool channelBusy;
    int backoffCounter;
public:
    AccessPoint() : channelBusy(false), backoffCounter(0) {}

    void sendData(int packetSize) {
        cout << "Access Point received data packet of size " << packetSize << " bytes from a client." << endl;
    }

    void acknowledge() {
        cout << "Access Point acknowledges the receipt of data." << endl;
    }

    void tryToSend() {
        if (!channelBusy && backoffCounter == 0) {
            cout << "Access Point is sending ACK." << endl;
            acknowledge();
            // Reset backoff counter
            backoffCounter = 0;
        }
        else if (backoffCounter > 0) {
            cout << "Access Point is waiting due to backoff." << endl;
            backoffCounter--;
        }
    }

    bool isChannelBusy() const {
        return channelBusy;
    }

    void senseChannel(bool busy) {
        channelBusy = busy;
    }

    void generateBackoff() {
        backoffCounter = rand() % MAX_BACKOFF + 1;
    }
};