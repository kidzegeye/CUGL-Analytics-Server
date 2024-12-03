#ifndef __CU_ANALYTICS_H__
#define __CU_ANALYTICS_H__

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <cugl/netcode/CUWebSocket.h>

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
                std::string name;
                // Make a UUID here that will be the task ID
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

                void setTaskStatus(Status status);

                struct Statistics
                {
                    std::unordered_map<std::string, int> intStatistics;
                    std::unordered_map<std::string, float> floatStatistics;
                    std::unordered_map<std::string, bool> booleanStatistics;
                };

                TaskAttempt(const std::shared_ptr<Task> task);

                void setNumFailures(int num);
                int getNumFailures() const;
                bool getIsActive() const;
                std::string getStartTime() const;
                std::string getEndTime() const;
                Status getStatus() const;
                void setStatistics() const;

            private:
                std::string _name;
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
                bool init(const InetAddress &address); // Make sure you init with our hardcoded path

#pragma mark Communication
            public:
                void open(bool secure = false);

                void close();

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
                void recordAction(const cugl::JsonValue actionBlob); // Add Action Data here

            private:
                std::shared_ptr<WebSocket> _webSocket;
            };
        }
    } // namespace netcode
} // namespace cugl

#endif // __CU_ANALYTICS_H__