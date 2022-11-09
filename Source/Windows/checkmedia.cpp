#include "../checkmedia.h"
#include <QDir>
#include <MediaInfoDLL/MediaInfoDLL.h>
#define MediaInfoNameSpace MediaInfoDLL;
#include <iostream>
#include <iomanip>

QString checkColorMatrix(QString matrix, QString videoWidth, QString videoHeight);
QString AVScolorspace(int colornum);
QList<QStringList> checkAVS(QString inputScript);

typedef struct AVSDLLVideoInfo {
    // Video
    int width;
    int height;
    int raten;
    int rated;
    int aspectn;
    int aspectd;
    int interlaced_frame;
    int top_field_first;
    int num_frames;
    int pixel_type;

    // Audio
    int audio_samples_per_second;
    int num_audio_samples;
    int sample_type;
    int nchannels;
} AVSDLLVideoInfo;

AVSDLLVideoInfo *vinfo = new AVSDLLVideoInfo();

extern "C" {
__declspec(dllexport) int __stdcall dimzon_avs_init(intptr_t* ppstr, char *func, char *arg, AVSDLLVideoInfo *vi, int* originalPixelType, int* originalSampleType, char *cs);
__declspec(dllexport) int __stdcall dimzon_avs_destroy(intptr_t* avs);
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

QString checkmedia::checkFormats(bool vpyfail)
{
    QString mediaformats = "Media Files (*.avi *.avs *.m2ts *.m4v *.mkv *.mov *.mp4 *.mpg *.mts *.ts *.vob *.vpy *.webm)";
    if (vpyfail == true)
    {
        mediaformats.replace(" *.vpy", "");
    }
    intptr_t* avs = new intptr_t(0);
    QByteArray funcstr("IsString");
    char *func = funcstr.data();
    QByteArray argstr("");
    char *arg = argstr.data();
    int* opt = new int(0);
    int* ost = new int(0);
    QByteArray csstr("");
    char *cs = csstr.data();
    int avisynth = dimzon_avs_init(avs,func, arg, vinfo, opt, ost, cs);
    dimzon_avs_destroy(avs);

    if (avisynth == 1)
    {
            mediaformats.replace(" *.avs", "");
    }
    return mediaformats;
}

QString checkColorMatrix(QString colormatrix, QString videoWidth, QString videoHeight)
{
    colormatrix.replace(" non-constant","NC");
    colormatrix.replace(" constant","C");
    QStringList matrixlist = {"BT.601","BT.709","BT.2020NC","BT.2020C"};
    if (!matrixlist.contains(colormatrix))
    {
        if (videoWidth.toInt() > 940 || videoHeight.toInt() > 580)
        {
            colormatrix = "BT.709";
        }
        else
        {
            colormatrix = "BT.601";
        }
    }

    return colormatrix;
}

QList<QStringList> checkmedia::checkMedia(QString inputFile)
{
    inputVideoStreamIDs.clear();
    inputVideoBitDepths.clear();
    inputVideoCodecs.clear();
    inputColorSpaces.clear();
    inputColorMatrix.clear();
    inputVideoWidth.clear();
    inputVideoHeight.clear();
    inputFPS.clear();
    inputAudioStreamIDs.clear();
    inputAudioCodecs.clear();
    
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
        QStringList TSVOB = { "BDAV", "MPEG-PS", "MPEG-TS" };
        QStringList acontainers = { "ADTS", "AVI", "MPEG Audio", "Wave" };

        inputContainer = QString::fromStdWString(MI.Get(Stream_General, 0, __T("Format_Profile"), Info_Text, Info_Name).c_str());
        if (inputContainer.toLower() != "quicktime")
        {
            inputContainer = QString::fromStdWString(MI.Get(Stream_General, 0, __T("Format"), Info_Text, Info_Name).c_str());
        }
        inputDuration = QString::fromStdWString(MI.Get(Stream_General, 0, __T("Duration"), Info_Text, Info_Name).c_str()).toFloat();
        inputVideoStreams = QString::fromStdWString(MI.Get(Stream_General,0, __T("VideoCount"), Info_Text, Info_Name).c_str()).toInt();
        inputAudioStreams = QString::fromStdWString(MI.Get(Stream_General,0, __T("AudioCount"), Info_Text, Info_Name).c_str()).toInt();

        for (int i = 0; i < inputVideoStreams; i++)
        {
            QString videoID;
            if (TSVOB.contains(inputContainer) || inputContainer == "AVI")
            {
                videoID = QString::fromStdWString(MI.Get(Stream_Video, i, __T("ID"), Info_Text, Info_Name).c_str());
            }
            else
            {
                videoID = QString::fromStdWString(MI.Get(Stream_Video, i, __T("StreamOrder"), Info_Text, Info_Name).c_str());
            }

            if (TSVOB.contains(inputContainer))
            {
                QString IDfix = QString::fromStdWString(MI.Get(Stream_Video, i, __T("ID/String"), Info_Text, Info_Name).c_str());
                videoID = IDfix.replace(videoID, "").replace("(","").replace(")", "").replace(" ","");
                if (videoID.contains("0xE"))
                {
                    videoID = videoID.replace("0x", "0x1");
                }
            }


            inputVideoStreamIDs.append(videoID);
            QString VideoCodec = QString::fromStdWString(MI.Get(Stream_Video, i, __T("Format_Commercial"), Info_Text, Info_Name).c_str());
            if (VideoCodec == "YUV" || VideoCodec == "RGBA" || VideoCodec == "RGB")
                VideoCodec = QString::fromStdWString(MI.Get(Stream_Video, i, __T("CodecID"), Info_Text, Info_Name).c_str());

            if (VideoCodec.contains("DNxH"))
                VideoCodec = "DNxHR";

            QString bitdepth;
            QString colorspace;
            QString colormatrix;
            QStringList UTcodec = { "ULRG", "UMRG", "ULRA", "UMRA", "UQRG", "UQRA", "ULY0", "ULY2", "UMY2", "ULY4", "UMY4", "ULH0", "ULH2", "UMH2", "ULH4", "UMH4", "UQY2" };


            if (UTcodec.contains(VideoCodec))
            {
                if (VideoCodec.contains("Q"))
                {
                    bitdepth = "10";
                }
                else
                {
                    bitdepth = "8";
                }
                if (VideoCodec.contains("R"))
                {
                    if (VideoCodec.contains("A"))
                        colorspace = "RGBA";
                    else
                        colorspace = "RGB24";
                }
                else
                {
                    if (VideoCodec.contains("0"))
                        colorspace = "YUV420";
                    if (VideoCodec.contains("2"))
                        colorspace = "YUV422";
                    if (VideoCodec.contains("4"))
                        colorspace = "YUV444";
                    if (VideoCodec.contains("ULY") || VideoCodec.contains("UMY"))
                        colormatrix = "BT.601";
                    if (VideoCodec.contains("ULH") || VideoCodec.contains("UMH"))
                        colormatrix = "BT.709";


                }

            }
            else
            {
                bitdepth = QString::fromStdWString(MI.Get(Stream_Video, i, __T("BitDepth"), Info_Text, Info_Name).c_str());
                colorspace = QString::fromStdWString(MI.Get(Stream_Video, i, __T("ColorSpace"), Info_Text, Info_Name).c_str());
                if (colorspace.toLower() == "yuv")
                    colorspace.append(QString::fromStdWString(MI.Get(Stream_Video, i, __T("ChromaSubsampling"), Info_Text, Info_Name).c_str())).replace(":","");
                if (VideoCodec == "ProRes")
                {
                    if (colorspace == "")
                    {
                        colorspace = "YUV";
                        colorspace.append(QString::fromStdWString(MI.Get(Stream_Video, i, __T("ChromaSubsampling"), Info_Text, Info_Name).c_str())).replace(":","");
                    }
                    bitdepth = "10";
                }
                colormatrix = QString::fromStdWString(MI.Get(Stream_Video, i, __T("matrix_coefficients"), Info_Text, Info_Name).c_str());
            }
            inputVideoCodecs.append(VideoCodec);

            if (inputContainer == "MPEG-4")
                inputContainer = "MP4";

            QString videowidth = QString::fromStdWString(MI.Get(Stream_Video, i, __T("Width"), Info_Text, Info_Name).c_str());
            QString videoheight = QString::fromStdWString(MI.Get(Stream_Video, i, __T("Height"), Info_Text, Info_Name).c_str());


            colormatrix = checkColorMatrix(colormatrix, videowidth, videoheight);

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


            inputVideoWidth.append(videowidth);
            inputVideoHeight.append(videoheight);
            inputFPS.append(QString::fromStdWString(MI.Get(Stream_Video, i, __T("FrameRate"), Info_Text, Info_Name).c_str()));
        }
        int audioCount = inputAudioStreams;
        int trueHDCount = 0;
        for (int i = 0; i < audioCount; i++)
        {
            QString audioID;
            if (TSVOB.contains(inputContainer) || acontainers.contains(inputContainer))
            {
                audioID = QString::fromStdWString(MI.Get(Stream_Audio, i, __T("ID"), Info_Text, Info_Name).c_str());
                if (audioID == "")
                    audioID = "0";
            }
            else
            {
                audioID = QString::fromStdWString(MI.Get(Stream_Audio, i, __T("StreamOrder"), Info_Text, Info_Name).c_str());
            }

            if (TSVOB.contains(inputContainer))
            {
                QString IDfix = QString::fromStdWString(MI.Get(Stream_Audio, i, __T("ID/String"), Info_Text, Info_Name).c_str());

                if (IDfix.count("x") > 1)
                {
                    audioID = IDfix.right(5).left(4);
                }
                else
                {
                    audioID = IDfix.replace(audioID, "").replace("(", "").replace(")","").replace(" ","");
                    if (audioID.contains("0xC") || audioID.contains("0xD"))
                    {
                        audioID = audioID.replace("0x", "0x1");
                    }
                }
            }

            QString audiocodec = QString::fromStdWString(MI.Get(Stream_Audio, i, __T("Format"), Info_Text, Info_Name).c_str());

            if (audiocodec.contains("MLP FBA") && inputContainer.contains("BDAV"))
            {
                int astream = inputVideoStreams+i+trueHDCount;
                trueHDCount = trueHDCount+1;
                inputAudioStreamIDs.append({QString::number(astream),QString::number(astream+1)});
                inputAudioCodecs.append({"TrueHD","AC-3"});
                inputAudioStreams = inputAudioStreams+1;
            }
            else
            {
                inputAudioStreamIDs.append(audioID);
                inputAudioCodecs.append(audiocodec);
            }

        }

    }
    else
    {
        inputMediaDetails.append({ "0", "0", "Error", "0" });
    }
    MI.Close();

    inputMediaDetails.append({ QString::number(inputVideoStreams),QString::number(inputAudioStreams),inputContainer, QString::number(inputDuration) });
    inputMediaInfo = { inputMediaDetails, inputVideoStreamIDs, inputVideoBitDepths, inputVideoCodecs, inputColorSpaces, inputColorMatrix, inputVideoWidth, inputVideoHeight, inputFPS, inputAudioStreamIDs, inputAudioCodecs };

    return inputMediaInfo;
}


