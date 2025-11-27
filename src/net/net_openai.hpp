#pragma once
#include <Arduino.h>

namespace net {

  // userMessage -> OpenAI Chat API -> assistantReply
  // true = başarı, false = HTTP / parse hatası
  bool openai_chat(const String& apiKey,
                   const String& userMessage,
                   String&       outAssistantReply);

} // namespace net