#include "wifi.h"
#include "accesspoint.h"
#include "clientnode.h"
#include "timer.h"

#include <chrono>
#include <thread>
#include <vector>
using std::vector;

wifi::wifi(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);
    connect(ui.pbSimulate, &QPushButton::clicked, this, &wifi::simulate);
    textConsole = ui.textConsole; 
}

wifi::~wifi()
{}

void wifi::simulate()
{
    srand(time(NULL));

    // Create an Access Point
    AccessPoint ap;

    vector<ClientNode> clients;
    // Initialize client nodes
    for (int i = 0; i < NUM_CLIENTS; ++i) {
        textConsole->append("client added");
        clients.push_back(ClientNode(i, &ap));
    }

    // Simulation loop
    Timer timer;
    while (true) {
        // Update AP about channel status
        ap.senseChannel(ap.isChannelBusy());

        // AP tries to send ACK
        ap.tryToSend();

        // Client nodes try to send data
        for (ClientNode& client : clients) {
            int packetSize = rand() % 100 + 1; // Generate random packet size (1 to 100 bytes)
            client.tryToSend(packetSize);
        }

        // Check for timeouts
        if (timer.hasElapsed(TIMEOUT_DURATION)) {
            textConsole->append("Timeout occurred. Resetting timer.");
            timer.reset();
        }

        textConsole->append("--------------------------------------------");

        // Sleep for some time to simulate real-time behavior
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
