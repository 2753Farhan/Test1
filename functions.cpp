#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <curl/curl.h>
#include <sstream>
#include <tuple>
using namespace std;

size_t WriteCallback(char *ptr, size_t size, size_t nmemb, std::string *userdata)
{
    size_t len = size * nmemb;
    userdata->append(ptr, len);
    return len;
}

std::vector<int> compute_prefix(const std::string& pattern)
{
    int n = pattern.size();
    std::vector<int> prefix(n);
    prefix[0] = 0;

    int j = 0;
    for (int i = 1; i < n; i++)
    {
        while (j > 0 && pattern[j] != pattern[i])
        {
            j = prefix[j-1];
        }
        if (pattern[j] == pattern[i])
        {
            j++;
        }
        prefix[i] = j;
    }

    return prefix;
}

// Search for the pattern within the text using the KMP algorithm
size_t Find(const std::string& text, const std::string& pattern)
{
    int n = text.size();
    int m = pattern.size();

    // Compute the prefix function for the pattern
    std::vector<int> prefix = compute_prefix(pattern);

    int j = 0;
    for (int i = 0; i < n; i++)
    {
        while (j > 0 && pattern[j] != text[i])
        {
            j = prefix[j-1];
        }
        if (pattern[j] == text[i])
        {
            j++;
        }
        if (j == m)
        {
            // Found a match at position i-m+1
            return i-m+1;
        }
    }

    // Pattern not found
    return std::string::npos;
}
std::string get_attribute_value(const std::string& tag, const std::string& attr_name)
{
    // Find the position of the attribute name in the tag
    size_t attr_pos = tag.find(attr_name);
    if (attr_pos == std::string::npos)
    {
        // Attribute not found
        return "";
    }

    // Find the value of the attribute
    size_t value_pos = tag.find("=", attr_pos);
    if (value_pos == std::string::npos)
    {
        // Attribute value not found
        return "";
    }

    // Skip the "=" character and any whitespace
    value_pos = tag.find_first_not_of(" \t", value_pos + 1);
    if (value_pos == std::string::npos)
    {
        // Attribute value not found
        return "";
    }

    // Find the end of the attribute value
    size_t value_end_pos = value_pos;
    if (tag[value_pos] == '\"' || tag[value_pos] == '\'')
    {
        // Attribute value is enclosed in quotes
        char quote_char = tag[value_pos];
        value_end_pos = tag.find(quote_char, value_pos + 1);
        if (value_end_pos == std::string::npos)
        {
            // Attribute value not properly terminated
            return "";
        }
    }
    else
    {
        // Attribute value is not enclosed in quotes
        value_end_pos = tag.find_first_of(" \t", value_pos);
        if (value_end_pos == std::string::npos)
        {
            // Attribute value extends to the end of the tag
            value_end_pos = tag.size();
        }
    }

    // Extract the attribute value and return it
    return tag.substr(value_pos+1, value_end_pos - value_pos-1);
}

// Extract parameters from an HTML file
std::vector<std::tuple<std::string, std::string, std::string>> extract_parameters(const std::string html)
{
    // Open the HTML file


    // Read the contents of the file into a string

    // Find all input tags in the HTML string
    std::vector<std::tuple<std::string, std::string, std::string>> params;
    size_t pos = 0;
    while ((pos = html.find("<input", pos)) != std::string::npos)
    {
        // Extract the tag and find the value of the "name", "type", and "value" attributes
        std::string tag = html.substr(pos, html.find(">", pos) - pos + 1);
        std::string name = get_attribute_value(tag, "name");
        std::string type = get_attribute_value(tag, "type");
        std::string value = get_attribute_value(tag, "value");
        if (!name.empty())
        {
            params.emplace_back(name, type, value);
        }
        pos += tag.size();
    }

    // Return the parameter names, types, and values
    return params;
}
string sendHttpRequest(string url)
{
    string response;
    CURL *curl = curl_easy_init();
    if (!curl)
    {
        std::cerr << "Failed to initialize curl\n";
        return "";
    }
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    curl_easy_setopt(curl, CURLOPT_HEADERDATA, NULL);
    curl_easy_setopt(curl, CURLOPT_HEADER, 1);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        std::cerr << "Failed to perform curl request: " << curl_easy_strerror(res) << "\n";
        curl_easy_cleanup(curl);
        return "";
    }

    if (response.empty())
    {
        std::cerr << "Empty response data\n";
    }
    curl_easy_cleanup(curl);
    return response;


}



