#include "amvtool.h"
#include "ui_amvtool.h"
#include "configure.h"
#include "filesettings.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QApplication>
#include <QProgressBar>
#include <QDebug>
#include <QScrollBar>

AMVtool::AMVtool(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AMVtool)
{
    ui->setupUi(this);
    ui->textEdit->setReadOnly(true);
}

AMVtool::~AMVtool()
{
    delete ui;
}

void AMVtool::on_addFiles_clicked()
{
    QString mediatypes;
    QFileInfo file_exists(vspipeexec);
    if (file_exists.exists())
    {
        mediatypes = "Media Files (*.avi *m2ts *.m4v *.mov *.mkv *.mp4 *ts *.vpy)";
    }
    else
    {
        mediatypes = "Media Files (*.avi *m2ts *.m4v *.mov *.mkv *.mp4 *ts)";
    }




    inputFiles = QFileDialog::getOpenFileNames(this, tr("Open Media File"), "", tr(mediatypes.toUtf8().constData()));

    foreach (const QString &file, inputFiles)
    {
        filesettings fs;
        QString fileInfo = fs.InputFiles(file);
        if (fileInfo != "Error")
        {
            ui->fileList->addItem(fileInfo);
        }
        else
        {
            QMessageBox::information(this,"Error","There was an error processing the following file and it could not be loaded: " + file);
        }

    }
}

void AMVtool::on_removeFiles_clicked()
{
    foreach(QListWidgetItem * item, ui->fileList->selectedItems())
    {
        int queuelocation = ui->fileList->row(item);
        delete ui->fileList->takeItem(queuelocation);
        filesettings fs;
        fs.removeSettings(queuelocation);

    }
    if (ui->fileList->count() < 1)
    {
        ui->configEncSettings->setEnabled(false);
    }
}

void AMVtool::on_convertFiles_clicked()
{
    if (ui->convertFiles->text() == "GO")
    {
        changeEnabled(false,"Cancel");
        CheckQueue();
    }
    else
    {
        changeEnabled(true,"GO");
        encode->kill();
        stopprocess = true;
    }
}


void AMVtool::on_configAll_clicked()
{
    openConfigBox(-1);
}

void AMVtool::on_configEncSettings_clicked()
{
    foreach (QListWidgetItem * item, ui->fileList->selectedItems())
    {
        openConfigBox(ui->fileList->row(item));
    }
}



void AMVtool::on_fileList_clicked()
{
    int selected = ui->fileList->selectedItems().count();
    if (selected > 0)
    {
        ui->configEncSettings->setEnabled(true);
    }
    else
    {
        ui->configEncSettings->setEnabled(false);
    }
}

void AMVtool::on_fileList_doubleClicked(const QModelIndex &index)
{
    int selectedfile = index.row();
    QString newtext = ui->fileList->item(selectedfile)->text().replace("CONVERTED | ", "").replace("ERROR | ", "").replace("SKIPPED | ", "");
    ui->fileList->item(selectedfile)->setText(newtext);
    ui->fileList->item(selectedfile)->setBackgroundColor(Qt::white);
    mainQueueInfo[selectedfile][2] = "Pending";
}

void AMVtool::openConfigBox(int selectedfile)
{
    filesettings fs;
    QString mediafile;
    QStringList configList;
    if (selectedfile >= 0)
    {
        mediafile = mainQueueInfo[selectedfile][0];
        configList.append(outputConfig[selectedfile]);
    }
    else
    {
        mediafile = "Default";
        configList.append(defaultConfiguration);
    }
    QList<QStringList> inputMediaInfo = fs.getInputDetails(mediafile);
    if (inputMediaInfo[0][0].toInt() > 0)
    {
        configure *configwindow = new configure(this);
        configwindow->setData(selectedfile, inputMediaInfo, configList);
        configwindow->setAttribute(Qt::WA_DeleteOnClose);
        configwindow->exec();
    }
    else
    {
        QMessageBox::information(this,"Error", "There was an error reading the file, please check it and try again.");
    }

}


void AMVtool::on_lossless_clicked()
{
    filesettings fs;
    fs.setupLossless();
    QMessageBox::information(this,"Settings Changed", "Settings for everything in the queue have been configured for lossless output.");
}

