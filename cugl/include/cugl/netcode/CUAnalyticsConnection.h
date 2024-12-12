//
//  CUAnalyticsConnection.h
//  Cornell University Game Library (CUGL)
//
//  This class provides the ability to keep track of Game Analytics. Analytics such
//  as Tasks, TaskAttempts and Actions can be recorded and sent to an external server.
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

#include <string>
#include <memory>
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
    * Tasks can be sent to the analytics server {@link AnalyticsConnection#addTask}.
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
    * {@link AnalyticsConnection#addTaskAttempt}, and then updated via {@link AnalyticsConnection#syncTaskAttempt}.
    */
    class TaskAttempt
    {
    public:
        /**
        * An enum representing the current status of a task attempt
        *
        * Task attempts with a terminating status can not have their status modified
        * afterwards. When a session is ended, all pending task attempts become preempted
        */
        enum class Status
        {
            /** Task attempt which has not been started yet */
            NOT_STARTED,
            /** Task attempt which is currently active */
            PENDING,

            // TERMINATING STATUSES

            /** Task attempt where the task was successfully completed */
            SUCCEEDED,
            /** Task attempt where the task has been failed irrevocably */
            FAILED,
            /** Task attempt where the player's session disconnected midway */
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
        * by the static constructor {@link #alloc} instead.
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
        * @param taskStatistics   The statistics recorded by the task attempt with their default values
        * @return true if initialization was successful
        */
        bool init(const std::shared_ptr<Task> task, std::shared_ptr<JsonValue> taskStatistics)
        {
            _task = task;
            _uuid = hashtool::generate_uuid();
            _taskStatistics = taskStatistics;
            _numFailures = 0;
            _status = Status::NOT_STARTED;
            return true;
        };
    public:
        /**
        * Returns a newly allocated TaskAttempt.
        *
        * @param task             The Task being attempted 
        * @param taskStatistics   The statistics recorded by the task attempt with their default values
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
        * @return true if the TaskAttempt is in a terminating state.
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

class AnalyticsConnection
{
private:
std::shared_ptr<WebSocket> _webSocket;
std::shared_ptr<WebSocketConfig> _config;
std::string _organization_name;
std::string _game_name;
std::string _version_number;
std::string _vendor_id;
std::string _platform;       
bool _init_data_sent;   

#pragma mark Constructors
public:
AnalyticsConnection();
~AnalyticsConnection();
private:
void dispose();
bool init(const WebSocketConfig &config, const std::string &organization_name, const std::string &game_name, const std::string &version_number, const bool &debug);

#pragma mark Communication
public:
bool open();

bool close();

private:

bool send(std::shared_ptr<JsonValue> &data); // This is the helper function to send data

#pragma mark Callbacks

void onReceiptCallback(const std::vector<std::byte> &message, Uint64 time);

void onStateChangeCallback(const WebSocket::State state);

#pragma mark Static Allocators

public:
static std::shared_ptr<AnalyticsConnection> alloc(const WebSocketConfig &config, const std::string &organization_name, const std::string &game_name, const std::string &version_number, const bool &debug=false)
{
    std::shared_ptr<AnalyticsConnection> result = std::make_shared<AnalyticsConnection>();
    return (result->init(config, organization_name, game_name, version_number, debug) ? result : nullptr);
}

#pragma mark Accessors

std::shared_ptr<WebSocket> getWebsocketConnection(){
    return _webSocket;
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
bool sendInitialData();
bool addTask(const std::shared_ptr<Task> &task);
bool addTasks(const std::vector<std::shared_ptr<Task>> &tasks);
bool addTaskAttempt(const std::shared_ptr<TaskAttempt> &taskAttempt);
bool syncTaskAttempt(const std::shared_ptr<TaskAttempt> &taskAttempt);
bool recordAction(const std::shared_ptr<JsonValue> &actionBlob); // Add Action Data here      
            };
        }
    } // namespace netcode
} // namespace cugl

#endif // __CU_ANALYTICS_CONNECTION_H__