#pragma once


#include <string>


#ifdef __EMSCRIPTEN__
#else
#define NOMINMAX
#include "curl/curl.h"
#endif


class Requests
{
public:
    Requests();
    Requests(const Requests&) = delete; // Delete copy constructor
    Requests& operator=(const Requests&) = delete; // Delete copy assignment operator
    ~Requests();

    std::string GetResponse(const std::string& data);



    
private:
#ifdef __EMSCRIPTEN__

#else
    static size_t WriteCallback(char* contents, size_t size, size_t nmemb, void* clientp);
    static bool initiated;
    CURL* handle;
    std::string API_KEY;
    std::string LLM_URL = "https://generativelanguage.googleapis.com/v1beta/interactions";
    static const bool stream = true;
    bool connected = false;
#endif
};