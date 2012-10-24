#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGui>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{

     fileName = QFileDialog::getOpenFileName(this,"","","*.yaml");
     QFile file(fileName);
     if(!file.open(QIODevice::ReadOnly)) {
         QMessageBox::information(0, "error", file.errorString());
		 return;
     }

     QTextStream in(&file);
     QString line;
	 
     while(!in.atEnd()) {
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

}
