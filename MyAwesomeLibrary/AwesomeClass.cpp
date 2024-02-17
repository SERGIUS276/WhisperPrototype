#define CURL_STATICLIB
#if defined _WIN64
#pragma once
    #include <iostream>
    #include <fstream>
    #include <string>
    #include <filesystem>
    #define AWESOMECLASS_EXPORT __declspec(dllexport)
#endif
#ifndef AWESOMECLASS_EXPORT
    #define AWESOMECLASS_EXPORT
#endif
#include "AwesomeClass.h"
#include "params.h"
#include "../Whisper/API/iContext.cl.h"
#include "../Whisper/API/iMediaFoundation.cl.h"
#include "../ComLightLib/comLightClient.h"
#include "textWriter.h"

#include <curl/curl.h>
#include <iostream>

using namespace Whisper;

std::string escape_json(const std::string& input) {
    std::string output;
    for (char c : input) {
        switch (c) {
        case '"':
            output += "\\\"";
            break;
        case '\\':
            output += "\\\\";
            break;
        case '\b':
            output += "";
            break;
        case '\f':
            output += "";
            break;
        case '\n':
            output += "";
            break;
        case '\r':
            output += "";
            break;
        case '\t':
            output += "";
            break;
        default:
            output += c;
            break;
        }
    }
    return output;
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append(static_cast<char*>(contents), total_size);
    return total_size;
}

static void __stdcall pfnListAdapter(const wchar_t* name, void*)
{
    wprintf(L"\"%s\"\n", name);
}

static void listGpus()
{
    printf("    Available graphic adapters:\n");
    HRESULT hr = Whisper::listGPUs(&pfnListAdapter, nullptr);
    if (SUCCEEDED(hr))
        return;
    std::cout << "Unable to enumerate GPUs";
}

CURLcode performHttpPost(const char* url, const char* contentType, const char* postData) {
    CURL* curl;
    CURLcode res;

    // Initialize curl
    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Error initializing curl\n");
        return CURLE_FAILED_INIT;
    }

    // Set the target URL
    curl_easy_setopt(curl, CURLOPT_URL, url);

    // Set the HTTP method to POST
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    // Set request headers
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "accept: application/json");
    headers = curl_slist_append(headers, contentType);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Set request payload
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData);

    // Perform the HTTP request
    res = curl_easy_perform(curl);

    // Clean up
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    return res;
}


