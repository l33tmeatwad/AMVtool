#include "../checkmedia.h"
#include <QDir>
#include "../MediaInfoDLL/MediaInfoDLL.h"
#define MediaInfoNameSpace MediaInfoDLL;
#include <iostream>
#include <iomanip>

typedef struct AVSDLLVideoInfo {
    int mt_import;

    // Video
    int width;
    int height;
    int raten;
    int rated;
    int num_frames;
    int field_based;
    int first_field;
    int pixel_type_orig;
    int pixel_type;

    // Audio
    int audio_samples_per_second;
    int sample_type_orig;
    int sample_type;
    int nchannels;
    int64_t num_audio_samples;
} AVSDLLVideoInfo;

AVSDLLVideoInfo *vi;

extern "C" {
__declspec(dllimport) int __stdcall dimzon_avs_init(void* avs, char *func, char *arg, AVSDLLVideoInfo *vi);
__declspec(dllexport) int __stdcall dimzon_avs_destroy(void* avs);
}

using namespace MediaInfoNameSpace;

#ifdef __MINGW32__
    #ifdef _UNICODE
        #define _itot _itow
    #else //_UNICODE
        #define _itot itoa
    #endif //_UNICODE
#endif //__MINGW32

#ifdef WIN32
#define OStype "WIN32"
#else
#define OStype "Other"
#endif

checkmedia::checkmedia(QObject *parent) : QObject(parent)
{

}

QString checkmedia::checkFormats()
{
    QString mediaformats = "Media Files (*.avi *.avs *m2ts *.m4v *.mov *.mkv *.mp4 *ts *.vpy)";
    vpyfail = false;
    vspipe = new QProcess(this);
    QStringList vspipecommand = { "--version" };
    connect(vspipe, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutput()));
    connect(vspipe, SIGNAL(error(QProcess::ProcessError)), this, SLOT(readErrors()));
    vspipe->setProcessChannelMode(QProcess::MergedChannels);
    vspipe->start(vspipeexec, vspipecommand);
    vspipe->waitForFinished();
    vspipe->deleteLater();

    if (vpyfail == true)
    {
        mediaformats.replace(" *.vpy", "");
    }

    QString funcstr = "";
    QByteArray func = funcstr.toLocal8Bit();
    void* avs = new intptr_t(0);
    int avisynth = dimzon_avs_init(avs,func.data(), func.data(), vi);
    dimzon_avs_destroy(avs);
    delete avs;

    if (avisynth != 0)
    {
            mediaformats.replace(" *.avs", "");
    }

    return mediaformats;
}


QList<QStringList> checkmedia::checkMedia(QString inputFile)
{
    if (inputFile.right(3).toLower() == "vpy")
    {
        inputMediaInfo = checkVPY(inputFile);
    }
    else
    {
        if (inputFile.right(3).toLower() == "avs")
        {
            inputMediaInfo = checkAVS(inputFile);
        }
        else
        {
            inputMediaInfo = getMediaInfo(inputFile);
        }
    }
    return inputMediaInfo;
}


