#include "gdrive/request.hpp"
#include "gdrive/util.hpp"
#include "gdrive/config.hpp"
#include <curl/curl.h>

namespace GDRIVE {

size_t Response::curl_write_callback(void* content, size_t size, size_t nmemb, void* userp) {
    std::string* self = (std::string*)userp;
    std::string curr_content((char*)content, size * nmemb);
    *self += curr_content;
    return size * nmemb;
}

Request::Request(std::string uri, RequestMethod method)
    :_uri(uri), _method(method) 
{
    _init_curl_handle();    
#ifdef GDIRVE_DEBUG
    CLASS_INIT_LOGGER("Request", L_DEBUG);
    CLOG_DEBUG("The method is %s\n", _method == RM_POST? "post" : "get");
#endif
}

Request::Request(std::string uri, RequestMethod method, RequestBody& body, RequestHeader& header)
    :_uri(uri), _method(method)
{
    _init_curl_handle();
    _body.insert(body.begin(), body.end());
    _header.insert(header.begin(), header.end());
#ifdef GDRIVE_DEBUG
    CLASS_INIT_LOGGER("Request", L_DEBUG);
#endif
}

Request::~Request() {
    curl_easy_cleanup(_handle);
}

void Request::_init_curl_handle() {
    curl_global_init(CURL_GLOBAL_ALL);
    _handle = curl_easy_init();
    curl_easy_setopt(_handle, CURLOPT_URL, _uri.c_str());
    curl_easy_setopt(_handle, CURLOPT_HEADERDATA, (void*)&_resp._header);
    curl_easy_setopt(_handle, CURLOPT_WRITEDATA, (void*)&_resp._content);
    curl_easy_setopt(_handle, CURLOPT_WRITEFUNCTION, Response::curl_write_callback);
}

void Request::add_header(RequestHeader& header) {
    _header.insert(header.begin(), header.end());
}

void Request::add_header(std::string key, std::string value) {
    _header[key] = value;
}

void Request::add_body(RequestBody& body) {
    _body.insert(body.begin(), body.end());
    CLOG_DEBUG("After add, body size: %d\n", _body.size());
}

void Request::add_body(std::string key, std::string value) {
    _body[key] = value;
}

curl_slist* Request::_build_header() {
    curl_slist* list = NULL;
    VarString vs;
    for(RequestHeader::iterator iter = _header.begin(); iter != _header.end(); iter ++) {
        list = curl_slist_append(list, vs.append(iter->first).append(':').append(iter->second).toString().c_str());
        vs.clear();
    }
    return list;
}

std::string Request::_urlencode_body() {
    VarString vs;
    for(RequestBody::iterator iter = _body.begin(); iter != _body.end(); iter ++) {
        vs.append(iter->first).append('=').append(URLHelper::encode(iter->second)).append('&');
    }
    if (_body.size() > 0) {
        return vs.drop().toString();
    } else {
        return "";
    }
}

std::string Request::_jsonencode_body() {
    return "";
}

void Request::request(EncodeMethod em_method) {
    std::string encoded_body;
    if (em_method == EM_URL) {
        encoded_body = _urlencode_body();
        _header["Content-Type"] = "application/x-www-form-urlencoded";
    }  else if (em_method == EM_JSON) {
        encoded_body = _jsonencode_body();
        _header["Content-Type"] = "application/json";
    } else {
        CLOG_ERROR("Unknown encode method\n");
    }

    VarString vs;
    if (_method == RM_POST) {
        curl_easy_setopt(_handle, CURLOPT_POSTFIELDS, encoded_body.c_str());
        if (_body.size() == 0) {
            _header["Content-Type"] = "";
        }
    } else if (_method == RM_GET) {
        if (_body.size() > 0) {
            vs.append(_uri).append('?').append(encoded_body);
            curl_easy_setopt(_handle, CURLOPT_URL, vs.toString().c_str());
        }
    } else if (_method == RM_DELETE) {
        curl_easy_setopt(_handle, CURLOPT_CUSTOMREQUEST, "DELETE");
        if(_body.size() > 0)  {
            curl_easy_setopt(_handle, CURLOPT_POSTFIELDS, encoded_body.c_str());
        }
    } else {
        CLOG_FATAL("Unknown  method\n");
    }
#ifdef GDRIVE_DEBUG
    curl_easy_setopt(_handle, CURLOPT_VERBOSE, 1);
#endif
    curl_easy_setopt(_handle, CURLOPT_USE_SSL, CURLUSESSL_ALL);
    curl_slist* header_list = NULL;
    if (_header.size() > 0) {
        header_list = _build_header();
        curl_easy_setopt(_handle, CURLOPT_HTTPHEADER, header_list);
    }
    CURLcode res = curl_easy_perform(_handle);
    
    if (header_list != NULL) {
        curl_slist_free_all(header_list);
    }

    if (res != CURLE_OK) {
        CLOG_ERROR("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }

    int status;
    curl_easy_getinfo(_handle, CURLINFO_RESPONSE_CODE, &status);
 
    _resp.set_status(status);
}


}