void AMVtool::on_recontainerAll_clicked()
{
    filesettings fs;
    fs.setupRecontainer();
    QMessageBox::information(this,"Settings Changed", "Settings changed to recontainer all files in the queue compatible with possible output containers.");

}


// SECTION FOR PROCESSING ENCODES

void AMVtool::CheckQueue()
{
    if (ui->fileList->currentRow() >= 0)
    {
        ui->fileList->currentItem()->setSelected(false);
        ui->configEncSettings->setEnabled(false);
    }
    queue = -1;
    int position = mainQueueInfo.count();
    for (int i = 0; i < position; i++)
    {
        if (mainQueueInfo[i][2] == "Pending")
        {
            queue = i;
            position = i;
        }
    }
    stopprocess = false;
    if (queue >= 0)
    {
        filesettings fs;
        progress = 0.0;
        bool errorfree;

        ui->addFiles->setEnabled(false);
        ui->removeFiles->setEnabled(false);
        ui->configEncSettings->setEnabled(false);

        QList<QStringList> inputDetails = fs.getInputDetails(mainQueueInfo[queue][0]);
        if (outputConfig[queue][10] != "Original Audio")
        {
            QList<QStringList> audioDetails = fs.getInputDetails(outputConfig[queue][10]);
            inputDetails[0][1] = audioDetails[0][1];
            inputDetails[8] = audioDetails[8];
        }

        int vstreamcount = inputDetails[0][0].toInt();
        int astreamcount = inputDetails[0][1].toInt();
        QString audiosel = outputConfig[queue][11];
        audiosel = audiosel.replace("All", inputDetails[0][1]);

        if (vstreamcount > outputConfig[queue][2].toInt())
        {
            if (audiosel != "None")
            {
                if (audiosel.toInt() <= astreamcount)
                {
                    errorfree = true;
                }
            }
            if (audiosel == "None")
            {
                errorfree = true;
            }
        }
        else
        {
            errorfree = false;
            ui->fileList->item(queue)->setText("ERROR | " + ui->fileList->item(queue)->text());
            ui->fileList->item(queue)->setBackgroundColor(Qt::red);
            mainQueueInfo[queue][2] = "Error";
            CheckQueue();
        }

        if (ui->ifExists->currentIndex() != 1 && errorfree == true)
        {
            bool skip = false;
            QFileInfo file_exists(outputConfig[queue][0] + mainQueueInfo[queue][1].left(mainQueueInfo[queue][1].length()-4) + "-AMVtool." + outputConfig[queue][1].toLower());
            if (file_exists.exists())
            {
                if (ui->ifExists->currentIndex() == 0)
                {
                    if (QMessageBox::question(this,"Overwrite File", "Output file already exists, overwrite it?", QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)
                    {
                        skip = true;
                    }
                }
                else
                {
                    skip = true;
                }
            }
            if (skip == true)
            {
                errorfree = false;
                ui->fileList->item(queue)->setText("SKIPPED | " + ui->fileList->item(queue)->text());
                ui->fileList->item(queue)->setBackgroundColor(Qt::yellow);
                mainQueueInfo[queue][2] = "File exists, skipping.";
                CheckQueue();
            }



        }


        if (errorfree)
        {

            Encode(queue,inputDetails, outputConfig[queue]);
        }
        else
        {

        }


    }
    else
    {
        changeEnabled(true,"GO");
    }

}


void AMVtool::Encode(int queue, QList<QStringList> inputDetails, QStringList configList)
{
    inputDuration = inputDetails[0][3].toInt();
    outputcreated = false;
    setupencode se;
    encode = new QProcess(this);
    connect(encode, SIGNAL(readyReadStandardOutput()),this,SLOT(readyReadStandardOutput()));
    connect(encode, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(encodeFinished(int,QProcess::ExitStatus)));
    encode->setProcessChannelMode(QProcess::MergedChannels);
    QStringList EncodeOptions = se.SetupEncode(queue, mainQueueInfo[queue], inputDetails, configList);

    QString debugbox;
    for (int i = 0; i < EncodeOptions.count(); i++)
    {
        debugbox += EncodeOptions[i] + " ";
    }
//    QMessageBox::information(this,"Debug",debugbox);

    pipe = new QProcess(this);
    if (inputDetails[0][2] == "VapourSynth")
    {
        pipe->setStandardOutputProcess(encode);
        pipe->start(vspipeexec, se.SetupPipe(mainQueueInfo[queue][0], inputDetails[3][0]));
    }
    encode->setWorkingDirectory(outputConfig[queue][0]);
    encode->start(ffmpegexec, EncodeOptions);

}

void AMVtool::changeEnabled(bool status, QString button)
{
    ui->addFiles->setEnabled(status);
    ui->removeFiles->setEnabled(status);
    ui->configAll->setEnabled(status);
    ui->convertFiles->setText(button);
}

void AMVtool::encodeFinished(int exitcode, QProcess::ExitStatus)
{
    inputDuration = 0;
    ui->progressBar->setValue(100);
    if (pipe->state() > 0)
    {
        pipe->kill();
    }


    if (stopprocess == true)
    {
        encode->deleteLater();
        pipe->deleteLater();
        QMessageBox::information(this, "Process Canceled", "The process queue was stopped by the user, no more files will be converted.");
        changeEnabled(true,"GO");
    }
    if (outputcreated == true)
    {
        if (stopprocess != true)
        {
            encode->deleteLater();
            pipe->deleteLater();
            int listcount = ui->fileList->count()-1;
            if (listcount == queue)
            {
                if (encodepass == "Single")
                {
                    mainQueueInfo[queue][2] = "Complete";
                    ui->fileList->item(queue)->setBackgroundColor(Qt::green);
                    ui->fileList->item(queue)->setText("CONVERTED | " + ui->fileList->item(queue)->text());
                    QMessageBox::information(this, "Process Complete", "Process Complete!");
                    changeEnabled(true,"GO");
                }
                if (encodepass == "1")
                {
                    CheckQueue();
                }

            }
            else
            {
                if (encodepass == "Single")
                {
                    QString derp = encodepass;

                    mainQueueInfo[queue][2] = "Complete";
                    ui->fileList->item(0)->setBackgroundColor(Qt::green);
                    ui->fileList->item(queue)->setText("CONVERTED | " + ui->fileList->item(queue)->text());
                    CheckQueue();
                }
                if (encodepass == "1")
                {
    //                encodepass = "2";

                    CheckQueue();
                }

            }

        }

    }
    else
    {
        mainQueueInfo[queue][2] = "Error";
        ui->fileList->item(queue)->setBackgroundColor(Qt::red);
        ui->fileList->item(queue)->setText("ERROR | " + ui->fileList->item(queue)->text());
        int listcount = ui->fileList->count()-1;
        if (listcount == queue)
        {
            QMessageBox::information(this, "Process Complete", "Process Complete!");
        }
        else
        {
            if (stopprocess != true)
            {
                CheckQueue();
            }
        }

    }

}

void AMVtool::readyReadStandardOutput()
{
    QString readline = encode->readAllStandardOutput();
    int currentpercent = currentProcess(readline);

    ui->progressBar->setValue(currentpercent);

    if (!readline.contains("time="))
    {

    }
    if (readline.contains("muxing overhead:"))
    {
        outputcreated = true;
    }
    mOutputString.append(readline);
    ui->textEdit->setText(mOutputString);
    // put the slider at the bottom
    ui->textEdit->verticalScrollBar()
            ->setSliderPosition(ui->textEdit->verticalScrollBar()->maximum());


}

int AMVtool::currentProcess(QString currentstatus)
{
    int currentpercent = 0;

    float duration = inputDuration;
    QRegExp findtime(timecode);
    int length = findtime.indexIn(currentstatus);
    QStringList findstatus;
    if (length != -1)
    {
        findstatus = findtime.capturedTexts();
        progress = parseTimecode(findstatus[0].right(11));
    }
    currentpercent = (progress/duration) *100;
    return currentpercent;
}

float AMVtool::parseTimecode(QString timecode)
{
    float hours = ((timecode.left(2).toInt() * 60) * 60) * 1000;
    float minutes = (timecode.left(5).right(2).toInt() * 60) * 1000;
    float seconds = (timecode.left(8).right(2).toInt()) * 1000;
    float milliseconds = (timecode.left(11).right(2).toInt()) * 10;
    float mstime = hours+minutes+seconds+milliseconds;
    return mstime;
}
