#include "queue.h"
#include "checkmedia.h"
#include "filesettings.h"


QString queue::InputFiles(QString inputFile)
{
    checkmedia cm;
    filesettings fs;

    QString openedFileInfo = "Error";
    QList<QStringList> inputMediaInfo;
    bool isVPY = false;
    if (inputFile.right(3) == "vpy")
    {
        isVPY = true;
    }
    inputMediaInfo = cm.checkMedia(inputFile);

    if (inputMediaInfo[3][0] == "Dovi5")
        openedFileInfo = "Dovi5";

    if (inputMediaInfo[0][0].toInt() > 0 && inputMediaInfo[3][0] != "Dovi5")
    {
        QString ifInterlaced = "";
        if (inputMediaInfo[4][0] != "")
            ifInterlaced = " " + inputMediaInfo[4][0];
        QFile f(inputFile);
        QFileInfo fileInfo(f);
        QString filename(fileInfo.fileName());
        QString Location = inputFile;
        QString LumaRange = inputMediaInfo[3][0];
        QString ColorMatrix = inputMediaInfo[7][0];
        QString bitDepth = "";
        if (inputMediaInfo[2][0] != "")
            bitDepth = inputMediaInfo[2][0] + " ";

        mainQueueInfo.append({inputFile, filename, bitDepth + inputMediaInfo[3][0] + ifInterlaced, "Pending", "1"});
        fs.addSettings(Location.replace(filename,""), LumaRange, ColorMatrix, isVPY);
        openedFileInfo = bitDepth + inputMediaInfo[3][0] + ifInterlaced + " | " + filename;
    }
    if (RecontainerSettings && openedFileInfo != "Error")
    {
        filesettings fs;
        fs.recontainerSettings(inputMediaInfo,0,mainQueueInfo.count()-1);
    }

    return openedFileInfo;
}


QList<QStringList> queue::getInputDetails(QString mediafile)
{
    checkmedia cm;
    if (mediafile != "Default")
    {
        return cm.checkMedia(mediafile);
    }
    else
    {
        QList<QStringList> inputMediaInfo = { { "1", "1", "Real Media" }, {"0"}, {"8"}, {"HDR"}, {"Progressive"}, {"Real Video"}, {"YUV420"}, {"BT.709" }, {"1280"}, {"720"}, {"23.976"}, {"0"}, {"Real Audio"}, {"6"}, {"L R C BL BR LFE"} };
        return inputMediaInfo;
    }

}


int queue::findPosition()
{
    int position = -1;
    int queue = mainQueueInfo.count();
    for (int i = 0; i < queue; i++)
    {
        if (mainQueueInfo[i][3] == "Pending")
        {
            position = i;
            queue = i;
        }
    }
    return position;
}

QList<QStringList> queue::checkInput(int position)
{
    checkmedia cm;
    QList<QStringList> inputMediaInfo = cm.checkMedia(mainQueueInfo[position][0]);
    if (outputConfig[position][12] != "Original Audio")
    {
        QList<QStringList> audioDetails = cm.checkMedia(outputConfig[position][12]);
        inputMediaInfo[0][1] = audioDetails[0][1];
        inputMediaInfo[11] = audioDetails[11];
        inputMediaInfo[12] = audioDetails[12];
		inputMediaInfo[13] = audioDetails[13];
		inputMediaInfo[14] = audioDetails[14];
    }

    int vstreamcount = inputMediaInfo[0][0].toInt();
    int astreamcount = inputMediaInfo[0][1].toInt();
    QString audiosel = outputConfig[position][13];
    audiosel = audiosel.replace("All", inputMediaInfo[0][1]);


    if (vstreamcount > outputConfig[position][2].toInt())
    {
        if (audiosel != "None")
        {
            if (audiosel.toInt() > astreamcount)
            {
                outputConfig[position][13] = "None";
            }
        }
    }
    else
    {
        mainQueueInfo[position][3] = "Error";
    }
    return inputMediaInfo;
}

void queue::setupRecontainer()
{
    RecontainerSettings = true;
    for (int i = 0; i < mainQueueInfo.count(); i++)
    {
        filesettings fs;
        int vstream = outputConfig[i][2].toInt();
        QList<QStringList> inputMediaInfo = getInputDetails(mainQueueInfo[i][0]);
        fs.recontainerSettings(inputMediaInfo, vstream, i);
    }
}
