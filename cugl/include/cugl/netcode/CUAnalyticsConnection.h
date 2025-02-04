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
#ifndef __CU_ANALYTICS_CONNECTION_H__
#define __CU_ANALYTICS_CONNECTION_H__

#include "cugl/core/assets/CUJsonValue.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <cugl/netcode/CUWebSocket.h>
#include <cugl/core/util/CUHashtools.h>
#include <cugl/netcode/CUWebSocketConfig.h>

namespace cugl
{
    /**
     * The classes to support CUGL networking.
     *
     * Currently CUGL supports ad-hoc game lobbies using web-sockets. The
     * sockets must connect connect to a CUGL game lobby server. However,
     * the actual network layer is supported by high speed WebRTC. See
     *
     *     https://libdatachannel.org
     *
     * for an explanation of our networking layer.
     */
    namespace netcode
    {
    /**
    * The classes to support the CUGL analytics server.
    */
        namespace analytics
        {
    /**
    * A class for defining task/quest analytics on the analytics server.
    *
    * Tasks can be sent to the analytics server {@link addTask}.
    * Once uploaded, the Task object can be used to create {@link TaskAttempt} objects, 
    * which are used for data collecting individual attempts of a task.
    */
    class Task
    {
    private:
        /** The unique name of a task. */
        std::string _name;
    public:
       /**
        * Creates a degenerate task.
        *
        * Because the name of a task is unique, it is not recommended to use
        * the default constructor.
        */
        Task() : _name("") {}

        /**
        * Deletes this configuration, disposing all resources
        */
        ~Task() { dispose(); }
    private:
        /**
        * Disposes of the Task resources.
        */
        void dispose()
        {
            _name = "";
        }
        /**
        * Initializes a new Task 
        *
        * @param name The name of the task.
        * @return true if initialization was successful, false otherwise.
        */
        bool init(std::string name)
        {
            _name = name;
            return true;
        };
    public:
        /**
        * Returns a newly allocated Task.
        *
        * @param name The name of the task.        
        *
        * @return a newly allocated Task with the given name
        */
        static std::shared_ptr<Task> alloc(std::string name)
        {
            std::shared_ptr<Task> result = std::make_shared<Task>();
            return (result->init(name) ? result : nullptr);
        }

        /**
        * Returns the name for this task.
        *
        * @return the name for this task.
        */
        std::string getName() const { return _name; }
    };


    /**
    * A class for recording a player's attempts at a task.
    *
    * These are linked to a specific {@link Task} object, and contains information
    * on the status of a task, the number of failures, start/completion time, and 
    * miscellaneous data. TaskAttempts are defined in the analytics server's database by using
    * {@link addTaskAttempt}, and then updated via {@link syncTaskAttempt}.
    */
    class TaskAttempt
    {
    public:
        /**
        * An enum representing the current status of a TaskAttempt
        *
        * TaskAttempts with a terminal status can not have their status modified
        * afterwards. When a session is ended, all pending TaskAttempts become preempted
        */
        enum class Status
        {
            /** TaskAttempt which has not been started yet */
            NOT_STARTED,
            /** TaskAttempt which is currently active */
            PENDING,

            // TERMINAL STATUSES

            /** TaskAttempt where the task was successfully completed (**Terminal**)*/
            SUCCEEDED,
            /** TaskAttempt where the task has been failed irrevocably (**Terminal**)*/
            FAILED,
            /** TaskAttempt where the player's session disconnected midway (**Terminal**)*/
            PREEMPTED
        };

    private:
        /** Shared pointer to the Task being attempted */
        std::shared_ptr<Task> _task;
        /** UUID for the TaskAttempt. Necessary because TaskAttempts aren't unique */
        std::string _uuid;
        /** Shared pointer to a JsonValue of miscellaneous statistics for a TaskAttempt */
        std::shared_ptr<JsonValue> _taskStatistics;
        /** Number of non-irrevocable ingame failures for TaskAttempt */
        int _numFailures;
        /** The current status of a TaskAttempt */
        Status _status;

