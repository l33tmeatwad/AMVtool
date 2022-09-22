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

void configure::setData(const int &selFile, QList<QStringList> inputMediaInfo, const QStringList configurationList, QList<int> hibitdepth)
{
    enableBitDepth(hibitdepth);

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

    if (configurationList[16].toInt() > 1)
    {
        ui->encodeIncompatible->setChecked(true);
        ui->copyAudio->setChecked(true);
    }
    else
        ui->copyAudio->setChecked(configurationList[16].toInt());

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
    setPreset();
    setTune();

    setAudioCodec();
    setAudioMode();

    ui->bitrateBox->setValue(videoEncBitrate);
    ui->bitrateBoxAudio->setValue(audioEncBitrate);
    ui->maxMuxing->setValue(configurationList[17].toInt());
    ui->experimentalFeatures->setChecked(configurationList[18].toInt());
}

// SHOW OR HIDE OPTIONS

void configure::enableBitDepth(QList<int> hibitdepth)
{
    if (!hibitdepth.contains(1))
    {
        ui->selectBitDepth->setVisible(false);
        ui->labelBitDepth->setVisible(false);
    }

    bool x264hi10 = hibitdepth[0];
    bool x264hi12 = hibitdepth[1];
    x264bitdepth = {x264hi10,x264hi12};

    bool x265hi10 = hibitdepth[2];
    bool x265hi12 = hibitdepth[3];
    x265bitdepth = {x265hi10,x265hi12};
}

