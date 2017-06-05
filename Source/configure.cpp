#include "configure.h"
#include "queue.h"
#include "ui_configure.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>

configure::configure(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::configure)
{
    ui->setupUi(this);
    ui->copyVideo->setVisible(false);
    ui->autoContainer->setVisible(false);
}

configure::~configure()
{
    delete ui;
}

void configure::setData(const int &selFile, QList<QStringList> inputMediaInfo, const QStringList configurationList)
{

    if (selFile == -1)
    {
        ui->copyVideo->setVisible(true);
        ui->copyVideo->setChecked(RecontainerSettings[0]);
        ui->autoContainer->setChecked(RecontainerSettings[1]);
    }


    selectedFile = selFile;

    inputVideoStreams = inputMediaInfo[0][0].toInt();
    inputAudioStreams = inputMediaInfo[0][1].toInt();
    inputContainer = inputMediaInfo[0][2];

    inputVideoBitDepths = inputMediaInfo[2];
    inputVideoCodecs = inputMediaInfo[3];
    inputColorSpaces = inputMediaInfo[4];
    inputColorMatrix = inputMediaInfo[5];
    inputVideoHeight = inputMediaInfo[7];
    inputAudioCodecs = inputMediaInfo[10];

    outputLocation = configurationList[0];
    outputContainer = configurationList[1];
    outputVideoStream = configurationList[2].toInt();
    outputColorSpace = configurationList[3];
    outputBitDepth = configurationList[4];
    outputColorMatrix = configurationList[5];
    outputVideoCodec = configurationList[6];
    videoEncMode = configurationList[7];
    videoEncPreset = configurationList[8];
    videoEncTune = configurationList[9];
    videoEncBitrate = configurationList[10].toInt();
    outputAudioSource = configurationList[11];
    outputAudioStream = configurationList[12];
    outputAudioCodec = configurationList[13];
    audioEncMode = configurationList[14];
    audioEncBitrate = configurationList[15].toInt();

    ui->copyAudio->setChecked(configurationList[16].toInt());
    if (outputAudioCodec != "Copy")
    {
        ui->encodeIncompatible->setChecked(true);
    }

    ui->outputFolder->setText(outputLocation);

    setVideoStream();
    if (outputAudioSource != "Original Audio")
    {
        ui->externalAudio->setChecked(true);
        getAltAudioCodecs(outputAudioSource);
    }
    else
    {
        setAudioStream();
    }


    setContainer();
    setVideoCodec();
    setColorSpace();
    setColorMatrix();
    setPreset();
    setMode();
    setTune();

    setAudioCodec();
    setAudioMode();

    ui->bitrateBox->setValue(videoEncBitrate);
    ui->bitrateBoxAudio->setValue(audioEncBitrate);
}

// SHOW OR HIDE OPTIONS

void configure::setVideoVisibility()
{
    bool bitrateoptions;
    bool bitratetype;
    bool matrixoptions;
    bool presetoptions;
    bool tuneoptions;
    bool colorspace;

    if (ui->selectCodec->currentText() == "UT Video" || ui->selectCodec->currentText() == "Copy")
    {
        bitrateoptions = false;
        presetoptions = false;
        tuneoptions = false;
    }
    else
    {
        bitrateoptions = true;
        presetoptions = true;
        tuneoptions = true;
    }

    if (ui->selectMode->currentText().contains("Bitrate") && bitrateoptions == true)
    {
        bitratetype = true;
    }
    else
    {
        bitratetype = false;
    }

    if (ui->selectCodec->currentText() == "Copy" || ui->selectColorSpace->currentText().contains("RGB"))
    {
        matrixoptions = false;
    }
    else
    {
        matrixoptions = true;
    }

    if (ui->selectCodec->currentText() == "Copy")
    {
        colorspace = false;
    }
    else
    {
        colorspace = true;
    }

    if (selectedFile < 0)
    {
        matrixoptions = false;
    }

    ui->bitrateBox->setVisible(bitrateoptions);
    ui->labelBitrate->setVisible(bitratetype);
    ui->selectMode->setVisible(bitrateoptions);
    ui->labelMode->setVisible(bitrateoptions);

    ui->selectColorSpace->setVisible(colorspace);
    ui->labelColorSpace->setVisible(colorspace);
    ui->selectMatrix->setVisible(matrixoptions);
    ui->labelMatrix->setVisible(matrixoptions);
    ui->selectPreset->setVisible(presetoptions);
    ui->labelPreset->setVisible(presetoptions);
    ui->selectTune->setVisible(tuneoptions);
    ui->labelTune->setVisible(tuneoptions);

}

