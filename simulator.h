#pragma once

#include <vector>
#include <random>
#include "backoff.h"

struct Transmissions
{
    int successful;
    int collisions;
};

class Simulator {
public:
    Simulator(int numberNodes=0, std::shared_ptr<BackoffStrategy> backoffStrategy = nullptr, int minPacketSize=0, int maxPacketSize=0, int simulationTime=0, int numSimulations=0);
    Transmissions simulateCSMACA(int numberNodes, std::shared_ptr<BackoffStrategy> backoffStrategy, int minPacketSize, int maxPacketSize, int simulationTime, int simulations) const;


    double getNumberNodes() 
    { 
        return numberNodes; 
    }

    std::shared_ptr<BackoffStrategy> getBackoffStrategy()
    {
        return backoffStrategy;
    }

    int getMinPacketSize() 
    { 
        return minPacketSize; 
    }

    int getMaxPacketSize() 
    { 
        return maxPacketSize; 
    }

    double getSimulationTime() 
    { 
        return simulationTime; 
    }

    // Number of simulations to run.
    int getNumSimulations() 
    { 
        return numSimulations; 
    }

    // Add member functions for setting parameters and performing simulations.
    void setParameters(int _numberNodes, std::shared_ptr<BackoffStrategy> _backoffStrategy, int _minPacketSize, int _maxPacketSize, double _simulationTime, int _numSimulations);

private:
    int numberNodes;
    std::shared_ptr<BackoffStrategy> backoffStrategy;
    int minPacketSize;
    int maxPacketSize;
    double simulationTime;
    int numSimulations; // Number of Monte Carlo simulations

    std::vector<double> finalPrices;
};