    public:
        /**
        * Creates a degenerate TaskAttempt.
        *
        * This object has not been initialized with a uuid and cannot not be used
        *
        * You should NEVER USE THIS CONSTRUCTOR. All connections should be created
        * by the static constructor {@link alloc} instead.
        */

        TaskAttempt() : _task(nullptr),
                        _uuid(""),
                        _taskStatistics(nullptr),
                        _numFailures(0),
                        _status(Status::NOT_STARTED) {}
    
        /**
        * Deletes this TaskAttempt, disposing all resources
        */
        ~TaskAttempt() { dispose(); }
    private:
        /**
        * Disposes all of the resources used by this TaskAttempt.
        */
        void dispose()
        {
            _task = nullptr;
            _uuid = "";
            _taskStatistics = nullptr;
            _numFailures = 0;
            _status = Status::NOT_STARTED;
        };

        /**
        * Initializes a new TaskAttempt.
        *
        * @param task             The Task being attempted 
        * @param taskStatistics   The statistics recorded by the TaskAttempt with their default values (*Optional*: Default {})
        * @param status           The status of the TaskAttempt (*Optional*: Default NOT_STARTED)
        * @param numFailures      The number of failures during the TaskAttempt (*Optional*: Default 0)
        * @return true if initialization was successful
        */
        bool init(const std::shared_ptr<Task> task, std::shared_ptr<JsonValue> taskStatistics = JsonValue::allocObject(), Status status = Status::NOT_STARTED, int numFailures = 0)
        {
            _task = task;
            _uuid = hashtool::generate_uuid();
            _taskStatistics = taskStatistics;
            _numFailures = numFailures;
            _status = status;
            return true;
        };
        
    public:
        /**
        * Returns a newly allocated TaskAttempt.
        *
        * @param task             The Task being attempted 
        * @param taskStatistics   The statistics recorded by the TaskAttempt with their default values
        * @return true if initialization was successful
        */
        static std::shared_ptr<TaskAttempt> alloc(const std::shared_ptr<Task> task, std::shared_ptr<JsonValue> taskStatistics)
        {
            std::shared_ptr<TaskAttempt> result = std::make_shared<TaskAttempt>();
            return (result->init(task, taskStatistics) ? result : nullptr);
        }

        /**
        * Returns the UUID for this TaskAttempt.
        *
        * @return the UUID for this TaskAttempt.
        */
        std::string  getUUID() const { return _uuid; }

        /**
        * Returns a shared pointer to the {@link Task} being attempted.
        *
        * @return shared pointer to the Task.
        */
        std::shared_ptr<Task> getTask() const { return _task; }

        /**
        * Returns a boolean indicating if the TaskAttempt has ended
        *
        * @return true if the TaskAttempt is in a terminal state.
        */
        bool hasEnded() const { return _status == Status::SUCCEEDED ||
                                        _status == Status::FAILED ||
                                        _status == Status::PREEMPTED; }
        
        /**
        * Returns the number of failures that have occurred in this TaskAttempt
        *
        * @return number of failures during the TaskAttempt
        */
        int  getNumFailures() const { return _numFailures; }

        /**
        * Sets the number of failures that have occurred in this TaskAttempt
        *
        * @param numFailures   The new number of failures during the TaskAttempt
        */
        void setNumFailures(int numFailures) { _numFailures = numFailures; }

        /**
        * Returns the status of this TaskAttempt
        *
        * @return the status of this TaskAttempt
        */
        Status getStatus() const { return _status; }

        /**
        * Returns the status of this TaskAttempt as a string
        *
        * @return the status of this TaskAttempt as a string
        */
        std::string getStatusAsString() const {
            switch (_status)
            {
                case Status::SUCCEEDED:   return "succeeded";
                case Status::FAILED:   return "failed";
                case Status::PENDING: return "pending";
                case Status::PREEMPTED:   return "preempted";
                case Status::NOT_STARTED:   return "not_started";
                default:      return "unknown";
            }
        }
        /**
        * Sets the status of this TaskAttempt
        *
        * @param status  The new status of this TaskAttempt
        */
        void setStatus(Status status) { _status = status; }

