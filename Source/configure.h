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

    void on_selectBitDepth_currentIndexChanged();

    void on_convertHDR_toggled();

    void on_convertHDR_clicked(bool checked);

    void on_interlaceOptions_currentIndexChanged(int index);

    void on_resizeVideo_toggled();

    void on_resizeResolution_toggled();

    void on_resizeHeight_toggled();

    void on_resizeWidth_toggled();

    void on_changeDAR_toggled(bool checked);

    void on_modifyMC_toggled();

private:
    Ui::configure *ui;    
    filesettings fs;

    int selectedFile;
    bool newExtAudio = false;

    QList<bool> x264bitdepth;
    QList<bool> x265bitdepth;
    QList<bool> vp9bitdepth;
    QString selectedDepth;

    int inputVideoStreams;
    int inputAudioStreams;
    QString inputContainer;
    int altAudioStreams;

    QStringList inputVideoBitDepths;
    QStringList inputLumaRange;
    QStringList inputScanType;
    QStringList inputVideoCodecs;
    QStringList inputVideoHeight;
    QStringList inputColorSpaces;
    QStringList inputColorMatrix;
    QStringList inputAudioCodecs;
    QStringList inputAudioLayouts;
    QStringList altAudioCodecs;
    QStringList altAudioLayouts;

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
    void setLumaConversion();
    void setContainer();

    void setAudioStream();
    void setAudioCodec();
    void setAudioMode();
    void setAudioBitrate();
    void canCopyAudio(QStringList containerinfo);
    bool cancopyaudio = false;
    void getAltAudioInfo(QString newAudio);
    bool externalaudio = false;

    void setInterlaceVisibility(int index);
    void setResizeOptions ();

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
    QString deinterlaceType;
    int cthreshValue;
    QString outputFieldOrder;
    QString outputResize;
    QString outputAR;
    int audioEncBitrate;
    int convertHDR2SDR;


    int vsIndex;
    int asIndex;
    QStringList AVI = { "ULRG", "ULRA", "ULY0", "ULH0", "ULY2", "ULH2", "YQY2", "XviD", "MPEG Audio", "PCM"};
    QStringList MOV = { "AVC", "DNxHR", "HEVC", "MPEG-4", "ProRes", "ULRG", "ULRA", "ULY0", "ULH0", "ULY2", "ULH2", "YQY2", "AAC", "AC-3", "ALAC", "PCM"};
    QStringList MP4 = { "AVC", "HEVC", "MPEG-4", "AAC", "AC-3", "MPEG Audio"};
    QStringList WEBM = { "VP8","VP9", "Opus","Vorbis"};

    QStringList FullResolutions = {"640x480", "854x480", "960x720", "1280x720", "1440x1080", "1920x1080", "2880x2160", "3840x2160", "5760x4320", "7680x4320"};
    QStringList WidthResolutions = {"640", "854", "960", "1280", "1440", "1920", "2880", "3840", "5760", "7680"};
    QStringList HeightResolutions = {"480", "720", "1080", "2160", "4320"};
};

#endif // CONFIGURE_H
