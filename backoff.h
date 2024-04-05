#pragma once
#include <random>

// BackoffStrategy Interface
class BackoffStrategy {
public:
    virtual ~BackoffStrategy() {}
    //virtual int calculateBackoffTime(std::mt19937& gen, int collisionCount) = 0;
    virtual int calculateBackoffTime(std::mt19937& gen, int collisionCount, int successfulCount) = 0;
};

// ExponentialBackoffStrategy Class
class ExponentialBackoffStrategy : public BackoffStrategy {
public:
    int calculateBackoffTime(std::mt19937& gen, int collisionCount, int successfulCount=0) override {
        std::uniform_int_distribution<> backoffDist(0, std::min(1024, 1 << collisionCount) - 1);
        return backoffDist(gen);
    }
};

// Binary Exponential Backoff (BEB) Strategy 
// 
// CWmin and CWmax: These are the minimum and maximum contention window sizes, respectively.
// The actual 802.11 standard defines different values for these based on the physical layer(PHY) 
// specification.For simplicity, default values are provided, but these can be adjusted as needed.
//
// Collision Count : The algorithm takes into account the number of collisions that have occurred 
// for a given packet.After each collision, the contention window size is doubled(up to CWmax), and 
// the backoff time is selected randomly from this window.
//
// Flexibility : By parameterizing CWmin and CWmax, this implementation allows flexibility for 
// different 802.11 PHY specifications or for experimentation with different contention window sizes.
//
// This strategy more accurately reflects the backoff mechanism used in 802.11 WiFi networks and provides
//  a more realistic simulation of the CSMA / CA protocol.You can further refine this model by incorporating 
// additional details from the 802.11 standards, such as differentiating between initial transmissions and 
// retries or adjusting the contention window based on specific network conditions or standards revisions.

class BinaryExponentialBackoffStrategy : public BackoffStrategy {
private:
    int CWmin; // Minimum contention window size, must be at least 2 to allow a range for random selection.
    int CWmax; // Maximum contention window size
public:
    BinaryExponentialBackoffStrategy(int CWmin = 16, int CWmax = 1024)
        : CWmin(std::max(2, CWmin)), CWmax(CWmax) { // Ensure CWmin is at least 2.
    }

    int calculateBackoffTime(std::mt19937& gen, int collisionCount, int successfulCount=0) override {
        // Ensure the collision count does not decrease the CW below 1.
        int CW = std::min(CWmax, CWmin * (1 << std::max(0, collisionCount)));
        // Subtract 1 from CW to account for zero-based index when using CW as range.
        std::uniform_int_distribution<> backoffDist(0, std::max(1, CW) - 1);
        return backoffDist(gen);
    }
};

// Adaptive Rate Backoff Strategy
// This strategy involves monitoring the collision rate and adjusting the contention window 
// size based on this rate.If the collision rate is high, indicating a congested network, the 
// contention window increases more aggressively than in BEB. Conversely, if the collision rate 
// is low, suggesting that the network is not congested, the contention window size is 
// increased less aggressively or even decreased to improve throughput.

// The strategy maintains a dynamic contention window size that adapts based on feedback :
// 
// Increase Factor(alpha) : Determines how aggressively the contention window is increased 
// after a collision.Higher values lead to a larger increase.
// 
// Decrease Factor(beta) : Determines how the contention window is decreased after a 
// successful transmission without collisions.Lower values decrease the contention window more aggressively.
// 
// Dynamic Adaptation : The contention window size dynamically adapts based on the network 
// condition, aiming to balance throughput and delay by adjusting the backoff time according 
// to the collision and success rates.
//
// Customizable Parameters : The alpha and beta factors can be tuned based on the specific 
// requirements of the network or application, allowing for flexibility in how aggressively 
// the strategy responds to changes in network conditions.
//
// This strategy requires additional logic to track the number of successful transmissions and 
// collisions.This could be implemented within the simulator or network protocol logic, which 
// would then provide these counts as input to the calculateBackoffTime method.

// By adapting the backoff rate based on actual network conditions, the Adaptive Rate Backoff 
// Strategy could potentially offer improvements in network performance, especially in
// environments with fluctuating load and congestion levels.

class AdaptiveRateBackoffStrategy : public BackoffStrategy {
private:
    int CWmin;
    int CWmax;
    double alpha; // Increase factor for the contention window.
    double beta;  // Decrease factor for the contention window.
    int currentCW;

public:
    AdaptiveRateBackoffStrategy(int CWmin = 16, int CWmax = 1024, double alpha = 2.0, double beta = 0.5)
        : CWmin(CWmin), CWmax(CWmax), alpha(alpha), beta(beta), currentCW(CWmin) {}

    int calculateBackoffTime(std::mt19937& gen, int collisionCount, int successfulCount) override {
        // Adjust contention window based on network conditions.
        if (collisionCount > 0) {
            currentCW = std::min(static_cast<int>(currentCW * alpha), CWmax);
        }
        else if (successfulCount > 0) {
            currentCW = std::max(static_cast<int>(currentCW * beta), CWmin);
        }

        std::uniform_int_distribution<> backoffDist(0, currentCW - 1);
        return backoffDist(gen);
    }
};