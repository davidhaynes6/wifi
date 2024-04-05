#pragma once

#include <QtWidgets/QMainWindow>
#include <QTextEdit>

#include "ui_wifi.h"

class wifi : public QMainWindow
{
    Q_OBJECT

public:
    wifi(QWidget *parent = nullptr);
    ~wifi();

public slots:
    void simulate();

private:
    Ui::wifiClass ui;
    QTextEdit* textConsole; 
};
