#include "AT_Command.h"
#include <Arduino.h>
#include <string.h>

// Default markers for AT responses
static const char* defaultMarkers[] = {
  "\r\nOK\r\n",
  "\r\nERROR\r\n",
  //"\r\n+CME ERROR:",
  //"\r\n+CMS ERROR:",
  "> "
};
static const size_t defaultMarkerCount = sizeof(defaultMarkers) / sizeof(defaultMarkers[0]);

AT_command::AT_command(Stream& serialPort)
  : serial(serialPort) {
  clearStaticBuffer();
}

void AT_command::getATCommand(uint8_t index, char* buffer, size_t bufferSize) {
  if (index >= TOTAL_COMMANDS || buffer == nullptr || bufferSize == 0) {
    if (buffer && bufferSize > 0) {
      buffer[0] = '\0';
    }
    return;
  }
  strncpy_P(buffer, atCommands[index], bufferSize - 1);
  buffer[bufferSize - 1] = '\0';
}

void AT_command::clearStaticBuffer() {
  memset(staticBuffer, 0, sizeof(staticBuffer));
}

void AT_command::clearBuffer(char* buffer, size_t bufferSize) {
  if (buffer != nullptr && bufferSize > 0) {
    memset(buffer, 0, bufferSize);
  }
}

void AT_command::addModifierOrParams(char* sendBuffer, size_t& sendPos, size_t sendBufferSize,
                                     char* returnBuffer, size_t& returnPos, size_t returnBufferSize,
                                     const char* param, AT_command::ATModifier modifier) {
  if (sendPos >= sendBufferSize - 1 || returnPos >= returnBufferSize - 1) {
    return;
  }

  if (modifier == QUERY) {
    sendBuffer[sendPos++] = '?';
    returnBuffer[returnPos++] = '?';
  } else if (modifier == TEST) {
    if (sendPos < sendBufferSize - 2 && returnPos < returnBufferSize - 2) {
      sendBuffer[sendPos++] = '=';
      sendBuffer[sendPos++] = '?';
      returnBuffer[returnPos++] = '=';
      returnBuffer[returnPos++] = '?';
    }
  } else if (param != nullptr && param[0] != '\0') {
    if (param[0] != '>') {
      sendBuffer[sendPos++] = '=';
      returnBuffer[returnPos++] = '=';
    }
    size_t paramLen = strlen(param);
    size_t copyLen = min(paramLen, min(sendBufferSize - sendPos - 1, returnBufferSize - returnPos - 1));
    strncpy(&sendBuffer[sendPos], param, copyLen);
    strncpy(&returnBuffer[returnPos], param, copyLen);
    sendPos += copyLen;
    returnPos += copyLen;
  }

  sendBuffer[sendPos] = '\0';
  returnBuffer[returnPos] = '\0';
}

const char* AT_command::sendATCommand(uint8_t index, const char* param, AT_command::ATModifier modifier,
                                      bool addCR, unsigned long timeout, bool readResponse) {
  if (index >= TOTAL_COMMANDS) {
    clearStaticBuffer();
    strncpy(staticBuffer, "ERROR NO DATA", sizeof(staticBuffer) - 1);
    staticBuffer[sizeof(staticBuffer) - 1] = '\0';
    Serial.println("Error: Invalid command index.");
    return nullptr;
  }

  char sendBuffer[MAX_COMMAND_LENGTH];
  size_t sendPos = 0;

  clearBuffer(sendBuffer, sizeof(sendBuffer));
  clearStaticBuffer();

  getATCommand(index, sendBuffer, sizeof(sendBuffer));
  sendPos = strlen(sendBuffer);
  strncpy(staticBuffer, sendBuffer, sizeof(staticBuffer) - 1);
  staticBuffer[sizeof(staticBuffer) - 1] = '\0';
  size_t returnPos = sendPos;

  addModifierOrParams(sendBuffer, sendPos, sizeof(sendBuffer),
                      staticBuffer, returnPos, sizeof(staticBuffer),
                      param, modifier);

  if (addCR && sendPos < sizeof(sendBuffer) - 1) {
    sendBuffer[sendPos++] = '\r';
    if (returnPos < sizeof(staticBuffer) - 1) {
      staticBuffer[returnPos++] = '\r';
    }
  }

  sendBuffer[sendPos] = '\0';
  staticBuffer[returnPos] = '\0';

  serial.print(sendBuffer);

  if (readResponse) {
    return readATResponse(timeout);
  }

  return nullptr;
}

