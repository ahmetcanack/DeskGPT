#include "net_openai.hpp"

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

namespace net {

  static const char* OPENAI_URL = "https://api.openai.com/v1/chat/completions";

  bool openai_chat(const String& apiKey,
                   const String& userMessage,
                   String&       outAssistantReply)
  {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("[OpenAI] WiFi not connected.");
      return false;
    }

    if (!apiKey.length()) {
      Serial.println("[OpenAI] API key is empty! (config etmen lazım)");
      return false;
    }

    WiFiClientSecure client;
    client.setInsecure();  // sertifika ile uğraşmamak için; prod'da CA eklenebilir

    HTTPClient http;
    if (!http.begin(client, OPENAI_URL)) {
      Serial.println("[OpenAI] HTTP begin() failed");
      return false;
    }

    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", String("Bearer ") + apiKey);

    // ---- JSON payload hazırlığı ----
    StaticJsonDocument<768> doc;
    doc["model"] = "gpt-4.1-mini";  // hafif ve hızlı bir model

    JsonArray messages = doc.createNestedArray("messages");

    JsonObject sys = messages.createNestedObject();
    sys["role"]    = "system";
    sys["content"] = "You are DeskGPT running on a small ESP32 touchscreen device. "
                     "Give short, friendly answers.";

    JsonObject user = messages.createNestedObject();
    user["role"]    = "user";
    user["content"] = userMessage;

    String payload;
    serializeJson(doc, payload);

    Serial.println("[OpenAI] Request payload:");
    Serial.println(payload);

    int httpCode = http.POST(payload);
    if (httpCode <= 0) {
      Serial.print("[OpenAI] HTTP error: ");
      Serial.println(httpCode);
      http.end();
      return false;
    }

    Serial.print("[OpenAI] HTTP status: ");
    Serial.println(httpCode);

    if (httpCode != 200) {
      String err = http.getString();
      Serial.println("[OpenAI] Non-200 response:");
      Serial.println(err);
      http.end();
      return false;
    }

    String response = http.getString();
    http.end();

    Serial.println("[OpenAI] Raw response:");
    Serial.println(response);

    // ---- JSON parse (cevabı çek) ----
    StaticJsonDocument<4096> respDoc;
    DeserializationError err = deserializeJson(respDoc, response);
    if (err) {
      Serial.print("[OpenAI] JSON parse error: ");
      Serial.println(err.c_str());
      return false;
    }

    JsonVariant content = respDoc["choices"][0]["message"]["content"];
    if (content.isNull()) {
      Serial.println("[OpenAI] choices[0].message.content not found.");
      return false;
    }

    outAssistantReply = String((const char*)content);
    return true;
  }

} // namespace net