void configure::setAudioVisibility()
{
    QString codec = ui->selectAudioCodec->currentText();
    QString stream = ui->selectAudioStream->currentText();
    bool audiocodec;
    bool bitrateoptions;
    bool audiocopy;
    bool encodeincompatible;

    if (codec == "ALAC" || codec == "FLAC" || codec == "PCM" || codec == "Copy" || stream == "None")
    {
        bitrateoptions = false;
    }
    else
    {
        bitrateoptions = true;
    }

    if (stream == "None")
    {
        audiocodec = false;
        audiocopy = false;
        encodeincompatible = false;
    }
    else
    {
        audiocodec = true;
        audiocopy = cancopyaudio;
        if (ui->copyAudio->isChecked())
        {
            encodeincompatible = cancopyaudio;
        }
        else
        {
            encodeincompatible = false;
        }

    }

    if (ui->copyAudio->isChecked() && ui->encodeIncompatible->checkState() == Qt::Unchecked)
    {
        bitrateoptions = false;
        audiocodec = false;
    }

    ui->selectAudioCodec->setVisible(audiocodec);
    ui->labelAudioCodec->setVisible(audiocodec);
    ui->selectAudioMode->setVisible(bitrateoptions);
    ui->labelAudioMode->setVisible(bitrateoptions);
    ui->bitrateBoxAudio->setVisible(bitrateoptions);
    ui->labelAudioBitrate->setVisible(bitrateoptions);
    ui->copyAudio->setVisible(audiocopy);
    ui->encodeIncompatible->setVisible(encodeincompatible);

    if (selectedFile == -1)
    {
        ui->externalAudio->setVisible(false);
        ui->internalAudio->setVisible(false);
    }
    ui->externalAudioSource->setVisible(externalaudio);
    ui->browseAudio->setVisible(externalaudio);
}

// SET VIDEO DETAILS

void configure::setVideoStream()
{
    if (inputVideoStreams < 2)
    {
        ui->labelVideoStream->setVisible(false);
        ui->selectVideoStream->setVisible(false);
    }
    for (int i = 1; i <= inputVideoStreams; i++)
    {
        ui->selectVideoStream->addItem(QString::number(i));
    }
    if (outputVideoStream < inputVideoStreams )
    {
        ui->selectVideoStream->setCurrentIndex(outputVideoStream);
    }
}


void configure::setContainer()
{
    ui->selectContainer->addItems({"AVI","MKV", "MOV", "MP4"});
    int containerindex = ui->selectContainer->findText(outputContainer);
    ui->selectContainer->setCurrentIndex(containerindex);
}

