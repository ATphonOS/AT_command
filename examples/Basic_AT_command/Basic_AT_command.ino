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
  
