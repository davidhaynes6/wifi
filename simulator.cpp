#include "simulator.h"

// Node structure to simulate each device in the network
struct Node {
    int packetSize;
    int backoffTime;
    bool isReadyToTransmit;
    std::shared_ptr<BackoffStrategy> backoffStrategy; // Pointer to backoff strategy

    // Constructor
    Node(std::shared_ptr<BackoffStrategy> strategy) : packetSize(0), backoffTime(0), isReadyToTransmit(false), backoffStrategy(strategy) {}

    // Randomize node behavior
    void randomize(std::mt19937& gen, std::uniform_int_distribution<>& packetSizeDist) {
        packetSize = packetSizeDist(gen);
        isReadyToTransmit = true; // Node is ready to transmit at the start
    }

    // Reset backoff time using the strategy pattern
    void resetBackoffTime(std::mt19937& gen, int collisionCount, int successfulCount) {
        if (backoffStrategy) {
            backoffTime = backoffStrategy->calculateBackoffTime(gen, collisionCount, successfulCount);
        }
    }

    // Simulate node behavior for a time unit
    void simulateTimeUnit() {
        if (backoffTime > 0) {
            backoffTime--;
            isReadyToTransmit = backoffTime == 0;
        }
    }
};


Simulator::Simulator(int numberNodes, std::shared_ptr<BackoffStrategy> backoffStrategy, int minPacketSize, int maxPacketSize, int simulationTime, int numSimulations)
    :numberNodes(numberNodes), minPacketSize(minPacketSize), maxPacketSize(maxPacketSize), simulationTime(simulationTime), numSimulations(numSimulations)
{
}

Transmissions Simulator::simulateCSMACA(int numberNodes, std::shared_ptr<BackoffStrategy> backoffStrategy, int minPacketSize, int maxPacketSize, int simulationTime, int simulations) const
{
    std::normal_distribution<double> distribution(0, 1);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> packetSizeDist(minPacketSize, maxPacketSize); 

    std::vector<Node> nodes(numberNodes, Node(std::move(backoffStrategy))); // Use the strategy for all nodes
    for (auto& node : nodes) {
        node.randomize(gen, packetSizeDist);
    }

    Transmissions transmissions{};
    int successful = 0;
    int collisions = 0;

    // Simulate each time unit
    for (int time = 0; time < simulationTime; ++time) {
        int transmittingNodes = 0;
        std::vector<int> transmittingIndices;

        // Determine which nodes are ready to transmit
        for (int i = 0; i < numberNodes; ++i) {
            if (nodes[i].isReadyToTransmit) {
                transmittingNodes++;
                transmittingIndices.push_back(i);
            }
        }

        if (transmittingNodes == 1) {
            // Successful transmission
            successful++;
            nodes[transmittingIndices[0]].isReadyToTransmit = false; // Transmission complete
        }
        else if (transmittingNodes > 1) {
            // Collision detected
            collisions++;
            for (int idx : transmittingIndices) {
                nodes[idx].resetBackoffTime(gen, collisions, successful); // Apply exponential backoff
            }
        }

        // Simulate passage of time for each node
        for (auto& node : nodes) {
            node.simulateTimeUnit();
        }
    }

    transmissions.collisions = collisions;
    transmissions.successful = successful;

    return transmissions;
}

void Simulator::setParameters(int _numberNodes, std::shared_ptr<BackoffStrategy> _backoffStrategy, int _minPacketSize, int _maxPacketSize, double _simulationTime, int _numSimulations)
{
    this->numberNodes = _numberNodes;
    this->backoffStrategy = _backoffStrategy;
    this->minPacketSize = _minPacketSize;
    this->maxPacketSize = _maxPacketSize;
    this->simulationTime = _simulationTime;
    this->numSimulations = _numSimulations;
}