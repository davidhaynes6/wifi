#include "simulation.h"

Simulation::Simulation(QObject* parent, int numSimulations)
    : QObject(parent), numSimulations(numSimulations) 
{
}

void Simulation::doWork(std::shared_ptr<Simulator> simulator)
{
    std::vector<Point> collisionData;
    auto averageCollisions = 0.0;

    // Variables to track simulation results
    int totalCollisions = 0;
    int totalSuccessful = 0;

    for (auto simulation = 0; simulation < simulator->getNumSimulations(); ++simulation) {

        Transmissions simulatedTransmissions = simulator->simulateCSMACA(simulator->getNumberNodes(), simulator->getBackoffStrategy(), simulator->getMinPacketSize(),
            simulator->getMaxPacketSize(), simulator->getSimulationTime(), simulator->getNumSimulations());

        totalCollisions += simulatedTransmissions.collisions;
        totalSuccessful += simulatedTransmissions.successful;

        // Store the simulated collisions with the corresponding time point
        collisionData.push_back(Point(simulation, simulatedTransmissions.collisions));

        // Calculate the percentage progress and emit it as the progress value
        int progressPercentage = static_cast<int>((simulation + 1) * 100.0 / simulator->getNumSimulations());
        emit progressUpdated(progressPercentage);
    }

    averageCollisions += (double)totalCollisions / simulator->getNumSimulations();

    emit collisionDataReady(collisionData); // Emit the transmission data - used in chartView
    emit finished(averageCollisions);       // Emit finished - will delete simulation and simulator
}