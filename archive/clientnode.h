#pragma once
#include "accesspoint.h"

class ClientNode {
private:
    int id;
    AccessPoint* ap;
public:
    ClientNode(int _id, AccessPoint* _ap) : id(_id), ap(_ap) {}

    void sendData(int packetSize) {
        cout << "Client Node " << id << " is sending data packet of size " << packetSize << " bytes to the Access Point." << endl;
        ap->sendData(packetSize);
    }

    void receiveAck() {
        cout << "Client Node " << id << " received ACK from the Access Point." << endl;
    }

    void tryToSend(int packetSize) {
        if (ap->isChannelBusy()) {
            cout << "Client Node " << id << " detected channel busy. Waiting..." << endl;
            return;
        }

        cout << "Client Node " << id << " is requesting to send data." << endl;
        sendData(packetSize);
    }
};