QList<QStringList> checkAVS(QString inputScript)
{
    int inputAudioStreams;
    QStringList inputVideoCodecs;
    QStringList inputColorSpaces;
    QStringList inputColorMatrix;
    QStringList inputVideoWidth;
    QStringList inputVideoHeight;
    QStringList inputFPS;
    QStringList inputAudioCodecs;
    QStringList inputMediaDetails;

    intptr_t* avs = new intptr_t(0);
    QByteArray funcstr("Import");
    char *func = funcstr.data();
    QByteArray filestr = inputScript.toLocal8Bit();
    char *file = filestr.data();
    int* opt = new int(0);
    int* ost = new int(0);
    QByteArray csstr("");
    char *cs = csstr.data();
    int avisynth = dimzon_avs_init(avs,func, file, vinfo, opt, ost, cs);
    dimzon_avs_destroy(avs);

    if (0 == avisynth)
    {
        float fpsnum = vinfo->raten;
        float fpsden = vinfo->rated;
        float avsframes = vinfo->num_frames;
        float duration = (avsframes/(fpsnum/fpsden))*1000;
        QString avsWidth = QString::number(vinfo->width);
        QString avsHeight = QString::number(vinfo->height);

        inputVideoCodecs.append("Script");
        inputColorSpaces.append(AVScolorspace(vinfo->pixel_type));
        inputColorMatrix.append(checkColorMatrix("",avsWidth,avsHeight));
        inputVideoWidth.append(avsWidth);
        inputVideoHeight.append(avsHeight);
        inputFPS.append(QString::number(vinfo->raten) + "/" + QString::number(vinfo->rated));

        if (vinfo->audio_samples_per_second > 0)
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

    QList<QStringList> inputMediaInfo = { inputMediaDetails, {"0"}, {"8bit"}, inputVideoCodecs, inputColorSpaces, inputColorMatrix, inputVideoWidth, inputVideoHeight, inputFPS, {"1"}, inputAudioCodecs };
    return inputMediaInfo;

}

QString AVScolorspace(int colornum)
{
    QString colorspace = "RGB24";
    if (colornum == -536870912)
        colorspace = "Gray";
    if (colornum == -1610612728)
        colorspace = "YV12";
    if (colornum == -1610612720)
        colorspace = "YUV420";
    if (colornum == -1610611959)
        colorspace = "YV411";
    if (colornum == 1610612740)
        colorspace = "YUY2";
    if (colornum == -1610611960)
        colorspace = "YV24";
    if (colornum == -1610611957)
        colorspace = "YV24";
    if (colornum == 1342177281)
        colorspace = "RGB24";
    if (colornum == 1342177282)
        colorspace = "RGBA";

    return colorspace;
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
        float duration = (vpyFrames.toFloat()/(fpsnum/fpsden))*1000;

        inputVideoBitDepths.append(vpyBitDepth + "bit");
        inputVideoCodecs.append("Script");
        inputColorSpaces.append(vpyColorSpace);
        inputColorMatrix.append(checkColorMatrix("",vpyWidth,vpyHeight));
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

    inputMediaInfo = { inputMediaDetails, {"0"}, inputVideoBitDepths, inputVideoCodecs, inputColorSpaces, inputColorMatrix, inputVideoWidth, inputVideoHeight, inputFPS, {""}, inputAudioCodecs };

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
        if (vsline.contains("Script evaluation failed") || vsline.contains("Library not loaded") || vsline.contains("Failed to retrieve output node."))
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
            if (vpyColorSpace.contains("P") && !vpyColorSpace.contains("COMPAT"))
            {
                vpyColorSpace = vpyColorSpace.left(vpyColorSpace.indexOf("P"));
            }
        }
        if (vsline.contains("Bits: ") )
        {
            vpyBitDepth = vsline.simplified().replace("Bits: ", "");
        }
    }
}
