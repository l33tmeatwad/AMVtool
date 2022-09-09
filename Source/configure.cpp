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
        ui->copyVideo->setChecked(RecontainerSettings);
    }


    selectedFile = selFile;

    inputVideoStreams = inputMediaInfo[0][0].toInt();
    inputAudioStreams = inputMediaInfo[0][1].toInt();
    inputContainer = inputMediaInfo[0][2];

    inputVideoBitDepths = inputMediaInfo[2];
    inputLumaRange = inputMediaInfo[3];
    inputVideoCodecs = inputMediaInfo[4];
    inputColorSpaces = inputMediaInfo[5];
    inputColorMatrix = inputMediaInfo[6];
    inputVideoHeight = inputMediaInfo[8];
    inputAudioCodecs = inputMediaInfo[11];

    outputLocation = configurationList[0];
    outputContainer = configurationList[1];
    outputVideoStream = configurationList[2].toInt();
    outputColorSpace = configurationList[3];
    outputBitDepth = configurationList[4];
    outputColorMatrix = configurationList[5];
    convertHDR2SDR = configurationList[6].toInt();
    outputVideoCodec = configurationList[7];
    videoEncMode = configurationList[8];
    videoEncPreset = configurationList[9];
    videoEncTune = configurationList[10];
    videoEncBitrate = configurationList[11].toInt();
    outputAudioSource = configurationList[12];
    outputAudioStream = configurationList[13];
    outputAudioCodec = configurationList[14];
    audioEncMode = configurationList[15];
    audioEncBitrate = configurationList[16].toInt();
    deinterlaceType = configurationList[18];
    cthreshValue = configurationList[19].toInt();
    outputFieldOrder = configurationList[20];
    outputResize = configurationList[21];
    outputAR = configurationList[22];

    if (configurationList[17].toInt() > 1)
    {
        ui->encodeIncompatible->setChecked(true);
        ui->copyAudio->setChecked(true);
    }
    else
        ui->copyAudio->setChecked(configurationList[17].toInt());

    int interlaceIndex = ui->interlaceOptions->findText(deinterlaceType);
    ui->interlaceOptions->setCurrentIndex(interlaceIndex);
    setInterlaceVisibility(interlaceIndex);

    ui->cthresh->setValue(cthreshValue);
    ui->fieldOrder->setCurrentIndex(ui->fieldOrder->findText(outputFieldOrder));

    if (outputResize != "No")
        ui->resizeVideo->setChecked(true);

    if (outputAR != "No")
        ui->changeDAR->setChecked(true);
    else
        ui->changeDARSel->setVisible(false);

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
    setLumaConversion();
    setPreset();
    setTune();

    setAudioCodec();
    setAudioMode();

    setResizeOptions();

    ui->bitrateBox->setValue(videoEncBitrate);
    ui->bitrateBoxAudio->setValue(audioEncBitrate);
    ui->maxMuxing->setValue(configurationList[23].toInt());
    ui->experimentalFeatures->setChecked(configurationList[24].toInt());
}

// SHOW OR HIDE OPTIONS