QList<QStringList> checkmedia::getMediaInfo(QString inputFile)
{
    QStringList inputMediaDetails;
    MediaInfo MI;
    std::wstring mediaFile = inputFile.toStdWString();
    if (MI.Open(mediaFile))
    {

        inputContainer = QString::fromStdWString(MI.Get(Stream_General, 0, __T("Format_Profile"), Info_Text, Info_Name).c_str());
        if (inputContainer.toLower() != "quicktime")
        {
            inputContainer = QString::fromStdWString(MI.Get(Stream_General, 0, __T("Format"), Info_Text, Info_Name).c_str());
        }
        inputDuration = QString::fromStdWString(MI.Get(Stream_General, 0, __T("Duration"), Info_Text, Info_Name).c_str()).toInt();
        inputVideoStreams = QString::fromStdWString(MI.Get(Stream_General,0, __T("VideoCount"), Info_Text, Info_Name).c_str()).toInt();
        inputAudioStreams = QString::fromStdWString(MI.Get(Stream_General,0, __T("AudioCount"), Info_Text, Info_Name).c_str()).toInt();

        inputMediaDetails.append({ QString::number(inputVideoStreams),QString::number(inputAudioStreams),inputContainer, QString::number(inputDuration) });
        for (int i = 0; i < inputVideoStreams; i++)
        {
            int videoID = QString::fromStdWString(MI.Get(Stream_Video, i, __T("ID"), Info_Text, Info_Name).c_str()).toInt();
            if (inputContainer == "AVI")
            {
                videoID = videoID + 1;
            }


            inputVideoStreamIDs.append(QString::number(videoID));
            QString bitdepth = QString::fromStdWString(MI.Get(Stream_Video, i, __T("BitDepth"), Info_Text, Info_Name).c_str());
            QString VideoCodec = QString::fromStdWString(MI.Get(Stream_Video, i, __T("Format"), Info_Text, Info_Name).c_str());
            if (VideoCodec == "YUV" || VideoCodec == "RGBA" || VideoCodec == "RGB")
                VideoCodec = QString::fromStdWString(MI.Get(Stream_Video, i, __T("CodecID"), Info_Text, Info_Name).c_str());

            QStringList UTcodec = { "ULRG", "ULRA", "UQRG", "UQRA", "ULY0", "ULY2", "ULH0", "ULH2", "UQY2" };
            foreach (QString codec, UTcodec)
            {
                if (VideoCodec.contains(codec))
                {
                    VideoCodec = codec;
                    if (VideoCodec.contains("Q"))
                    {
                        bitdepth = "10";
                    }
                    else
                    {
                        bitdepth = "8";
                    }
                }
            }
            inputVideoCodecs.append(VideoCodec);

            QString colorspace = QString::fromStdWString(MI.Get(Stream_Video, i, __T("ColorSpace"), Info_Text, Info_Name).c_str());
            QString colormatrix = QString::fromStdWString(MI.Get(Stream_Video, i, __T("matrix_coefficients"), Info_Text, Info_Name).c_str());

            if (inputContainer.toLower() == "quicktime")
            {
                if (inputVideoCodecs[i] == "ULRG")
                {
                    colorspace = "RGB24";
                }
                if (inputVideoCodecs[i] == "ULRA")
                {
                    colorspace = "RGBA";
                }
                if (inputVideoCodecs[i] == "ULY0" || inputVideoCodecs[i] == "ULY2")
                {
                    colorspace = "YUV";
                    colormatrix = "BT.601";
                }
                if (inputVideoCodecs[i] == "ULH0" || inputVideoCodecs[i] == "ULH2" || inputVideoCodecs[i] == "UQY2")
                {
                    colorspace = "YUV";
                    colormatrix = "BT.709";
                }
            }
            if (colorspace.toLower() == "yuv")
            {
                colorspace.append(QString::fromStdWString(MI.Get(Stream_Video, i, __T("ChromaSubsampling"), Info_Text, Info_Name).c_str())).replace(":","");
                colorspace.append("P" + QString::fromStdWString(MI.Get(Stream_Video, i, __T("BitDepth"), Info_Text, Info_Name).c_str()));

            }

            inputColorSpaces.append(colorspace);
            inputColorMatrix.append(colormatrix);

            if (bitdepth == "")
            {
                bitdepth = "Unknown";
            }
            else
            {
                bitdepth = bitdepth + "bit";
            }
            inputVideoBitDepths.append(bitdepth);


            inputVideoWidth.append(QString::fromStdWString(MI.Get(Stream_Video, i, __T("Width"), Info_Text, Info_Name).c_str()));
            inputVideoHeight.append(QString::fromStdWString(MI.Get(Stream_Video, i, __T("Height"), Info_Text, Info_Name).c_str()));
            inputFPS.append(QString::fromStdWString(MI.Get(Stream_Video, i, __T("FrameRate"), Info_Text, Info_Name).c_str()));
        }
        for (int i = 0; i < inputAudioStreams; i++)
        {
            int audioID = QString::fromStdWString(MI.Get(Stream_Audio, i, __T("ID"), Info_Text, Info_Name).c_str()).toInt();
            if (inputContainer == "AVI")
            {
                audioID = audioID + 1;
            }
            inputAudioStreamIDs.append(QString::number(audioID));

            inputAudioCodecs.append(QString::fromStdWString(MI.Get(Stream_Audio, i, __T("Format"), Info_Text, Info_Name).c_str()));
        }

    }
    else
    {
        inputMediaDetails.append({ "0", "0", "Error", "0" });
    }
    MI.Close();
    inputMediaInfo = { inputMediaDetails, inputVideoStreamIDs, inputVideoBitDepths, inputVideoCodecs, inputColorSpaces, inputColorMatrix, inputVideoWidth, inputVideoHeight, inputFPS, inputAudioStreamIDs, inputAudioCodecs };

    return inputMediaInfo;
}


