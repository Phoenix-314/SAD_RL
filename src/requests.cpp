#include "requests.h"

#include <iostream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>

#else // If not emscripten, then visual studio

bool Requests::initiated = false; // Avoid multiple initializations of curl_global_init

Requests::Requests() {
    char* buf = nullptr;
    size_t sz = 0;
    if (_dupenv_s(&buf, &sz, "LLM_API_KEY") == 0 && buf != nullptr)
    {
        API_KEY = std::string(buf);
        free(buf);
    } else {
        std::cout << "Warning: LLM_API_KEY environment variable not set. Using default API key." << std::endl;
        throw std::runtime_error("LLM_API_KEY environment variable not set. Please set it to your API key.");
    }
    
    
    
    if (initiated) { 
        throw std::runtime_error("Curl has already been initialized, but another Requests instance was created. This is not allowed");
        return; 
    }
    initiated = true;
    curl_global_init(CURL_GLOBAL_DEFAULT);

    handle = curl_easy_init();
    if (!handle) {
        throw std::runtime_error("Failed to initialize CURL");
    }
    curl_easy_setopt(handle, CURLOPT_VERBOSE, 0L); // Maybe enable verbose output for debugging
    curl_easy_setopt(handle, CURLOPT_CAINFO, "../cacert.pem"); // Set the path to the CA certificate bundle
    curl_easy_setopt(handle, CURLOPT_URL, LLM_URL.c_str());
}

Requests::~Requests() {
    if (handle) {
        curl_easy_cleanup(handle);
    }
    curl_global_cleanup();
}

size_t Requests::WriteCallback(char* contents, size_t size, size_t nmemb, void* clientp) {
    auto& responseString = *static_cast<std::string*>(clientp);
    
    std::string serverResponse(contents, size * nmemb);
    // Input comes in repeated blocks of the form:
    // event: <event_type>\n
    // data: <json_data>\n
    // \n
    

    const int eventTypePrefixLength = 7; // Length of "event: "
    const int dataPrefixLength = 6; // Length of "data: "
    if (stream) {
        size_t pos = 0;
        while (pos < serverResponse.size()) {
            size_t nextPos = serverResponse.find("\n", pos);
            std::string eventType = serverResponse.substr(pos + eventTypePrefixLength, nextPos - pos - eventTypePrefixLength);
            pos = nextPos + 1;

            nextPos = serverResponse.find("\n\n", pos);
            std::string jsonData = serverResponse.substr(pos + dataPrefixLength, nextPos - pos - dataPrefixLength);
            pos = nextPos + 2; // skipping the \n\n
            
            if (eventType == "done") {
                break;
            }
            json jsonResponse = json::parse(jsonData);

            if (eventType == "interaction.created") {
                std::cout << "Starting to receive response..." << std::endl;
                continue;
            }
            if (eventType == "interaction.status_update" || eventType == "interaction.completed") {
                continue;
            }
            if (eventType == "step.start" || eventType == "step.stop") {
                continue;
            }
            if (eventType == "step.delta") {
                std::string stepType = jsonResponse["delta"]["type"];
                if (stepType == "thought") {
                    continue;
                }
                if (stepType == "text") {
                    std::string content = jsonResponse["delta"]["text"];
                    responseString.append(content);
                }
            }
        }
    } else {
        throw std::runtime_error("Not implemented");
    }

    return size * nmemb;
}

std::string Requests::GetResponse(const std::string& data)
{

    // URL Handling
    std::cout << "Sending request to: " << LLM_URL << std::endl;
    
    // Data Handling
    json payloadJson = {
        {"model", "gemini-3.5-flash-lite"},
        {"input", data},
        {"stream", stream}
    };
    std::string payload = payloadJson.dump();
    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, payload.c_str());

    // Header Handling
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("x-goog-api-key: " + API_KEY).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);

    // Response Handling
    std::string responseString;
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*) &responseString);

    CURLcode res = curl_easy_perform(handle);
    if (res != CURLE_OK) {
        std::cout << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        return "";
    }

    curl_slist_free_all(headers); // Free the header list

    return responseString;
}

#endif // End Visual Studio block for curl