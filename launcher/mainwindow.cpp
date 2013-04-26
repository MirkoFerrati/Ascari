#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGui>
#include <viewer.h>
#include <monitor_viewer.h>
#include <udp_world_sniffer.h>
#include <zmq_world_sniffer.hpp>
#include <zmq_identifier_sniffer.hpp>
#include "../viewer/gui/agent_router_viewer.h"
#include "../viewer/gui/task_assignment_viewer.h"

MainWindow::MainWindow ( QWidget *parent ) :
    QMainWindow ( parent ),
    ui ( new Ui::MainWindow )
{
    ui->setupUi ( this );

    qout=new QDebugStream ( std::cout, ui->ShellOutput );
    qerr=new QDebugStream ( std::cerr, ui->ShellOutput );
    insideViewer=0;
    sniffer=0;
    simulator=0;
    QCoreApplication::setOrganizationName ( "TODO" );
    QCoreApplication::setOrganizationDomain ( "TODO" );
    QCoreApplication::setApplicationName ( "Launcher" );

    QList<int> sizes;
    sizes.push_back ( 500 );
    sizes.push_back ( 500 );
    ui->HorizSplitter->setSizes ( sizes );
    sizes.clear();
    sizes.push_back ( 300 );
    sizes.push_back ( 300 );
    sizes.push_back ( 300 );
    ui->VertSplitter->setSizes ( sizes );

    settings=new QSettings();
    restoreGeometry ( settings->value ( "mainWindowGeometry" ).toByteArray() );
    restoreState ( settings->value ( "mainWindowState" ).toByteArray() );

    agentPath=settings->value ( "paths/agent","" ).toString();
    simulatorPath=settings->value ( "paths/simulator","" ).toString();
    viewerPath=settings->value ( "paths/viewer","" ).toString();
    fileName = settings->value ( "paths/lastopen","" ).toString();
    if ( fileName.compare ( "" ) !=0 )
    {
        try
        {
            openFile();
        }
        catch ( ... )
        {
            std::cerr<<"impossibile aprire il file "<<fileName.toStdString() <<std::endl;
        }
    }


}



void MainWindow::closeEvent ( QCloseEvent *event )
{
    QSettings settings;
    settings.setValue ( "mainWindowGeometry", saveGeometry() );
    settings.setValue ( "mainWindowState", saveState() );
    QWidget::closeEvent ( event );
    if ( identifier_sniffer )
        identifier_sniffer->stop_receiving();
    if ( sniffer )
        sniffer->stop_receiving();
    delete qout;
    delete qerr;

}

MainWindow::~MainWindow()
{
    s_interrupted=1;
    delete insideViewer;
    delete ui;
    if ( simulator )
    {
        QObject::disconnect ( simulator, SIGNAL ( finished ( int ) ), this, SLOT ( simulatorExited ( int,QProcess::ExitStatus ) ) );
        simulator->kill();
        delete ( simulator );
    }

    for ( unsigned int i=0; i<agents.size(); i++ )
    {
        agents[i]->kill();
        delete ( agents[i] );
    }
}

void MainWindow::openFile()
{
    QFile file ( fileName );
    QDir d = QFileInfo ( file ).absoluteDir();

    if ( !file.open ( QIODevice::ReadOnly ) )
    {
        QMessageBox::information ( 0, "error", file.errorString() );
        return;
    }

    QTextStream in ( &file );
    QString line;

    while ( !in.atEnd() )
    {
        line+= in.readLine() +"\n";
    }

    file.close();
    try
    {
        world=parse_file ( fileName.toStdString() );
        QString temp="Agents: ";
        QString num;
        num.setNum ( world.agents.size() );
        ui->StartAgents->setText ( temp.append ( num ) );
        ui->ShowFile->setText ( line );
        settings->setValue ( "paths/lastopen",fileName );
    }
    catch ( ... )
    {
        ui->StartAgents->setText ( "impossibile parsare il file" );
    }
}

void MainWindow::on_actionOpen_triggered()
{

    fileName = QFileDialog::getOpenFileName ( this,"","","*.yaml" );
    if ( fileName.compare ( "" ) !=0 )
        openFile();
}

void MainWindow::on_PauseSimulator_clicked()
{
    if ( simulator )
    {
        simulator->putChar ( 'p' );
    }
}


