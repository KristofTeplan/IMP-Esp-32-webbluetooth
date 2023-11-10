# ESP-32 Metronome with WebBluetooth Control

## Overview

This project implements a metronome using an ESP-32 microcontroller, and it can be controlled wirelessly through a web interface using WebBluetooth. The metronome allows users to set the BPM (Beats Per Minute), volume, and beep length.

## ESP-32 Code

### Dependencies

- [ArduinoBLE Library](https://www.arduino.cc/en/Reference/ArduinoBLE)
- [Arduino Ticker Library](https://www.arduino.cc/reference/en/libraries/ticker/)

### ESP-32 Metronome Service

- **Service UUID:** `5035fbd0-7d40-11ee-b962-0242ac120002`
- **Characteristics:**
  - BPM: `8835439a-7d42-11ee-b962-0242ac120002`
  - Volume: `912554cc-7d42-11ee-b962-0242ac120002`
  - Beep Length: `95fd1818-7d42-11ee-b962-0242ac120002`

### Functionality

- The metronome produces a beep at a specified BPM, volume, and beep length.
- It uses interrupts to control the speaker and LED timing.
- The metronome settings can be adjusted via Bluetooth characteristics.

### Usage

1. Upload the provided ESP-32 code to your ESP-32 microcontroller.
2. Connect catode of speaker to GPIO 12 pin and anode to GND pin
3. Connect the ESP-32 to a power source.


## WebBluetooth Webpage

### Usage

1. Open the HTML file in a compatible web browser.
2. Click the "Show Devices" button to connect to the ESP-32.
3. Adjust the BPM, volume, and beep length using the input fields.
4. Click the "Apply" button to send the settings to the ESP-32.

### Webpage Code

The HTML file contains a simple webpage with input fields for BPM, volume, and beep length. It also includes buttons to show connected devices and apply the settings.

The JavaScript code handles the Bluetooth interaction, allowing users to connect to the ESP-32 and send the desired metronome settings.

## Important Note

Make sure your ESP-32 is flashed with the provided code, and the WebBluetooth webpage is served using a secure connection (HTTPS) or on localhost. Modern browsers require secure origins for WebBluetooth to work.

### Compatible Web Browsers for WebBluetooth:

- Google Chrome
- Microsoft Edge
- Opera

Ensure that your browser is updated to the latest version to leverage WebBluetooth functionality seamlessly. Keep in mind that browser compatibility may evolve, so checking for the latest updates and documentation is recommended.