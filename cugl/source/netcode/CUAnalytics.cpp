//
//  CUAnalytics.h
//  Cornell University Game Library (CUGL)
//
//  [ desc ]
//
//  This class uses our standard shared-pointer architecture.
//
//  1. The constructor does not perform any initialization; it just sets all
//     attributes to their defaults.
//
//  2. All initialization takes place via init methods, which can fail if an
//     object is initialized more than once.
//
//  3. All allocation takes place via static constructors which return a shared
//     pointer.
//
//  Note, however, that it is never safe to put this particular object on the
//  stack. therefore, everything except for the static constructors are private.
//
//
//  CUGL MIT License:
//      This software is provided 'as-is', without any express or implied
//      warranty.  In no event will the authors be held liable for any damages
//      arising from the use of this software.
//
//      Permission is granted to anyone to use this software for any purpose,
//      including commercial applications, and to alter it and redistribute it
//      freely, subject to the following restrictions:
//
//      1. The origin of this software must not be misrepresented; you must not
//      claim that you wrote the original software. If you use this software
//      in a product, an acknowledgment in the product documentation would be
//      appreciated but is not required.
//
//      2. Altered source versions must be plainly marked as such, and must not
//      be misrepresented as being the original software.
//
//      3. This notice may not be removed or altered from any source distribution.
//
//  Author:
//  Version:
#include <cugl/netcode/CUNetworkLayer.h>
#include <cugl/netcode/CUAnalytics.h>
#include <cugl/netcode/CUNetcodeSerializer.h>
#include <cugl/core/assets/CUJsonValue.h>
#include <cugl/netcode/CUAnalytics.h>
#include <cugl/netcode/CUWebSocket.h>
#include <cugl/core/util/CUDebug.h>
#include <SDL_app.h>

using namespace cugl::netcode::analytics;
using namespace std;

#pragma mark Constructors

AnalyticsConnection::AnalyticsConnection() : _webSocket(nullptr), _serializer(nullptr), _dispatcher(nullptr), _deserializer(nullptr), _organization_name(""), _game_name(""), _version_number(""), _vendor_id(""), _platform("") {}

AnalyticsConnection::~AnalyticsConnection()
{
    dispose();
}

bool AnalyticsConnection::init(const InetAddress &address, const std::string &organization_name, const std::string &game_name, const std::string &version_number, bool secure)
{

    // Get UUID from the hashtool functions system_uuid()
    // System platform
    _webSocket = WebSocket::allocWithPath(address, "/ws/analytics/");
    _serializer = NetcodeSerializer::alloc();
    _deserializer = NetcodeDeserializer::alloc();
    _organization_name = organization_name;
    _game_name = game_name;
    _version_number = version_number;
    _vendor_id = hashtool::system_uuid();
    _platform = APP_GetDeviceModel();
    _dispatcher = [this](const std::vector<std::byte> &message, Uint64 time)
    {
        _deserializer->receive(message);
        std::shared_ptr<JsonValue> responseMessage = _deserializer->readJson();
        if (responseMessage->has("error"))
        {
            std::string errorMessage = responseMessage->get("error")->asString();
            throw(errorMessage);
        }
    };
    _webSocket->open(secure);
    
    while (!_webSocket->isOpen()){}
    std::string initJSONString = "{\"message_type\": \"init\","
        "\"message_payload\": {"
        "\"organization_name\": \"" + _organization_name + "\","
        "\"game_name\": \"" + _game_name + "\","
        "\"version_number\": \"" + _version_number + "\","
        "\"vendor_id\": \"" + _vendor_id + "\","
        "\"platform\": \"" + _platform + "\""
        "}}";
    std::shared_ptr<JsonValue> initPayload = JsonValue::allocWithJson(initJSONString);

    send(initPayload);
    return true;
}

void AnalyticsConnection::dispose()
{
    _webSocket = nullptr;
    _serializer = nullptr;
    _deserializer = nullptr;
    _dispatcher = nullptr;
    _organization_name = "";
    _game_name = "";
    _version_number = "";
    _vendor_id = "";
    _platform = "";
}

void AnalyticsConnection::open(bool secure)
{
    _webSocket->open(secure);
}

