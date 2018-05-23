#include "filesettings.h"
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>


bool filesettings::checkFolder(QString folder)
{
    bool validpath;

    if (OS == "Windows")
    {
        QFile testDir(folder + ".AMVtooltest");
        validpath = testDir.open(QIODevice::WriteOnly);
        testDir.remove();
    }
    else
    {
        QFileInfo testDir(folder);
        validpath = testDir.permission(QFile::WriteUser);
    }

    return validpath;
}

void filesettings::addSettings(QString originalLocation, QString colormatrix, bool isVPY)
{
    outputConfig.append(defaultConfiguration);
    if (defaultConfiguration[6] == "Ut Video" && defaultConfiguration[3].contains("YUV") && colormatrix.contains("BT.2020"))
        outputConfig[outputConfig.count()-1][3] = "RGB24";

    outputConfig[outputConfig.count()-1][5] = colormatrix;

    if (defaultConfiguration[0] == "Original File Location")
    {
        outputConfig[outputConfig.count()-1][0] = originalLocation;
    }
    if (isVPY)
    {
        outputConfig[outputConfig.count()-1][12] = "None";
    }
}


void filesettings::changeSettings(int ql, QString bitdepth, QStringList configurationList)
{
    if (ql >= 0)
    {
        mainQueueInfo[ql][2] = bitdepth;
        outputConfig[ql] = configurationList;
    }
    else
    {
        defaultConfiguration = configurationList;

        for (int i = 0; i < outputConfig.count(); i++)
        {
            if (configurationList[6] == "Ut Video" && configurationList[3].contains("YUV") && outputConfig[i][5].contains("BT.2020"))
                configurationList[3] = "RGB24";
            configurationList[2] = outputConfig[i][2];
            configurationList[5] = outputConfig[i][5];
            configurationList[11] = outputConfig[i][11];
            if (configurationList[0] == "Original File Location")
            {
                configurationList[0] = outputConfig[i][0];
            }
            if (mainQueueInfo[i][1].right(3) == "vpy" && outputConfig[i][11] == "Original Audio")
            {
                configurationList[12] = "None";
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


void filesettings::recontainerSettings(QList<QStringList> mediaInfo, int vstream, int queue, bool autoCon)
{
    if (mediaInfo[0][2] != "Error" && mediaInfo[0][2] != "AviSynth" && mediaInfo[0][2] != "VapourSynth")
    {
        QStringList containers = findContainers(mediaInfo[3][vstream]);

        if (autoCon == true && containers[0] == mediaInfo[0][2])
        {
            containers.removeFirst();
            outputConfig[queue][1] = containers[0];
        }
        if (autoCon == false && !containers.contains(outputConfig[queue][1]))
        {

        }
        else
        {
            outputConfig[queue][6] = "Copy";
            outputConfig[queue][16] = "1";
            if (outputConfig[queue][1] == "MP4")
            {
                outputConfig[queue][13] = "AAC";
            }
            else
            {
                outputConfig[queue][13] = "PCM";
            }
        }
    }
}

QStringList filesettings::findContainers(QString codec)
{
    QStringList containerlist;
    if (MP4.contains(codec))
        containerlist.append("MP4");
    if (AVI.contains(codec))
        containerlist.append("AVI");
    if (MOV.contains(codec))
        containerlist.append("MOV");

    containerlist.append("MKV");
    return containerlist;
}
