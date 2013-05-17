#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCheckBox>
#include <QSettings>
#include "yaml_parser.h"
#include <QProcess>
#include "QDebugStream.hpp"
#include <boost/asio.hpp>
#include <viewer.h>
#include <udp_world_sniffer.h>
#include "../shared/types/monitor_packet.h"
namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow ( QWidget *parent = 0 );
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();

    void on_actionSimulator_triggered();

    void on_actionExit_triggered();

    void on_actionAgent_triggered();

    void on_actionYaml_files_triggered();

    void on_StartAgents_clicked();

    void on_StartSimulator_clicked();

    void on_PauseSimulator_clicked();

    void on_Updateshell_clicked();

    void on_StartViewer_clicked();

    void on_playall_clicked();

    void on_stopall_clicked();

    void closeEvent ( QCloseEvent *event );

    void simulatorExited ( int exitcode,QProcess::ExitStatus exitstatus );

    void agentSelected ( int );

    bool startViewer();
    void startAgents();
    void startSimulator();


    void on_selectAll_stateChanged ( int arg1 );

    void on_speed_sliderMoved(int position);

private:
    Ui::MainWindow *ui;
    QDebugStream *qout,*qerr;
    QProcess *simulator;
    boost::asio::io_service io_service;
    //std::vector<char> buffer;
    world_sim_packet buffer;
    std::map<std::string,monitor_packet> monitor_buffer;
    std::shared_ptr<std::mutex> mutex, monitor_mutex;
    void openFile();
    QSettings *settings;
    Parsed_World world;
    Viewer * insideViewer;
    std::vector<abstract_viewer_plugin*> plugins;
    std::vector<QCheckBox*> agentcontainer;
    std::unique_ptr<world_sniffer_abstract> sniffer;
    std::unique_ptr<world_sniffer_abstract> identifier_sniffer;
    std::map<std::string,QProcess*> agents;
    QString fileName,simulatorPath,agentPath,yamlsPath;
    int selectedAgents;
    bool disable;
};

#endif // MAINWINDOW_H