void configure::setVideoCodec()
{
    QString outputcodec;
    if (ui->selectCodec->currentIndex() != -1)
    {
        outputcodec = ui->selectCodec->currentText();
    }
    else
    {
        outputcodec = outputVideoCodec;
    }

    ui->selectCodec->clear();

    QStringList codecs;
    if (ui->selectContainer->currentText() == "AVI")
    {
        if (AVI.contains(inputVideoCodecs[vsIndex]))
        {
            codecs.append("Copy");
        }
        codecs.append("UT Video");
    }
    if (ui->selectContainer->currentText() == "MKV")
    {
        if (inputVideoCodecs[vsIndex] != "Script" && selectedFile != -1)
        {
            codecs.append("Copy");
        }
        codecs.append({"UT Video","x264", "x265"});
    }
    if (ui->selectContainer->currentText() == "MOV")
    {
        if (MOV.contains(inputVideoCodecs[vsIndex]))
        {
            codecs.append("Copy");
        }
        codecs.append({"UT Video","x264", "x265"});
    }
    if (ui->selectContainer->currentText() == "MP4")
    {
        if (MP4.contains(inputVideoCodecs[vsIndex]))
        {
            codecs.append("Copy");
        }
        codecs.append({"x264", "x265"});
    }

    ui->selectCodec->addItems(codecs);
    int index = ui->selectCodec->findText(outputcodec);
    if (index == -1)
    {
        if (codecs.contains("Copy"))
        {
            index = 1;
        }
        else
        {
            index = 0;
        }
    }
    ui->selectCodec->setCurrentIndex(index);
}

void configure::setColorSpace()
{
    QString codec = ui->selectCodec->currentText();
    QString colorspace = inputColorSpaces[ui->selectVideoStream->currentIndex()];
    if (colorspace == "RGB")
    {
        colorspace = "RGB24";
    }
    QStringList colorspaceoptions;
    colorspaceoptions.append("YUV420");

    if (colorspace.contains("RGB") || colorspace.contains("422") || colorspace.contains("444") || colorspace.contains("BGR"))
    {
        colorspaceoptions.append("YUV422");
        if (!colorspace.contains("422"))
        {
            colorspaceoptions.append("YUV444");
        }
    }
    if (codec == "UT Video")
    {
        colorspaceoptions.clear();
        if (ui->selectBitDepth->currentText() == "8")
        {
            colorspaceoptions.append({"YUV420", "YUV422", "RGB24","RGBA"});
        }
        else
        {
            colorspaceoptions.append({"YUV422", "RGB24","RGBA"});
        }
    }
    ui->selectColorSpace->clear();
    ui->selectColorSpace->addItems(colorspaceoptions);
    int index = ui->selectColorSpace->findText(colorspace);
    if(index != -1) { ui->selectColorSpace->setCurrentIndex(index); }
    else { ui->selectColorSpace->setCurrentIndex(0); }

}


void configure::setColorMatrix()
{
    if (outputColorMatrix == "DETECT")
    {
        if (inputColorMatrix[vsIndex] != "")
        {
            outputColorMatrix = inputColorMatrix[vsIndex];
        }
        else
        {
            if (inputVideoHeight[vsIndex].toInt() > 580)
            {
                outputColorMatrix = "BT.709";
            }
            else
            {
                outputColorMatrix = "BT.601";
            }
        }
    }

    ui->selectMatrix->clear();
    ui->selectMatrix->addItems({"BT.601", "BT.709"});
    int index = ui->selectMatrix->findText(outputColorMatrix);
    if(index != -1) { ui->selectMatrix->setCurrentIndex(index); }
    else { ui->selectMatrix->setCurrentIndex(0); }

}

void configure::setMode()
{
    int encModeIndex = ui->selectMode->findText(videoEncMode);
    ui->selectMode->setCurrentIndex(encModeIndex);
}

void configure::setPreset()
{
    int encPresetIndex = ui->selectPreset->findText(videoEncPreset);
    ui->selectPreset->setCurrentIndex(encPresetIndex);
}

void configure::setTune()
{
    int encTuneIndex = ui->selectTune->findText(videoEncTune);
    ui->selectTune->setCurrentIndex(encTuneIndex);
}


// SET AUDIO DETAILS

