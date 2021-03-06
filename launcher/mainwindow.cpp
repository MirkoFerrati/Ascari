#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGui>
#include <viewer.h>
#include <udp_world_sniffer.h>
#include <zmq_world_sniffer.hpp>
//#include "../plugins/agent_router/agent_router_viewer.h"
//#include "../plugins/task_assignment/task_assignment_viewer.h"
#include "../plugins/abstract_viewer_plugin.h"
//#include "../plugins/agent_router/agent_router_parsed_world.h"
#include "../plugins/addplugins.h"

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
    selectedAgents=0;
    QCoreApplication::setOrganizationName ( "K2bRobotics" );
    QCoreApplication::setOrganizationDomain ( "K2bRobotics.com" );
    QCoreApplication::setApplicationName ( "Launcher" );

    QList<int> sizes;
    sizes.push_back ( 500 );
    sizes.push_back ( 500 );
    ui->ViewerSplitter->setSizes ( sizes );
    sizes.clear();
    sizes.push_back ( 300 );
    sizes.push_back ( 300 );
    sizes.push_back ( 300 );
    ui->VertSplitter->setSizes ( sizes );

    settings=new QSettings();
    restoreGeometry ( settings->value ( "mainWindowGeometry" ).toByteArray() );
    restoreState ( settings->value ( "mainWindowState" ).toByteArray() );

    auto filesTable=ui->tableWidget;
    QStringList labels;
    labels << tr("Config Variable") << tr("Value");
    filesTable->setHorizontalHeaderLabels(labels);
    filesTable->setColumnCount( labels.size() );
    filesTable->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    filesTable->verticalHeader()->hide();
    filesTable->setShowGrid(false);
    filesTable->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
    agentPath=settings->value ( "paths/agent","" ).toString();
    simulatorPath=settings->value ( "paths/simulator","" ).toString();
    yamlsPath=settings->value ( "paths/yamls","" ).toString();
    fileName = settings->value ( "paths/lastopen","" ).toString();

    QFile file ( fileName );
    QDir d = QFileInfo ( file ).absoluteDir();

    addConfigRow("paths/agent",agentPath);
    addConfigRow("paths/simulator",simulatorPath);
    addConfigRow("paths/yamls",yamlsPath);
    addConfigRow("paths/lastopen",fileName);
    addConfigRow("paths/fullfilepath",d.absolutePath());

    generic_plugins=createPlugins();

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
    ui->selectViewType->clear();
    ui->selectViewType->addItem ( "Vuoto" );
for ( auto plugin:generic_plugins )
    {
        ui->selectViewType->addItem ( QString::fromStdString(plugin->getType()) );
    }

}


void MainWindow::addConfigRow(std::string name, std::string value)
{
  addConfigRow(name,QString::fromStdString(value));
}
void MainWindow::addConfigRow(std::string name, QString value)
{
     auto filesTable=ui->tableWidget;
    int row=-1;
     if (!config_to_row.count(name))
     {
    row = filesTable->rowCount();
    filesTable->insertRow(row);
    config_to_row[name]=row;
     }
    else
    {
         row=config_to_row[name];
     }
     QTableWidgetItem *nameItem = new QTableWidgetItem;
     nameItem->setText(QString::fromStdString(name));
     QTableWidgetItem *valueItem = new QTableWidgetItem;
     valueItem->setText(value);
    filesTable->setItem(row, 0, nameItem);
    filesTable->setItem(row, 1, valueItem);
        filesTable->resizeColumnsToContents();
  
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
    if ( !agentcontainer.empty() )
    {
    for ( auto widg:agentcontainer )
        {
            ui->agentList->removeWidget ( widg );
            delete widg;
        }
        agentcontainer.clear();
    }

}

MainWindow::~MainWindow()
{
    s_interrupted=1;
    delete insideViewer;
    insideViewer=0;
    delete ui;
    if ( simulator )
    {
        QObject::disconnect ( simulator, SIGNAL ( finished ( int ) ), this, SLOT ( simulatorExited ( int,QProcess::ExitStatus ) ) );
        simulator->kill();
        delete ( simulator );
    }

    for ( auto a=agents.begin(); a!=agents.end(); ++a )
    {
        a->second->kill();
        delete ( a->second );
    }

    if ( !agentcontainer.empty() )
    {
    for ( auto widg:agentcontainer )
        {
            ui->agentList->removeWidget ( widg );
            delete widg;
        }
        agentcontainer.clear();
    }

}

