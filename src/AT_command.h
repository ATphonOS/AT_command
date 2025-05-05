/**
 * @file AT_command.h
 * @brief C++ library for sending and receiving AT commands via serial interface on Arduino 
 * for GSM modules, with support for modifiers, parameters, and PROGMEM storage.
 * @version 1.0.0
 * @author ATphonOS
 * @date 2024
 * MIT license
 */

#ifndef AT_COMMAND_H
#define AT_COMMAND_H

#include <Arduino.h>

/** @brief Total number of AT commands supported */
const uint8_t TOTAL_COMMANDS = 15;

/** @brief Maximum length of an AT command string */
const uint8_t MAX_COMMAND_LENGTH = 255;  // Max value 2^8-1 [0-255]

/**
 * @brief AT commands stored directly in PROGMEM as a 2D array.
 * @note Stored in flash memory to save RAM.
 */
const char atCommands[TOTAL_COMMANDS][MAX_COMMAND_LENGTH] PROGMEM = {
  "AT",       /**< 0. Check module GSM reply */
  "ATE1",     /**< 1. Enable echo */
  "AT+CLIP",  /**< 2. Calling line identification presentation */
  "AT+CSDVC", /**< 3. Speaker output */
  "AT+CBC",   /**< 4. Battery charge status */
  "AT+CSQ",   /**< 5. Signal quality */
  "AT+CGMI",  /**< 6. Manufacturer identification */
  "AT+CGSN",  /**< 7. Device serial number */
  "ATI",      /**< 8. Product identification info */
  "AT+IPR",   /**< 9. Query baud rate (requires ? for query) */
  "AT+CPOWD", /**< 10. Power off */
  "ATD",      /**< 11. Dial a number (requires phone number or memory reference) */
  "AT+COPS",  /**< 12. Operator selection (requires =? for query) */
  "AT+CMGS",  /**< 13. Send SMS (requires phone number and message) */
  "A/"        /**< 14. Re-issue last command */
};

/**
 * @brief Class for handling AT commands over a serial interface.
 */
class AT_command {
  char staticBuffer[512]; /**< Static buffer for storing responses */
  Stream& serial;         /**< Reference to the serial port for communication */

public:
  /**
     * @brief Enum for AT command modifiers.
     */
  enum ATModifier {
    NONE,  /**< No modifier, adds SET ('=') automatically if param is provided */
    QUERY, /**< Query modifier ('?') */
    TEST   /**< Test modifier ('=?') */
  };

  /**
     * @brief Enum for AT response status.
     */
  /*
    enum ATResponseStatus {
        AT_RESPONSE_SUCCESS,        // Marker found
        AT_RESPONSE_TIMEOUT,        // Time out
        AT_RESPONSE_BUFFER_FULL,    // Buffer full
        AT_RESPONSE_INVALID_PARAM   // Invalid parameters
    };*/

  /**
     * @brief Constructor for AT_command class.
     * @param serialPort Serial port to use for communication.
     */
  AT_command(Stream& serialPort);

  /**
     * @brief Retrieves an AT command from PROGMEM by index.
     * @param index Index of the command in the atCommands array.
     * @param buffer Buffer to store the retrieved command.
     * @param bufferSize Size of the buffer.
     */
  void getATCommand(uint8_t index, char* buffer, size_t bufferSize);

  /**
     * @brief Clears the static buffer used for responses.
     */
  void clearStaticBuffer();

  /**
     * @brief Clears a provided buffer.
     * @param buffer Buffer to clear.
     * @param bufferSize Size of the buffer.
     */
  void clearBuffer(char* buffer, size_t bufferSize);

  /**
     * @brief Adds modifier or parameters to the AT command buffer.
     * @param sendBuffer Buffer to store the constructed command.
     * @param sendPos Current position in the send buffer.
     * @param sendBufferSize Size of the send buffer.
     * @param returnBuffer Buffer to store the command for debugging.
     * @param returnPos Current position in the return buffer.
     * @param returnBufferSize Size of the return buffer.
     * @param param Optional parameter to append to the command.
     * @param modifier Modifier to apply (NONE, QUERY, or TEST).
     */
  void addModifierOrParams(char* sendBuffer, size_t& sendPos, size_t sendBufferSize,
                           char* returnBuffer, size_t& returnPos, size_t returnBufferSize,
                           const char* param, AT_command::ATModifier modifier);

  /**
     * @brief Sends an AT command with optional parameters and modifier.
     * @param index Index of the command in the atCommands array.
     * @param param Optional parameter to append to the command.
     * @param modifier Modifier to apply (NONE, QUERY, or TEST).
     * @param addCR Whether to append a carriage return (CR) to the command.
     * @param timeout Timeout for reading the response in milliseconds.
     * @param readResponse Whether to read and return the response.
     * @return Pointer to the response in staticBuffer, or nullptr if no response is read or an error occurs.
     */
  const char* sendATCommand(uint8_t index, const char* param, AT_command::ATModifier modifier,
                            bool addCR, unsigned long timeout, bool readResponse);

  /**
     * @brief Generates a debug string for an AT command.
     * @param index Index of the command in the atCommands array.
     * @param param Optional parameter to append to the command.
     * @param modifier Modifier to apply (NONE, QUERY, or TEST).
     * @param addCR Whether to append a carriage return (CR) to the command.
     * @return Pointer to the debug string in staticBuffer.
     */
  const char* debugATCommand(uint8_t index, const char* param, AT_command::ATModifier modifier,
                             bool addCR);

  /**
     * @brief Reads the response to an AT command from the serial port with default markers.
     * @param timeout Timeout for reading the response in milliseconds (default: 1000).
     * @return Pointer to the response in staticBuffer, or nullptr if an error occurs.
     */
  const char* readATResponse(unsigned long timeout = 1000);

  /**
     * @brief Reads the response to an AT command from the serial port with custom markers.
     * @param timeout Timeout for reading the response in milliseconds.
     * @param endMarkers Array of end markers to detect.
     * @param markerCount Number of end markers.
     * @return Pointer to the response in staticBuffer, or nullptr if an error occurs.
     */
  const char* readATResponse(unsigned long timeout, const char* endMarkers[], size_t markerCount);
};

#endif  // AT_COMMAND_H