void configure::setVideoVisibility()
{
    bool bitrateoptions;
    bool bitratetype;
    bool matrixoptions;
    bool modeoptions;
    bool presetoptions;
    bool tuneoptions;
    bool colorspace;
    QStringList lossless = {"Copy", "DNxHR","ProRes","UT Video"};

    if (lossless.contains(ui->selectCodec->currentText()))
    {
        bitrateoptions = false;
        presetoptions = false;
        tuneoptions = false;
        if (ui->selectCodec->currentText() == "DNxHR" || ui->selectCodec->currentText() == "ProRes")
            modeoptions = true;
        else
            modeoptions = false;
    }
    else
    {
        bitrateoptions = true;
        modeoptions = true;
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

    QStringList noMatrix = {"Copy","DNxHR"};
    if (noMatrix.contains(ui->selectCodec->currentText()) || ui->selectColorSpace->currentText().contains("RGB"))
    {
        matrixoptions = false;
    }
    else
    {
        matrixoptions = true;
    }

    if (ui->selectCodec->currentText() == "Copy" || ui->selectCodec->currentText() == "DNxHR")
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
    ui->selectMode->setVisible(modeoptions);
    ui->labelMode->setVisible(modeoptions);

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
    ui->selectContainer->clear();
    if (!outputColorMatrix.contains("BT.2020"))
        ui->selectContainer->addItem("AVI");
    ui->selectContainer->addItems({"MKV", "MOV", "MP4"});
    int containerindex = ui->selectContainer->findText(outputContainer);
    if (containerindex == -1)
        ui->selectContainer->setCurrentIndex(ui->selectContainer->findText("MP4"));
    else
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
        codecs.append({"x264", "x265"});
    }
    if (ui->selectContainer->currentText() == "MOV")
    {
        if (MOV.contains(inputVideoCodecs[vsIndex]))
        {
            codecs.append("Copy");
        }
        codecs.append({"DNxHR","ProRes","x264", "x265"});
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

void configure::setBitDepth()
{
    QString outputbit;
/*    QList<bool> bitdepth = {false,false};
    if (selectedDepth != "")
        outputbit = selectedDepth;
    else */
        outputbit = outputBitDepth;

    ui->selectBitDepth->clear();

    QStringList losslessMOV = {"ProRes"};
    if (!losslessMOV.contains(ui->selectCodec->currentText()))
        ui->selectBitDepth->addItem("8");

    if (ui->selectCodec->currentText() == "ProRes")
        ui->selectBitDepth->addItem("10");

    QStringList DNxHR10 = {"","High Quality","Finishing Quality"};
    if (ui->selectCodec->currentText() == "DNxHR" && DNxHR10.contains(ui->selectMode->currentText()))
        ui->selectBitDepth->addItem("10");


    if (ui->selectCodec->currentText() == "x264")
    {
        if (x264bitdepth[0])
            ui->selectBitDepth->addItem("10");
        if (x264bitdepth[1])
            ui->selectBitDepth->addItem("12");
    }

    if (ui->selectCodec->currentText() == "x265")
    {
        if (x265bitdepth[0])
            ui->selectBitDepth->addItem("10");
        if (x265bitdepth[1])
            ui->selectBitDepth->addItem("12");
    }

    int index = ui->selectBitDepth->findText(outputbit);
    if(index != -1) { ui->selectBitDepth->setCurrentIndex(index); }
    else { ui->selectBitDepth->setCurrentIndex(0); }

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
            int cvs = ui->selectVideoStream->currentIndex();
            colorspaceoptions.append({"YUV420", "YUV422", "RGB24","RGBA"});
        }
        if (codec == "ProRes")
        {
            colorspaceoptions.clear();
            colorspaceoptions.append({"YUV422", "YUVA444"});
        }


        ui->selectColorSpace->clear();
        ui->selectColorSpace->addItems(colorspaceoptions);
        int index;
        if (ui->selectCodec->currentText() == "x264" || ui->selectCodec->currentText() == "x265" || ui->selectCodec->currentText() == "ProRes")
            index = ui->selectColorSpace->findText(outputColorSpace);
        else
            index = ui->selectColorSpace->findText(colorspace);
        if(index != -1) { ui->selectColorSpace->setCurrentIndex(index); }
        else { ui->selectColorSpace->setCurrentIndex(0); }

}

void configure::setColorMatrix()
{
    ui->selectMatrix->clear();
    ui->selectMatrix->addItems({"BT.601", "BT.709"});

    if (ui->selectCodec->currentText() != "UT Video")
    {
        if (inputColorMatrix[vsIndex].contains("BT.2020"))
        {
            ui->selectMatrix->clear();
            ui->selectMatrix->addItem("BT.2020");
        }
    }

    int index = ui->selectMatrix->findText(outputColorMatrix);
    if(index != -1) { ui->selectMatrix->setCurrentIndex(index); }
    else { ui->selectMatrix->setCurrentIndex(0); }

}

void configure::setMode()
{
    ui->selectMode->clear();
    QStringList losslessMOV = {"DNxHR","ProRes"};
    if (losslessMOV.contains(ui->selectCodec->currentText()))
    {
        ui->labelMode->setText("Profile:");
        if (ui->selectCodec->currentText() == "DNxHR" && ui->selectBitDepth->currentText() == "8")
            ui->selectMode->addItems({"Low Bandwidth","Standard Quality","High Quality"});
        if (ui->selectCodec->currentText() == "DNxHR" && ui->selectBitDepth->currentText() == "10")
            ui->selectMode->addItems({"High Quality","Finishing Quality"});
        if (ui->selectCodec->currentText() == "ProRes" && ui->selectColorSpace->currentText() == "YUV422")
            ui->selectMode->addItems({"Auto","Proxy","LT","Standard","High Quality"});
        if (ui->selectCodec->currentText() == "ProRes" && ui->selectColorSpace->currentText() == "YUVA444")
            ui->selectMode->addItems({"High Quality","Highest Quality"});

        int index = ui->selectMode->findText(videoEncMode);
        if(index != -1){ ui->selectMode->setCurrentIndex(index); }
        else
        {
            index = ui->selectMode->findText("High Quality");
            if(index != -1){ ui->selectMode->setCurrentIndex(index); }
            else { ui->selectMode->setCurrentIndex(0); }
        }

    }
    else
    {
        ui->labelMode->setText("Mode:");
        ui->selectMode->addItems({"Constant Rate Factor","Bitrate 1 Pass","Bitrate 2 Pass"});
        int index = ui->selectMode->findText(videoEncMode);
        if(index != -1){ ui->selectMode->setCurrentIndex(index); }
        else { ui->selectMode->setCurrentIndex(0); }


    }

}

void configure::setPreset()
{
    int encPresetIndex = ui->selectPreset->findText(videoEncPreset);
    ui->selectPreset->setCurrentIndex(encPresetIndex);
}

void configure::setTune()
{
    if (ui->selectCodec->currentText().contains("x26"))
    {
        QString outputtune;
        if (ui->selectTune->currentIndex() != -1 && ui->selectTune->currentText() != "(None)")
            outputtune = ui->selectTune->currentText();
        else
            outputtune = videoEncTune;

        ui->selectTune->clear();
        ui->selectTune->addItem("(None)");
        if (ui->selectCodec->currentText() == "x264")
            ui->selectTune->addItems({"Film","Animation","Still Image"});

        ui->selectTune->addItems({"Grain","Fast Decode","Zero Latency","PSNR","SSIM"});

        int index = ui->selectTune->findText(outputtune);
        if(index != -1) { ui->selectTune->setCurrentIndex(index); }
        else { ui->selectTune->setCurrentIndex(0); }
    }
}


// SET AUDIO DETAILS

void configure::setAudioStream()
{
    int audioStreams;
    if (externalaudio && ui->externalAudioSource->text() != "")
        audioStreams = altAudioStreams;
    else
        audioStreams = inputAudioStreams;

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
    int index = ui->selectAudioStream->findText(outputAudioStream);

    if (newExtAudio && ui->externalAudio->isChecked())
        index = 0;
    if (outputAudioSource != "Original Audio" && !ui->externalAudio->isChecked())
        index = 0;

    if(index != -1)
        ui->selectAudioStream->setCurrentIndex(index);
    else
        ui->selectAudioStream->setCurrentIndex(0);

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
        ui->externalAudio->setChecked(false);
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
            value = 5;
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
    outputBitDepth = ui->selectBitDepth->currentText();
    videoEncMode = ui->selectMode->currentText();
    outputColorMatrix = ui->selectMatrix->currentText();
    if (outputColorMatrix == "BT.2020")
            outputColorMatrix = inputColorMatrix[outputVideoStream];
    if (outputVideoCodec == "DNxHR")
    {
        if (videoEncMode == "Finishing Quality")
            outputColorSpace = "YUV444";
        else
            outputColorSpace = "YUV422";
    }
    else
        outputColorSpace = ui->selectColorSpace->currentText();
    videoEncPreset = ui->selectPreset->currentText();
    if (ui->selectTune->currentIndex() != -1)
        videoEncTune = ui->selectTune->currentText();
    videoEncBitrate = ui->bitrateBox->value();
    outputAudioStream = ui->selectAudioStream->currentText();

    int copyaudio = 0;

    if (ui->copyAudio->isChecked() && ui->encodeIncompatible->checkState() == Qt::Unchecked)
    {
        copyaudio = 1;
    }
    else
    {
        if (ui->copyAudio->isChecked() && ui->encodeIncompatible->isChecked())
        {
            copyaudio = 2;
        }
    }
    outputAudioCodec = ui->selectAudioCodec->currentText();
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
                                    audioEncMode, QString::number(audioEncBitrate),QString::number(copyaudio),QString::number(ui->maxMuxing->value()),QString::number(ui->experimentalFeatures->isChecked())};


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

}

