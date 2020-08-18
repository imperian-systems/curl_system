#include <curl_system.hpp>
#include <libthe-seed/ComponentLoader.hpp>
#include "../node_modules/@imperian-systems/http_request_component/src/http_request_component.hpp"
#include "../node_modules/@imperian-systems/http_response_component/src/http_response_component.hpp"
#include <fstream>

size_t write_data(void *ptr, size_t size, size_t nmemb, std::shared_ptr<ecs::Component> component)
{
    auto bytes = size * nmemb;
    auto response = std::dynamic_pointer_cast<http_response_component>(component);
    response->length += bytes;
    response->body.append((char *)ptr, size * nmemb);

    return size * nmemb;
}

size_t write_header(void *ptr, size_t size, size_t nmemb, std::shared_ptr<ecs::Component> component)
{
    auto response = std::dynamic_pointer_cast<http_response_component>(component);
    response->header.append((char *)ptr, size * nmemb);

    return size * nmemb;
}

size_t write_data_file(void *ptr, size_t size, size_t nmemb, std::shared_ptr<ecs::Component> component)
{
    auto bytes = size * nmemb;
    auto response = std::dynamic_pointer_cast<http_response_component>(component);

    if(!response->output_file.is_open())
    {
        response->output_file.open(response->output_filename, std::ios::out | std::ios::binary);
    }

    response->length += bytes;
    response->output_file.write((char *)ptr, bytes);

    return size * nmemb;
}

curl_system::curl_system() 
{ 
    this->Handle = "curl_system";
}

curl_system::curl_system(Json::Value config)
{
    this->Handle = "curl_system";
}

Json::Value curl_system::Export()
{
    Json::Value config;
    return config;
}

void curl_system::Init()
{
    this->ComponentRequest("http_request_component");
    this->ComponentRequest("http_response_component");

    curl_global_init(CURL_GLOBAL_ALL);
}

void curl_system::Update()
{
    auto dt = this->DeltaTimeGet();

    this->curl = curl_easy_init();
    if(!this->curl)
    {
        throw std::runtime_error("Clairvoyance Service couldn't initialize libcurl.");
    }

    auto Components = this->ComponentsGet();
    for(auto &[entity, component] : Components["http_request_component"])
    {
        auto request = std::dynamic_pointer_cast<http_request_component>(component);
        auto response_check = Components["http_response_component"][entity];
        if(response_check != nullptr) continue;

        auto e = this->Container->Entity(entity);
        auto response_component = e->Component(ComponentLoader::Create("@imperian-systems/http_response_component")); 
        auto response = std::dynamic_pointer_cast<http_response_component>(response_component);

        struct curl_slist *chunk = NULL;
        if(request->header.size() > 0)
        {
            for(auto &k : request->header.getMemberNames())
            {
                std::string header = k +": " + request->header[k].asString();
                chunk = curl_slist_append(chunk, header.c_str());
            }
            curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, chunk);
        }

        std::string post_fields;
        if(request->method == "POST")
        {
            for(auto &k : request->post_data.getMemberNames())
            {
                post_fields += k + "=" + request->post_data[k].asString()  + "&";
            }
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields.c_str());
        }

        curl_easy_setopt(this->curl, CURLOPT_URL, request->url.c_str());

        if(request->output_filename.size())
        {
            /* Write http response to output_filename */
            response->output_filename = request->output_filename;
            curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, write_data_file);
        }
        else
        {
            /* Store http response in component */
            curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, write_data);
        }
        curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, response_component);

        curl_easy_setopt(this->curl, CURLOPT_HEADERFUNCTION, write_header);
        curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, response_component);

        /* This is a bug, fix it */
        curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYPEER, 0); 


        CURLcode result = curl_easy_perform(this->curl);

        if(chunk != NULL) curl_slist_free_all(chunk);
        curl_easy_reset(this->curl);
    }

    curl_easy_cleanup(this->curl);
}

extern "C"
{
    ecs::System *create_system(void *p)
    {
        if(p == nullptr) return new curl_system();

        Json::Value *config = (Json::Value *)p;
        return new curl_system(*config);
    }
}
