FROM python:3.9

COPY requirements.txt ./requirements.txt


RUN pip install -r requirements.txt

COPY /CUGL-Analytics-Server/ /CUGL-Analytics-Server


WORKDIR /CUGL-Analytics-Server

CMD ["python", "main.py", "runserver", "0.0.0.0:8000"]