bool AnalyticsConnection::send(std::shared_ptr<JsonValue> &data)
{
    if (!_webSocket->isOpen()){
        CULogError("ANALYTICS ERROR: Websocket was not opened before sending");
        return false;
    }
    // Need to encode bytes without metadata from NetcodeSerializer
    std::vector<std::byte> bytes;
    try
    {
        for (char& c : data->toString()) {
            bytes.push_back(static_cast<std::byte>(c));
        }
        _webSocket->send(bytes);
        _webSocket->onReceipt(_dispatcher);
    }
    catch (const std::exception &ex)
    {
        CULogError("ANALYTICS ERROR: %s", ex.what());
        _webSocket->close();
        return false;
    }
    if (getDebug()){
        CULog("ANALYTICS SENT: %s", data->toString().c_str());
    }
    return true;
}

void AnalyticsConnection::close()
{
    _webSocket->close();
}

void AnalyticsConnection::setDebug(bool flag)
{
    _webSocket->setDebug(flag);
}

bool AnalyticsConnection::getDebug()
{
    return _webSocket->getDebug();
}

bool AnalyticsConnection::addTask(const std::shared_ptr<Task> &task){
    std::string taskString = "{\"message_type\": \"task\","
        "\"message_payload\": {"
        "\"organization_name\": \"" + _organization_name + "\","
        "\"game_name\": \"" + _game_name + "\","
        "\"version_number\": \"" + _version_number + "\","
        "\"vendor_id\": \"" + _vendor_id + "\","
        "\"platform\": \"" + _platform + "\","
        "\"vendor_id\": \"" + _vendor_id + "\","
        "\"task_uuid\": \"" + task->getUUID() + "\","
        "\"task_name\": \"" + task->getName() + "\""
        "}}";

    std::shared_ptr<JsonValue> taskPayload = JsonValue::allocWithJson(taskString);

    send(taskPayload);
    return true;
}
bool AnalyticsConnection::addTasks(const std::vector<std::shared_ptr<Task>> &tasks){
    bool success = true;
    for(const std::shared_ptr<Task>& task : tasks){
        success = success && addTask(task);
    }
    return success;
};

bool AnalyticsConnection::addTaskAttempt(const std::shared_ptr<TaskAttempt> &taskAttempt){
    std::string taskAttemptString = "{\"message_type\": \"task_attempt\","
        "\"message_payload\": {"
        "\"task_uuid\": \"" + taskAttempt->getTask()->getUUID() + "\","
        "\"task_attempt_uuid\": \"" + taskAttempt->getUUID() + "\","
        "\"statistics\": \"" + taskAttempt->getTaskStatistics()->toString() + "\""
        "}}";

    std::shared_ptr<JsonValue> taskAttemptPayload = JsonValue::allocWithJson(taskAttemptString);

    send(taskAttemptPayload);
    return true;
}
bool AnalyticsConnection::syncTaskAttempt(const std::shared_ptr<TaskAttempt> &taskAttempt){
    std::string syncTaskAttemptString = "{\"message_type\": \"sync_task_attempt\","
        "\"message_payload\": {"
        "\"task_attempt_uuid\": \"" + taskAttempt->getUUID() + "\","
        "\"status\": \"" + taskAttempt->getStatusAsString() + "\","
        "\"num_failures\": \"" + std::to_string(taskAttempt->getNumFailures()) + "\","
        "\"statistics\": \"" + taskAttempt->getTaskStatistics()->toString() + "\""
        "}}";

    std::shared_ptr<JsonValue> syncTaskAttemptPayload = JsonValue::allocWithJson(syncTaskAttemptString);

    send(syncTaskAttemptPayload);
    return true;
}
bool AnalyticsConnection::recordAction(const std::shared_ptr<JsonValue> &actionBlob){
    std::string actionString = "{\"message_type\": \"action\","
        "\"message_payload\": {"
        "\"data\": \"" + actionBlob->toString() + "\""
        "}}";

    std::shared_ptr<JsonValue> actionPayload = JsonValue::allocWithJson(actionString);

    send(actionPayload);
    return true;
}

