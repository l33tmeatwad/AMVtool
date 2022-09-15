#ifndef SETUPENCODE_H
#define SETUPENCODE_H

#include <QObject>
#include <QApplication>
#include "filesettings.h"

#ifdef WIN32
#define pass1null "NUL"
#else
#define pass1null "/dev/null"
#endif

class setupencode : public QObject
{
    Q_OBJECT
public:
    explicit setupencode(QObject *parent = 0);
    QStringList SetupEncode(int queue, QStringList fileInfo, QList<QStringList> inputDetails, QStringList configList);
    QString VSPipe();
    QStringList SetupPipe(QString inputFile, QString colorspace);

signals:

public slots:

private:
    bool canCopyAudio(QString container, QString format);
    QString OutputFile(QString originalfile, QString originalType, QString newtype);
    QString getCodecName(QString codecname);
    QString getColorSpace(QString colorspace, QString bitdepth);
    QString getColorMatrix(QString colormatrix);
    QString getProfile(QString codec, QString bitdepth, QString profile);
    QString getAudioCodecName(QString codecname);

    QString outputfile;
    filesettings fs;
};

#endif // SETUPENCODE_H