const char* AT_command::debugATCommand(uint8_t index, const char* param, AT_command::ATModifier modifier, bool addCR) {
  if (index >= TOTAL_COMMANDS) {
    clearStaticBuffer();
    strncpy(staticBuffer, "ERROR NO DATA", sizeof(staticBuffer) - 1);
    staticBuffer[sizeof(staticBuffer) - 1] = '\0';
    return staticBuffer;
  }

  char sendBuffer[MAX_COMMAND_LENGTH];
  size_t sendPos = 0;

  clearBuffer(sendBuffer, sizeof(sendBuffer));
  clearStaticBuffer();

  getATCommand(index, sendBuffer, sizeof(sendBuffer));
  sendPos = strlen(sendBuffer);
  strncpy(staticBuffer, sendBuffer, sizeof(staticBuffer) - 1);
  staticBuffer[sizeof(staticBuffer) - 1] = '\0';
  size_t returnPos = sendPos;

  addModifierOrParams(sendBuffer, sendPos, sizeof(sendBuffer),
                      staticBuffer, returnPos, sizeof(staticBuffer),
                      param, modifier);

  if (addCR && returnPos < sizeof(staticBuffer) - 2) {
    staticBuffer[returnPos++] = '\\';
    staticBuffer[returnPos++] = 'r';
  }

  sendBuffer[sendPos] = '\0';
  staticBuffer[returnPos] = '\0';

  return staticBuffer;
}

const char* AT_command::readATResponse(unsigned long timeout, const char* endMarkers[], size_t markerCount) {
  if (endMarkers == nullptr || markerCount == 0) {
    clearStaticBuffer();
    strncpy(staticBuffer, "ERROR NO DATA", sizeof(staticBuffer) - 1);
    staticBuffer[sizeof(staticBuffer) - 1] = '\0';
    return staticBuffer;
  }

  clearStaticBuffer();
  size_t pos = 0;
  unsigned long startTime = millis();
  bool endDetected = false;

  while (millis() - startTime < timeout && pos < sizeof(staticBuffer) - 1) {
    if (serial.available()) {
      char c = serial.read();
      staticBuffer[pos++] = c;

      for (size_t i = 0; i < markerCount; i++) {
        if (endMarkers[i] && strstr(staticBuffer, endMarkers[i])) {
          endDetected = true;
          break;
        }
      }

      if (endDetected) {
        Serial.print("Captured response: ");
        Serial.println(staticBuffer);
        return staticBuffer;
      }
    } else {
      delay(1);
    }
  }

  if (pos == 0) {
    clearStaticBuffer();
    strncpy(staticBuffer, "ERROR NO DATA", sizeof(staticBuffer) - 1);
    staticBuffer[sizeof(staticBuffer) - 1] = '\0';
  } else {
    // Check if the data contains invalid characters
    bool isCorrupted = false;
    for (size_t i = 0; i < pos; i++) {
      char c = staticBuffer[i];
      if (c != '\r' && c != '\n' && (c < 32 || c > 126)) {
        isCorrupted = true;
        break;
      }
    }
    clearStaticBuffer();
    if (isCorrupted) {
      strncpy(staticBuffer, "DATA CORRUPTED", sizeof(staticBuffer) - 1);
    } else {
      strncpy(staticBuffer, "ERROR NO DATA", sizeof(staticBuffer) - 1);
    }
    staticBuffer[sizeof(staticBuffer) - 1] = '\0';
  }

  return staticBuffer;
}

const char* AT_command::readATResponse(unsigned long timeout) {
  return readATResponse(timeout, defaultMarkers, defaultMarkerCount);
}
