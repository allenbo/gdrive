#include "gdrive/oauth.hpp"
#include "gdrive/request.hpp"
#include "jconer/json.hpp"
#include <time.h>

using namespace JCONER;

namespace GDRIVE {
OAuth::OAuth(std::string client_id, std::string client_secret)
    :_client_id(client_id), _client_secret(client_secret)
{
#ifdef GDRIVE_DEBUG
    CLASS_INIT_LOGGER("OAuth", L_DEBUG)
#endif
}

std::string OAuth::get_authorize_url() {
    VarString vs;
    vs.append(OAUTH_URL, '?')
      .append("scope", '=', URLHelper::encode(SCOPE)).append('&')
      .append("redirect_uri", '=', URLHelper::encode(REDIRECT_URI)).append('&')
      .append("response_type", '=', URLHelper::encode(RESPONSE_TYPE)).append('&')
      .append("client_id", '=', URLHelper::encode(_client_id.c_str())).append('&')
      .append("access_type", '=', URLHelper::encode(ACCESS_TYPE));

    return vs.toString();
}

void OAuth::_parse_response(std::string content) {
    PError perr;
    JObject* rst = (JObject*)loads(content, perr);
    if (rst != NULL){
        if (rst->contain("access_token")) {
            _resp.access_token = ((JString*)rst->get("access_token"))->getValue();
        }
        if (rst->contain("id_token")) {
            _resp.id_token = ((JString*)rst->get("id_token"))->getValue();
        }
        if (rst->contain("refresh_token")) {
            _resp.refresh_token = ((JString*)rst->get("refresh_token"))->getValue();
        }
        if (rst->contain("token_type")) {
            _resp.token_type = ((JString*)rst->get("token_type"))->getValue();
        }
        if (rst->contain("expires_in")) {
            _resp.expires_in = ((JInt*)rst->get("expires_in"))->getValue();
            _resp.token_expiry = (long)time(NULL) + _resp.expires_in;
        }
        delete rst;
    }
}

bool OAuth::build_credential(std::string code, Credential& cred) {
    _code = code;
    std::map<std::string, std::string>  body;
    body["grant_type"] = "authorization_code";
    body["client_id"] = _client_id;
    body["client_secret"] = _client_secret;
    body["code"] = _code;
    body["redirect_uri"] = REDIRECT_URI;
    body["scope"] = SCOPE;

    RequestHeader header;
    header["content-type"] = "application/x-www-form-urlencoded";
    header["user-agent"] = USER_AGENT;

    HttpRequest request(TOKEN_URL, RM_POST, header, URLHelper::encode(body));
    HttpResponse resp = request.request();
   
    if (resp.status() == 200) {
        CLOG_DEBUG("Response:%s\n", resp.content().c_str());
        _parse_response(resp.content());
        CLOG_DEBUG("access_token:%s\n", _resp.access_token.c_str());
        CLOG_DEBUG("refresh_token:%s\n", _resp.refresh_token.c_str());
        CLOG_DEBUG("token_expiry:%d\n", _resp.token_expiry); 
        cred.refresh(_resp.access_token, _resp.refresh_token, _resp.token_expiry, _resp.id_token);
        return true;
    }
    else {
        CLOG_WARN("error_msg: %s\n", resp.content().c_str());
        return false;
    }
}

}
