//
//  CUAnalyticsConnection.h
//  Cornell University Game Library (CUGL)
//
//  This class provides the ability to keep track of Game Analytics. Analytics such
//  as Tasks, TaskAttempts and actions can be recorded and sent to an external server.
//  It can also be used to keep track of play session data. So every user's session can be
//  logged using this class. This class makes use of CUWebSocket class to connect to
//  an external server which then stores the analytics to a Postgres database.
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
//  Author: Shivam Thakkar, Kidus Zegeye, Cody Zheng
//  Version: 12/21/24 (CUGL 3.0 reorganization)
//
#include <cugl/netcode/CUNetworkLayer.h>
#include <cugl/core/assets/CUJsonValue.h>
#include <cugl/netcode/CUAnalyticsConnection.h>
#include <cugl/netcode/CUWebSocket.h>
#include <cugl/core/util/CUDebug.h>

#include <SDL_app.h>
#include <memory>
#include <sstream>
#include <thread>

using namespace cugl;
using namespace netcode;
using namespace analytics;
using namespace std;

#pragma mark Constructors
/**
 * Creates a degenerate websocket connection along with empty initializations for gameMetaData.
 *
 * This object has not been initialized with a {@link NetcodeConfig} and cannot
 * be used.
 *
 * You should NEVER USE THIS CONSTRUCTOR. All connections should be created by
 * the static constructor {@link alloc} instead.
 */
AnalyticsConnection::AnalyticsConnection() : _webSocket(nullptr),
                                             _config(nullptr),
                                             _organization_name(""),
                                             _game_name(""),
                                             _version_number(""),
                                             _vendor_id(""),
                                             _platform(""),
                                             _init_data_sent(false) {}

/**
 * Deletes the analytics websocket connection, disposing all resources
 */
AnalyticsConnection::~AnalyticsConnection()
{
    dispose();
}

/**
 * Initializes the AnalyticsConnection with the given websocket configuration along
 * with the game MetaData. Opens the connection in order to send intialization data to the
 * analytics server.
 *
 * @param config The WebSocket configuration.
 * @param organization_name The name of the organization.
 * @param game_name The name of the game.
 * @param version_number The version number of the game.
 * @param debug  Whether to log debug messages from the connection
 * @return true if initialization was successful, false otherwise.
 */
bool AnalyticsConnection::init(const WebSocketConfig &config, const std::string &organization_name, const std::string &game_name, const std::string &version_number, const bool &debug)
{

    InetAddress address = InetAddress(config.bindaddr, config.port);
    _webSocket = WebSocket::alloc(address);
    _organization_name = organization_name;
    _game_name = game_name;
    _version_number = version_number;
    _vendor_id = hashtool::system_uuid();
    _platform = APP_GetDeviceModel();
    _config = std::make_shared<WebSocketConfig>(config);
    _init_data_sent = false;
    setDebug(debug);

    WebSocket::Dispatcher dispatcher = [this](const std::vector<std::byte> &message, Uint64 time) { onReceiptCallback(message, time);};

    WebSocket::StateCallback stateCallback = [this](const WebSocket::State state) {onStateChangeCallback(state);};

    _webSocket->onReceipt(dispatcher);
    _webSocket->onStateChange(stateCallback);

    open();
    sendInitialData();
    return true;
}

/**
 * Disposes of the AnalyticsConnection resources.
 *
 * Closes the WebSocket and resets member variables to their default values.
 */
void AnalyticsConnection::dispose()
{
    close();

    _webSocket = nullptr;
    _organization_name = "";
    _game_name = "";
    _version_number = "";
    _vendor_id = "";
    _platform = "";
    _config = nullptr;
    _init_data_sent = false;
}

#pragma mark Communication

/**
 * Opens the WebSocket connection.
 *
 * @return true if the connection was successfully opened.
 */
bool AnalyticsConnection::open()
{
    _webSocket->open(_config->secure);

    while (!_webSocket->isOpen())
    {
        // Normally should be CONNECTING
        if (_webSocket->getState() == WebSocket::State::CLOSED || _webSocket->getState() == WebSocket::State::FAILED){
            return false;
        }
            std::this_thread::sleep_for(100ms);

    }
    return true;
}

/**
 * Closes the WebSocket connection.
 *
 * @return true if the connection was successfully closed.
 */
