#pragma once

#include <winsock2.h>
#include <libecs-cpp/ecs.hpp>
#include <curl/curl.h>

class curl_system : public ecs::System
{
  public:
    curl_system(); 
    curl_system(Json::Value);
    Json::Value Export();
    void Update();
    void Init();
  private:
    CURL *curl = nullptr;
};
