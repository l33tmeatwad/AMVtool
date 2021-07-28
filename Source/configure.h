#ifndef CONFIGURE_H
#define CONFIGURE_H
#include <QDialog>
#include <QString>
#include "filesettings.h"


namespace Ui {
class configure;
}

class configure : public QDialog
{
    Q_OBJECT

public:
    explicit configure(QWidget *parent = 0);
    ~configure();
    void setData(const int &selFile, QList<QStringList> inputMediaInfo, const QStringList configurationList, QList<int> hibitdepth);

private slots:
    void on_buttonBox_accepted();

    void on_selectContainer_currentIndexChanged();

    void on_selectCodec_currentIndexChanged();

    void on_selectMode_currentIndexChanged();

    void on_selectVideoStream_currentIndexChanged(int index);

    void on_selectColorSpace_currentIndexChanged();

    void on_selectAudioStream_currentIndexChanged(int index);

    void on_selectAudioMode_currentIndexChanged();

    void on_selectAudioCodec_currentIndexChanged();

    void on_browseOutput_clicked();

    void on_externalAudio_toggled(bool checked);

    void on_browseAudio_clicked();

    void on_copyAudio_toggled(bool checked);

    void on_encodeIncompatible_toggled();

    void on_recontainer_clicked();

    void on_copyVideo_toggled(bool checked);

    void on_selectBitDepth_currentIndexChanged();

private:
    Ui::configure *ui;    
    filesettings fs;

    int selectedFile;
    bool newExtAudio = false;

    QList<bool> x264bitdepth;
    QList<bool> x265bitdepth;
    QString selectedDepth;

    int inputVideoStreams;
    int inputAudioStreams;
    QString inputContainer;
    int altAudioStreams;

    QStringList inputVideoBitDepths;
    QStringList inputVideoCodecs;
    QStringList inputVideoHeight;
    QStringList inputColorSpaces;
    QStringList inputColorMatrix;
    QStringList inputAudioCodecs;
    QStringList altAudioCodecs;

    void enableBitDepth(QList<int> hibitdepth);
    void setVideoStream();
    void setVideoVisibility();
    void setAudioVisibility();
    void setColorSpace();
    void setColorMatrix();
    void setBitDepth();
    void setPreset();
    void setMode();
    void setTune();
    void setVideoCodec();
    void setContainer();

    void setAudioStream();
    void setAudioCodec();
    void setAudioMode();
    void setAudioBitrate();
    void canCopyAudio(QStringList containerinfo);
    bool cancopyaudio = false;
    void getAltAudioCodecs(QString newAudio);
    bool externalaudio = false;


    QString outputLocation;
    QString outputContainer;
    int outputVideoStream;
    QString outputColorSpace;
    QString outputBitDepth;
    QString outputColorMatrix;
    QString outputVideoCodec;
    QString videoEncMode;
    QString videoEncPreset;
    QString videoEncTune;
    int videoEncBitrate;
    QString outputAudioSource;
    QString outputAudioStream;
    QString outputAudioCodec;
    QString audioEncMode;
    int audioEncBitrate;


    int vsIndex;
    int asIndex;
    QStringList AVI = { "ULRG", "ULRA", "ULY0", "ULH0", "ULY2", "ULH2", "YQY2", "XviD", "MPEG Audio", "PCM"};
    QStringList MOV = { "AVC", "DNxHD", "HEVC", "MPEG-4", "ProRes", "ULRG", "ULRA", "ULY0", "ULH0", "ULY2", "ULH2", "YQY2", "AAC", "AC-3", "PCM"};
    QStringList MP4 = { "AVC", "HEVC", "MPEG-4", "AAC", "AC-3", "MPEG Audio"};
};

#endif // CONFIGURE_H
