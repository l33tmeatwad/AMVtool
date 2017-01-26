#include "../checkmedia.h"
#include <QDir>
#include "../MediaInfoDLL/MediaInfoDLL.h"
#define MediaInfoNameSpace MediaInfoDLL;

#include <iostream>
#include <iomanip>

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
            inputVideoStreamIDs.append(QString::fromStdWString(MI.Get(Stream_Video, i, __T("ID"), Info_Text, Info_Name).c_str()));
            QString VideoCodec = QString::fromStdWString(MI.Get(Stream_Video, i, __T("Format"), Info_Text, Info_Name).c_str());
            if (VideoCodec == "YUV" || VideoCodec == "RGBA" || VideoCodec == "RGB")
                VideoCodec = QString::fromStdWString(MI.Get(Stream_Video, i, __T("CodecID"), Info_Text, Info_Name).c_str());

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



            inputVideoWidth.append(QString::fromStdWString(MI.Get(Stream_Video, i, __T("Width"), Info_Text, Info_Name).c_str()));
            inputVideoHeight.append(QString::fromStdWString(MI.Get(Stream_Video, i, __T("Height"), Info_Text, Info_Name).c_str()));
            inputFPS.append(QString::fromStdWString(MI.Get(Stream_Video, i, __T("FrameRate"), Info_Text, Info_Name).c_str()));
        }
        for (int i = 0; i < inputAudioStreams; i++)
        {
            inputAudioStreamIDs.append(QString::fromStdWString(MI.Get(Stream_Audio, i, __T("ID"), Info_Text, Info_Name).c_str()));
            inputAudioCodecs.append(QString::fromStdWString(MI.Get(Stream_Audio, i, __T("Format"), Info_Text, Info_Name).c_str()));
        }

    }
    else
    {
        inputMediaDetails.append({ "0", "0", "Error", "0" });
    }
    MI.Close();
    inputMediaInfo = { inputMediaDetails, inputVideoStreamIDs, inputVideoCodecs, inputColorSpaces, inputColorMatrix, inputVideoWidth, inputVideoHeight, inputFPS, inputAudioStreamIDs, inputAudioCodecs };

    return inputMediaInfo;
}

QList<QStringList> checkmedia::checkVPY(QString inputScript)
{

    vpyfail = false;
    vspipe = new QProcess(this);
    QStringList vspipecommand = { "--info", inputScript, "-" };
    connect(vspipe, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutput()));
    connect(vspipe, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(setVPYDetails(int, QProcess::ExitStatus)));
    vspipe->setProcessChannelMode(QProcess::MergedChannels);
    vspipe->start(vspipeexec, vspipecommand);
    vspipe->waitForFinished();
    vspipe->deleteLater();

    return inputMediaInfo;
}

void checkmedia::setVPYDetails(int, QProcess::ExitStatus)
{
    QStringList inputMediaDetails;
    if (vpyfail == false)
    {


        float fpsnum = vpyFPS.left(vpyFPS.indexOf("/")).toInt();
        float fpsden = vpyFPS.right(vpyFPS.indexOf("/")-1).toInt();
        int duration = (vpyFrames.toFloat()/(fpsnum/fpsden))*1000;

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

    inputMediaInfo = { inputMediaDetails, {"1"}, inputVideoCodecs, inputColorSpaces, inputColorMatrix, inputVideoWidth, inputVideoHeight, inputFPS, {"0"}, inputAudioCodecs };

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
    }
}
