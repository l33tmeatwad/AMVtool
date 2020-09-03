AMVtool is a GUI created with Qt for FFMpeg that is geared towards the needs of AMV editors.  In addition to handling almost all the formats that editors will use, it also helps out by automatically adjusting certain encode settings that are often incorrectly selected by many editors.  Here are some of the features:


- Available on Linux, macOS, and Windows.
- Automatically chooses the color matrix based on the existing file or guesses based on resolution.
- Automatically detects compatibile containers for copying video.
- Easy button for setting up file(s) to be recontainered.
- Queue for batch encoding.
- Shows bit depth of the currently selected video stream.
- Supports encoding of AviSynth (Windows Only) & VapourSynth scripts.
- Supports encoding to various lossless and lossy formats, including but not limited to x264 & x265.
- Supports multiple audio streams.

Configure vs Configure All

Configure will allow you to configure the settings for the currently selected file in the queue.  Unlike Configure All, this option will allow you to select an alternate audio file to use when encoding and allow you to select which audio or video stream you want to use (if applicable).  Configure All will allow you to change the settings for all files in the queue and will also be the new default settings for any new files added to the queue until the program is restarted.
