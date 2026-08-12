#include <Arduino.h>
#include <Stepper.h>
#include <WiFiS3.h>
#include "secrets.h"

const int STEPS_PER_REV = 4096;
const int PIN_IN1 = 8;
const int PIN_IN2 = 9;
const int PIN_IN3 = 10;
const int PIN_IN4 = 11;
Stepper myStepper(STEPS_PER_REV, PIN_IN1, PIN_IN2, PIN_IN3, PIN_IN4);

enum MotorState { IDLE, RUNNING, ERROR };
MotorState state = IDLE;
long currentPos = 0;

WiFiServer server(80);

String urlDecode(const String& s) {
  String out;
  out.reserve(s.length());
  for (size_t i = 0; i < s.length(); i++) {
    char c = s[i];
    if (c == '+') {
      out += ' ';
    } else if (c == '%' && i + 2 < s.length()) {
      char hex[3] = {s[i + 1], s[i + 2], 0};
      out += (char)strtoul(hex, nullptr, 16);
      i += 2;
    } else {
      out += c;
    }
  }
  return out;
}

void sendResponse(WiFiClient& c, int code, const char* ct, const String& body) {
  c.print("HTTP/1.1 ");
  c.print(code);
  c.print(' ');
  c.print(code == 200 ? "OK"
         : code == 400 ? "Bad Request"
         : code == 404 ? "Not Found"
         : code == 405 ? "Method Not Allowed"
         : code == 409 ? "Conflict"
         : "Error");
  c.print("\r\nContent-Type: ");
  c.print(ct);
  c.print("\r\nContent-Length: ");
  c.print(body.length());
  c.print("\r\nConnection: close\r\n\r\n");
  c.print(body);
}

String jsonStatus() {
  String s = "{\"state\":\"";
  s += (state == IDLE ? "idle" : state == RUNNING ? "running" : "error");
  s += "\",\"position\":";
  s += currentPos;
  s += ",\"ip\":\"";
  s += WiFi.localIP().toString();
  s += "\",\"ssid\":\"";
  s += WIFI_SSID;
  s += "\",\"rssi\":";
  s += WiFi.RSSI();
  s += "}";
  return s;
}

void handleStep(WiFiClient& c, const String& query) {
  if (state == RUNNING) {
    sendResponse(c, 409, "application/json", "{\"ok\":false,\"error\":\"busy\"}");
    return;
  }

  long steps = 0;
  int dir = 1;

  int qStart = query.indexOf('?');
  String params = (qStart >= 0) ? query.substring(qStart + 1) : "";
  while (params.length() > 0) {
    int amp = params.indexOf('&');
    String kv = (amp >= 0) ? params.substring(0, amp) : params;
    int eq = kv.indexOf('=');
    if (eq > 0) {
      String k = urlDecode(kv.substring(0, eq));
      String v = urlDecode(kv.substring(eq + 1));
      if (k == "steps") steps = v.toInt();
      else if (k == "dir") {
        if (v == "ccw" || v == "-1") dir = -1;
        else dir = 1;
      }
    }
    if (amp < 0) break;
    params = params.substring(amp + 1);
  }

  if (steps == 0) {
    sendResponse(c, 400, "application/json", "{\"ok\":false,\"error\":\"steps=0\"}");
    return;
  }

  state = RUNNING;
  myStepper.step((int)(steps * dir));
  currentPos += steps * dir;
  state = IDLE;

  String resp = "{\"ok\":true,\"requested\":";
  resp += steps;
  resp += ",\"direction\":\"";
  resp += (dir > 0 ? "cw" : "ccw");
  resp += "\"}";
  sendResponse(c, 200, "application/json", resp);
}

void handleClient(WiFiClient& c) {
  String reqLine;
  String method;
  String path;
  unsigned long timeout = millis() + 2000;

  while (c.connected() && millis() < timeout) {
    if (c.available()) {
      char ch = c.read();
      if (ch == '\n') {
        if (reqLine.length() == 0) break;
        int sp1 = reqLine.indexOf(' ');
        int sp2 = reqLine.indexOf(' ', sp1 + 1);
        if (sp1 > 0 && sp2 > sp1) {
          method = reqLine.substring(0, sp1);
          path = reqLine.substring(sp1 + 1, sp2);
        }
        reqLine = "";
      } else if (ch != '\r') {
        reqLine += ch;
      }
    }
  }

  if (method == "GET" && (path == "/" || path == "/index.html")) {
    String html = "<h1>UNO R4 WiFi Stepper Server</h1>"
                  "<ul>"
                  "<li>GET /status</li>"
                  "<li>POST /step?steps=N&amp;dir=cw|ccw</li>"
                  "<li>POST /stop</li>"
                  "</ul>";
    sendResponse(c, 200, "text/html; charset=utf-8", html);
    return;
  }

  if (method == "GET" && (path == "/status" || path.startsWith("/status?"))) {
    sendResponse(c, 200, "application/json", jsonStatus());
    return;
  }

  if (method == "POST" && (path == "/step" || path.startsWith("/step?"))) {
    handleStep(c, path);
    return;
  }

  if (method == "POST" && (path == "/stop" || path.startsWith("/stop?"))) {
    sendResponse(c, 200, "application/json",
                 "{\"ok\":true,\"note\":\"MVP: stop is no-op while step() blocks\"}");
    return;
  }

  sendResponse(c, 404, "application/json", "{\"ok\":false,\"error\":\"not found\"}");
}

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(10);
  }
  Serial.println("Booting UNO R4 WiFi Stepper Server...");

  myStepper.setSpeed(15);

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true) delay(1000);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.print("Please upgrade the firmware (current: ");
    Serial.print(fv);
    Serial.println(")");
  }

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting Wi-Fi");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("Connected. IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("Wi-Fi connect failed. Server will still start.");
  }

  server.begin();
  Serial.println("HTTP server started on port 80.");
}

void loop() {
  WiFiClient c = server.available();
  if (c) {
    handleClient(c);
    c.stop();
  }
}
