#include "amvtool.h"
#include "ui_amvtool.h"
#include "configure.h"
#include "dependencies.h"
#include "filesettings.h"
#include "progress.h"
#include "queue.h"
#include "setupencode.h"
#include <QFileInfo>
#include <QMessageBox>
#include <QApplication>
#include <QProgressBar>
#include <QScrollBar>
#include <QMimeData>


AMVtool::AMVtool(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AMVtool)
{
    ui->setupUi(this);
    ui->textEdit->setReadOnly(true);
    setAcceptDrops(true);
    mediatypes = checkDependencies();
    timeRemaining = new QLabel(this);
    ui->statusBar->addPermanentWidget(timeRemaining, 0);
}

AMVtool::~AMVtool()
{
    delete ui;
}


QString AMVtool::checkDependencies()
{
    dependencies dep;
    QStringList depStatus = dep.checkDependencies();
    if (depStatus[1].contains("Not Found: "))
    {
        depStatus[1].replace("Not Found: ", "");
    }

    if (depStatus[2].contains("Not Found: "))
    {
        ui->statusBar->showMessage("FFmpeg executable is missing!");
        depStatus[2].replace("Not Found: ", "");
    }
    else
    {
        ui->statusBar->showMessage("Ready");
    }
    vspipeexec = depStatus[1];
    ffmpegexec = depStatus[2];
    hibitdepth = { depStatus[3].toInt(), depStatus[4].toInt(), depStatus[5].toInt(), depStatus[6].toInt() };

    return depStatus[0];

}


void AMVtool::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls())
    {
        e->acceptProposedAction();

    }

}

void AMVtool::dropEvent(QDropEvent *e)
{
    QStringList fileList;
    foreach (const QUrl &url, e->mimeData()->urls())
    {
        if (mediatypes.contains(url.toLocalFile().right(3).toLower()))
        {
            fileList.append(url.toLocalFile());
        }
    }
    if (fileList.length() > 0)
    {
        addFilesToQueue(fileList);
    }

}

void AMVtool::on_addFiles_clicked()
{
    setAcceptDrops(false);
    QStringList inputFiles = dialog.getOpenFileNames(this, tr("Open Media File"), "", tr(mediatypes.toUtf8().constData()));
    addFilesToQueue(inputFiles);
    setAcceptDrops(true);
}

