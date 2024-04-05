#include "wifi.h"
#include <QThread>
#include <QtCharts>
#include <sstream>
#include <ctime>


wifi::wifi(QWidget* parent) : QMainWindow(parent), numberNodes(100), minPacketSize(64), maxPacketSize(1500), simulationTime(100), numSimulations(1000)
{
    ui.setupUi(this);
    ui.editNumberNodes->setText(QString::number(numberNodes));
    ui.editMinPacketSize->setText(QString::number(minPacketSize));
    ui.editMaxPacketSize->setText(QString::number(maxPacketSize));
    ui.editSimulationTime->setText(QString::number(simulationTime));
    ui.editNumSimulations->setText(QString::number(numSimulations));
    ui.progressBar->setRange(0, 100);
    ui.progressBar->setValue(0);

    // populate backoff strategies
    ui.cbBackoffStrategy->addItem("Exponential");
    ui.cbBackoffStrategy->addItem("BEB");
    ui.cbBackoffStrategy->addItem("AdaptiveRate");

    connect(ui.buttonExecute, &QPushButton::clicked, this, &wifi::onButtonClicked);
    simThread = nullptr;
}

wifi::~wifi()
{
}

void wifi::initializeThread() {
    if (sim.get() == nullptr && simulator.get() == nullptr && simThread == nullptr) {
        sim = std::make_unique<Simulation>(this, numSimulations);
        simulator = std::make_unique<Simulator>();
        simThread = new QThread(this);
        sim->moveToThread(simThread);

        connect(simThread, &QThread::started, this, &wifi::startSimulation);
        connect(simThread, &QThread::finished, simThread, &QThread::deleteLater);       // schedule object for deletion
        connect(sim.get(), &Simulation::finished, this, &wifi::finishedTask);
        connect(sim.get(), &Simulation::finished, simThread, &QThread::quit);
        connect(sim.get(), &Simulation::finished, sim.get(), &Simulation::deleteLater); // schedule object for deletion
        connect(sim.get(), &Simulation::progressUpdated, this, &wifi::updateProgress);
        connect(sim.get(), &Simulation::collisionDataReady, this, &wifi::createChart);
    }
}

void wifi::onButtonClicked()
{
    ui.progressBar->setValue(0);
    ui.buttonExecute->setDisabled(true);

    // Retrieve user input and update member variables
    this->numberNodes = ui.editNumberNodes->text().toInt();
    this->minPacketSize = ui.editMinPacketSize->text().toInt();
    this->maxPacketSize = ui.editMaxPacketSize->text().toInt();
    this->simulationTime = ui.editSimulationTime->text().toInt();
    this->numSimulations = ui.editNumSimulations->text().toInt();

    // Determine selected backoff strategy from UI
    this->selectedStrategy = ui.cbBackoffStrategy->currentText();
    
    if (this->selectedStrategy == "Exponential") {
        this->backoffStrategy = std::make_unique<ExponentialBackoffStrategy>();
    }
    else if (this->selectedStrategy == "BEB") {
        this->backoffStrategy = std::make_unique<BinaryExponentialBackoffStrategy>();
    }
    else if (this->selectedStrategy == "AdaptiveRate") {
        this->backoffStrategy = std::make_unique<AdaptiveRateBackoffStrategy>();
    }
    else {
        // Handle unknown selection or set a default
        this->backoffStrategy = std::make_unique<ExponentialBackoffStrategy>(); // Default case
    }

    startTask();
}

void wifi::startTask()
{
    initializeThread();
    simThread->start();
}

void wifi::startSimulation()
{
    // Pass the updated values to the simulation object
    simulator->setParameters(numberNodes, backoffStrategy, minPacketSize, maxPacketSize, simulationTime, numSimulations);
    sim->doWork(std::move(simulator));
}

void wifi::updateProgress(int value)
{
    ui.progressBar->setValue(value);
}

void wifi::finishedTask(double value)
{
    // Get the current time
    std::time_t currentTime = std::time(nullptr);

    // Convert the time to a tm structure for extracting date and time components
    std::tm* localTime = std::localtime(&currentTime);

    // Define a stringstream for the formatted date and time
    std::stringstream ss;

    // Format the date and time as "YYYY-MM-DD HH:MM:SS"
    ss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");

    // Stream result for display, add Time + Average Price + Years selected
    std::ostringstream result;
    result << ss.str() << " -- Avg Number of Collisions: " << value << std::endl 
        << "Nodes: " << numberNodes << std::endl
        << "Backoff Strategy: " << selectedStrategy.toStdString() << std::endl
        << "Min Packet Size: " << minPacketSize << " Max Packet Size: " << maxPacketSize << " Time Units: " << simulationTime << std::endl
        << "Simulations:: " << numSimulations << std::endl;

    ui.editResult->append(result.str().c_str());

    // Stop the previous simulation and wait for it to finish
    if (simThread->isRunning()) {
        simThread->quit();
        simThread->wait();
    }

    // Reset Simulation and Simulator
    sim.reset();
    simulator.reset();

    // memory managed by Qt
    delete simThread;
    simThread = nullptr;

    ui.buttonExecute->setDisabled(false);
}

void wifi::createChart(const std::vector<Point>& data) {
    // Check if a QChartView already exists in the container
    QChartView* chartView = nullptr;
    if (ui.chartContainer->layout() && ui.chartContainer->layout()->count() > 0) {
        chartView = dynamic_cast<QChartView*>(ui.chartContainer->layout()->itemAt(0)->widget());
    }

    // If QChartView doesn't exist, create and set it up
    if (!chartView) {
        chartView = new QChartView();
        chartView->setRenderHint(QPainter::Antialiasing);
        chartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        QVBoxLayout* layout = new QVBoxLayout();
        layout->addWidget(chartView);
        ui.chartContainer->setLayout(layout);
    }

    // Clear the existing chart data
    QChart* chart = chartView->chart();
    chart->removeAllSeries();
    chart->removeAxis(chart->axisX());
    chart->removeAxis(chart->axisY());
    chart->createDefaultAxes();

    // Create new series with the new data
    QLineSeries* series = new QLineSeries();
    series->setName("Simulations");

    for (const Point& point : data) {
        series->append(static_cast<qreal>(point.simulation), static_cast<qreal>(point.collisions));
    }

    // Add the new series to the chart
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->axisX()->setTitleText("Simulation Number");
    chart->axisY()->setTitleText("Number of Collisions");
}