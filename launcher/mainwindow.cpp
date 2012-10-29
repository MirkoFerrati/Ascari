#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGui>
#include <viewer.h>
#include <udp_world_sniffer.h>


MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    insideViewer=0;
    sniffer=0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{

    fileName = QFileDialog::getOpenFileName(this,"","","*.yaml");
    QFile file(fileName);
    QDir d = QFileInfo(file).absoluteDir();
    std::cout<<d.absolutePath().toStdString()<<"ciao mirko"<<std::endl;
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
        return;
    }

    QTextStream in(&file);
    QString line;

    while (!in.atEnd()) {
        line+= in.readLine()+"\n";
    }

    file.close();
    world=parse_file(fileName.toStdString());
    QString temp="Agents: ";
    QString num;
    num.setNum(world.agents.size());
    ui->StartAgents->setText(temp.append(num));
    ui->ShowFile->setText(line);
}

void MainWindow::on_actionSimulator_triggered()
{
    QFileDialog openApp(this);
    openApp.setDirectory("..");
    openApp.setFileMode(QFileDialog::ExistingFile);
    openApp.setFilter(QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot);
    openApp.setNameFilter("simulator");
    if (openApp.exec())
        simulatorPath=openApp.selectedFiles().first();
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionAgent_triggered()
{
    QFileDialog openApp(this);
    openApp.setDirectory("..");
    openApp.setFileMode(QFileDialog::ExistingFile);
    openApp.setFilter(QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot);
    openApp.setNameFilter("agent");
    if (openApp.exec())
        agentPath=openApp.selectedFiles().first();
}

void MainWindow::on_actionViewer_triggered()
{
    QFileDialog openApp(this);
    openApp.setDirectory("..");
    openApp.setFileMode(QFileDialog::ExistingFile);
    openApp.setFilter(QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot);
    openApp.setNameFilter("viewer");
    if (openApp.exec())
        viewerPath=openApp.selectedFiles().first();
}

void MainWindow::on_StartAgents_clicked()
{
    for (unsigned int i=0;i<world.agents.size();i++)
    {
        QProcess *agent;
        QStringList arguments;
        arguments<< "-a" << QString::fromStdString(world.agents[i].name);
        arguments<< "-f"<< fileName;
        agent=new QProcess(this);
        QFile file(fileName);
        QDir d = QFileInfo(file).absoluteDir();

        agent->setWorkingDirectory(d.absolutePath());
        agents.push_back(agent);
        agents[i]->setProcessChannelMode(QProcess::MergedChannels);
        agents[i]->start(agentPath,arguments);
    }

}

void MainWindow::on_StartSimulator_clicked()
{
    QStringList arguments;
    arguments<< "-f"<< fileName;
    simulator=new QProcess(this);
    QFile file(fileName);
    QDir d = QFileInfo(file).absoluteDir();
    simulator->setWorkingDirectory(d.absolutePath());
    simulator->setProcessChannelMode(QProcess::MergedChannels);
    simulator->start(simulatorPath,arguments);
}

void MainWindow::on_Updateshell_clicked()
{
    for (unsigned int i=0;i<agents.size();i++)
    {
        QString strout= agents[i]->readAllStandardOutput();
        std::cout<<strout.toStdString()<<std::endl;
        strout= agents[i]->readAllStandardError();
        std::cout<<strout.toStdString()<<std::endl;
    }
//     QString strout= viewer->readAllStandardOutput();
//     std::cout<<strout.toStdString()<<std::endl;
//     strout= viewer->readAllStandardError();
//     std::cout<<strout.toStdString()<<std::endl;
}

void MainWindow::on_StartViewer_clicked()
{
    int viewerType=-1;
    QStringList arguments;

    if (ui->selectViewType->selectedItems().first()->text().compare("Baseball")==0)
    {
        viewerType=1;
    }
    if (ui->selectViewType->selectedItems().first()->text().compare("Grafo")==0)
    {
        viewerType=2;
        arguments<< "-f"<<QString::fromStdString(world.graphName).toLower();
    }
    if (ui->selectViewType->selectedItems().first()->text().compare("Vuoto")==0)
    {
        viewerType=3;
    }
    if (viewerType>0)
    {
        QString a;
        arguments <<"-t"<< a.setNum(viewerType);
        //viewer=new QProcess(this);
        std::string graphname="";
        if (viewerType==2)
        {
            QFile file(fileName);
            QDir d = QFileInfo(file).absoluteDir();
            //viewer->setWorkingDirectory(d.absolutePath());
            graphname=(d.absolutePath().append("/").append(QString::fromStdString(world.graphName).toLower())).toStdString();
        }
        //viewer->setProcessChannelMode(QProcess::MergedChannels);
        //viewer->start(viewerPath,arguments);

        buffer.resize(MAX_PACKET_LENGTH);
        //TODO: sistemare il path qui usando QDir
        if (!sniffer)
        {
            sniffer=new udp_world_sniffer(buffer,io_service);
            sniffer->start_receiving();
        }
        if (insideViewer)
        {
            ui->asdf->removeWidget(insideViewer);
            delete insideViewer;
        }
        insideViewer=new Viewer(buffer,io_service,0,viewerType,graphname);

//        QWidget *test=new QWidget();

//        QFormLayout *layout = new QFormLayout;

//        layout->addRow("test",insideViewer);
//        layout->addRow("ciao",new QPushButton("test"));
//        test->setLayout(layout);
//        ui->asdf->addWidget(test);
        //ui->asdf->
        ui->asdf->addWidget(insideViewer);
        insideViewer->start();


    }

}
