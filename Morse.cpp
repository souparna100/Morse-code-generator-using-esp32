#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

WebServer server(80);

const int ledPin = 2;

// Morse timing
const int dotTime = 200;
const int dashTime = 600;
const int symbolGap = 200;
const int letterGap = 600;
const int wordGap = 1400;

// Morse table
struct Morse {
  char letter;
  const char* code;
};

Morse morseTable[] = {
  {'A', ".-"},   {'B', "-..."}, {'C', "-.-."}, {'D', "-.."},
  {'E', "."},    {'F', "..-."}, {'G', "--."},  {'H', "...."},
  {'I', ".."},   {'J', ".---"}, {'K', "-.-"},  {'L', ".-.."},
  {'M', "--"},   {'N', "-."},   {'O', "---"},  {'P', ".--."},
  {'Q', "--.-"}, {'R', ".-."},  {'S', "..."},  {'T', "-"},
  {'U', "..-"},  {'V', "...-"}, {'W', ".--"},  {'X', "-..-"},
  {'Y', "-.--"}, {'Z', "--.."},
  {'0', "-----"}, {'1', ".----"}, {'2', "..---"}, {'3', "...--"},
  {'4', "....-"}, {'5', "....."}, {'6', "-...."}, {'7', "--..."},
  {'8', "---.."}, {'9', "----."}
};

// HTML Page
String webpage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<title>ESP32 Morse Code</title>
<style>
body { font-family: Arial; text-align: center; margin-top: 50px; }
input { font-size: 20px; padding: 10px; }
button { font-size: 20px; padding: 10px 20px; }
</style>
</head>
<body>
<h2>Morse Code LED Blinker</h2>
<form action="/send">
<input name="msg" placeholder="Enter text">
<br><br>
<button type="submit">Send</button>
</form>
</body>
</html>
)rawliteral";

void blinkDot() {
  digitalWrite(ledPin, HIGH);
  delay(dotTime);
  digitalWrite(ledPin, LOW);
}

void blinkDash() {
  digitalWrite(ledPin, HIGH);
  delay(dashTime);
  digitalWrite(ledPin, LOW);
}

const char* getMorse(char c) {
  for (auto &m : morseTable) {
    if (m.letter == c) return m.code;
  }
  return "";
}

void blinkMorse(String text) {
  text.toUpperCase();

  for (int i = 0; i < text.length(); i++) {
    if (text[i] == ' ') {
      delay(wordGap);
      continue;
    }

    const char* code = getMorse(text[i]);
    for (int j = 0; code[j] != '\0'; j++) {
      if (code[j] == '.') blinkDot();
      else if (code[j] == '-') blinkDash();
      delay(symbolGap);
    }
    delay(letterGap);
  }
}

void handleRoot() {
  server.send(200, "text/html", webpage);
}

void handleSend() {
  String msg = server.arg("msg");
  server.send(200, "text/html", "<h3>Sending Morse...</h3><a href='/'>Back</a>");
  blinkMorse(msg);
}

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/send", handleSend);
  server.begin();
}

void loop() {
  server.handleClient();
}
