#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "yaml_parser.h"
namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();

    void on_actionSimulator_triggered();

    void on_actionExit_triggered();

    void on_actionAgent_triggered();

    void on_actionViewer_triggered();

    void on_StartAgents_clicked();

private:
    Ui::MainWindow *ui;
	Parsed_World world;
    QString fileName,simulatorPath,agentPath,viewerPath;
};

#endif // MAINWINDOW_H