void configure::setAudioStream()
{
    int audioStreams;
    if (externalaudio && ui->externalAudioSource->text() != "")
    {
        audioStreams = altAudioStreams;
    }
    else
    {
        audioStreams = inputAudioStreams;
    }

    ui->selectAudioStream->clear();
    if (ui->selectContainer->currentText() != "AVI")
    {
        if (audioStreams > 1 || selectedFile == -1)
        {
            ui->selectAudioStream->addItem("All");
        }
    }
    for (int i = 1; i <= audioStreams; i++)
    {
        ui->selectAudioStream->addItem(QString::number(i));
    }
    ui->selectAudioStream->addItem("None");
    if (outputAudioStream != "All")
    {
        if (outputAudioStream.toInt() < audioStreams )
        {
            ui->selectAudioStream->setCurrentIndex(outputAudioStream.toInt());
        }
    }

    int index = ui->selectAudioStream->findText(outputAudioStream);
    if(index != -1) { ui->selectAudioStream->setCurrentIndex(index); }
    else { ui->selectAudioStream->setCurrentIndex(0); }

}

void configure::setAudioCodec()
{

    QString outputcodec;
    cancopyaudio = false;
    if (ui->selectAudioCodec->currentIndex() != -1)
    {
        outputcodec = ui->selectAudioCodec->currentText();
    }
    else
    {
        outputcodec = outputAudioCodec;
    }

    ui->selectAudioCodec->clear();

    QStringList codecs;
    if (ui->selectContainer->currentText() == "AVI")
    {
        canCopyAudio(AVI);
        codecs.append({ "MP3", "PCM" });
    }
    if (ui->selectContainer->currentText() == "MKV")
    {
        cancopyaudio = true;
        codecs.append({ "AAC", "ALAC", "FLAC", "MP3", "PCM" });
    }
    if (ui->selectContainer->currentText() == "MOV")
    {
        canCopyAudio(MOV);
        codecs.append({ "AAC", "ALAC", "PCM" });
    }
    if (ui->selectContainer->currentText() == "MP4")
    {
        canCopyAudio(MP4);
        codecs.append("AAC");
    }
    ui->selectAudioCodec->addItems(codecs);
    int index = ui->selectAudioCodec->findText(outputcodec);

    if (index == -1)
    {
        index = 0;
    }
    ui->selectAudioCodec->setCurrentIndex(index);
    if (selectedFile == -1)
    {
        cancopyaudio = true;

    }

    setAudioVisibility();
}

void configure::canCopyAudio(QStringList containerinfo)
{
    QStringList audioCodecs;
    if (externalaudio && ui->externalAudioSource->text() != "")
    {
        audioCodecs.append(altAudioCodecs);
    }
    else
    {
        audioCodecs.append(inputAudioCodecs);
    }

    QList<bool> cancopy;
    foreach (QString codec, audioCodecs)
    {
        cancopy.append(containerinfo.contains(codec));
    }
    if (cancopy.contains(true))
    {
        cancopyaudio = true;
    }
    else
    {
        cancopyaudio = false;
    }

}

void configure::setAudioMode()
{
    int encModeIndex = ui->selectAudioMode->findText(audioEncMode);
    ui->selectAudioMode->setCurrentIndex(encModeIndex);
}

void configure::getAltAudioCodecs(QString newAudio)
{
    queue queue;
    QList<QStringList> newAudioInfo = queue.getInputDetails(newAudio);
    int streamcount = newAudioInfo[0][1].toInt();
    if (streamcount > 0)
    {
        altAudioStreams = streamcount;
        altAudioCodecs = newAudioInfo[10];
        ui->externalAudioSource->setText(newAudio);
        setAudioStream();
    }
    else
    {
        QMessageBox::information(this,"Error","External audio file contains no audio streams, switching back to internal audio.");
        outputAudioSource = "OriginalAudio";
        ui->externalAudioSource->setText("");
        ui->internalAudio->setChecked(true);
    }

}

