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
* Action-recording websocket can be connected to at `ws://<url>/ws/record-action/session/{session_id}/`
  * Connection will only be successful if the session given by `session_id` is active.
  * Sessions will end automatically upon websocket disconnection, a new session will need to be created.
  * Can record Actions via sending messages with the following JSON input format:
    ```json
        {
            "task_attempt_id": "<(optional) task_attempt_id>",
            "data": {
                    "<fields of your choosing>"
                  }
        }
    ```
        