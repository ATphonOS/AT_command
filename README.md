This documentation is also available in **[Spanish](./README_es.md)**.

# 📶 AT_command
## Overview
`AT_command` is a C++ library designed for sending and receiving AT commands via a serial interface on Arduino-compatible microcontrollers for GSM modules. It provides functions to construct, send, and read AT command responses, with support for modifiers (such as query and test), parameters, and PROGMEM storage to optimize memory usage.

Designed as a library for use in Arduino environments.

---

## ⚙️ Features

- Construction and sending of AT commands with support for modifiers (`?` for query, `=?` for test).
- Reading AT responses with detection of custom or predefined markers (e.g., `OK`, `ERROR`).
- Storage of AT commands in PROGMEM to reduce RAM usage.
- Management of dynamic parameters for commands like `ATD` (dial number) or `AT+CMGS` (send SMS).
- Debugging of AT commands by generating text strings for inspection.
- Support for standard (`HardwareSerial`) and software (`SoftwareSerial`) serial interfaces.
- Robust error handling, including validation of command indices and detection of corrupted data.
- Flexible configuration of response timeouts.

> The library simplifies interaction with GSM modules by encapsulating the logic for sending and receiving AT commands, with a design optimized for resource-constrained environments and support for real-time debugging.

### 📌 Compatible Microcontrollers

Compatible with 8-bit AVR, 32-bit ARM Cortex-M, and Xtensa architectures, provided they are supported by the Arduino IDE and have a serial interface (hardware or software).

---

## 📦 Installation

Option 1: Copy and include the files:

1. Copy the following files into your Arduino project:
   - `AT_command.h`
   - `AT_command.cpp`

2. Include the header in your source file:
   ```cpp
   #include "AT_command.h"
   ```

Option 2: Install and include the library:

1. Download the repository as a ZIP file.
2. Open the Arduino IDE.
3. Go to **Sketch > Include Library > Add .ZIP Library** in the menu.
4. Select the library's ZIP file.
5. Import the library:

To use the library in your sketch, go to **Sketch > Include Library** and select `AT_command` from the list to add its `#include` directive to your code.
```cpp
#include <AT_command.h>
```

---

## 📚 Library: `AT_command`

### Key Definitions and Structures

```cpp
const uint8_t TOTAL_COMMANDS = 15;
const uint8_t MAX_COMMAND_LENGTH = 255;
const char atCommands[TOTAL_COMMANDS][MAX_COMMAND_LENGTH] PROGMEM = { ... };
enum ATModifier { NONE, QUERY, TEST };
```

| Definition | Description |
|------------|-------------|
| `TOTAL_COMMANDS` | Total number of supported AT commands (15). |
| `MAX_COMMAND_LENGTH` | Maximum length of an AT command string (255 characters). |
| `atCommands` | 2D array in PROGMEM storing predefined AT commands (e.g., `AT`, `AT+CSQ`, `ATD`). |
| `ATModifier` | Enumerates modifiers for AT commands: `NONE` (no modifier or with parameter), `QUERY` (`?`), `TEST` (`=?`). |

---

## 🔓 Public Functions

### Initialization

```cpp
AT_command(Stream& serialPort);
```

| Function | Description | Parameters | Return |
|----------|-------------|------------|--------|
| `AT_command(Stream& serialPort)` | Constructs an `AT_command` object with the specified serial interface. | `serialPort (Stream&)`: Serial port (e.g., `Serial`, `SoftwareSerial`). | None |

### Buffer Management

```cpp
void getATCommand(uint8_t index, char* buffer, size_t bufferSize);
void clearStaticBuffer();
void clearBuffer(char* buffer, size_t bufferSize);
```

| Function | Description | Parameters | Return |
|----------|-------------|------------|--------|
| `void getATCommand(uint8_t index, char* buffer, size_t bufferSize)` | Retrieves an AT command from PROGMEM by its index. | `index (uint8_t)`: Command index<br>`buffer (char*)`: Destination buffer<br>`bufferSize (size_t)`: Buffer size | `void` |
| `void clearStaticBuffer()` | Clears the internal static buffer used for responses. | None | `void` |
| `void clearBuffer(char* buffer, size_t bufferSize)` | Clears a provided buffer. | `buffer (char*)`: Buffer to clear<br>`bufferSize (size_t)`: Buffer size | `void` |

### Command Construction and Sending

```cpp
void addModifierOrParams(char* sendBuffer, size_t& sendPos, size_t sendBufferSize,
                         char* returnBuffer, size_t& returnPos, size_t returnBufferSize,
                         const char* param, AT_command::ATModifier modifier);
const char* sendATCommand(uint8_t index, const char* param, ATModifier modifier,
                          bool addCR, unsigned long timeout, bool readResponse);
const char* debugATCommand(uint8_t index, const char* param, ATModifier modifier, bool addCR);
```

