#include "dependencies.h"
#include "checkmedia.h"
#include <QMessageBox>

dependencies::dependencies(QObject *parent) : QObject(parent)
{

}


QStringList dependencies::checkDependencies()
{
    QString ffmpegLoc = findDir("ffmpeg");
    checkmedia cm;
    QStringList depStatus = { cm.checkFormats(), ffmpegLoc };
    return depStatus;
}

QString dependencies::findDir(QString executable)
{
    QString execDir = qApp->applicationDirPath() + "/" + executable;
    if (OS == "Windows")
        execDir = execDir + ".exe";
    bool pathfail = checkEXEC(execDir);

    if (OS == "Unix" && pathfail)
    {
        pathfail = checkEXEC("/usr/local/bin/" + executable);
        if (!pathfail)
            execDir = "/usr/local/bin/" + executable;
    }

    if (OS == "Unix" && pathfail)
    {
        pathfail = checkEXEC("/usr/bin/" + executable);
        if (!pathfail)
            execDir = "/usr/bin/" + executable;
    }

    if (pathfail)
    {
        execDir = "Not Found: " + execDir;
    }
    return execDir;
}

bool dependencies::checkEXEC(QString execDir)
{
    execfail = false;
    exec = new QProcess(this);
    QStringList ffmpegcommand = { "-version" };
    connect(exec, SIGNAL(error(QProcess::ProcessError)), this, SLOT(readErrors()));
    exec->setProcessChannelMode(QProcess::MergedChannels);
    exec->start(execDir, ffmpegcommand);
    exec->waitForFinished();
    exec->deleteLater();
    return execfail;
}


void dependencies::readErrors()
{
    execfail = true;
    exec->kill();
}