void MainWindow::openFile()
{
    QFile file ( fileName );
    QDir d = QFileInfo ( file ).absoluteDir();

    if ( !file.open ( QIODevice::ReadOnly ) )
    {
        QMessageBox::information ( 0, "error", file.errorString() );
        addConfigRow("ERROR",QString("Cannot open file"));
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
        yaml_parser parser;
	for (auto plugin:generic_plugins)
	{
	  plugin->createParserPlugin();
	  parser.addPlugin(plugin->getParserPlugin());
	}
	world=parser.parse_file ( fileName.toStdString() );
	if ( !world.parsedSuccessfully )
        {
            ui->StartAgents->setText ( "impossibile parsare il file" );
            return;
        }
        QString temp="Agents: ";
        QString num;
        selectedAgents=world.agents.size();
        num.setNum ( selectedAgents );
        ui->selectAll->setCheckState ( Qt::Checked );

        if ( !agentcontainer.empty() )
        {
        for ( auto widg:agentcontainer )
            {
                ui->agentList->removeWidget ( widg );
                delete widg;
            }
            agentcontainer.clear();
        }

        int i=0;
    for ( auto ag:world.agents )
        {
            QCheckBox* temp= new QCheckBox ( QString::fromStdString ( ag.name ) );
            temp->setTristate ( false );
            temp->setCheckState ( Qt::Checked );
            QObject::connect ( temp, SIGNAL ( stateChanged ( int ) ),
                               this, SLOT ( agentSelected ( int ) ) );
            agentcontainer.push_back ( temp );
            ( ( QVBoxLayout* ) ui->agentList )->addWidget ( temp,i+1,0 );
            i++;
        }
        ui->StartAgents->setText ( temp.append ( num ) );
        ui->ShowFile->setText ( line );
        settings->setValue ( "paths/lastopen",fileName );
    }
    catch ( std::exception ex )
    {
        ui->StartAgents->setText ( ex.what() );
    }
}

void MainWindow::agentSelected ( int state )
{

    QString temp="Agents: ";
    QString num;
    selectedAgents+= ( state-1 );
    if ( selectedAgents==world.agents.size() )
    {
// 	  	ui->selectAll->setTristate(false);

        ui->selectAll->setCheckState ( Qt::Checked );
    }
    else if ( selectedAgents>0 )
    {
// 	  ui->selectAll->setTristate(true);
        disable=true;
        ui->selectAll->setCheckState ( Qt::PartiallyChecked );
        disable=false;
    }
    else if ( selectedAgents==0 )
    {
// 	  	ui->selectAll->setTristate(false);

        ui->selectAll->setCheckState ( Qt::Unchecked );
    }
    num.setNum ( selectedAgents );
    ui->StartAgents->setText ( temp.append ( num ) );
}


