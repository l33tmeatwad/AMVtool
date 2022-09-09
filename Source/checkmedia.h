#ifndef CHECKMEDIA_H
#define CHECKMEDIA_H
#include <QApplication>
#include <QObject>
#include <QProcess>

extern QString vspipeexec;

class checkmedia : public QObject
{

    Q_OBJECT

public:
    QString checkFormats(bool vpyfail);
    checkmedia(QObject *parent = 0);
    QList<QStringList> checkMedia(QString inputFile);

private slots:
    void readErrors();
    void readOutput();

private:
    QList<QStringList> getMediaInfo(QString inputFile);
    QList<QStringList> checkVPY(QString inputScript);
    void setVPYDetails();

    QString vpyWidth;
    QString vpyHeight;
    QString vpyFrames;
    QString vpyFPS;
    QString vpyColorSpace;
    QString vpyBitDepth;
    bool vpyfail;
    QProcess *vspipe;
    QList<QStringList> inputMediaInfo;

    int inputVideoStreams;
    int inputAudioStreams;
    float inputDuration;
    QString inputContainer;
    QStringList inputVideoStreamIDs;
    QStringList inputVideoBitDepths;
    QStringList inputLumaRange;
    QStringList inputVideoCodecs;
    QStringList inputColorSpaces;
    QStringList inputColorMatrix;
    QStringList inputVideoWidth;
    QStringList inputVideoHeight;
    QStringList inputFPS;
    QStringList inputAudioStreamIDs;
    QStringList inputAudioCodecs;

};

#endif // CHECKMEDIA_H
