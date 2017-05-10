# fanspeedcontrol
An application to control the fan speeds of supported devices (in the moment Nvidia GPUs with the official Nvidia driver).
This application is WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. Use this application at your own risk.
The fan speeds are configured by a custom configuration file in the JSON format
Error messages can be alert in different formats (in the moment there are a logger - which logs to the standard output, syslog and files - libnotify and sound via beep and ffplay).

The usage of the fanspeedcontrol can be displayed with the option --help (fanspeedcontrol --help).

To control the fan speed of a Nvidia GPU it is necessary to activate manual fan control for the Nvidia GPU. The instructions to do this are in the section [Nvidia control](#nvidiaControl).

The application can be extended by further devices and further ways to alert error messages. In section [extend devices](#extendDevices) and in section [extend observer](#extendObservers) it is explained how to do this.

## supported operating systems
This application is developed for Linux distributions. With little effort, it should be possible to port the application to other operating systems.

## dependencies
libraries: Boost.Interprocess, Boost.Format, Boost.Program_options, gettext, json for modern C++ of the github user nlohman, libnotify, nvctrl, spdlog, x11lib

optional applications in the path: beep, ffplay

build: C++11 compiler, cmake, pkgconfig

## build process
mkdir build && cd build && cmake .. && make && make install

## configuration file format
The configuration file must be in the JSON format and has the following structure for a single device configuration:
required attributes: type (value: "nvidia" (support must be activated in the Nvidia driver configuration)), id (value: id of the device as integer), displayName (value: display name of x server connected to the device as string)
optional attributes: hysteresis (value: hysteresis in celsius as integer, warn (value: warn temperature in celsius as integer), arbitrary number of attributes temperature in celsius as integer (value: fan speed in percent as integer) 

example single device JSON file:

    {
        "20": 0,
        "40": 25,
        "60": 40,
        "75": 60,
        "80": 80,
        "85": 99,
        "90": 100,
        "displayName": ":1",
        "hysteresis": 5,
        "id": 0,
        "type": "nvidia",
        "warn": 85
    }

The following structure is for a multi device configuration:
required attributes: deviceArray (value: array with JSON objects described for the single device configuration)
optional attributes: defaultHysteresis (value: default hysteresis in celsius as integer), defaultWarn (value: default warn temperature in celsius as integer)

example multi device JSON file:

    {
        "defaultHysteresis": 5,
        "defaultWarn": 85,
        "devices": [
            {
                "20": 0,
                "40": 25,
                "60": 40,
                "75": 60,
                "80": 80,
                "85": 99,
                "90": 100,
                "displayName": ":1",
                "hysteresis": 5,
                "id": 0,
                "type": "nvidia",
                "warn": 85
            },
            {
                "20": 0,
                "40": 25,
                "60": 40,
                "75": 60,
                "80": 80,
                "85": 99,
                "90": 100,
                "displayName": ":1",
                "hysteresis": 5,
                "id": 1,
                "type": "nvidia",
                "warn": 85
            }
        ]
    }

## <a name="nvidiaControl"></a>Nvidia control
Add in the in the Nvidia X11 configuration file (in many distributions /etc/X11/xorg.conf) in the section of your device that should be controlled `Option "Coolbits" "4"`.

## <a name="extendDevices"></a>extend device support
To add a device, create a class which inherits of fanspeedcontrol/devices/AbstractDevice and implement all virtual methods and add in the function getDeviceOptional in ArgsAndConfigProcessor.cpp necessary things to create an instance of your added class.

## <a name="extendObservers"></a>extend observer support
To add an observer, create a class which inherits of patterns/observer/AbstractObserver and add in the function processArguments in ArgsAndConfigProcessor.cpp necessary things to add your observer.

## license and CLA
fanspeedcontrol is licensed under the GPLv3. I want to leave open the possibility to license it under the BSD-3-Clause license maybe in the future. Therefore, if you want to get your pull request accepted, you have to accept that you have to license your contribution under the GPLv3 and the BSD-3-Clause license.