void MainWindow::on_actionOpen_triggered()
{

    fileName = QFileDialog::getOpenFileName ( this,"Select yaml file",yamlsPath,"*.yaml" );
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

        for ( auto a=agents.begin(); a!=agents.end(); ++a )
        {
            a->second->kill();
            delete ( a->second );
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

void MainWindow::on_actionYaml_files_triggered()
{
    QFileDialog openApp ( this );
    openApp.setDirectory ( "../../" );
    openApp.setFileMode ( QFileDialog::DirectoryOnly );
    openApp.setFilter ( QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot );
    openApp.setNameFilter ( "" );
    if ( openApp.exec() )
    {
        yamlsPath=openApp.selectedFiles().first();
        settings->setValue ( "paths/yamls",yamlsPath );
    }
}

void MainWindow::startAgents()
{
    if ( !agents.empty() )
    {
        for ( auto a=agents.begin(); a!=agents.end(); ++a )
        {
            a->second->kill();
            delete ( a->second );
        }
        agents.clear();
    }
    for ( unsigned int i=0; i<agentcontainer.size(); i++ )
    {
        if ( !agentcontainer.at ( i )->isChecked() )
            continue;
        QProcess *agent;
        QStringList arguments;
        arguments<< "-a" <<  agentcontainer.at ( i )->text() ;
        arguments<< "-f"<< fileName;
        agent=new QProcess ( this );
        QFile file ( fileName );
        QDir d = QFileInfo ( file ).absoluteDir();
        //agent->setWorkingDirectory ( d.absolutePath() );
        agent->setProcessChannelMode ( QProcess::MergedChannels );
        agent->start ( agentPath,arguments );
        agents[agentcontainer.at ( i )->text().toStdString()]=  agent ;

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
        QMessageBox::warning ( this,"errore","il simulatore potrebbe non essersi avviato" );
        std::cout<<"errore, il simulatore potrebbe non essersi avviato"<<std::endl;
	return;
    };
    QObject::connect ( simulator, SIGNAL ( finished ( int,QProcess::ExitStatus ) ),
                       this, SLOT ( simulatorExited ( int,QProcess::ExitStatus ) ) );
}

void MainWindow::on_Updateshell_clicked()
{
    for ( auto a=agents.begin(); a!=agents.end(); ++a )
    {
        QString strout= a->second->readAllStandardOutput();
        QFile file ( QString::fromStdString ( a->first ).append ( ".log" ) );
        file.open ( QIODevice::WriteOnly );
        QTextStream out ( &file );
        out<<strout<<"\n";
        //std::cout<<strout.toStdString()<<std::endl;
        strout= a->second->readAllStandardError();
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
    //int viewerType=-1;
//     QStringList arguments;
    if ( ui->selectViewType->currentIndex() ==-1 )
        return false;

    /*
        if ( ui->selectViewType->currentText().compare ( "Baseball" ) ==0 )
        {
            viewerType=1;
        }
        if ( ui->selectViewType->currentText().compare ( "Grafo" ) ==0 )
        {
            viewerType=2;
        }
        if ( ui->selectViewType->currentText().compare ( "Vuoto" ) ==0 )
        {
            viewerType=3;
        }
        if ( ui->selectViewType->currentText().compare ( "TaskAssignment" ) ==0 )
        {
            viewerType=4;
        }
        if ( ui->selectViewType->currentText().compare ( "Monitor" ) ==0 )
        {
            viewerType=5;
        }*/

    //if ( viewerType>0 )
    //{
    if ( insideViewer )
    {
        ui->ViewerContainer->removeWidget ( insideViewer );
        delete insideViewer;
        insideViewer=0;
    }
for ( auto plugin:plugins )
        delete plugin;
    plugins.clear();
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

for ( auto plugin: generic_plugins )
    {
        if ( ui->selectViewType->currentText().compare(QString::fromStdString(plugin->getType()))==0 )
        {
            plugin->createViewerPlugin ( insideViewer, &world );
            plugins.push_back ( plugin->getViewerPlugin() );
            plugin->getViewerPlugin()->setfather ( insideViewer );
            insideViewer->addPlugin ( plugin->getViewerPlugin() );
        }
    }

/*    switch ( viewerType )
    {


    case 2:
    {
        std::string graphname="";

        QFile file ( fileName );
        QDir d = QFileInfo ( file ).absoluteDir();
        graphname= ( d.absolutePath().append ( "/" ).append (
                         QString::fromStdString (
                             reinterpret_cast<agent_router_parsed_world*> ( world.parsed_items_from_plugins[0] )->graphName
                         ).toLower()
                     ) ).toStdString();
        abstract_viewer_plugin* temp=new agent_router_viewer ( graphname );
        temp->setfather ( insideViewer );
        insideViewer->addPlugin ( temp );
        plugins.push_back ( temp );
    }

    case 4:
    {
        abstract_viewer_plugin* temp=new task_assignment_viewer ( insideViewer->getTime(),mutex,buffer );
        temp->setfather ( insideViewer );
        temp->setAgentSize ( 0.2 );
        temp->setPainterScale ( 1000.0 );
        insideViewer->addPlugin ( temp );
        plugins.push_back ( temp );
    }
    break;

    case 5:
    {
        std::string mapfilename="";
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
        QFile file ( fileName );
        QDir d = QFileInfo ( file ).absoluteDir();
        if ( ! ( world.mapfilename=="UNSET" ) )
            mapfilename= ( d.absolutePath().append ( "/" ).append ( QString::fromStdString ( world.mapfilename ).toLower() ) ).toStdString();
        abstract_viewer_plugin* temp=new monitor_viewer ( &monitor_buffer,monitor_mutex,mapfilename );
        temp->setfather ( insideViewer );
        insideViewer->addPlugin ( temp );
        plugins.push_back ( temp );
    }

    default:
    {


    }
    }*/

    ui->ViewerContainer->addWidget ( insideViewer );
    insideViewer->init ( fileName.toStdString() );
    insideViewer->start();

    return true;

    //}
    //return false;

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

void MainWindow::on_selectAll_stateChanged ( int arg1 )
{
    if ( disable ) return;
    if ( arg1==1 )
    {
        disable=true;
        ui->selectAll->setCheckState ( Qt::Checked );
    }
    disable=true;

for ( auto i:agentcontainer )
    {
        i->setChecked ( arg1>0 );
    }
    disable=false;

}

void MainWindow::on_speed_sliderMoved ( int position ) //from 0 to 100
{
    if ( simulator )
    {
        char temp[10];
        snprintf ( temp,10,"s%u!", ( 100- ( position ) ) * ( 100-position ) *2 );
        std::cout<<temp<<std::endl;
        simulator->write ( temp );
    }
}