bool AnalyticsConnection::close()
{
    _webSocket->close();
    while (!(_webSocket->getState() == WebSocket::State::CLOSED))
    {
        std::this_thread::sleep_for(100ms);
    }
    if (getDebug()){
        CULog("Websocket closed");
    }
    return true;
}

/**
 * Sends data to the WebSocket server.
 *
 * @param data The JSON data to send.
 * @return true if the data was successfully sent, false otherwise.
 */
bool AnalyticsConnection::send(std::shared_ptr<JsonValue> &data)
{
    if (!_webSocket->isOpen())
    {
        if (!open()){
            return false;
        }
    }
    // Need to encode bytes without metadata from NetcodeSerializer
    std::vector<std::byte> bytes;
    try
    {
        for (char &c : data->toString())
        {
            bytes.push_back(static_cast<std::byte>(c));
        }
        _webSocket->send(bytes);
    }
    catch (const std::exception &ex)
    {
        CULogError("ANALYTICS ERROR: %s", ex.what());
        _webSocket->close();
        return false;
    }
    if (getDebug())
    {
        CULog("ANALYTICS SENT: %s", data->toString().c_str());
    }
    return true;
}
#pragma mark Callbacks

/**
 * Callback function that logs responses from the analytics server
 *
 * @param message The message received from the server
 * @param time The time when the message was received
 */
void AnalyticsConnection::onReceiptCallback(const std::vector<std::byte> &message, Uint64 time) {
        std::ostringstream disp;
        for (const auto &byte : message)
        {
            disp << static_cast<char>(byte);
        }

        std::shared_ptr<JsonValue> responseJSON = JsonValue::allocWithJson(disp.str());
        CULog("ANALYTICS RESPONSE: %s", responseJSON->toString().c_str());
        if (responseJSON->has("error"))
        {
            std::string errorMessage = responseJSON->get("error")->asString();
            throw(errorMessage);
        }
    };

/**
 * Callback function that logs state changes in the websocket connection
 *
 * @param state The new websocket state
 */
void AnalyticsConnection::onStateChangeCallback(const WebSocket::State state){
    CULog("State change: %d", state);
}

#pragma mark Accessors

/**
* Toggles the debugging status of this connection.
*
* If debugging is active, connections will be quite verbose
*
* @param flag  Whether to activate debugging
*/
void AnalyticsConnection::setDebug(bool flag)
{
    _webSocket->setDebug(flag);
}

/**
* Returns the debugging status of this connection.
*
* If debugging is active, connections will be quite verbose
*
* @return the debugging status of this connection.
*/
bool AnalyticsConnection::getDebug()
{
    return _webSocket->getDebug();
}

#pragma mark AnalyticsData
/**
* Sends initialization data to the analytics server.
*
* @return true if initialization data has been sent successfully 
*/
bool AnalyticsConnection::sendInitialData(){
    if (!_init_data_sent){
        std::string initJSONString = "{\"message_type\": \"init\","
                                    "\"message_payload\": {"
                                        "\"organization_name\": \"" + _organization_name + "\","
                                        "\"game_name\": \"" + _game_name + "\","
                                        "\"version_number\": \"" + _version_number + "\","
                                        "\"vendor_id\": \"" +  _vendor_id + "\","
                                        "\"platform\": \"" +  _platform + "\""
                                    "}}";
        std::shared_ptr<JsonValue> initPayload = JsonValue::allocWithJson(initJSONString);
        _init_data_sent = send(initPayload);
    }
    return _init_data_sent;
}

/**
 * Adds a task to the analytics database.
 *
 * @param task The task to add.
 * @return true if the task was successfully added, false otherwise.
 */
bool AnalyticsConnection::addTask(const std::shared_ptr<Task> &task)
{
    std::string taskString = "{\"message_type\": \"task\","
                             "\"message_payload\": {"
                                "\"organization_name\": \"" + _organization_name + "\","
                                "\"game_name\": \"" + _game_name + "\","
                                "\"version_number\": \"" + _version_number + "\","
                                "\"vendor_id\": \"" + _vendor_id + "\","
                                "\"platform\": \"" + _platform + "\","
                                "\"task_name\": \"" + task->getName() + "\""
                             "}}";

    std::shared_ptr<JsonValue> taskPayload = JsonValue::allocWithJson(taskString);

    return send(taskPayload);
}