void MainWindow::on_actionSimulator_triggered()
{
    QFileDialog openApp ( this );
    openApp.setDirectory ( ".." );
    openApp.setFileMode ( QFileDialog::ExistingFile );
    openApp.setFilter ( QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot );
    openApp.setNameFilter ( "simulator" );
    if ( openApp.exec() )
    {
        simulatorPath=openApp.selectedFiles().first();
        settings->setValue ( "paths/simulator",simulatorPath );
    }
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_stopall_clicked()
{
    on_Updateshell_clicked();

    if ( !agents.empty() )
    {
        for ( unsigned int i=0; i<agents.size(); i++ )
        {
            agents[i]->kill();
            delete ( agents[i] );
        }
        agents.clear();
    }
    if ( simulator )
    {

        simulator->kill();
        delete ( simulator );
        simulator=0;
    }
}


void MainWindow::on_actionAgent_triggered()
{
    QFileDialog openApp ( this );
    openApp.setDirectory ( ".." );
    openApp.setFileMode ( QFileDialog::ExistingFile );
    openApp.setFilter ( QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot );
    openApp.setNameFilter ( "agent" );
    if ( openApp.exec() )
    {
        agentPath=openApp.selectedFiles().first();
        settings->setValue ( "paths/agent",agentPath );
    }
}

void MainWindow::on_actionViewer_triggered()
{
    QFileDialog openApp ( this );
    openApp.setDirectory ( ".." );
    openApp.setFileMode ( QFileDialog::ExistingFile );
    openApp.setFilter ( QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot );
    openApp.setNameFilter ( "viewer" );
    if ( openApp.exec() )
    {
        viewerPath=openApp.selectedFiles().first();
        settings->setValue ( "paths/viewer",viewerPath );
    }
}

void MainWindow::startAgents()
{
    if ( !agents.empty() )
    {
        for ( unsigned int i=0; i<agents.size(); i++ )
        {
            agents[i]->kill();
            delete ( agents[i] );
        }
        agents.clear();
    }
    for ( unsigned int i=0; i<world.agents.size(); i++ )
    {
        QProcess *agent;
        QStringList arguments;
        arguments<< "-a" << QString::fromStdString ( world.agents[i].name );
        arguments<< "-f"<< fileName;
        agent=new QProcess ( this );
        QFile file ( fileName );
        QDir d = QFileInfo ( file ).absoluteDir();
        agent->setWorkingDirectory ( d.absolutePath() );
        agents.push_back ( agent );
        agents[i]->setProcessChannelMode ( QProcess::MergedChannels );
        agents[i]->start ( agentPath,arguments );
    }

}

void MainWindow::startSimulator()
{
    QStringList arguments;
    arguments<< "-f"<< fileName;
    if ( ui->collisionEnabler->isChecked() )
        arguments<< "-check_collision";
    if ( simulator )
    {

        simulator->kill();
        delete ( simulator );
        simulator=0;
    }
    simulator=new QProcess ( this );
    QFile file ( fileName );
    QDir d = QFileInfo ( file ).absoluteDir();
    simulator->setWorkingDirectory ( d.absolutePath() );
    simulator->setProcessChannelMode ( QProcess::MergedChannels );
    simulator->start ( simulatorPath,arguments );
    if ( !simulator->waitForStarted ( 2000 ) )
    {
        QMessageBox::warning ( this,"errore","il simulatore non si è avviato" );
        std::cout<<"errore, il simulatore non si è avviato"<<std::endl;
    };
    QObject::connect ( simulator, SIGNAL ( finished ( int,QProcess::ExitStatus ) ),
                       this, SLOT ( simulatorExited ( int,QProcess::ExitStatus ) ) );
}

void MainWindow::on_Updateshell_clicked()
{
    for ( unsigned int i=0; i<agents.size(); i++ )
    {
        QString strout= agents[i]->readAllStandardOutput();
        QFile file ( QString::fromStdString ( world.agents[i].name ).append ( ".log" ) );
        file.open ( QIODevice::WriteOnly );
        QTextStream out ( &file );
        out<<strout<<"\n";
        //std::cout<<strout.toStdString()<<std::endl;
        strout= agents[i]->readAllStandardError();
        out<<strout<<"\n";
    }
    if ( simulator )
    {
        QString strout=simulator->readAllStandardOutput();
        QFile file ( "simulator.log" );
        file.open ( QIODevice::WriteOnly );
        QTextStream out ( &file );
        out<<strout<<"\n";
        strout=simulator->readAllStandardError();
        out<<strout<<"\n";
    }

}

bool MainWindow::startViewer()
{
    int viewerType=-1;
    QStringList arguments;

    if ( ui->selectViewType->selectedItems().size() <1 )
        return false;

    if ( ui->selectViewType->selectedItems().first()->text().compare ( "Baseball" ) ==0 )
    {
        viewerType=1;
    }
    if ( ui->selectViewType->selectedItems().first()->text().compare ( "Grafo" ) ==0 )
    {
        viewerType=2;
        arguments<< "-f"<<QString::fromStdString ( world.graphName ).toLower();
    }
    if ( ui->selectViewType->selectedItems().first()->text().compare ( "Vuoto" ) ==0 )
    {
        viewerType=3;
    }
    if ( ui->selectViewType->selectedItems().first()->text().compare ( "TaskAssignment" ) ==0 )
    {
        viewerType=4;
    }
    if ( ui->selectViewType->selectedItems().first()->text().compare ( "Monitor" ) ==0 )
    {
        viewerType=5;
    }

    if ( viewerType>0 )
    {
        if ( insideViewer )
        {
            ui->ViewerContainer->removeWidget ( insideViewer );
            delete insideViewer;
        }
    for ( auto plugin:plugins )
            delete plugin;
        if ( !mutex )
        {
            std::shared_ptr<std::mutex> temp ( new std::mutex );
            mutex.swap ( temp );

        }
        else
        {
            mutex->unlock();
        }
        if ( !sniffer )
        {
            sniffer=std::unique_ptr<zmq_world_sniffer<world_sim_packet> > ( new zmq_world_sniffer<world_sim_packet> ( buffer,mutex ) );
            sniffer->start_receiving();
        }
        insideViewer=new Viewer ( buffer,mutex,NULL );
        QString a;
        // arguments <<"-t"<< a.setNum ( viewerType );
        std::string graphname="";


        switch ( viewerType )
        {


        case 2:
        {
            QFile file ( fileName );
            QDir d = QFileInfo ( file ).absoluteDir();
            graphname= ( d.absolutePath().append ( "/" ).append ( QString::fromStdString ( world.graphName ).toLower() ) ).toStdString();
            viewer_plugin* temp=new agent_router_viewer();
            temp->setfather ( insideViewer );
            insideViewer->addPlugin ( temp );
            plugins.push_back ( temp );
        }

        case 4:
        {
            viewer_plugin* temp=new task_assignment_viewer(insideViewer->getTime(),mutex,buffer);
            temp->setfather ( insideViewer );
	    temp->setAgentSize(0.2);
	    temp->setPainterScale(1000.0);
            insideViewer->addPlugin ( temp );
            plugins.push_back ( temp );
        }
        break;

        case 5:
        {
            if ( !monitor_mutex )
            {
                std::shared_ptr<std::mutex> temp ( new std::mutex );
                monitor_mutex.swap ( temp );
            }
            else
            {
                monitor_mutex->unlock();
            }
            if ( !identifier_sniffer )
            {
                identifier_sniffer= std::unique_ptr<zmq_identifier_sniffer> ( new zmq_identifier_sniffer ( monitor_buffer,monitor_mutex ) );
                identifier_sniffer->start_receiving();
            }
            viewer_plugin* temp=new monitor_viewer ( &monitor_buffer,monitor_mutex );
            temp->setfather ( insideViewer );
            insideViewer->addPlugin ( temp );
            plugins.push_back ( temp );
        }

        default:
        {


        }
        }

        ui->ViewerContainer->addWidget ( insideViewer );
        insideViewer->init ( graphname );
        insideViewer->start();

        return true;

    }
    return false;

}

void MainWindow::on_playall_clicked()
{
    startAgents();
    startSimulator();
    startViewer();
}

void MainWindow::on_StartAgents_clicked()
{
    startAgents();
}

void MainWindow::on_StartSimulator_clicked()
{
    startSimulator();
}

void MainWindow::on_StartViewer_clicked()
{
    startViewer();
}

void MainWindow::simulatorExited ( int exitcode, QProcess::ExitStatus exitstatus )
{
    std::cout<<"SIMULATOR EXITED: exitcode:"<<exitcode<< ( exitstatus==QProcess::NormalExit?"":" and crashed" ) <<std::endl;
    std::cout<<QString ( simulator->readAllStandardError() ).toStdString();
    std::cout<<QString ( simulator->readAllStandardOutput() ).toStdString();
}
