// curl_fetch.cpp - transferring data with libcurl's easy interface. It reads
// a file:// URL so the example needs no network; an http:// or https:// URL
// goes through the same calls.
#include <curl/curl.h>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>

static std::size_t append(char* data, std::size_t size, std::size_t count, void* out)
{
    static_cast<std::string*>(out)->append(data, size * count);
    return size * count;                         // anything else aborts the transfer
}

static CURLcode fetch(const std::string& url, std::string& body)
{
    CURL* curl = curl_easy_init();
    if (curl == nullptr)
        return CURLE_FAILED_INIT;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, append);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
    CURLcode rc = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return rc;
}

int main()
{
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK)
        return 1;

    const char* path = "curl_fetch_input.txt";
    std::ofstream(path) << "line one\nline two\n";

    std::string body;
    // A file:// URL needs an absolute path: file:///home/... on POSIX systems.
    const std::string url = "file://" + std::filesystem::absolute(path).generic_string();
    CURLcode rc = fetch(url, body);
    std::printf("fetch existing file: %s, %zu bytes\n", curl_easy_strerror(rc), body.size());

    body.clear();
    rc = fetch("file:///no/such/file.txt", body);
    std::printf("fetch missing file: %s\n", curl_easy_strerror(rc));

    std::remove(path);
    curl_global_cleanup();
    return 0;
}