void AMVtool::addFilesToQueue(QStringList inputFiles)
{
    foreach (const QString &file, inputFiles)
    {
        queue q;
        QString fileInfo = q.InputFiles(file);
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
    ui->convertFiles->setEnabled(false);
    if (ui->convertFiles->text() == "START")
    {
        stopprocess = false;
        changeEnabled(false,"Cancel");
        setAcceptDrops(false);
        CheckQueue();
    }
    else
    {
        stopprocess = true;
        encode->kill();
        timeRemaining->setText("");
        ui->statusBar->showMessage("Process Canceled");
        QMessageBox::information(this, "Process Canceled", "The process queue was stopped by the user, no more files will be converted.");
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
        int queue = ui->fileList->row(item);
        openConfigBox(queue);
        ui->fileList->item(queue)->setText(mainQueueInfo[queue][2] + " | " + mainQueueInfo[queue][1]);
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
    int pos = index.row();
    updateQueue(pos,"Pending");
    ui->statusBar->showMessage("Ready");
}

void AMVtool::openConfigBox(int selectedfile)
{
    queue queue;

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
    QList<QStringList> inputMediaInfo = queue.getInputDetails(mediafile);
    if (inputMediaInfo[0][0].toInt() > 0)
    {
        setAcceptDrops(false);
        configure *configwindow = new configure(this);
        configwindow->setData(selectedfile, inputMediaInfo, configList, hibitdepth);
        configwindow->setAttribute(Qt::WA_DeleteOnClose);
        configwindow->exec();
        setAcceptDrops(true);
    }
    else
    {
        QMessageBox::information(this,"Error", "There was an error reading the file, please check it and try again.");
    }

}

void AMVtool::on_showDetails_clicked()
{
    if (ui->showDetails->text() == "Show Details")
    {
        ui->showDetails->setText("Hide Details");
        QMainWindow::setMaximumHeight(540);
        QMainWindow::setMinimumHeight(540);
    }
    else
    {
        ui->showDetails->setText("Show Details");
        QMainWindow::setMaximumHeight(365);
        QMainWindow::setMinimumHeight(365);
        QMainWindow::adjustSize();
    }
}



// SECTION FOR PROCESSING THE QUEUE

void AMVtool::updateQueue(int pos, QString status)
{
    if (status == "Complete")
    {
        ui->fileList->item(pos)->setText("COMPLETE | " + ui->fileList->item(pos)->text());
        ui->fileList->item(pos)->setBackgroundColor(Qt::green);
        mainQueueInfo[pos][3] = "Complete";
    }
    if (status == "Error")
    {
        if (packetbuffererror == true && outputConfig[pos][17] == "")
        {
            outputConfig[pos][17] = "9999";
        }
        else
        {
            packetbuffererror = false;
            ui->fileList->item(pos)->setText("ERROR | " + ui->fileList->item(pos)->text());
            ui->fileList->item(pos)->setBackgroundColor(Qt::red);
            mainQueueInfo[pos][3] = "Error";
        }
    }
    if (status == "Pending")
    {
        ui->fileList->item(pos)->setText(mainQueueInfo[pos][2] + " | " + mainQueueInfo[pos][1]);
        ui->fileList->item(pos)->setBackgroundColor(Qt::white);
        mainQueueInfo[pos][3] = "Pending";
        mainQueueInfo[pos][4] = "1";
    }
    if (status == "Skipped")
    {
        ui->fileList->item(pos)->setText("SKIPPED | " + ui->fileList->item(pos)->text());
        ui->fileList->item(pos)->setBackgroundColor(Qt::yellow);
        mainQueueInfo[pos][3] = "Skipped";
    }

}

void AMVtool::CheckQueue()
{
    if (ui->fileList->currentRow() >= 0)
    {
        ui->fileList->currentItem()->setSelected(false);
        ui->configEncSettings->setEnabled(false);
    }
    queue queue;
    position = queue.findPosition();
    ui->convertFiles->setEnabled(false);
    if (position >= 0)
    {
        ProcessFile(position);
    }
    else
    {
        if (ui->fileList->count() > 0)
        {
            QMessageBox::information(this, "Process Complete", "Process Complete!");
        }
        changeEnabled(true,"START");
        ui->convertFiles->setEnabled(true);
        setAcceptDrops(true);
    }

}

void AMVtool::ProcessFile(int pos)
{
    ui->progressBar->setValue(0);
    ui->addFiles->setEnabled(false);
    ui->removeFiles->setEnabled(false);
    ui->configEncSettings->setEnabled(false);

    queue queue;
    QList<QStringList> inputDetails = queue.checkInput(pos);

    if (mainQueueInfo[pos][3] != "Error")
    {
        filesettings fs;
        if (!fs.checkFolder(outputConfig[pos][0]))
        {
            if (QMessageBox::question(this,"Output Error", "Cannot write to output folder, select another?", QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
            {
                QString newFolder = selectNewFolder();
                if (newFolder != "Skipped")
                {
                    outputConfig[pos][0] = newFolder;
                }
                else
                {
                    updateQueue(pos,"Skipped");
                }
            }
            else
            {
                updateQueue(pos,"Skipped");
            }

        }
        if (mainQueueInfo[pos][3] != "Skipped" && packetbuffererror != true)
        {
            QFileInfo file_exists(outputConfig[pos][0] + mainQueueInfo[pos][1].left(mainQueueInfo[pos][1].length()-4) + "-AMVtool." + outputConfig[pos][1].toLower());
            if (file_exists.exists() && mainQueueInfo[pos][4] != "2")
            {
                if (ui->ifExists->currentIndex() == 0)
                {
                    if (QMessageBox::question(this,"Overwrite File", "Output file already exists, overwrite it?", QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)
                    {
                        updateQueue(pos,"Skipped");
                    }
                }
                else
                {
					if (ui->ifExists->currentIndex() == 2)
                    	updateQueue(pos,"Skipped");
                }
            }
        }
        else
        {
            packetbuffererror = false;
        }
    }

    if (mainQueueInfo[pos][3] == "Pending")
    {
        Encode(pos,inputDetails, outputConfig[pos]);
    }
    else
    {
        CheckQueue();
    }
}

QString AMVtool::selectNewFolder()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly);
    QString outputFolder = "Try Again";
    while (outputFolder == "Try Again")
    {
        QString newFolder = dialog.getExistingDirectory(this,"Select Output Directory");
        if (newFolder != "")
        {
            if (newFolder.right(1) != "/")
                newFolder = newFolder + "/";

            filesettings fs;
            if (fs.checkFolder(newFolder))
            {
                outputFolder = newFolder;
            }
            else
            {
                QMessageBox::warning(this,"Error","Destination folder is not writable!");
            }

        }
        else
        {
            outputFolder = "Skipped";
        }
    }

    return outputFolder;
}

// SECTION FOR PROCESSING ENCODES

void AMVtool::Encode(int pos, QList<QStringList> inputDetails, QStringList configList)
{
    ui->statusBar->showMessage("Starting Encode");
    inputDuration = inputDetails[0][3].toFloat();
    outputcreated = false;
    setupencode se;
    encode = new QProcess(this);
    connect(encode, SIGNAL(readyReadStandardOutput()),this,SLOT(readyReadStandardOutput()));
    connect(encode, SIGNAL(error(QProcess::ProcessError)), this, SLOT(readErrors()));
    connect(encode, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(encodeFinished(int,QProcess::ExitStatus)));
    encode->setProcessChannelMode(QProcess::MergedChannels);
    QStringList EncodeOptions = se.SetupEncode(pos, mainQueueInfo[pos], inputDetails, configList);

    QString debugbox;
    for (int i = 0; i < EncodeOptions.count(); i++)
    {
        debugbox += EncodeOptions[i] + " ";
    }
//    QMessageBox::information(this,"Encode Options", debugbox);
    pipe = new QProcess(this);
    if (inputDetails[0][2] == "VapourSynth")
    {
        connect(pipe, SIGNAL(error(QProcess::ProcessError)), this, SLOT(readFromStdErr()));
        pipe->setStandardOutputProcess(encode);
        pipe->start(vspipeexec, se.SetupPipe(mainQueueInfo[pos][0], inputDetails[4][0]));
    }
    encode->setWorkingDirectory(outputConfig[pos][0]);
    encode->start(ffmpegexec, EncodeOptions);

    ui->convertFiles->setEnabled(true);
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
    if (pipe->state() > 0)
    {
        pipe->kill();
    }
    encode->deleteLater();
    pipe->deleteLater();

    if (exitcode == 0)
    {
        ui->progressBar->setValue(100);
        if (outputcreated == true)
        {
            if (mainQueueInfo[position][4] == "1" && outputConfig[position][7].contains("2 Pass") )
            {
                mainQueueInfo[position][4] = "2";
            }
            else
            {
                updateQueue(position, "Complete");
            }
        }
        else
        {
            updateQueue(position, "Error");
        }

        CheckQueue();
    }
    else
    {
        if (stopprocess == false)
        {
            updateQueue(position, "Error");
            CheckQueue();
        }
    }
}

void AMVtool::readErrors()
{
    QString error = encode->errorString();
    if (error.contains("No such file or directory"))
    {
        encode->kill();
        ui->statusBar->showMessage("FFmpeg executable is missing!");
    }
    ui->progressBar->setValue(0);
    setAcceptDrops(true);
    changeEnabled(true,"START");
    ui->convertFiles->setEnabled(true);
}

void AMVtool::readyReadStandardOutput()
{
    float duration = inputDuration;
    QString readline = encode->readAllStandardOutput();
    if (readline.contains("Too many packets buffered for output stream"))
        packetbuffererror = true;

    if (readline.contains("time="))
    {
        progress cp;
        QList<QString> currentprogress = cp.currentProcess(readline, duration);
        ui->statusBar->showMessage("Encoding: " + mainQueueInfo[position][1]);
        timeRemaining->setText(currentprogress[1] + "  ");
        ui->progressBar->setValue(currentprogress[0].toInt());
    }
    else
    {
        mOutputString.append(readline);
        ui->textEdit->setText(mOutputString);
        ui->textEdit->verticalScrollBar()->setSliderPosition(ui->textEdit->verticalScrollBar()->maximum());
    }
    if (readline.contains("muxing overhead:"))
    {
        outputcreated = true;
        timeRemaining->setText("");
        ui->statusBar->showMessage("Finished");
    }






}
