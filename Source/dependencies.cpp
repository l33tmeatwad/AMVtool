#include "dependencies.h"
#include "checkmedia.h"
#include <QMessageBox>

dependencies::dependencies(QObject *parent) : QObject(parent)
{

}

QStringList dependencies::checkDependencies()
{
    QString ffmpegLoc = findDir("ffmpeg","-version");
    QList<bool> x264;
    QList<bool> x265;
    bool ffmpegFail;
    if (!ffmpegLoc.contains("Not Found"))
    {
        ffmpegFail = checkEXEC(ffmpegLoc, {"-h", "encoder=libx264"});
        x264 = {hibit10, hibit12};

        ffmpegFail = checkEXEC(ffmpegLoc, {"-h", "encoder=libx265"});
        x265 = {hibit10, hibit12};
    }
    else
    {
        x264 = {false,false};
        x265 = {false,false};
    }
    QString vspipeLoc = findDir("vspipe","--version");

    checkmedia cm;
    QStringList depStatus = { cm.checkFormats(vspipeLoc.contains("Not Found")), vspipeLoc, ffmpegLoc, QString::number(x264[0]), QString::number(x264[1]), QString::number(x265[0]), QString::number(x265[1]) };
    return depStatus;
}

QString dependencies::findDir(QString executable, QString version)
{
    QString execDir = qApp->applicationDirPath() + "/" + executable;
    if (OS == "Windows")
        execDir = execDir + ".exe";
    bool pathfail = checkEXEC(execDir, {version});

    if (OS == "Unix" && pathfail)
    {
        pathfail = checkEXEC("/usr/local/bin/" + executable, {"-version"});
        if (!pathfail)
            execDir = "/usr/local/bin/" + executable;
    }

    if (OS == "Unix" && pathfail)
    {
        pathfail = checkEXEC("/usr/bin/" + executable, {"-version"});
        if (!pathfail)
            execDir = "/usr/bin/" + executable;
    }

    if (pathfail)
    {
        execDir = "Not Found: " + execDir;
    }
    return execDir;
}

bool dependencies::checkEXEC(QString execDir, QStringList execCommand)
{
    execfail = false;
    exec = new QProcess(this);
    connect(exec, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutput()));
    connect(exec, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(readErrors()));
    exec->setProcessChannelMode(QProcess::MergedChannels);
    exec->start(execDir, execCommand);
    exec->waitForFinished();
    exec->deleteLater();
    return execfail;
}


void dependencies::readErrors()
{
    execfail = true;
    exec->kill();
}

void dependencies::readOutput()
{
    while (exec->canReadLine())
    {
        QString execline = exec->readLine();
        if (execline.contains("Supported pixel formats: "))
        {
            if (execline.contains("10le"))
                hibit10 = true;
            else
                hibit10 = false;
            if (execline.contains("12le"))
                hibit12 = true;
            else
                hibit12 = false;
        }
    }
}