QList<QStringList> checkmedia::checkAVS(QString inputScript)
{
    QStringList inputMediaDetails;
    QString funcstr = "Import";
    QByteArray func = funcstr.toLocal8Bit();

    QByteArray file = inputScript.toLocal8Bit();

    void* avs = new intptr_t(0);

    AVSDLLVideoInfo *vinfo = new AVSDLLVideoInfo();
    int avisynth = dimzon_avs_init(avs,func.data(), file.data(), vi);
    dimzon_avs_destroy(avs);
    delete avs;

    if (0 == avisynth)
    {
        float fpsnum = vinfo->raten;
        float fpsden = vinfo->rated;
        float avsframes = vinfo->num_frames;
        int duration = (avsframes/(fpsnum/fpsden))*1000;

        inputVideoCodecs.append("Script");
        inputColorSpaces.append("RGB");
        inputColorMatrix.append("DETECT");
        inputVideoWidth.append(QString::number(vinfo->width));
        inputVideoHeight.append(QString::number(vinfo->height));
        inputFPS.append(QString::number(vinfo->raten) + "/" + QString::number(vinfo->rated));
        inputDuration = duration;

        if (vinfo->nchannels > 0)
        {
            inputAudioCodecs.append("AviSynth");
            inputAudioStreams = 1;
        }
        else
        {
            inputAudioStreams = 0;
        }


        inputMediaDetails.append({ "1", QString::number(inputAudioStreams), "AviSynth", QString::number(duration) });
    }
    else
    {
        inputMediaDetails.append({ "0", "0", "Error", "0" });
    }
    delete vinfo;

    inputMediaInfo = { inputMediaDetails, {"1"}, {"8bit"}, inputVideoCodecs, inputColorSpaces, inputColorMatrix, inputVideoWidth, inputVideoHeight, inputFPS, {"2"}, inputAudioCodecs };
    return inputMediaInfo;

}


QList<QStringList> checkmedia::checkVPY(QString inputScript)
{

    vpyfail = false;
    vspipe = new QProcess(this);
    QStringList vspipecommand = { "--info", inputScript, "-" };
    connect(vspipe, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutput()));
    connect(vspipe, SIGNAL(error(QProcess::ProcessError)), this, SLOT(readErrors()));
    vspipe->setProcessChannelMode(QProcess::MergedChannels);
    vspipe->start(vspipeexec, vspipecommand);
    vspipe->waitForFinished();
    vspipe->deleteLater();
    setVPYDetails();

    return inputMediaInfo;
}

void checkmedia::setVPYDetails()
{
    QStringList inputMediaDetails;
    if (vpyfail == false)
    {


        float fpsnum = vpyFPS.left(vpyFPS.indexOf("/")).toInt();
        float fpsden = vpyFPS.right(vpyFPS.indexOf("/")-1).toInt();
        int duration = (vpyFrames.toFloat()/(fpsnum/fpsden))*1000;

        inputVideoBitDepths.append(vpyBitDepth + "bit");
        inputVideoCodecs.append("Script");
        inputColorSpaces.append(vpyColorSpace);
        inputColorMatrix.append("DETECT");
        inputVideoWidth.append(vpyWidth);
        inputVideoHeight.append(vpyHeight);
        inputFPS.append(vpyFPS);
        inputDuration = duration;

        inputMediaDetails.append({ "1", "0", "VapourSynth", QString::number(duration) });

    }
    else
    {
        inputMediaDetails.append({ "0", "0", "Error", "0" });
    }

    inputMediaInfo = { inputMediaDetails, {"1"}, inputVideoBitDepths, inputVideoCodecs, inputColorSpaces, inputColorMatrix, inputVideoWidth, inputVideoHeight, inputFPS, {"0"}, inputAudioCodecs };

}

void checkmedia::readErrors()
{
    vpyfail = true;
    vspipe->kill();
}

void checkmedia::readOutput()
{
    while (vspipe->canReadLine())
    {
        QString vsline = vspipe->readLine();
        if (vsline.contains("Script evaluation failed") || vsline.contains("Library not loaded"))
        {
            vpyfail = true;
        }
        if (vsline.contains("Width") )
        {
            vpyWidth = vsline.simplified().replace("Width: ", "");
        }
        if (vsline.contains("Height") )
        {
            vpyHeight = vsline.simplified().replace("Height: ", "");
        }
        if (vsline.contains("Frames") )
        {
            vpyFrames = vsline.simplified().replace("Frames: ", "");
        }
        if (vsline.contains("FPS") )
        {
            vpyFPS = vsline.simplified().replace("FPS: ", "");
            vpyFPS = vpyFPS.left(vpyFPS.indexOf("(")-1);
        }
        if (vsline.contains("Format Name") )
        {
            vpyColorSpace = vsline.simplified().replace("Format Name: ", "");
        }
        if (vsline.contains("Bits: ") )
        {
            vpyBitDepth = vsline.simplified().replace("Bits: ", "");
        }
    }
}
