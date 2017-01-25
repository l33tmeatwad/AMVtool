#include "filesettings.h"
#include "checkmedia.h"
#include <QFile>
#include <QFileInfo>

filesettings::filesettings()
{

}


QString filesettings::InputFiles(QString inputFile)
{
    checkmedia cm;
    QString openedFileInfo = "Error";
    QList<QStringList> mediainfo;
    bool isVPY = false;
    if (inputFile.right(3) == "vpy")
    {
        mediainfo = cm.checkVPY(inputFile);
        isVPY = true;
    }
    else
    {
        mediainfo = cm.getMediaInfo(inputFile);
    }

    if (mediainfo[0][0].toInt() > 0)
    {
        QFile f(inputFile);
        QFileInfo fileInfo(f);
        QString filename(fileInfo.fileName());
        QString Location = inputFile;
        mainQueueInfo.append({inputFile, filename, "Pending"});
        addSettings(Location.replace(filename,""), isVPY);
        openedFileInfo = filename;
    }

    return openedFileInfo;
}

void filesettings::addSettings(QString originalLocation, bool isVPY)
{
    outputConfig.append(defaultConfiguration);

    if (defaultConfiguration[0] == "Original File Location")
    {
        outputConfig[outputConfig.count()-1][0] = originalLocation;
    }
    if (isVPY)
    {
        outputConfig[outputConfig.count()-1][11] = "None";
    }
}

void filesettings::chnageSettings(int ql, QStringList configurationList)
{
    if (ql >= 0)
    {
        outputConfig[ql] = configurationList;
    }
    else
    {
        defaultConfiguration = configurationList;

        for (int i = 0; i < outputConfig.count(); i++)
        {
            configurationList[10] = outputConfig[i][10];
            if (configurationList[0] == "Original File Location")
            {
                configurationList[0] = outputConfig[i][0];
            }
            if (mainQueueInfo[i][1].right(3) == "vpy" && outputConfig[i][10] == "Original Audio")
            {
                configurationList[11] = "None";
            }
            outputConfig[i] = configurationList;
        }

    }

}

void filesettings::removeSettings(int ql)
{
    mainQueueInfo.removeAt(ql);
    outputConfig.removeAt(ql);
}



QList<QStringList> filesettings::getInputDetails(QString mediafile)
{
    checkmedia cm;
    if (mediafile != "Default")
    {
        if (mediafile.right(3) == "vpy")
        {
            return cm.checkVPY(mediafile);
        }
        else
        {

            return cm.getMediaInfo(mediafile);
        }
    }
    else
    {
        QList<QStringList> mediaDetails = { { "1", "1", "Real Media" }, {"0"}, {"Real Video"}, {"YUV420P8"}, {"BT.709" }, {"1280"}, {"720"}, {"23.976"}, {"0"}, {"Real Audio"} };
        return mediaDetails;
    }

}

void filesettings::setupLossless()
{
    for (int i = 0; i < mainQueueInfo.count(); i++)
    {
        int vstream = outputConfig[i][2].toInt();
        QList<QStringList> mediaInfo = getInputDetails(mainQueueInfo[i][0]);
        if (mediaInfo[0][2] != "Error")
        {
            outputConfig[i][1] = lcontainer;

            if (mediaInfo[0][2] == "VapourSynth" && outputConfig[i][10] == "Original Audio")
            {
                outputConfig[i][11] = "None";
            }
            else
            {
                outputConfig[i][11] = lastreams;
            }

            outputConfig[i][5] = "UT Video";
            outputConfig[i][12] = lacodec;
            if (mediaInfo[4][vstream].contains("YUV") && !mediaInfo[4][vstream].contains("444"))
            {
                if (mediaInfo[4][vstream].contains("P8") || mediaInfo[4][vstream].contains("P10"))
                {
                    outputConfig[i][3] = mediaInfo[4][vstream];
                }
                else
                {
                    outputConfig[i][3] = "YUV420P8";
                }

            }
            else
            {
                if (mediaInfo[4][vstream].contains("RGB"))
                {
                    outputConfig[i][3] = mediaInfo[4][vstream];
                }
                else
                {
                    outputConfig[i][3] = "RGB24";
                }
            }


            if (mediaInfo[4][vstream] == "BT.601" || mediaInfo[4][vstream] == "BT.709")
            {
                outputConfig[i][4] = mediaInfo[4][vstream];
            }
            else
            {
                if (mediaInfo[6][vstream].toInt() < 580)
                {
                    outputConfig[i][4] = "BT.601";
                }
                else
                {
                    outputConfig[i][4] = "BT.709";
                }
            }

        }
    }
}

void filesettings::setupRecontainer()
{
    for (int i = 0; i < mainQueueInfo.count(); i++)
    {

        int vstream = outputConfig[i][2].toInt();
        QList<QStringList> mediaInfo = getInputDetails(mainQueueInfo[i][0]);
        if (mediaInfo[0][2] != "Error")
        {
            QString container = pickContainer(mediaInfo[0][2], mediaInfo[2][vstream]);

            if (container != "Error")
            {
                outputConfig[i][1] = container;
                outputConfig[i][5] = "Copy";
                outputConfig[i][15] = "1";
                if (container == "MP4")
                {
                    outputConfig[12] = "AAC";
                }
                else
                {
                    outputConfig[12] = "PCM";
                }
            }
        }
    }
}

QString filesettings::pickContainer(QString container, QString codec)
{
    QStringList AVI = { "ULRG", "ULRA", "ULY0", "ULH0", "ULY2", "ULH2", "YQY2", "XviD"};
    QStringList MOV = { "AVC", "HEVC", "MPEG-4", "ULRG", "ULRA", "ULY0", "ULH0", "ULY2", "ULH2", "YQY2"};
    QStringList MP4 = { "AVC", "HEVC", "MPEG-4"};
    QString newcontainer = "Error";

    if (container == "AVI")
    {
        if (MOV.contains(codec))
            newcontainer = "MOV";
        if (MP4.contains(codec))
            newcontainer = "MP4";
    }
    if (container == "Matroska")
    {
        if (AVI.contains(codec))
            newcontainer = "AVI";
        if (MP4.contains(codec))
            newcontainer = "MP4";
    }
    if (container == "QuickTime")
    {
        if (AVI.contains(codec))
            newcontainer = "AVI";
        if (MP4.contains(codec))
            newcontainer = "MP4";
    }
    if (container == "MPEG-4")
    {
        if (MOV.contains(codec))
            newcontainer = "MOV";
    }
    if (container == "MPEG-TS")
    {
        if (MP4.contains(codec))
            newcontainer = "MP4";
    }
    return newcontainer;
}
