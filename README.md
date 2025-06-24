# Trident IoT SDK application

This is an application project generated with elcap.

## Trident IoT SDK documentation

All Trident IoT SDK documentation can be found here https://tridentiot.github.io/tridentiot-sdk

## Using elcap

Information about how to install and use elcap can be found here https://tridentiot.github.io/elcap-cli

# Industrial Temperature Z-Wave Sensor

## Overview

This project demonstrates how easy it is to create a final product based Trident IoT hardware and software components. The implementation uses:

- **Hardware**: DKNCZ20B Neptune board
- **Software**: TridentIoT SDK and elcap
- **Base Application**: Starting point multilevel sensor app from Z-Wave SDK

This industrial temperature Z-Wave sensor utilizes a MAX6675 Module with K Type Thermocouple Temperature Sensor to provide accurate temperature readings in industrial environments.

## Key Modifications

### Custom Libraries Added

- **Temperature Reading Library**: Custom implementation based on MAX6675 datasheet for temperature acquisition
- **Display Library**: Custom screen library written according to ST7565 display datasheet specifications

### Z-Wave Configuration

- **Role**: Set to `ZWAVEPLUS_INFO_REPORT_ROLE_TYPE_END_NODE_ALWAYS_ON`
- **Node Configuration**: `APPLICATION_NODEINFO_LISTENING`
- **Template Modification**: Modified `cc_multilevel_sensor_air_temperature_interface_read_value` template for real temperature reading and reporting

## Hardware Pin Configuration

### MAX6675 SPI Configuration

```
DEFAULT_SPI_CLK_PIN:  22
DEFAULT_SPI_CS_PIN:   23
DEFAULT_SPI_IO_0_PIN: 28 (NOT CONNECTED)
DEFAULT_SPI_IO_1_PIN: 29 (DATA)
```

### ST7565 Display Configuration

```
SPI1_CLK_PIN_DISPLAY:  7
SPI1_IO0_PIN_DISPLAY:  15  (DATA)
SPI1_IO1_PIN_DISPLAY:  31  (NOT CONNECTED)
SPI1_CS0_PIN_DISPLAY:  9
DISPLAY_DC_PIN:        8
DISPLAY_RST_PIN:       6
```
