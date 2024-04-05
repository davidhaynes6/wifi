#pragma once

#include <QObject>
#include <vector>
#include <random>
#include "simulator.h"

struct Point
{
    int simulation;
    int collisions;

    Point(int simulation, int collisions) : simulation(simulation), collisions(collisions) {}
};

class Simulation : public QObject {
    Q_OBJECT

public:
    explicit Simulation(QObject* parent=nullptr, int numSimulations=1000);

signals:
    void progressUpdated(int value);
    void finished(double value);
    void collisionDataReady(std::vector<Point> value);

public slots:
    void doWork(std::shared_ptr<Simulator> simulator);

private:
    int numSimulations;
};