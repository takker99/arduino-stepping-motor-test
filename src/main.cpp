#include <Arduino.h>
#include <Stepper.h>
#include <WiFiS3.h>
#include "secrets.h"

const int STEPS_PER_REV = 2048;  // 28BYJ-48 1 回転 (4 相 2 相励磁シーケンス)
const int PIN_IN1 = 7;
const int PIN_IN2 = 6;
const int PIN_IN3 = 5;
const int PIN_IN4 = 4;
// 相順問題: 第 2・3 引数 (IN2/IN3) は入れ替えて渡す (2026-08-13 確定)
Stepper myStepper(STEPS_PER_REV, PIN_IN1, PIN_IN3, PIN_IN2, PIN_IN4);

enum MotorState { IDLE, RUNNING, ERROR };
MotorState state = IDLE;
long currentPos = 0;
int motorSpeed = 5;  // 現在の速度 (rpm), /step の speed パラメータで変更可

WiFiServer server(80);

// ===== OpenAPI ドキュメント (GET /) =====
// head + 現在の IP + tail を連結して返す
const char kOpenApiHead[] = R"json({
  "openapi": "3.0.3",
  "info": {
    "title": "UNO R4 WiFi Stepper API",
    "description": "HTTP API to control a 28BYJ-48 stepper motor via ULN2003 (UNO R4 WiFi).",
    "version": "0.3.0"
  },
  "servers": [
    {
      "url": "http://)json";
const char kOpenApiTail[] = R"json("
    }
  ],
  "paths": {
    "/": {
      "get": {
        "summary": "Return this OpenAPI document",
        "responses": {
          "200": {
            "description": "OpenAPI JSON",
            "content": {
              "application/json": {}
            }
          }
        }
      }
    },
    "/status": {
      "get": {
        "summary": "Current state",
        "responses": {
          "200": {
            "description": "OK",
            "content": {
              "application/json": {
                "schema": { "$ref": "#/components/schemas/Status" }
              }
            }
          }
        }
      }
    },
    "/step": {
      "post": {
        "summary": "Run the motor (blocking until finished)",
        "parameters": [
          {
            "name": "steps",
            "in": "query",
            "description": "Number of steps (1 revolution = 2048 steps)",
            "required": true,
            "schema": { "type": "integer", "minimum": 1 }
          },
          {
            "name": "dir",
            "in": "query",
            "description": "Direction (default: cw)",
            "schema": { "type": "string", "enum": ["cw", "ccw"] }
          },
          {
            "name": "speed",
            "in": "query",
            "description": "Speed in rpm (1-60, default: current speed)",
            "schema": { "type": "integer", "minimum": 1, "maximum": 60 }
          }
        ],
        "responses": {
          "200": {
            "description": "Step finished",
            "content": {
              "application/json": {
                "schema": { "$ref": "#/components/schemas/StepResult" }
              }
            }
          },
          "400": { "description": "Bad Request (steps=0)" },
          "409": { "description": "Conflict (busy)" }
        }
      }
    },
    "/stop": {
      "post": {
        "summary": "De-energize all coils (all pins LOW)",
        "responses": {
          "200": { "description": "Coils de-energized" },
          "409": { "description": "Conflict (busy)" }
        }
      }
    }
  },
  "components": {
    "schemas": {
      "Status": {
        "type": "object",
        "properties": {
          "state": { "type": "string", "enum": ["idle", "running", "error"] },
          "position": { "type": "integer", "description": "Accumulated steps" },
          "speed": { "type": "integer", "description": "Current speed (rpm)" },
          "ssid": { "type": "string" },
          "ip": { "type": "string" },
          "rssi": { "type": "integer" }
        }
      },
      "StepResult": {
        "type": "object",
        "properties": {
          "ok": { "type": "boolean" },
          "requested": { "type": "integer" },
          "direction": { "type": "string", "enum": ["cw", "ccw"] },
          "speed": { "type": "integer" }
        }
      }
    }
  }
})json";

String openApiDoc() {
  String s = String(kOpenApiHead);
  s += WiFi.localIP().toString();
  s += kOpenApiTail;
  return s;
}

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
  s += ",\"speed\":";
  s += motorSpeed;
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
  int speed = 0;  // 0 = 指定なし (現在の speed を維持)

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
      else if (k == "speed") speed = v.toInt();
    }
    if (amp < 0) break;
    params = params.substring(amp + 1);
  }

  if (steps == 0) {
    sendResponse(c, 400, "application/json", "{\"ok\":false,\"error\":\"steps=0\"}");
    return;
  }

  if (speed > 0) {
    if (speed < 1) speed = 1;
    if (speed > 60) speed = 60;
    motorSpeed = speed;
    myStepper.setSpeed(speed);
  }

  state = RUNNING;
  myStepper.step((int)(steps * dir));
  currentPos += steps * dir;
  state = IDLE;

  String resp = "{\"ok\":true,\"requested\":";
  resp += steps;
  resp += ",\"direction\":\"";
  resp += (dir > 0 ? "cw" : "ccw");
  resp += "\",\"speed\":";
  resp += motorSpeed;
  resp += "}";
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

  if (method == "GET" && path == "/") {
    sendResponse(c, 200, "application/json", openApiDoc());
    return;
  }

  if (method == "GET" && path == "/index.html") {
    String html = "<h1>UNO R4 WiFi Stepper Server</h1>"
                  "<p>API 仕様は <code>GET /</code> で OpenAPI JSON を取得。</p>"
                  "<ul>"
                  "<li>GET / (OpenAPI JSON)</li>"
                  "<li>GET /status</li>"
                  "<li>POST /step?steps=N&amp;dir=cw|ccw&amp;speed=RPM(1-60)</li>"
                  "<li>POST /stop (全ピン LOW で通電遮断)</li>"
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
    if (state == RUNNING) {
      sendResponse(c, 409, "application/json", "{\"ok\":false,\"error\":\"busy\"}");
      return;
    }
    // step() 完了後も最後の 2 相は励磁されたまま → 全ピン LOW で通電遮断
    digitalWrite(PIN_IN1, LOW);
    digitalWrite(PIN_IN2, LOW);
    digitalWrite(PIN_IN3, LOW);
    digitalWrite(PIN_IN4, LOW);
    sendResponse(c, 200, "application/json",
                 "{\"ok\":true,\"note\":\"coils de-energized (all pins LOW)\"}");
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

  myStepper.setSpeed(5);

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
  Serial.println();
  Serial.print("status: ");
  Serial.println(WiFi.status());
  unsigned long ipWait = millis();
  while (WiFi.localIP() == IPAddress(0, 0, 0, 0) && millis() - ipWait < 10000) {
    delay(500);
  }
  Serial.print("localIP: ");
  Serial.println(WiFi.localIP());
  Serial.print("gateway: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("subnet: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());

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
