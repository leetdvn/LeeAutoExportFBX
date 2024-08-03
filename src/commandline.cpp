#include "commandline.h"




CommandLine::CommandLine(QObject *parent)
    :QObject(parent)
{
    this->moveToThread(&mThread);

    connect(&mThread,SIGNAL(started()),this,SLOT(ReadSTDIN()));
    connect(this,SIGNAL(OnReadLine(QString)),this,SLOT(HandleSTDIN(QString)));

    mThread.start();
}

CommandLine::~CommandLine()
{
    if(mThread.isRunning())
        mThread.quit();
}

void CommandLine::ReadSTDIN()
{
    QTextStream stream(stdin);
    QString Line = stream.readLine();
    if(Line.isEmpty()|| Line.isNull()) return;
    qDebug() << Line;
    emit OnReadLine(Line);
    ReadSTDIN();
}

void CommandLine::HandleSTDIN(QString Line)
{
    qDebug() << Line;

    //WriteSTDOUT("You entered : " + Line);
}

void CommandLine::WriteSTDOUT(QString Line)
{
    //QTextStream stream(stdout);
    //qDebug() << Line;
    //stream << Line;
}

void CommandLine::StopThread()
{
    if(mThread.isRunning()) mThread.quit();
}
