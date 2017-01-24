#ifndef SETUPENCODE_H
#define SETUPENCODE_H

#include <QObject>
#include <QApplication>
#include "filesettings.h"

#ifdef WIN32
#define ffmpegexec qApp->applicationDirPath() + "/" + "ffmpeg.exe"
#define vspipeexec qApp->applicationDirPath() + "/" + "vspipe.exe"
#define pass1null "NUL"
#else
#define ffmpegexec qApp->applicationDirPath() + "/" + "ffmpeg"
#define vspipeexec "/usr/local/bin/vspipe"
#define pass1null "/dev/null"
#endif

extern QString encodepass;

class setupencode : public QObject
{
    Q_OBJECT
public:
    explicit setupencode(QObject *parent = 0);
    QString Encoder();
    QStringList SetupEncode(int queue, QStringList fileInfo, QList<QStringList> inputDetails, QStringList configList);
    QString VSPipe();
    QStringList SetupPipe(QString inputFile, QString colorspace);

signals:

public slots:

private:
    bool canCopyAudio(QString container, QString format);
    QString OutputFile(QString originalfile, QString newtype);
    QString getCodecName(QString codecname);
    QString getColorSpace(QString colorspace);
    QString getColorMatrix(QString colormatrix);
    QString getAudioCodecName(QString codecname);

    QString outputfile;
    filesettings fs;
};

#endif // SETUPENCODE_H
