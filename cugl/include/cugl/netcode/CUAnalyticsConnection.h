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

        namespace analytics
        {
            class Task
            {
            private:
                std::string _name;
            public:
                Task() : _name("") {}
        
                ~Task() { dispose(); }
            private:
                void dispose()
                {
                    _name = "";
                }
                bool init(std::string name)
                {
                    _name = name;
                    return true;
                };
            public:
                static std::shared_ptr<Task> alloc(std::string name)
                {
                    std::shared_ptr<Task> result = std::make_shared<Task>();
                    return (result->init(name) ? result : nullptr);
                }

                std::string getName() const { return _name; }
            };

            class TaskAttempt
            {
            public:
                enum class Status
                {
                    SUCCEEDED,
                    FAILED,
                    PENDING,
                    PREEMPTED,
                    NOT_STARTED
                };

            private:
                std::shared_ptr<Task> _task;
                std::string _uuid;
                std::shared_ptr<JsonValue> _taskStatistics;
                int _numFailures;
                Status _status;
                std::string _startTime;
                std::string _endTime;

            public:
                TaskAttempt() : _task(nullptr),
                                _uuid(""),
                                _taskStatistics(nullptr),
                                _numFailures(0),
                                _status(Status::NOT_STARTED),
                                _startTime(""),
                                _endTime("") {}

                ~TaskAttempt() { dispose(); }
            private:
                void dispose()
                {
                    _task = nullptr;
                    _uuid = "";
                    _taskStatistics = nullptr;
                    _numFailures = 0;
                    _status = Status::NOT_STARTED;
                    _startTime = "";
                    _endTime = "";
                };

                bool init(const std::shared_ptr<Task> task, std::shared_ptr<JsonValue> taskStatistics)
                {
                    _task = task;
                    _uuid = hashtool::generate_uuid();
                    _taskStatistics = taskStatistics;
                    _numFailures = 0;
                    _status = Status::NOT_STARTED;
                    _startTime = "";
                    _endTime = "";
                    return true;
                };
            public:
                static std::shared_ptr<TaskAttempt> alloc(const std::shared_ptr<Task> task, std::shared_ptr<JsonValue> taskStatistics)
                {
                    std::shared_ptr<TaskAttempt> result = std::make_shared<TaskAttempt>();
                    return (result->init(task, taskStatistics) ? result : nullptr);
                }

                std::string  getUUID() const { return _uuid; }

                std::shared_ptr<Task> getTask() const { return _task; }

                bool hasEnded() const { return _status == Status::SUCCEEDED ||
                                               _status == Status::FAILED ||
                                               _status == Status::PREEMPTED; }
                
                int  getNumFailures() const { return _numFailures; }
                void setNumFailures(int numFailures) { _numFailures = numFailures; }

                Status getStatus() const { return _status; }
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
                void setStatus(Status status) { _status = status; }
                std::shared_ptr<JsonValue> getTaskStatistics() const { return _taskStatistics; }
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