| Function | Description | Parameters | Return |
|----------|-------------|------------|--------|
| `void addModifierOrParams(...)` | Adds modifiers (`?`, `=?`) or parameters to an AT command. | `sendBuffer (char*)`: Command buffer<br>`sendPos (size_t&)`: Position in send buffer<br>`sendBufferSize (size_t)`: Send buffer size<br>`returnBuffer (char*)`: Return buffer<br>`returnPos (size_t&)`: Position in return buffer<br>`returnBufferSize (size_t)`: Return buffer size<br>`param (const char*)`: Optional parameter<br>`modifier (ATModifier)`: Modifier (`NONE`, `QUERY`, `TEST`) | `void` |
| `const char* sendATCommand(...)` | Sends an AT command with parameters and modifiers, optionally reads the response. | `index (uint8_t)`: Command index<br>`param (const char*)`: Optional parameter<br>`modifier (ATModifier)`: Modifier<br>`addCR (bool)`: Add carriage return<br>`timeout (unsigned long)`: Timeout (ms)<br>`readResponse (bool)`: Read response | `const char*`: Response in static buffer or `nullptr` |
| `const char* debugATCommand(...)` | Generates a debug string for an AT command without sending it. | `index (uint8_t)`: Command index<br>`param (const char*)`: Optional parameter<br>`modifier (ATModifier)`: Modifier<br>`addCR (bool)`: Add carriage return | `const char*`: String in static buffer |

### Response Reading

```cpp
const char* readATResponse(unsigned long timeout = 1000);
const char* readATResponse(unsigned long timeout, const char* endMarkers[], size_t markerCount);
```

| Function | Description | Parameters | Return |
|----------|-------------|------------|--------|
| `const char* readATResponse(unsigned long timeout)` | Reads the AT response using predefined markers (`OK`, `ERROR`, etc.). | `timeout (unsigned long)`: Timeout (ms, default: 1000) | `const char*`: Response in static buffer |
| `const char* readATResponse(unsigned long timeout, const char* endMarkers[], size_t markerCount)` | Reads the AT response using custom markers. | `timeout (unsigned long)`: Timeout (ms)<br>`endMarkers (const char*[])`: Array of markers<br>`markerCount (size_t)`: Number of markers | `const char*`: Response in static buffer |

---

## 🧪 Example Usage

The following example demonstrates how to use the `AT_command` library to interact with a GSM module via a `SoftwareSerial` interface. The code sends various AT commands, reads responses, and uses the debug function.

```cpp
#include <SoftwareSerial.h>
#include "AT_command.h"

// Configure SoftwareSerial for the GSM module
SoftwareSerial gsmSerial(7, 8); // RX, TX
AT_command at(gsmSerial);

void setup() {
  Serial.begin(9600);
  gsmSerial.begin(9600);
  delay(1000); // Wait for the module to initialize

  Serial.println("=== AT Commands Test ===");

  // Example 1: Send AT command without reading response
  Serial.println("Example 1: Sending AT command without capturing response...");
  at.sendATCommand(0, nullptr, AT_command::NONE, true, 2000, false);
  Serial.println("AT command sent.");

  // Example 2: Send and get response (AT+CSDVC)
  Serial.println("\nExample 2: Sending AT+CSDVC...");
  const char* response = at.sendATCommand(3, nullptr, AT_command::NONE, true, 3000, true);
  Serial.print("Result: ");
  Serial.println(response);
  at.clearStaticBuffer();

  // Example 3: Send and get response (AT+CSQ)
  Serial.println("\nExample 3: Sending AT+CSQ...");
  response = at.sendATCommand(5, nullptr, AT_command::NONE, true, 3000, true);
  Serial.print("Result: ");
  Serial.println(response);
  at.clearStaticBuffer();

  // Example 4: Command debugging
  Serial.println("\nExample 4: Debugging commands...");
  Serial.print("TEST (AT+COPS=?): ");
  Serial.println(at.debugATCommand(12, nullptr, AT_command::TEST, true));
  Serial.print("PARAM (ATD=123456789): ");
  Serial.println(at.debugATCommand(11, "123456789", AT_command::NONE, true));
  Serial.print("QUERY (AT+COPS?): ");
  Serial.println(at.debugATCommand(12, nullptr, AT_command::QUERY, true));

  // Example 5: Manually read response
  Serial.println("\nExample 5: Sending AT and manually reading response...");
  at.sendATCommand(0, nullptr, AT_command::NONE, true, 2000, false);
  response = at.readATResponse(2000);
  Serial.print("Result: ");
  Serial.println(response);
  at.clearStaticBuffer();
}

void loop() {
  // Empty loop
}
```

---

## 🧠 Design Notes

- **Memory Optimization**: AT commands are stored in PROGMEM to minimize RAM usage.
- **Flexible Serial Interface**: The library uses the `Stream` base class to support both `HardwareSerial` and `SoftwareSerial`, providing compatibility with various hardware configurations and allowing users to choose the appropriate serial interface.
- **Robust Error Handling**: Includes comprehensive validations for invalid command indices, buffer sizes, and corrupted response data (characters outside the printable ASCII range).
- **Dynamic Modifiers**: Support for modifiers (`QUERY`, `TEST`) and dynamic parameters enables the construction of complex AT commands (e.g., `AT+COPS=?` or `ATD=123456789`) flexibly, without requiring users to manually format strings.
- **Simplified Debugging**: The `debugATCommand` function generates textual representations of AT commands before sending, facilitating inspection and troubleshooting during development, especially useful for commands with parameters or modifiers.
- **Customizable Markers**: The ability to define custom response end markers (in addition to defaults like `OK` and `ERROR`) allows handling non-standard responses from GSM modules.
- **Efficient Static Buffer**: Uses a 512-byte static buffer to store responses, avoiding dynamic memory allocation. The buffer must be cleared (`clearStaticBuffer`) between operations.
- **Configurable Timeouts**: Adjustable timeouts in `sendATCommand` and `readATResponse` allow adaptation to GSM modules with varying response speeds.

---

## 🧾 License
This project is licensed under the MIT License — see the [LICENSE](https://github.com/ATphonOS/AT_command/blob/main/LICENSE.md) file for details.
