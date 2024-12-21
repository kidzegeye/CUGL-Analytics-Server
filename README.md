# CUGL-Analytics-Server
CUGL Analytics Server M.Eng project

## Running the python server

* Ensure you have [Python](https://www.python.org/downloads/) and [Pip](https://pip.pypa.io/en/stable/installation/) installed. 
* First install [Docker](https://docs.docker.com/engine/install/) on your computer.
* Then install [Docker Compose](https://docs.docker.com/compose/install/).
* Enter the python-server directory and create a `.env` file.
  * Fill the `.env` file with the fields detailed in the `.env-template` file.
* It is recommended to create a virtual environment to install dependencies. There are some instructions [here](https://www.arch.jhu.edu/python-virtual-environments/).
* Run `pip install -r requirements.txt` to install the required dependencies.
* Run `docker compose up --build -d` to start the python server and postgres server.
* The server will now be able to accept requests at the host and port specified in the `.env` file.

## Documentation

* REST Api documentation can be found at `http://<url>/api/schema/swagger-ui/`
* The main websocket can be connected to at the base endpoint `ws://<url>/`
  * Sessions will end automatically upon websocket disconnection.
  * Messages sent can be received in text or bytes form. 
  * Nessages sent should be formatted with the following fields, along with specific fields for
    the chosen message type.
  * Message format:
    ```json
    {
        "message_type" : <init|task|task_attempt|sync_task_attempt|action>,
        "message_payload": {
            <message_type specific fields>
        }
    }
    ```
  * To learn more about the message_type specific fields, they are detailed in
    [./python-server/src/analytics_server/analytics_api/consumers.py](./python-server/src/analytics_server/analytics_api/consumers.py) under each of their handler functions.