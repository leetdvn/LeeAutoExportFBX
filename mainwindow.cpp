#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->inputEditText,&QTextEdit::textChanged, this, &MainWindow::OnBrowserChanged);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OnBrowserChanged(QString inContent)
{
    //qDebug() << inContent << Qt::endl;
}
