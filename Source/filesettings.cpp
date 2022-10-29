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

void filesettings::addSettings(QString originalLocation, QString lumarange, QString colormatrix, bool isVPY)
{
    outputConfig.append(defaultConfiguration);
    if (lumarange == "HDR" && defaultConfiguration[6] == "1")
        outputConfig[outputConfig.count()-1][5] = "BT.709";
    else
        outputConfig[outputConfig.count()-1][5] = colormatrix;

    if (defaultConfiguration[0] == "Original File Location")
    {
        outputConfig[outputConfig.count()-1][0] = originalLocation;
    }
    if (isVPY)
    {
        outputConfig[outputConfig.count()-1][13] = "None";
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
            configurationList[2] = outputConfig[i][2];
            if (outputConfig[i][5].contains("BT.2020") && configurationList[6] == "1")
                configurationList[5] = "BT.709";
            else
                configurationList[5] = outputConfig[i][5];
            configurationList[12] = outputConfig[i][12];
            if (mainQueueInfo[i][1].right(3) == "vpy" && outputConfig[i][12] == "Original Audio")
                configurationList[13] = "None";

            QString fileLoc = outputConfig[i][0];
            outputConfig[i] = configurationList;
            if (configurationList[0] == "Original File Location")
                outputConfig[i][0] = fileLoc;
        }

    }

}


void filesettings::removeSettings(int ql)
{
    mainQueueInfo.removeAt(ql);
    outputConfig.removeAt(ql);
}


void filesettings::recontainerSettings(QList<QStringList> inputMediaInfo, int vstream, int queue)
{
    if (inputMediaInfo[0][2] != "Error" && inputMediaInfo[0][2] != "AviSynth" && inputMediaInfo[0][2] != "VapourSynth")
    {
        QStringList containers = findContainers(inputMediaInfo[5][vstream]);
        if (containers.contains(outputConfig[queue][1]))
        {
            outputConfig[queue][7] = "Copy";
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
