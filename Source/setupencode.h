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
    QStringList getTilesAndThreads(int height);
    QString SetupFilters(bool isHDR, bool convertHDR, QString inputcolorspace, QString colorspace, QString colormatrix, QString deinterlace, int cthresh, QString fieldorder, QString resize, QString aspectratio);
    QString getAudioCodecName(QString codecname);

    QString outputfile;
    filesettings fs;

    QStringList WidthResolutions = {"640", "854", "960", "1280", "1440", "1920", "2880", "3840", "5760", "7680"};
    QStringList HeightResolutions = {"480", "720", "1080", "2160", "4320"};
};

#endif // SETUPENCODE_H