void configure::setAudioBitrate()
{
    int min;
    int max;
    int value;
    QString valueinfo;

    if (ui->selectAudioMode->currentText() == "Quality")
    {
        QString codec = ui->selectAudioCodec->currentText();
        if (codec == "AAC")
        {
            min = 1;
            max = 5;
            value = 4;
            valueinfo = "5 (Best) to 1 (Worst)";
        }
        if (codec == "MP3")
        {
            min = 0;
            max = 9;
            value = 5;
            valueinfo = "0 (Best) to 9 (Worst)";
        }
        if (codec == "ALAC" || codec == "FLAC" || codec == "PCM" || codec == "Copy")
        {
            min = 0;
            max = 0;
            value = 0;
            valueinfo = "";
        }

    }
    else
    {
        min = 45;
        max = 320;
        value = 128;
        valueinfo = "Kbps";
    }
    ui->bitrateBoxAudio->setMinimum(min);
    ui->bitrateBoxAudio->setMaximum(max);
    ui->bitrateBoxAudio->setValue(value);

    ui->labelAudioBitrate->setText(valueinfo);
}


// BUTTON ACTIONS


void configure::on_buttonBox_accepted()
{
    outputLocation = ui->outputFolder->text();
    outputContainer = ui->selectContainer->currentText();
    outputVideoStream = ui->selectVideoStream->currentIndex();
    outputVideoCodec = ui->selectCodec->currentText();
    outputColorSpace = ui->selectColorSpace->currentText();
    outputBitDepth = ui->selectBitDepth->currentText();
    outputColorMatrix = ui->selectMatrix->currentText();
    videoEncMode = ui->selectMode->currentText();
    videoEncPreset = ui->selectPreset->currentText();
    videoEncTune = ui->selectTune->currentText();
    videoEncBitrate = ui->bitrateBox->value();
    outputAudioStream = ui->selectAudioStream->currentText();
    bool copyaudio = false;

    if (ui->copyAudio->isChecked() && ui->encodeIncompatible->checkState() == Qt::Unchecked && cancopyaudio == true)
    {
        outputAudioCodec = "Copy";
        copyaudio = true;
    }
    else
    {
        if (ui->encodeIncompatible->isChecked())
        {
            copyaudio = true;
        }
        outputAudioCodec = ui->selectAudioCodec->currentText();
    }

    audioEncMode = ui->selectAudioMode->currentText();
    audioEncBitrate = ui->bitrateBoxAudio->value();

    if (externalaudio && ui->externalAudioSource->text() != "")
    {
        outputAudioSource = ui->externalAudioSource->text();
    }
    else
    {
        outputAudioSource = "Original Audio";
    }


    QStringList configurationList = { outputLocation, outputContainer, QString::number(outputVideoStream), outputColorSpace, outputBitDepth, outputColorMatrix, outputVideoCodec,
                                      videoEncMode, videoEncPreset, videoEncTune, QString::number(videoEncBitrate), outputAudioSource, outputAudioStream, outputAudioCodec,
                                    audioEncMode, QString::number(audioEncBitrate),QString::number(copyaudio)};


    fs.changeSettings(selectedFile, inputVideoBitDepths[ui->selectVideoStream->currentIndex()], configurationList);

    if (selectedFile == -1 && ui->copyVideo->isChecked())
    {
        queue queue;
        queue.setupRecontainer(ui->autoContainer->isChecked());
    }


    this->close();
}

