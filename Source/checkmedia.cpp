#include "checkmedia.h"
#include "MediaInfoDLL/MediaInfoDLL_Static.h"
#include "ZenLib/Ztring.h"

using namespace MediaInfoDLL;
using namespace ZenLib;

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

    mediaformats.replace(" *.avs", "");

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

    if (MI.Open(__T(inputFile.toUtf8().constData())))
    {

        inputContainer = QString::fromStdString(MI.Get(Stream_General, 0, __T("Format_Profile"), Info_Text, Info_Name));
        if (inputContainer.toLower() != "quicktime")
        {
            inputContainer = QString::fromStdString(MI.Get(Stream_General, 0, __T("Format"), Info_Text, Info_Name));
        }
        inputDuration = QString::fromStdString(MI.Get(Stream_General, 0, __T("Duration"), Info_Text, Info_Name)).toInt();
        inputVideoStreams = QString::fromStdString(MI.Get(Stream_General,0, __T("VideoCount"), Info_Text, Info_Name)).toInt();
        inputAudioStreams = QString::fromStdString(MI.Get(Stream_General,0, __T("AudioCount"), Info_Text, Info_Name)).toInt();

        inputMediaDetails.append({ QString::number(inputVideoStreams),QString::number(inputAudioStreams),inputContainer, QString::number(inputDuration) });
        for (int i = 0; i < inputVideoStreams; i++)
        {
            inputVideoStreamIDs.append(QString::fromStdString(MI.Get(Stream_Video, i, __T("ID"), Info_Text, Info_Name)));
            QString bitdepth = QString::fromStdString(MI.Get(Stream_Video, i, __T("BitDepth"), Info_Text, Info_Name));
            QString VideoCodec = QString::fromStdString(MI.Get(Stream_Video, i, __T("Format"), Info_Text, Info_Name));
            if (VideoCodec == "YUV" || VideoCodec == "RGBA" || VideoCodec == "RGB")
                VideoCodec = QString::fromStdString(MI.Get(Stream_Video, i, __T("CodecID"), Info_Text, Info_Name));

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

            QString colorspace = QString::fromStdString(MI.Get(Stream_Video, i, __T("ColorSpace"), Info_Text, Info_Name));
            QString colormatrix = QString::fromStdString(MI.Get(Stream_Video, i, __T("matrix_coefficients"), Info_Text, Info_Name));

            if (inputContainer.toLower() == "quicktime")
            {
                if (inputVideoCodecs[i].contains("ULRG"))
                {
                    colorspace = "RGB24";
                }
                if (inputVideoCodecs[i].contains("ULRA"))
                {
                    colorspace = "RGBA";
                }
                if (inputVideoCodecs[i].contains("ULY0") || inputVideoCodecs[i].contains("ULY2"))
                {
                    colorspace = "YUV";
                    colormatrix = "BT.601";
                }
                if (inputVideoCodecs[i].contains("ULH0") || inputVideoCodecs[i].contains("ULH2") || inputVideoCodecs[i].contains("UQY2"))
                {
                    colorspace = "YUV";
                    colormatrix = "BT.709";
                }
            }




            if (colorspace.toLower() == "yuv")
            {
                colorspace.append(QString::fromStdString(MI.Get(Stream_Video, i, __T("ChromaSubsampling"), Info_Text, Info_Name))).replace(":","");
                colorspace.append("P" + bitdepth);

            }

            inputColorSpaces.append(colorspace);
            inputColorMatrix.append(colormatrix);

            if (bitdepth == "")
            {
                bitdepth = "Bitdepth Unknown";
            }
            else
            {
                bitdepth = bitdepth + "bit";
            }
            inputVideoBitDepths.append(bitdepth);


            inputVideoWidth.append(QString::fromStdString(MI.Get(Stream_Video, i, __T("Width"), Info_Text, Info_Name)));
            inputVideoHeight.append(QString::fromStdString(MI.Get(Stream_Video, i, __T("Height"), Info_Text, Info_Name)));
            inputFPS.append(QString::fromStdString(MI.Get(Stream_Video, i, __T("FrameRate"), Info_Text, Info_Name)));
        }
        for (int i = 0; i < inputAudioStreams; i++)
        {
            inputAudioStreamIDs.append(QString::fromStdString(MI.Get(Stream_Audio, i, __T("ID"), Info_Text, Info_Name)));
            QString audiocodec = QString::fromStdString(MI.Get(Stream_Audio, i, __T("Format"), Info_Text, Info_Name));
            if (audiocodec == "MPEG Audio")
            {
                audiocodec = QString::fromStdString(MI.Get(Stream_Audio, i, __T("CodecID/Hint"), Info_Text, Info_Name));
            }
            inputAudioCodecs.append(audiocodec);
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
    inputMediaDetails.append({ "0", "0", "Error", "0" });

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
