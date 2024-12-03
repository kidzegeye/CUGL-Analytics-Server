#ifndef __CU_ANALYTICS_H__
#define __CU_ANALYTICS_H__

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <cugl/netcode/CUWebSocket.h>

namespace cugl
{
    namespace netcode
    {
        namespace analytics
        {

            class User
            {
            public:
                User(const std::string &vendorId);
                std::string getVendorId() const;

            private:
                std::string _vendorId;
            };

            class Task
            {
            public:
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

                Task(const std::string &name);
                Task(const std::string &name, const std::shared_ptr<Statistics> &taskStatistics);

                void setTaskStatus(Status status);
                void setNumFailures(int num);
                int getNumFailures() const;
                bool getIsActive() const;
                std::string getStartTime() const;
                std::string getEndTime() const;
                Status getStatus() const;
                std::shared_ptr<Statistics> getStatistics() const;

            private:
                std::string _name;
                std::shared_ptr<Statistics> _taskStatistics;
                int _numFailures;
                Status _status;
                bool _isActive;
                std::string _startTime;
                std::string _endTime;
            };

            class Session
            {
            public:
                bool isCurrentSession() const;
                void addTask(const std::shared_ptr<Task> &task);
                const std::vector<std::shared_ptr<Task>> &getTasks() const;

            private:
                std::vector<std::shared_ptr<Task>> _tasks;
                bool _isCurrentSession;
            };

            class AnalyticsConnection
            {
            public:
                AnalyticsConnection(const std::string &url); // Get all of the info for vendor id and user detail
                void init();                                 // This will have the connect part as well.
                // void connect(); // Connect to the WebSocket
                std::shared_ptr<User> getUser();
                // void startSession();
                // void endSession();
                // std::shared_ptr<Session> getCurrentSession();
                // std::shared_ptr<Session> getLastEndedSession();
                void defineTask(const std::shared_ptr<Task> &task);
                void addTaskAttempt(const std::shared_ptr<Task> &task);
                // void endTask(const std::shared_ptr<Task> &task);
                void syncTaskAttempt(const std::shared_ptr<Task> &task);
                void recordAction(const std::shared_ptr<Task> &task); // Add Action Data here

            private:
                std::shared_ptr<WebSocket> _webSocket;
                std::shared_ptr<User> _user;
            };
        }
    } // namespace netcode
} // namespace cugl

#endif // __CU_ANALYTICS_H__