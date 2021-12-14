# OE_whitening
A online whitening plugin for the Open Ephys GUI

![comparison](https://user-images.githubusercontent.com/3406709/145995564-7b4942b5-1d6c-48c1-b9a2-11f89c6059c8.png)


## Usage

<img width="225" alt="image" src="https://user-images.githubusercontent.com/3406709/145992578-022b389d-8f80-43b0-ba9a-001eb396a8b6.png">


This filter plugin implements online whitening on neural signal. It is useful for online spike detection. By default, it collect the first 10s of signal to estimate the whitening matrix, and then apply whitening online. The amount of data used to estimate the whitening matrix can be configured in the `Buffer size` option in the plugin editor. The buffer can be reset by pressing the `Reset buffer` button which will make the plugin estimate the whitening matrix again. The `Apply whitening` toggle control whether online whitening is enabled. The whitening matrix is estimated using singular value decomposition. 

## Building and installation
Please follow the instruction in the [Open Ephys wiki](https://open-ephys.atlassian.net/wiki/spaces/OEW/pages/1301643269/Creating+Build+files) for building this plugin. This plugin is developed in Visual Studio 2019 in Windows. It should also works in other Visual Studio verions or OS, but they are not tested. 

## Notes
- This plugin only applys whitening on data channel, so the channel type must be set properly.
- After whitening, the signal amplitude will change, so you may want to adjust the signal range of the LFP Viewer accordingly.