// CHANGED VALUE EVENTS

void configure::on_selectBitDepth_currentIndexChanged()
{
    if (ui->selectBitDepth->count() > 1)
    {
        outputBitDepth = ui->selectBitDepth->currentText();
    }
    if (ui->selectCodec->currentText() == "DNxHR")
        setMode();
}

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

void configure::on_selectCodec_currentIndexChanged()
{
    setBitDepth();
    setColorSpace();
    setColorMatrix();
    setMode();
    setTune();
    setVideoVisibility();
}

void configure::on_selectColorSpace_currentIndexChanged()
{
    setVideoVisibility();
    QStringList losslessMOV = {"DNxHR","ProRes"};
    if (losslessMOV.contains(ui->selectCodec->currentText()))
    {
        setBitDepth();
        setMode();
    }
}

void configure::on_selectMode_currentIndexChanged()
{
    if (ui->selectCodec->currentText().contains("x26"))
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
        outputColorMatrix = inputColorMatrix[index];
        setContainer();
        setVideoCodec();
        setBitDepth();
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
    if (ui->externalAudioSource->text() != "")
    {
        setAudioStream();
        setAudioCodec();
    }
    else
    {
        setAudioVisibility();
    }
}

void configure::on_browseAudio_clicked()
{
    QString newAudio = QFileDialog::getOpenFileName(this, tr("Open Media File"), "", "Media Files (*.avi *aac *m2ts *.m4v *mka *.mkv *.mov *mp3 *.mp4 *ts *wav)");
    if (newAudio != "")
    {
        newExtAudio = true;
        getAltAudioCodecs(newAudio);
    }
    setAudioCodec();
    setAudioStream();
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

