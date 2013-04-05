#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGui>
#include <viewer.h>
#include <udp_world_sniffer.h>
#include <zmq_world_sniffer.hpp>
#include <zmq_identifier_sniffer.hpp>


MainWindow::MainWindow ( QWidget *parent ) :
    QMainWindow ( parent ),
    ui ( new Ui::MainWindow )
{
    ui->setupUi ( this );
    insideViewer=0;
    sniffer=0;
    simulator=0;
    QCoreApplication::setOrganizationName ( "TODO" );
    QCoreApplication::setOrganizationDomain ( "TODO" );
    QCoreApplication::setApplicationName ( "Launcher" );
    settings=new QSettings();
        restoreGeometry(settings->value("mainWindowGeometry").toByteArray());
    restoreState(settings->value("mainWindowState").toByteArray());

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
    QList<int> sizes;
    sizes.push_back ( 500 );
    sizes.push_back ( 500 );
    ui->splitter->setSizes ( sizes );

}



void MainWindow::closeEvent(QCloseEvent *event) {
    QSettings settings;
    settings.setValue("mainWindowGeometry", saveGeometry());
    settings.setValue("mainWindowState", saveState());
    QWidget::closeEvent(event);
    if (identifier_sniffer)
      	identifier_sniffer->stop_receiving();
    if (sniffer)
	sniffer->stop_receiving();
}

MainWindow::~MainWindow()
{
  s_interrupted=1;
    delete insideViewer;
    delete ui;
    if ( simulator )
    {

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
    arguments<< "-check_collision";
    if ( simulator )
    {

        simulator->kill();
        delete ( simulator );
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
    if (ui->selectViewType->selectedItems().first()->text().compare("TaskAssignment")==0)
    {
        viewerType=4;
    }
    if ( ui->selectViewType->selectedItems().first()->text().compare ( "Monitor" ) ==0 )
    {
        viewerType=5;
    }
    if ( viewerType>0 )
    {
        QString a;
       // arguments <<"-t"<< a.setNum ( viewerType );
        std::string graphname="";
        if ( viewerType==2 )
        {
            QFile file ( fileName );
            QDir d = QFileInfo ( file ).absoluteDir();
            graphname= ( d.absolutePath().append ( "/" ).append ( QString::fromStdString ( world.graphName ).toLower() ) ).toStdString();
        }
	
	//written by Alessandro Settimi
	if (viewerType==4)
        {
            graphname=fileName.toStdString();
        }
	//written by Alessandro Settimi

        //buffer.resize(MAX_PACKET_LENGTH);
        if (!mutex)
	{
	  std::shared_ptr<std::mutex> temp(new std::mutex);
	  mutex.swap(temp);
	  
	}
	else
	{
	 mutex->unlock();
	 //mutex->~mutex();
	  //std::shared_ptr<std::mutex> temp(new std::mutex);
	  //mutex.swap(temp);
	}
        if (!sniffer)
        {
            sniffer=std::unique_ptr<zmq_world_sniffer<world_sim_packet> > (new zmq_world_sniffer<world_sim_packet> ( buffer,mutex ));
            sniffer->start_receiving();
        }
        else
        {
        }
        if ( insideViewer )
        {
            ui->asdf->removeWidget ( insideViewer );
            delete insideViewer;
        }

	std::cout<<viewerType<<"viewertype";
        if ( viewerType==5 )
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
                identifier_sniffer= std::unique_ptr<zmq_identifier_sniffer>(new zmq_identifier_sniffer( monitor_buffer,monitor_mutex ));
                identifier_sniffer->start_receiving();
            }
        }

        insideViewer=new Viewer ( buffer,mutex,NULL,viewerType,graphname );
        if ( viewerType==5 )
        {
            insideViewer->setMonitor ( &monitor_buffer,monitor_mutex );
        }
        ui->asdf->addWidget ( insideViewer );
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