// Function to convert audio file to text file
AWESOMECLASS_EXPORT std::string PrettyPrint()
{
    // Load the Whisper model
    Whisper::sModelSetup setup;

    Whisper::sLoggerSetup logSetup;
    logSetup.flags = eLoggerFlags::UseStandardError;
    logSetup.level = eLogLevel::Debug;
    Whisper::setupLogger(logSetup);
    listGpus();
    //setup.impl = Whisper::eModelImplementation::GPU;
    std::string narrowString1 = "NVIDIA GeForce RTX 2060";

    // Convert narrow string to wide string
    std::wstring wideString1(narrowString1.begin(), narrowString1.end());

    // Get pointer to wide-character string
    LPCTSTR gpu = wideString1.c_str();
    /*    setup.adapter = gpu;*/

    ComLight::CComPtr<Whisper::iModel> model;
    std::string narrowString2 = "C:\\Users\\shymk\\Documents\\WhisperDesktop\\ggml-small.bin";

    // Convert narrow string to wide string
    std::wstring wideString2(narrowString2.begin(), narrowString2.end());

    // Get pointer to wide-character string
    LPCTSTR modelPath = wideString2.c_str();
    HRESULT hr = Whisper::loadModel(modelPath, setup, nullptr, &model);
    //if (FAILED(hr)) {
    //    // Handle error
    //    return hr;
    //}
    std::cout << "Model loaded\n";
    // Create context
    ComLight::CComPtr<Whisper::iContext> context;
    hr = model->createContext(&context);
    //if (FAILED(hr)) {
    //    // Handle error
    //    return hr;
    //}

    // Initialize Media Foundation
    ComLight::CComPtr<Whisper::iMediaFoundation> mf;
    hr = initMediaFoundation(&mf);
    //if (FAILED(hr)) {
    //    // Handle error
    //    return hr;
    //}

    // Load and process the audio file
    ComLight::CComPtr<Whisper::iAudioBuffer> buffer;
    std::string narrowString = "C:\\Users\\shymk\\Documents\\Unreal Projects\\Whisper_Prototype1 - Copy\\Saved\\BouncedWavFiles\\Test.wav";

    // Convert narrow string to wide string
    std::wstring wideString(narrowString.begin(), narrowString.end());

    // Get pointer to wide-character string
    LPCTSTR path = wideString.c_str();

    hr = mf->loadAudioFile(path, false, &buffer);
    //if (FAILED(hr)) {
    //    // Handle error
    //    return hr;
    //}

    // Define parameters for processing
    Whisper::sFullParams wparams;
    context->fullDefaultParams(Whisper::eSamplingStrategy::Greedy, &wparams);
    // Set necessary flags and parameters for text generation


    // Run full processing
    hr = context->runFull(wparams, buffer);
    //if (FAILED(hr)) {
    //    // Handle error
    //    return hr;
    //}

    // Write the text output to a file
    bool timestamps = false; // Adjust as needed
    hr = writeText(context, L"output.txt", timestamps);
    //if (FAILED(hr)) {
    //    // Handle error
    //    return hr;
    //}

    // Release resources
    context = nullptr;
    std::string filename = "output.txt"; // Replace with your file name
    std::ifstream file(filename);
    std::cout << "STT finished\n";
    if (file.is_open()) {
        std::string prompt((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
        //std::filesystem::path currentDir = std::filesystem::current_path();
        //std::filesystem::current_path("C:\\Users\\shymk\\Documents\\vsc prog\\llama_langchain_locally");
        //
        //std::string command = R"(python "C:\Users\shymk\Documents\vsc prog\llama_langchain_locally\llama_cpu2.py")";
        //command += " \"" + prompt + "\"";
        //// Execute the Python script without capturing the output
        //std::system(command.c_str());

        //std::filesystem::current_path(currentDir);

        std::string escaped_prompt = escape_json(prompt);
        std::cout << "Prompts:" << escaped_prompt;
        curl_global_init(CURL_GLOBAL_ALL);

        CURL* curl = curl_easy_init();
        if (!curl) {
            std::cerr << "Error initializing libcurl." << std::endl;
            return "1";
        }

        std::string url = "http://localhost:8080"; // Update with your server's URL
        // Create JSON data for the POST request
        /*std::string prompt = "Do you have any friends?";*/
        std::string post_data = "{\"prompt\":\"" + escaped_prompt + "\"}";

        // Set the URL to the server
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set the request type to POST
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        // Set the "Content-Type" header
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the POST data
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());

        // Create a string to store the response
        std::string response;

        // Set the callback function to capture the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Perform the HTTP request
        CURLcode res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        else {
            // Print the received response
            std::cout << "Response:\n" << response << std::endl;
        }

        // Clean up
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }

    
    const char* url1 = "http://localhost:8011/A2F/USD/Load";
    const char* contentType1 = "Content-Type: application/json";
    const char* postData1 = R"({
        "file_name": "C:/Users/shymk/Documents/test.usd"
    })";

    /*CURLcode res1 = performHttpPost(url1, contentType1, postData1);
    std::cout << "res1" << " " << res1;
    if (res1 != CURLE_OK) {
        std::cerr << "First request failed: " << curl_easy_strerror(res1) << std::endl;
    }

    // Second curl request
    const char* url2 = "http://localhost:8011/A2F/Exporter/ActivateStreamLivelink";
    const char* contentType2 = "Content-Type: application/json";
    const char* postData2 = R"({
        "node_path": "/World/audio2face/StreamLivelink",
        "value": true
    })";

    CURLcode res2 = performHttpPost(url2, contentType2, postData2);
    std::cout << "res2" << " " << res2;
    if (res2 != CURLE_OK) {
        std::cerr << "Second request failed: " << curl_easy_strerror(res2) << std::endl;
    }

    // Third curl request
    const char* url3 = "http://localhost:8011/A2F/Player/SetTrack";
    const char* contentType3 = "Content-Type: application/json";
    const char* postData3 = R"({
        "a2f_player": "/World/audio2face/Player",
        "file_name": "test.wav",
        "time_range": [
            0,
            -1
        ]
    })";

    CURLcode res3 = performHttpPost(url3, contentType3, postData3);
    std::cout << "res3" << " " << res3;
    if (res3 != CURLE_OK) {
        std::cerr << "Third request failed: " << curl_easy_strerror(res3) << std::endl;
    }*/

    // Fourth curl request
    const char* url4 = "http://localhost:8011/A2F/Player/Play";
    const char* contentType4 = "Content-Type: application/json";
    const char* postData4 = R"({
        "a2f_player": "/World/audio2face/Player"
    })";

    CURLcode res4 = performHttpPost(url4, contentType4, postData4);
    std::cout << "res4" << " " << res4;
    if (res4 != CURLE_OK) {
        std::cerr << "Fourth request failed: " << curl_easy_strerror(res4) << std::endl;
    }




    return "OK";
}