void configure::on_recontainer_clicked()
{
    if (QMessageBox::question(this,"Audio Option", "Would you like to include the audio?", QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)
    {
        int index = ui->selectAudioStream->findText("None");
        if (index < 0)
        {
            index = 0;
        }
        ui->selectAudioStream->setCurrentIndex(index);
    }
    else
    {
        if (ui->selectAudioStream->currentText() == "None")
        {
            ui->selectAudioStream->setCurrentIndex(0);
        }
    }
    if (selectedFile < 0)
    {
        ui->copyVideo->setChecked(true);
        if (QMessageBox::question(this,"Audio Option", "Would you like to only recontainer videos compatible with " + ui->selectContainer->currentText() + "?", QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
        {
            ui->autoContainer->setChecked(false);
        }
        else
        {
            ui->autoContainer->setChecked(true);
        }

    }
    else
    {
        filesettings fs;
        QStringList containers = fs.findContainers(inputVideoCodecs[ui->selectVideoStream->currentIndex()]);

        if (containers[0] == inputContainer)
        {
            containers.removeFirst();
        }

        if (containers[0] != "None")
        {
            int containerindex = ui->selectContainer->findText(containers[0]);
            ui->selectContainer->setCurrentIndex(containerindex);
            int index = ui->selectCodec->findText("Copy");
            if (index == -1)
            {
                index = 0;
            }
            ui->selectCodec->setCurrentIndex(index);
            ui->copyAudio->setChecked(true);
        }
    }
}

// CHANGED VALUE EVENTS


void configure::on_selectContainer_currentIndexChanged()
{
    if (ui->selectCodec->currentIndex() != -1)
    {
        setVideoCodec();
    }
    if (ui->selectAudioCodec->currentIndex() != -1)
    {
        setAudioCodec();
    }
    setVideoVisibility();
    setAudioVisibility();
    setAudioStream();
}


void configure::on_selectBitDepth_currentIndexChanged()
{
    if (ui->selectCodec->currentIndex() != -1)
    {
        setVideoCodec();
    }
    setVideoVisibility();
}

void configure::on_selectCodec_currentIndexChanged()
{
    setColorSpace();
    setVideoVisibility();
}

void configure::on_selectColorSpace_currentIndexChanged()
{
    setVideoVisibility();
}

void configure::on_selectMode_currentIndexChanged()
{
    if (ui->selectMode->currentText() == "Constant Rate Factor")
    {
        ui->bitrateBox->setMaximum(51);
        ui->bitrateBox->setValue(18);
    }
    else
    {
        ui->bitrateBox->setMinimum(5);
        ui->bitrateBox->setMaximum(50000);
        ui->bitrateBox->setValue(2500);
    }
    setVideoVisibility();
}

void configure::on_browseOutput_clicked()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly);
    QString outputFolder = dialog.getExistingDirectory(this,"Select Output Directory");
    if (outputFolder != "")
    {
        if (outputFolder.right(1) != "/")
            outputFolder = outputFolder + "/";

        filesettings fs;
        if (fs.checkFolder(outputFolder))
        {
            ui->outputFolder->setText(outputFolder);
        }
        else
        {
            QMessageBox::warning(this,"Error","Destination folder is not writable!");
        }

    }
}

void configure::on_selectVideoStream_currentIndexChanged(int index)
{
    vsIndex = index;
    if (ui->selectMatrix->currentIndex() != -1)
    {
        outputColorMatrix = "DETECT";
        setColorSpace();
        setColorMatrix();
    }

}

void configure::on_selectAudioStream_currentIndexChanged(int index)
{
    if (index <= inputAudioStreams && index != 0 )
    {
        asIndex = index-1;
    }
    else
    {
        asIndex = 0;
    }
    setAudioVisibility();

}

void configure::on_selectAudioMode_currentIndexChanged()
{
    setAudioBitrate();
}


void configure::on_selectAudioCodec_currentIndexChanged()
{
    setAudioBitrate();
    setAudioVisibility();
}



void configure::on_externalAudio_toggled(bool checked)
{
    externalaudio = checked;
    setAudioStream();
    setAudioCodec();
}

void configure::on_browseAudio_clicked()
{
    QString newAudio = QFileDialog::getOpenFileName(this, tr("Open Media File"), "", "Media Files (*.avi *aac *m2ts *.m4v *mka *.mkv *.mov *mp3 *.mp4 *ts *wav)");
    if (newAudio != "")
    {
        getAltAudioCodecs(newAudio);
    }
    setAudioCodec();
}

void configure::on_copyAudio_toggled(bool checked)
{
    ui->encodeIncompatible->setVisible(checked);
    setAudioVisibility();
}

void configure::on_encodeIncompatible_toggled()
{
    setAudioVisibility();
}

void configure::on_copyVideo_toggled(bool checked)
{
    ui->autoContainer->setVisible(checked);
}
