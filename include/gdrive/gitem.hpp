#ifndef __GDRIVE_GFILE_HPP__
#define __GDRIVE_GFILE_HPP__

#include <string>
#include <time.h>
#include <set>
#include <map>
#include "jconer/json.hpp"

#define GETTER(type, name) type get_##name() { return name;}

#define SETTER(type, name) void set_##name(type v) {\
    name = v; \
    _fields.insert(#name); \
}

#define READONLY(type, name) \
    private: \
        type name;\
    public: \
        GETTER(type, name)

#define WRITABLE(type, name) \
    private: \
        type name; \
    public: \
        SETTER(type, name) \
        GETTER(type, name)


using namespace JCONER;

namespace GDRIVE {

class GFileLabel {
public:
    GFileLabel();
    bool starred;
    bool hidden;
    bool trashed;
    bool restricted;
    bool viewed;
    void from_json(JObject* obj);
    JObject* to_json();
};

class GUser {
public:
    GUser();
    std::string displayName;
    std::string picture_url;
    bool isAuthenticatedUser;
    std::string permissionId;
    void from_json(JObject* obj);
    JObject* to_json();
};

class GParent {
public:
    GParent();
    std::string id;
    std::string selfLink;
    std::string parentLink;
    bool isRoot;
    void from_json(JObject* obj);
    JObject* to_json();
};


class GProperty {
public:
    GProperty();
    std::string etag;
    std::string selfLink;
    std::string key;
    std::string visibility;
    std::string value;
    void from_json(JObject* obj);
    JObject* to_json();
};

class GPermission {
public:
    GPermission();
    std::string etag;
    std::string id;
    std::string selfLink;
    std::string name;
    std::string emailAddress;
    std::string domain;
    std::string role;
    std::vector<std::string> additionalRoles;
    std::string type;
    std::string value;
    std::string authKey;
    std::string withLink;
    std::string photoLink;
    void from_json(JObject* obj);
    JObject* to_json();
};

class GImageMediaMetaData {
public:
    GImageMediaMetaData();
    int width;
    int height;
    int rotation;
    class Location {
    public:
        double latitude;
        double longitude;
        double altitude;
        void from_json(JObject* obj);
        JObject* to_json();
    } location;
    std::string date;
    std::string cameraMaker;
    std::string cameraModel;
    double exposureTime;
    double aperture;
    bool flashUsed;
    double focalLength;
    int isoSpeed;
    std::string meteringMode;
    std::string sensor;
    std::string exposureMode;
    std::string colorSpace;
    std::string whiteBalance;
    double exposureBias;
    double maxApertureValue;
    int subjectDistance;
    std::string lens;
    void from_json(JObject* obj);
    JObject* to_json();
};

typedef std::map<std::string, std::string> GExportLink;
typedef std::map<std::string, std::string> Links;

class GFile {
public:
    GFile();
    void from_json(JObject* obj);
    JObject* to_json();
    std::set<std::string> get_modified_fields() { return _fields;}
    

    READONLY(std::string, id)
    READONLY(std::string, etag)
    READONLY(std::string, selfLink)
    READONLY(std::string, webContentLink)
    READONLY(std::string, alternateLink)
    READONLY(std::string, embedLink)
    READONLY(Links, openWithLinks)
    READONLY(std::string, defaultOpenWithLink)
    READONLY(std::string, iconLink)
    READONLY(std::string, thumbnailLink)
    //thumbnail
    WRITABLE(std::string, title)
    READONLY(std::string, mimeType)
    WRITABLE(std::string, description)
    WRITABLE(GFileLabel, labels)
    READONLY(struct tm, createdDate)
    WRITABLE(struct tm, modifiedDate)
    READONLY(struct tm, modifiedByMeDate)
    READONLY(struct tm, lastViewedByMeDate)
    READONLY(struct tm, sharedWithMeDate)
    READONLY(std::string, version)
    READONLY(GUser, sharingUser)
    WRITABLE(std::vector<GParent>, parents)
    READONLY(GExportLink, exportLinks)
    WRITABLE(std::string, indexableText)
    READONLY(GPermission, userPermission)
    READONLY(std::vector<GPermission>, permissions)
    READONLY(std::string, originalFilename)
    READONLY(std::string, fileExtension)
    READONLY(std::string, md5Checksum)
    READONLY(long, fileSize)
    READONLY(int, quotaBytesUsed)
    READONLY(std::vector<std::string>, ownerNames)
    READONLY(std::vector<GUser>, owners)
    READONLY(std::string, lastModifyingUserName)
    READONLY(GUser, lastModifyingUser)
    READONLY(bool, editable)
    READONLY(bool, copyable)
    READONLY(bool, shared)
    READONLY(bool, explicitlyTrashed)
    READONLY(bool, appDataContents)
    READONLY(std::string, headRevisionId)
    READONLY(std::vector<GProperty>, properties)
    READONLY(GImageMediaMetaData, imageMediaMetadata)
    WRITABLE(bool, writersCanShare)
private:
    std::set<std::string> _fields;
};

}

#endif
