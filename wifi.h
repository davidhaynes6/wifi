#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_wifi.h"
#include "simulation.h"
#include "simulator.h"
#include "backoff.h"

class wifi : public QMainWindow
{
    Q_OBJECT

public:
    explicit wifi(QWidget *parent = nullptr);
    ~wifi();

    void initializeThread();

public slots:
    void onButtonClicked();
    void startTask();
    void startSimulation();
    void updateProgress(int value);
    void finishedTask(double value);
    void createChart(const std::vector<Point>& data);

private:
    int numberNodes;
    std::shared_ptr<BackoffStrategy> backoffStrategy;
    int minPacketSize;
    int maxPacketSize;
    int simulationTime;
    int numSimulations;
    QString selectedStrategy;


    Ui::wifiClass ui;

    QThread* simThread;

    std::unique_ptr<Simulation> sim;
    std::unique_ptr<Simulator> simulator;
};