        /**
        * Returns the task statistics of this TaskAttempt
        *
        * @return the task statistics of this TaskAttempt
        */
        std::shared_ptr<JsonValue> getTaskStatistics() const { return _taskStatistics; }


        /**
        * Sets the task statistics of this TaskAttempt
        *
        * @param taskStatistics the new task statistics of this TaskAttempt
        */
        void setTaskStatistics(std::shared_ptr<JsonValue> taskStatistics) { _taskStatistics = taskStatistics; }
    };

/**
* This class represents the connection to an external gameplay analytics server.
*
* The main use for the AnalyticsConnection class is to send live gameplay data called actions to an external analytics server,
* which can be used for analyzing how players behave in certain parts of the game.
*
* The AnalyticsConnection class can be used to define ingame tasks/quests using {@link Task} objects and then
* record statistics for each run of a task via {@link TaskAttempt} objects. Each action contains a {@link JsonValue} with
* miscellaneous data representing the player's actions, and can be optionally linked to one or more active TaskAttempts. Each time
* this client connects to a server, a new session is created which is linked to the actions and TaskAttempts made during
* the session. Sessions are automatically ended upon disconnection from the server.
*
* The AnalyticsConnection uses the {@link WebSocket} class to make the connection to an 
* external server. Because of this, it is completely unsafe it to be used on the stack. For
* that reason, this class hides the initialization methods (and the constructors
* create uninitialized connections). You are forced to go through the static
* allocator {@link alloc} to create instances of this class.
*/
class AnalyticsConnection
{
private:
/** The websocket connection used to communicate with an external analytics server */
std::shared_ptr<WebSocket> _webSocket;
/** The configuration of the websocket connection */
std::shared_ptr<WebSocketConfig> _config;
/** The name of the game's organization */
std::string _organization_name;
/** The name of the game */
std::string _game_name;
/** The version number of the game */
std::string _version_number;
/** The unique vendor id of the player's device */
std::string _vendor_id;
/** The hardware platform of the player's device */
std::string _platform;       
/** Indicates if the initialization data has been sent to the server already. */
bool _init_data_sent;   
/** The tasks added to the analytics connection. Indexed by task name. */
std::unordered_map<std::string, std::shared_ptr<Task>> _tasks;

#pragma mark Constructors
public:
/**
 * Creates a degenerate websocket connection along with empty initializations for gameMetaData.
 *
 * This object has not been initialized with a {@link NetcodeConfig} and cannot
 * be used.
 *
 * You should NEVER USE THIS CONSTRUCTOR. All connections should be created by
 * the static constructor {@link alloc} instead.
 */
AnalyticsConnection();
/**
 * Deletes the analytics websocket connection, disposing all resources
 */
~AnalyticsConnection();

private:
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
bool init(const WebSocketConfig &config, const std::string &organization_name, const std::string &game_name, const std::string &version_number, const bool &debug);


/**
 * Disposes of the AnalyticsConnection resources.
 *
 * Closes the WebSocket and resets member variables to their default values.
 */
void dispose();

#pragma mark Communication
public:

/**
 * Opens the WebSocket connection.
 *
 * @return true if the connection was successfully opened.
 */
bool open();

/**
 * Closes the WebSocket connection.
 *
 * @return true if the connection was successfully closed.
 */
bool close();

private:

/**
 * Sends data to the WebSocket server.
 *
 * @param data The JSON data to send.
 * @return true if the data was successfully sent, false otherwise.
 */
bool send(std::shared_ptr<JsonValue> &data); // This is the helper function to send data

#pragma mark Callbacks

/**
 * Callback function that logs responses from the analytics server
 *
 * @param message The message received from the server
 * @param time The time when the message was received
 */
void onReceiptCallback(const std::vector<std::byte> &message, Uint64 time);

/**
 * Callback function that logs state changes in the websocket connection
 *
 * @param state The new websocket state
 */
void onStateChangeCallback(const WebSocket::State state);

#pragma mark Static Allocators

public:

/**
 * Allocates a new  AnalyticsConnection with the given websocket configuration along
 * with the game MetaData. Opens the connection in order to send intialization data to the
 * analytics server.
 *
 * @param config The WebSocket configuration.
 * @param organization_name The name of the organization.
 * @param game_name The name of the game.
 * @param version_number The version number of the game.
 * @param debug  Whether to log debug messages from the connection.
 * @return a newly allocated analytics connection
 */
static std::shared_ptr<AnalyticsConnection> alloc(const WebSocketConfig &config, const std::string &organization_name, const std::string &game_name, const std::string &version_number, const bool &debug=false)
{
    std::shared_ptr<AnalyticsConnection> result = std::make_shared<AnalyticsConnection>();
    return (result->init(config, organization_name, game_name, version_number, debug) ? result : nullptr);
}

#pragma mark Accessors

/**
* Returns the websocket connection to the analytics server.
*
* @return the websocket connection
*/
std::shared_ptr<WebSocket> getWebsocketConnection(){
    return _webSocket;
}

/**
* Returns the stored tasks.
*
* @return the map of tasks stored
*/
std::unordered_map<std::string, std::shared_ptr<Task>> getTasks(){
    return _tasks;
}

/**
* Toggles the debugging status of this connection.
*
* If debugging is active, connections will be quite verbose
*
* @param flag  Whether to activate debugging
*/
void setDebug(bool flag);

/**
* Returns the debugging status of this connection.
*
* If debugging is active, connections will be quite verbose
*
* @return the debugging status of this connection.
*/
bool getDebug();

#pragma mark AnalyticsData

/**
* Sends initialization data to the analytics server.
*
* @return true if initialization data has been sent successfully 
*/
bool sendInitialData();

/**
 * Adds a Task to the analytics database.
 *
 * @param task The Task to add.
 * @return true if the Task was successfully added, false otherwise.
 */
bool addTask(const std::shared_ptr<Task> &task);

/**
 * Adds multiple Tasks to the analytics database.
 *
 * @param tasks The vector of Tasks to add.
 * @return true if all Tasks were successfully added, false otherwise.
 */
bool addTasks(const std::vector<std::shared_ptr<Task>> &tasks);

/**
 * Adds a TaskAttempt to the analytics database.
 *
 * @param taskAttempt The TaskAttempt to add.
 * @return true if the TaskAttempt was successfully added, false otherwise.
 */
bool addTaskAttempt(const std::shared_ptr<TaskAttempt> &taskAttempt);

/**
 * Adds multiple TaskAttempts to the analytics database.
 *
 * @param taskAttempts The vector of TaskAttempts to add.
 * @return true if all TaskAttempts were successfully added, false otherwise.
 */
bool addTaskAttempts(const std::vector<std::shared_ptr<TaskAttempt>> &taskAttempts);

/**
 * Synchronizes a TaskAttempt with the analytics database. This updates the
 * data of a specific taskAttempt on the analytics server
 *
 * @param taskAttempt The TaskAttempt to synchronize.
 * @return true if the synchronization was successful, false otherwise.
 */
bool syncTaskAttempt(const std::shared_ptr<TaskAttempt> &taskAttempt);

/**
 * Records an action in the analytics database.
 *
 * @param actionBlob The JSON data representing the action.
 * @param relatedTaskAttempts The TaskAttempts related to this action.
 * @return true if the action was successfully recorded, false otherwise.
 */
bool recordAction(const std::shared_ptr<JsonValue> &actionBlob, const std::vector<std::shared_ptr<TaskAttempt>> &relatedTaskAttempts = std::vector<std::shared_ptr<TaskAttempt>>());  
            
            };
        } // namespace analytics
    } // namespace netcode
} // namespace cugl

#endif // __CU_ANALYTICS_CONNECTION_H__