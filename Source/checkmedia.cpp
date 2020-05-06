#include "checkmedia.h"
#include <MediaInfoDLL/MediaInfoDLL_Static.h>
#include "ZenLib/Ztring.h"

QString checkColorMatrix(QString matrix, QString videoWidth, QString videoHeight);

using namespace MediaInfoDLL;
using namespace ZenLib;

checkmedia::checkmedia(QObject *parent) : QObject(parent)
{

}

QString checkmedia::checkFormats(bool vpyfail)
{
    QString mediaformats = "Media Files (*.avi *.m2ts *.m4v *.mkv *.mov *.mp4 *.mpg *.mts *.ts *.vob *.vpy *.webm)";
    if (vpyfail == true)
    {
        mediaformats.replace(" *.vpy", "");
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
            if (videoWidth.toInt() > 2048 || videoHeight.toInt() > 1080)
                colormatrix = "BT.2020NC";
            else
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
        inputMediaInfo = getMediaInfo(inputFile);
    }
    return inputMediaInfo;
}


QList<QStringList> checkmedia::getMediaInfo(QString inputFile)
{
    QStringList inputMediaDetails;
    MediaInfo MI;

    if (MI.Open(__T(inputFile.toUtf8().constData())))
    {
        QStringList TSVOB = { "BDAV", "MPEG-PS", "MPEG-TS" };
        QStringList acontainers = { "ADTS", "AVI", "MPEG Audio", "Wave" };

        inputContainer = QString::fromStdString(MI.Get(Stream_General, 0, __T("Format_Profile"), Info_Text, Info_Name));
        if (inputContainer.toLower() != "quicktime")
        {
            inputContainer = QString::fromStdString(MI.Get(Stream_General, 0, __T("Format"), Info_Text, Info_Name));
        }
        inputDuration = QString::fromStdString(MI.Get(Stream_General, 0, __T("Duration"), Info_Text, Info_Name)).toFloat();
        inputVideoStreams = QString::fromStdString(MI.Get(Stream_General,0, __T("VideoCount"), Info_Text, Info_Name)).toInt();
        inputAudioStreams = QString::fromStdString(MI.Get(Stream_General,0, __T("AudioCount"), Info_Text, Info_Name)).toInt();

        for (int i = 0; i < inputVideoStreams; i++)
        {
            QString videoID;

            if (TSVOB.contains(inputContainer) || inputContainer == "AVI")
            {
                videoID = QString::fromStdString(MI.Get(Stream_Video, i, __T("ID"), Info_Text, Info_Name));
            }
            else
            {
                videoID = QString::fromStdString(MI.Get(Stream_Video, i, __T("StreamOrder"), Info_Text, Info_Name));
            }

            if (TSVOB.contains(inputContainer))
            {
                QString IDfix = QString::fromStdString(MI.Get(Stream_Video, i, __T("ID/String"), Info_Text, Info_Name));
                videoID = IDfix.replace(videoID, "").replace("(","").replace(")", "").replace(" ","");
                if (videoID.contains("0xE"))
                {
                    videoID = videoID.replace("0x", "0x1");
                }
            }


            inputVideoStreamIDs.append(videoID);
            QString VideoCodec = QString::fromStdString(MI.Get(Stream_Video, i, __T("Format_Commercial"), Info_Text, Info_Name));
            if (VideoCodec == "YUV" || VideoCodec == "RGBA" || VideoCodec == "RGB")
                VideoCodec = QString::fromStdString(MI.Get(Stream_Video, i, __T("CodecID"), Info_Text, Info_Name));

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
                bitdepth = QString::fromStdString(MI.Get(Stream_Video, i, __T("BitDepth"), Info_Text, Info_Name));
                colorspace = QString::fromStdString(MI.Get(Stream_Video, i, __T("ColorSpace"), Info_Text, Info_Name));
                if (colorspace.toLower() == "yuv")
                    colorspace.append(QString::fromStdString(MI.Get(Stream_Video, i, __T("ChromaSubsampling"), Info_Text, Info_Name))).replace(":","");
                colormatrix = QString::fromStdString(MI.Get(Stream_Video, i, __T("matrix_coefficients"), Info_Text, Info_Name));
            }
            inputVideoCodecs.append(VideoCodec);

            if (inputContainer == "MPEG-4")
                inputContainer = "MP4";

            QString videowidth = QString::fromStdString(MI.Get(Stream_Video, i, __T("Width"), Info_Text, Info_Name));
            QString videoheight = QString::fromStdString(MI.Get(Stream_Video, i, __T("Height"), Info_Text, Info_Name));

            colormatrix = checkColorMatrix(colormatrix, videowidth, videoheight);

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


            inputVideoWidth.append(videowidth);
            inputVideoHeight.append(videoheight);
            inputFPS.append(QString::fromStdString(MI.Get(Stream_Video, i, __T("FrameRate"), Info_Text, Info_Name)));
        }
        int audioCount = inputAudioStreams;
        for (int i = 0; i < audioCount; i++)
        {
            QString audioID;
            if (TSVOB.contains(inputContainer) || acontainers.contains(inputContainer))
            {
                audioID = QString::fromStdString(MI.Get(Stream_Audio, i, __T("ID"), Info_Text, Info_Name));
                if (audioID == "")
                    audioID = "0";
            }
            else
            {
                audioID = QString::fromStdString(MI.Get(Stream_Audio, i, __T("StreamOrder"), Info_Text, Info_Name));
            }

            if (TSVOB.contains(inputContainer))
            {
                QString IDfix = QString::fromStdString(MI.Get(Stream_Audio, i, __T("ID/String"), Info_Text, Info_Name));

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




            QString audiocodec = QString::fromStdString(MI.Get(Stream_Audio, i, __T("Format"), Info_Text, Info_Name));

            if (audiocodec.contains("TrueHD / AC-3"))
            {
                int astream = inputVideoStreams+i;
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
            if (vpyColorSpace.contains("P") && !vpyColorSpace.contains("COMPAT"))
            {
                QRegExp findbitdpeth("P");
                vpyColorSpace = vpyColorSpace.left(findbitdpeth.indexIn(vpyColorSpace));
            }
        }
        if (vsline.contains("Bits: ") )
        {
            vpyBitDepth = vsline.simplified().replace("Bits: ", "");
        }
    }
}
