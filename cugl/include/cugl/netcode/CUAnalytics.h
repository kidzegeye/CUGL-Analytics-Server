#ifndef __CU_ANALYTICS_H__
#define __CU_ANALYTICS_H__

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <cugl/netcode/CUWebSocket.h>
#include <cugl/core/util/CUHashtools.h>
#include <cugl/netcode/CUNetcodeSerializer.h>
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
                std::string _uuid;

                Task() : _name(""), _uuid("") {}

                ~Task() { dispose(); }

            private:
                void dispose()
                {
                    _name = "";
                    _uuid = "";
                }
                bool init(std::string name)
                {
                    _name = name;
                    _uuid = hashtool::generate_uuid();
                };

            public:
                static std::shared_ptr<Task> alloc(std::string name)
                {
                    std::shared_ptr<Task> result = std::make_shared<Task>();
                    return (result->init(name) ? result : nullptr);
                }

                std::string getName() const { return _name; }
                std::string getUUID() const { return _uuid; }
            };

            class TaskAttempt
            {
            public:
                // Keep a unique ID for the task attempt ID
                enum class Status
                {
                    COMPLETED,
                    FAILED,
                    PENDING,
                    CANCELED,
                    NOT_STARTED
                };

                struct Statistics
                {
                    std::unordered_map<std::string, int> intStatistics;
                    std::unordered_map<std::string, float> floatStatistics;
                    std::unordered_map<std::string, bool> booleanStatistics;
                };

                TaskAttempt() : 
                _task(nullptr),
                _uuid(""),
                _taskStatistics(nullptr),
                _numFailures(0),
                _status(Status::NOT_STARTED),
                _startTime(""),
                _endTime("") {}

                ~TaskAttempt() { dispose(); }
                
            private:
                void dispose() {
                    _task = nullptr;
                    _uuid = "";
                    _taskStatistics = nullptr;
                    _numFailures = 0;
                    _status = Status::NOT_STARTED;
                    _startTime = "";
                    _endTime = "";
                };

                bool init(const std::shared_ptr<Task> task, std::shared_ptr<Statistics> taskStatistics) {
                    _task = task;
                    _uuid = hashtool::generate_uuid();
                    _taskStatistics = taskStatistics;
                    _numFailures = 0;
                    _status = Status::NOT_STARTED;
                    _startTime = "";
                    _endTime = "";

                };

            public:
                static std::shared_ptr<TaskAttempt> alloc(const std::shared_ptr<Task> task, std::shared_ptr<Statistics> taskStatistics)
                {
                    std::shared_ptr<TaskAttempt> result = std::make_shared<TaskAttempt>();
                    return (result->init(task, taskStatistics) ? result : nullptr);
                }

                void setNumFailures(int num);
                bool getIsActive() const;
                std::string getStartTime() const;
                std::string getEndTime() const;
                void setTaskStatus(Status status);
                Status getStatus() const;
                void setStatistics() const;

            private:
                std::shared_ptr<Task> _task;
                std::string _uuid;
                std::shared_ptr<Statistics> _taskStatistics;
                int _numFailures;
                Status _status;
                bool _isActive;
                std::string _startTime;
                std::string _endTime;
            };

            class AnalyticsConnection
            {

#pragma mark Constructors
            public:
                AnalyticsConnection();
                ~AnalyticsConnection();

            private:
                void dispose();
                bool init(const InetAddress &address, bool secure = false); // Make sure you init with our hardcoded path

#pragma mark Communication
            public:
                void open(bool secure = false);

                void close();

            private:
                void send(std::shared_ptr<JsonValue> &data); // This is the helper function to send data

#pragma mark Static Allocators

            public:
                static std::shared_ptr<AnalyticsConnection> alloc(const InetAddress &address)
                {
                    std::shared_ptr<AnalyticsConnection> result = std::make_shared<AnalyticsConnection>();
                    return (result->init(address) ? result : nullptr);
                }

#pragma mark Debugging
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

                void addTask(const std::shared_ptr<Task> &task);
                void addTasks(const std::vector<std::shared_ptr<Task>> &tasks);
                void addTaskAttempt(const std::shared_ptr<TaskAttempt> &TaskAttempt);
                // void endTask(const std::shared_ptr<Task> &task);
                void syncTaskAttempt(const std::shared_ptr<TaskAttempt> &TaskAttempt);
                void recordAction(const std::shared_ptr<JsonValue> &actionBlob); // Add Action Data here

            private:
                std::shared_ptr<WebSocket> _webSocket;
                std::shared_ptr<NetcodeSerializer> _serializer;
                std::shared_ptr<NetcodeDeserializer> _deserializer;
                WebSocket::Dispatcher _dispatcher1;
            };
        }
    } // namespace netcode
} // namespace cugl

#endif // __CU_ANALYTICS_H__