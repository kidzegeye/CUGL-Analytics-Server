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

using namespace cugl::netcode::analytics;
using namespace std;
#include <cugl/netcode/CUAnalytics.h>
#include <cugl/netcode/CUWebSocket.h>
#include <cugl/core/util/CUDebug.h>

using namespace cugl::netcode::analytics;
using namespace std;

#pragma mark Constructors

AnalyticsConnection::AnalyticsConnection() : _webSocket(nullptr), _serializer(nullptr), _dispatcher(nullptr), _deserializer(nullptr) {}

AnalyticsConnection::~AnalyticsConnection()
{
    dispose();
}

bool AnalyticsConnection::init(const InetAddress &address, bool secure)
{

    // Get UUID from the hashtool functions system_uuid()
    // System platform
    _webSocket = WebSocket::allocWithPath(address, "/ws/analytics/");
    // bool _status = true;
    _serializer = NetcodeSerializer::alloc();
    _deserializer = NetcodeDeserializer::alloc();
    _dispatcher = [this](const std::vector<std::byte> &message, Uint64 time)
    {
        _deserializer->receive(message);
        std::shared_ptr<JsonValue> responseMessage = _deserializer->readJson();
        if (responseMessage->has("error"))
        {
            std::string errorMessage = responseMessage->get("error")->asString();
            throw("Error message: %s", errorMessage);
        }
    };
    _webSocket->open(secure);
    std::shared_ptr<JsonValue> gameMetaData = JsonValue::allocWithJson("{\"key\":\"value\"}");
    _serializer->writeJson(gameMetaData);
    _webSocket->send(_serializer->serialize());
    _serializer->reset();
    try
    {
        _webSocket->onReceipt(_dispatcher);
    }
    catch (const std::exception &ex)
    {

        CULogError("NETCODE ERROR: %s", ex.what());
        return false;
    }
    return true;
}

void AnalyticsConnection::dispose()
{
    _webSocket = nullptr;
    _serializer = nullptr;
    _deserializer = nullptr;
    _dispatcher = nullptr;
}

void AnalyticsConnection::open(bool secure)
{
    _webSocket->open(secure);
}
