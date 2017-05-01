#include "queue.h"
#include "checkmedia.h"
#include "filesettings.h"


QString queue::InputFiles(QString inputFile)
{
    checkmedia cm;
    filesettings fs;

    QString openedFileInfo = "Error";
    QList<QStringList> mediainfo;
    bool isVPY = false;
    if (inputFile.right(3) == "vpy")
    {
        isVPY = true;
    }
    mediainfo = cm.checkMedia(inputFile);

    if (mediainfo[0][0].toInt() > 0)
    {
        QFile f(inputFile);
        QFileInfo fileInfo(f);
        QString filename(fileInfo.fileName());
        QString Location = inputFile;
        mainQueueInfo.append({inputFile, filename, mediainfo[2][0], "Pending", "1"});
        fs.addSettings(Location.replace(filename,""), mediainfo[5][0], mediainfo[7][0].toInt(), isVPY);
        openedFileInfo = mediainfo[2][0] + " | " + filename;
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
        QList<QStringList> mediaDetails = { { "1", "1", "Real Media" }, {"0"}, {"8"}, {"Real Video"}, {"YUV420"}, {"BT.709" }, {"1280"}, {"720"}, {"23.976"}, {"0"}, {"Real Audio"} };
        return mediaDetails;
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
    QList<QStringList> inputDetails = cm.checkMedia(mainQueueInfo[position][0]);
    if (outputConfig[position][11] != "Original Audio")
    {
        QList<QStringList> audioDetails = cm.checkMedia(outputConfig[position][11]);
        inputDetails[0][1] = audioDetails[0][1];
        inputDetails[9] = audioDetails[9];
    }

    int vstreamcount = inputDetails[0][0].toInt();
    int astreamcount = inputDetails[0][1].toInt();
    QString audiosel = outputConfig[position][12];
    audiosel = audiosel.replace("All", inputDetails[0][1]);


    if (vstreamcount > outputConfig[position][2].toInt())
    {
        if (audiosel != "None")
        {
            if (audiosel.toInt() > astreamcount)
            {
                mainQueueInfo[position][3] = "Error";
            }
        }
    }
    else
    {
        mainQueueInfo[position][3] = "Error";
    }
    return inputDetails;
}

void queue::setupRecontainer()
{
    for (int i = 0; i < mainQueueInfo.count(); i++)
    {
        filesettings fs;
        int vstream = outputConfig[i][2].toInt();
        QList<QStringList> mediaInfo = getInputDetails(mainQueueInfo[i][0]);
        fs.recontainerSettings(mediaInfo, vstream, i);
    }
}