void configure::enableBitDepth(QList<int> hibitdepth)
{
    bool x264hi10 = hibitdepth[0];
    bool x264hi12 = hibitdepth[1];
    x264bitdepth = {x264hi10,x264hi12};

    bool x265hi10 = hibitdepth[2];
    bool x265hi12 = hibitdepth[3];
    x265bitdepth = {x265hi10,x265hi12};

    bool vp9hi10 = hibitdepth[4];
    bool vp9hi12 = hibitdepth[5];
    vp9bitdepth = {vp9hi10,vp9hi12};
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
    bool codecsettings;
    bool changedar;
    QStringList productionCodecs = {"Copy", "DNxHR","ProRes","UT Video"};

    if (productionCodecs.contains(ui->selectCodec->currentText()))
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
        if (ui->selectCodec->currentText() == "VP9")
        {
            presetoptions = false;
            tuneoptions = false;
        }
        else
        {
            presetoptions = true;
            tuneoptions = true;
        }
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

    if (ui->selectCodec->currentText() == "Copy" || ui->selectCodec->currentText() == "UT Video")
    {
        codecsettings = false;
    }
    else
    {
        codecsettings = true;
    }

    if (selectedFile < 0)
    {
        matrixoptions = false;
    }

    if (ui->selectMode->currentText().contains("Lossless"))
    {
        bitrateoptions = false;
        tuneoptions = false;
    }

    if (ui->selectContainer->currentText() == "AVI")
    {
        changedar = false;
        ui->changeDARSel->setVisible(changedar);
    }
    else
    {
        changedar = true;
        ui->changeDARSel->setVisible(ui->changeDAR->isChecked());
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
    ui->codecSettings->setVisible(codecsettings);
    ui->changeDAR->setVisible(changedar);

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

    if (ui->copyAudio->isChecked() && ui->encodeIncompatible->checkState() == Qt::Unchecked && cancopyaudio)
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
        //ui->internalAudio->setVisible(false);
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
    ui->selectContainer->addItems({"AVI","MKV", "MOV", "MP4","WEBM"});
    int containerindex = ui->selectContainer->findText(outputContainer);
    ui->selectContainer->setCurrentIndex(containerindex);
}

void configure::setLumaConversion()
{
    ui->convertHDR->setChecked(convertHDR2SDR);
    if (ui->selectCodec->currentText() == "DNxHR" || ui->selectCodec->currentText() == "UT Video")
    {
        ui->convertHDR->setChecked(true);
        ui->convertHDR->setEnabled(false);
    }
    else
    {
        ui->convertHDR->setChecked(convertHDR2SDR);
        ui->convertHDR->setEnabled(true);
    }
    if (inputLumaRange[ui->selectVideoStream->currentIndex()].contains("HDR"))
        ui->convertHDR->setVisible(true);
    else
        ui->convertHDR->setVisible(false);

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
        codecs.append({"x264", "x265", "VP9"});
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
    if (ui->selectContainer->currentText() == "WEBM")
    {
        if (WEBM.contains(inputVideoCodecs[vsIndex]))
        {
            codecs.append("Copy");
        }
        codecs.append("VP9");
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

    if (!ui->selectCodec->currentText().contains("ProRes"))
        ui->selectBitDepth->addItem("8");

    if (ui->selectCodec->currentText() == "ProRes")
        ui->selectBitDepth->addItem("10");

    QStringList DNxHR10 = {"High Quality","Finishing Quality"};
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

    if (ui->selectCodec->currentText() == "VP9")
    {
        if (vp9bitdepth[0])
            ui->selectBitDepth->addItem("10");
        if (vp9bitdepth[1])
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
            if (ui->selectBitDepth->currentText() == "8")
                colorspaceoptions.append({"YUV420", "YUV422", "RGB24","RGBA"});
            else
                colorspaceoptions.append({"YUV422", "RGB24","RGBA"});
        }
        if (codec == "ProRes")
        {
            colorspaceoptions.clear();
            colorspaceoptions.append({"YUV422", "YUVA444"});
        }


        ui->selectColorSpace->clear();
        ui->selectColorSpace->addItems(colorspaceoptions);
        int index = ui->selectColorSpace->findText(outputColorSpace);
        if(index != -1) { ui->selectColorSpace->setCurrentIndex(index); }
        else { ui->selectColorSpace->setCurrentIndex(0); }

}

void configure::setColorMatrix()
{
    ui->selectMatrix->clear();
    ui->selectMatrix->addItems({"BT.601", "BT.709"});

    if (ui->convertHDR->isChecked() && inputLumaRange[ui->selectVideoStream->currentIndex()].contains("HDR") && outputColorMatrix == "BT.2020NC")
    {
        outputColorMatrix = "BT.709";
    }
    else
    {
        if (ui->selectCodec->currentText() != "")
        {
            if (ui->selectCodec->currentText() != "UT Video")
            {
                if (ui->selectCodec->currentText() != "DNxHR")
                {
                    if (inputColorMatrix[vsIndex].contains("BT.2020") && !ui->convertHDR->isChecked())
                    {
                        ui->selectMatrix->clear();
                        ui->selectMatrix->addItem("BT.2020");
                    }
                }

            }
        }

    }

    int index = ui->selectMatrix->findText(outputColorMatrix);
    if(index != -1) { ui->selectMatrix->setCurrentIndex(index); }
    else { ui->selectMatrix->setCurrentIndex(0); }

}

void configure::setMode()
{
    ui->selectMode->clear();
    QStringList mezzanineMOV = {"DNxHR","ProRes"};
    if (mezzanineMOV.contains(ui->selectCodec->currentText()))
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
        if(ui->selectCodec->currentText() == "VP9")
            ui->selectMode->addItems({"Constant Rate 1 Pass","Constant Rate 2 Pass"});
        else
            ui->selectMode->addItem("Constant Rate Factor");
        ui->selectMode->addItems({"Bitrate 1 Pass","Bitrate 2 Pass"});
        if (ui->selectCodec->currentText().contains("x265"))
            ui->selectMode->addItem("Lossless");
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
    if (externalaudio || ui->externalAudioSource->text() != "")
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
        codecs.append({ "AAC", "ALAC", "FLAC", "MP3", "Opus", "PCM" });
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
    if (ui->selectContainer->currentText() == "WEBM")
    {
        canCopyAudio(WEBM);
        codecs.append("Opus");
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

    if (ui->selectAudioCodec->currentText() == "Opus")
    {
        ui->selectAudioMode->clear();
        ui->selectAudioMode->addItem("Bitrate");
    }
    else
    {
       ui->selectAudioMode->clear();
       ui->selectAudioMode->addItems({"Bitrate","Quality"});
    }
    int encModeIndex = ui->selectAudioMode->findText(audioEncMode);
    if (encModeIndex != -1)
        ui->selectAudioMode->setCurrentIndex(encModeIndex);
    else
        ui->selectAudioMode->setCurrentIndex(0);
}

void configure::getAltAudioCodecs(QString newAudio)
{
    queue queue;
    QList<QStringList> newAudioInfo = queue.getInputDetails(newAudio);
    int streamcount = newAudioInfo[0][1].toInt();
    if (streamcount > 0)
    {
        altAudioStreams = streamcount;
        altAudioCodecs = newAudioInfo[11];
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
    int min = 0;
    int max = 0;
    int value = 0;
    QString valueinfo = "";

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
    }
    else
    {
        min = 45;
        max = 320;
        value = 192;
        valueinfo = "Kbps";
    }
    ui->bitrateBoxAudio->setMinimum(min);
    ui->bitrateBoxAudio->setMaximum(max);
    ui->bitrateBoxAudio->setValue(value);

    ui->labelAudioBitrate->setText(valueinfo);
}

//SET ADVANCED OPTIONS

void configure::setInterlaceVisibility(int index)
{
    if (index == 0 || index == 2 || ui->selectCodec->currentText() == "Copy")
    {
        ui->labelcthresh->setVisible(false);
        ui->cthresh->setVisible(false);
        if (index == 2)
        {
            ui->labelFieldOrder->setVisible(true);
            ui->fieldOrder->setVisible(true);
        }
        else
        {
            ui->labelFieldOrder->setVisible(false);
            ui->fieldOrder->setVisible(false);
        }
    }
    else
    {
        ui->labelcthresh->setVisible(true);
        ui->cthresh->setVisible(true);
        ui->labelFieldOrder->setVisible(true);
        ui->fieldOrder->setVisible(true);
    }
    if (ui->selectCodec->currentText() == "Copy")
        ui->interlaceOptions->setVisible(false);
    else
        ui->interlaceOptions->setVisible(true);
}

void configure::setResizeOptions ()
{
    bool visible = false;
    if (!ui->resizeResolution->isChecked() && !ui->resizeWidth->isChecked() && !ui->resizeHeight->isChecked())
    {
        if (outputResize != "No" && ui->resizeVideo->isChecked())
        {
            if (FullResolutions.contains(outputResize))
                ui->resizeResolution->setChecked(true);
            if (WidthResolutions.contains(outputResize))
                ui->resizeWidth->setChecked(true);
            if (HeightResolutions.contains(outputResize))
                ui->resizeHeight->setChecked(true);
            visible = true;
        }
        else
        {
            if (ui->resizeVideo->isChecked() && outputResize == "No")
            {
                ui->resizeResolution->setChecked(true);
                visible = true;
            }
        }
    }
    else
    {
        if (ui->resizeVideo->isChecked())
            visible = true;
    }
    if (ui->selectCodec->currentText() == "Copy")
    {
        ui->resizeVideo->setVisible(false);
        visible = false;
    }
    else
        ui->resizeVideo->setVisible(true);

    ui->resizeVideoSel->setVisible(visible);
    ui->resizeResolution->setVisible(visible);
    ui->resizeWidth->setVisible(visible);
    ui->resizeHeight->setVisible(visible);

    if (visible)
    {
        ui->resizeVideoSel->clear();
        if (ui->resizeResolution->isChecked())
            ui->resizeVideoSel->addItems(FullResolutions);
        if (ui->resizeWidth->isChecked())
            ui->resizeVideoSel->addItems(WidthResolutions);
        if (ui->resizeHeight->isChecked())
            ui->resizeVideoSel->addItems(HeightResolutions);

        int index = ui->resizeVideoSel->findText(outputResize);
        if(index != -1) { ui->resizeVideoSel->setCurrentIndex(index); }
        else { ui->resizeVideoSel->setCurrentIndex(0); }
        outputResize = ui->resizeVideoSel->currentText();
    }
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
        outputColorMatrix = "BT.2020NC";
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

    deinterlaceType = ui->interlaceOptions->currentText();

    if (deinterlaceType != "None" && deinterlaceType != "Blend" && outputVideoCodec != "Copy")
    {
        cthreshValue = ui->cthresh->value();
        outputFieldOrder = ui->fieldOrder->currentText();
    }
    else
        if (outputVideoCodec == "Copy")
            deinterlaceType = "None";

    if (ui->resizeVideo->isChecked() && outputVideoCodec != "Copy")
        outputResize = ui->resizeVideoSel->currentText();
    else
        outputResize = "No";

    if (ui->changeDAR->isChecked() && outputContainer != "AVI")
        outputAR = ui->changeDARSel->currentText();
    else
        outputAR = "No";


    QStringList configurationList = { outputLocation, outputContainer, QString::number(outputVideoStream), outputColorSpace, outputBitDepth, outputColorMatrix, QString::number(ui->convertHDR->isChecked()), outputVideoCodec,
                                      videoEncMode, videoEncPreset, videoEncTune, QString::number(videoEncBitrate), outputAudioSource, outputAudioStream, outputAudioCodec,
                                    audioEncMode, QString::number(audioEncBitrate), QString::number(copyaudio), deinterlaceType, QString::number(cthreshValue), outputFieldOrder, outputResize, outputAR, QString::number(ui->maxMuxing->value()),QString::number(ui->experimentalFeatures->isChecked())};


    fs.changeSettings(selectedFile, inputVideoBitDepths[ui->selectVideoStream->currentIndex()] + " " + inputLumaRange[ui->selectVideoStream->currentIndex()], configurationList);

    if (selectedFile == -1 && ui->copyVideo->isChecked())
    {
        queue queue;
        queue.setupRecontainer();
    }


    this->close();
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
    setLumaConversion();
    setBitDepth();
    setColorSpace();
    setColorMatrix();
    setMode();
    setTune();
    setVideoVisibility();
    setInterlaceVisibility(ui->interlaceOptions->currentIndex());
    setResizeOptions();
}

void configure::on_selectColorSpace_currentIndexChanged()
{
    setVideoVisibility();
    QStringList mezzanineMOV = {"DNxHR","ProRes"};
    if (mezzanineMOV.contains(ui->selectCodec->currentText()))
    {
        setBitDepth();
        setMode();
    }
}

void configure::on_selectMode_currentIndexChanged()
{
    QStringList lossyCodecs = {"x264","x265","VP9"};
    if (lossyCodecs.contains(ui->selectCodec->currentText()))
    {
        if (ui->selectMode->currentText().contains("Constant Rate"))
        {
            ui->bitrateBox->setMaximum(51);
            ui->bitrateBox->setValue(18);
            ui->bitrateBox->setMinimum(1);
        }
        else
        {
            if (!ui->selectMode->currentText().contains("Lossless"))
            {
                ui->bitrateBox->setMinimum(5);
                ui->bitrateBox->setMaximum(50000);
                ui->bitrateBox->setValue(2500);
            }

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
    setAudioMode();
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

void configure::on_convertHDR_toggled()
{
    setColorMatrix();
    setVideoVisibility();
}

void configure::on_convertHDR_clicked(bool checked)
{
    convertHDR2SDR = checked;
}

void configure::on_interlaceOptions_currentIndexChanged(int index)
{
    setInterlaceVisibility(index);
}

void configure::on_resizeVideo_toggled()
{
    setResizeOptions();
}

void configure::on_resizeResolution_toggled()
{
    setResizeOptions();
}


void configure::on_resizeHeight_toggled()
{
    setResizeOptions();
}


void configure::on_resizeWidth_toggled()
{
    setResizeOptions();
}

void configure::on_changeDAR_toggled(bool checked)
{
    ui->changeDARSel->setVisible(checked);
    if (outputAR != "No")
    {
        int index = ui->changeDARSel->findText(outputAR);
        if(index != -1) { ui->changeDARSel->setCurrentIndex(index); }
        else { ui->changeDARSel->setCurrentIndex(0); }
        outputAR = ui->changeDARSel->currentText();
    }
}




