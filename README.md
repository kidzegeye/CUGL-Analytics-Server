# CUGL-Analytics-Server
CUGL Analytics Server M.Eng project

## Running the python server

* Ensure you have [Python](https://www.python.org/downloads/) and [Pip](https://pip.pypa.io/en/stable/installation/) installed. 
* First install Docker on your computer [here](https://docs.docker.com/engine/install/)
* Then install Docker Compose [here](https://docs.docker.com/compose/install/)
* Enter the python-server directory and create a `.env` file.
  * Fill the `.env` file with the fields detailed in the `.env-template` file.
* It is recommended to create a virtual environment to install dependencies. There are some instructions [here](https://www.arch.jhu.edu/python-virtual-environments/).
* Run `pip install -r requirements.txt` to install the required dependencies.
* Run `docker compose up --build -d` to start the python server and postgres server.
* The server will now be able to accept requests at the host and port specified in the `.env` file.
