#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include "yaml_parser.h"
#include <QProcess>
#include <boost/asio.hpp>
#include <viewer.h>
#include <udp_world_sniffer.h>
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

    void on_StartSimulator_clicked();

    void on_Updateshell_clicked();

    void on_StartViewer_clicked();

    void on_playall_clicked();

    bool startViewer();
    void startAgents();
    void startSimulator();

private:
    Ui::MainWindow *ui;
    QProcess *simulator;
    QProcess *viewer;
    boost::asio::io_service io_service;
    std::vector<char> buffer;
    void openFile();
    QSettings *settings;
	Parsed_World world;
        Viewer * insideViewer;
        udp_world_sniffer *sniffer;
	std::vector<QProcess*> agents;
    QString fileName,simulatorPath,agentPath,viewerPath;
};

#endif // MAINWINDOW_H
