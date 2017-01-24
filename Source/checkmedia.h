#ifndef CHECKMEDIA_H
#define CHECKMEDIA_H
#include <QApplication>
#include <QObject>
#include <QProcess>

#ifdef WIN32
#define _UNICODE
#define vspipeexec qApp->applicationDirPath() + "/" + "vspipe.exe"
#else
#define vspipeexec "/usr/local/bin/vspipe"
#endif

class checkmedia : public QObject
{

    Q_OBJECT

public:
    checkmedia(QObject *parent = 0);
    QList<QStringList> getMediaInfo(QString inputFile);
    QList<QStringList> checkVPY(QString inputScript);



private slots:
    void setVPYDetails(int, QProcess::ExitStatus);
    void readOutput();

private:
        QString vpyWidth;
        QString vpyHeight;
        QString vpyFrames;
        QString vpyFPS;
        QString vpyColorSpace;
        bool vpyfail;
        QProcess *vspipe;
        QList<QStringList> inputMediaInfo;

        int inputVideoStreams;
        int inputAudioStreams;
        QString inputContainer;
        QStringList inputVideoStreamIDs;
        QStringList inputVideoCodecs;
        QStringList inputColorSpaces;
        QStringList inputColorMatrix;
        QStringList inputVideoWidth;
        QStringList inputVideoHeight;
        int inputDuration;
        QStringList inputFPS;
        QStringList inputAudioStreamIDs;
        QStringList inputAudioCodecs;

};

#endif // CHECKMEDIA_H
