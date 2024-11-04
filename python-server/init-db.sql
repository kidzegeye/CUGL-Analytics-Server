CREATE TABLE IF NOT EXISTS GameMetaData(
package_name TEXT PRIMARY KEY,
game_name TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS Game(
game_version_id SERIAL PRIMARY KEY,
package_name TEXT NOT NULL REFERENCES GameMetaData(package_name),
version_number INT NOT NULL
);

CREATE TYPE PLATFORM AS ENUM ('IOS', 'Android', 'Other');

CREATE TABLE IF NOT EXISTS User(
user_id SERIAL PRIMARY KEY,
game_version_id INT NOT NULL REFERENCES Game(game_version_id),
vendor_id TEXT NOT NULL,
platform PLATFORM NOT NULL
);

CREATE TABLE IF NOT EXISTS Session(
session_id SERIAL PRIMARY KEY,
user_id INT NOT NULL REFERENCES User(user_id),
started_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
ended_at TIMESTAMP,
ended BOOLEAN NOT NULL
);

CREATE TABLE IF NOT EXISTS Task(
task_id SERIAL PRIMARY KEY,
user_id INT NOT NULL REFERENCES User(user_id),
name TEXT NOT NULL
);

CREATE TYPE TASK_STATUS AS ENUM ('Succeeded', 'Failed', 'Pending','NotStarted', 'Preempted');

CREATE TABLE IF NOT EXISTS TaskAttempts(
task_attempts_id SERIAL PRIMARY KEY,
task_id INT NOT NULL REFERENCES Task(task_id),
session_id INT NOT NULL REFERENCES Session(session_id),
status TASK_STATUS NOT NULL,
started_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
ended_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
num_failures INT NOT NULL DEFAULT 0,
statistics JSONB NOT NULL DEFAULT {} 
);

CREATE TABLE IF NOT EXISTS Actions(
action_id SERIAL PRIMARY KEY,
task_id INT NULL REFERENCES Task(task_id),
session_id INT NOT NULL REFERENCES Session(session_id),
json_blob JSONB NOT NULL,
timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
);