/**
 * Adds multiple tasks to the analytics database.
 *
 * @param tasks The list of tasks to add.
 * @return true if all tasks were successfully added, false otherwise.
 */
bool AnalyticsConnection::addTasks(const std::vector<std::shared_ptr<Task>> &tasks)
{
    bool success = true;
    for (const std::shared_ptr<Task> &task : tasks)
    {
        success = success && addTask(task);
    }
    return success;
};

/**
 * Adds a TaskAttempt to the analytics database.
 *
 * @param taskAttempt The TaskAttempt to add.
 * @return true if the TaskAttempt was successfully added, false otherwise.
 */
bool AnalyticsConnection::addTaskAttempt(const std::shared_ptr<TaskAttempt> &taskAttempt)
{
    std::string taskAttemptString = "{\"message_type\": \"task_attempt\","
                                    "\"message_payload\": {"
                                        "\"organization_name\": \"" + _organization_name + "\","
                                        "\"game_name\": \"" + _game_name + "\","
                                        "\"version_number\": \"" + _version_number + "\","
                                        "\"vendor_id\": \"" +  _vendor_id + "\","
                                        "\"platform\": \"" + _platform + "\","
                                        "\"task_name\": \"" + taskAttempt->getTask()->getName() + "\","
                                        "\"task_attempt_uuid\": \"" + taskAttempt->getUUID() + "\"," +
                                        "\"status\": \"" + taskAttempt->getStatusAsString() + "\","
                                        "\"num_failures\": \"" + std::to_string(taskAttempt->getNumFailures()) + "\","
                                        "\"statistics\": " + taskAttempt->getTaskStatistics()->toString() +
                                    "}}";

    std::shared_ptr<JsonValue> taskAttemptPayload = JsonValue::allocWithJson(taskAttemptString);

    return send(taskAttemptPayload);
}

/**
 * Synchronizes a TaskAttempt with the analytics database. This updates the
 * data of a specific taskAttempt on the analytics server
 *
 * @param taskAttempt The TaskAttempt to synchronize.
 * @return true if the synchronization was successful, false otherwise.
 */
bool AnalyticsConnection::syncTaskAttempt(const std::shared_ptr<TaskAttempt> &taskAttempt)
{
    std::string syncTaskAttemptString = "{\"message_type\": \"sync_task_attempt\","
                                        "\"message_payload\": {"
                                            "\"task_attempt_uuid\": \"" + taskAttempt->getUUID() + "\","
                                            "\"status\": \"" + taskAttempt->getStatusAsString() + "\","
                                            "\"num_failures\": \"" + std::to_string(taskAttempt->getNumFailures()) + "\","
                                            "\"statistics\": " + taskAttempt->getTaskStatistics()->toString() + 
                                        "}}";

    std::shared_ptr<JsonValue> syncTaskAttemptPayload = JsonValue::allocWithJson(syncTaskAttemptString);

    return send(syncTaskAttemptPayload);
}

/**
 * Records an action in the analytics database.
 *
 * @param actionBlob The JSON data representing the action.
 * @param relatedTaskAttempts The TaskAttempts related to this action.
 * @return true if the action was successfully recorded, false otherwise.
 */
bool AnalyticsConnection::recordAction(const std::shared_ptr<JsonValue> &actionBlob, const std::vector<std::shared_ptr<TaskAttempt>> relatedTaskAttempts)
{
    std::shared_ptr<JsonValue> taskAttemptArray = JsonValue::allocArray();
    for (auto & ta : relatedTaskAttempts) {
        taskAttemptArray->appendValue(ta->getUUID());
    }

    std::string actionString = "{\"message_type\": \"action\","
                               "\"message_payload\": {"
                                    "\"organization_name\": \"" + _organization_name + "\","
                                    "\"game_name\": \"" + _game_name + "\","
                                    "\"version_number\": \"" + _version_number + "\","
                                    "\"vendor_id\": \"" + _vendor_id + "\","
                                    "\"platform\": \"" + _platform + "\","
                                    "\"task_attempt_uuids\": " + taskAttemptArray->toString() + ","
                                    "\"data\": " + actionBlob->toString() + 
                                "}}";
    std::shared_ptr<JsonValue> actionPayload = JsonValue::allocWithJson(actionString);

    return send(actionPayload);
}
