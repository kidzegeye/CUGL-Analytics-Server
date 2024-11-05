import json
from analytics_server.analytics_api.models import Action, Session, TaskAttempt
from analytics_server.analytics_api.serializers import SessionSerializer, ActionSerializer
from channels.generic.websocket import WebsocketConsumer
from datetime import datetime


class RecordActionConsumer(WebsocketConsumer):
    session = None

    def connect(self):
        """
        Starts the websocket connection
        """
        self.accept()
        session_id = self.scope["url_route"]["kwargs"]["session_id"]

        self.session = Session.objects.filter(id=session_id, ended=False).first()
        if not self.session:
            self.send(close=True, text_data=json.dumps(
                {"error": f"No active sessions for session_id {session_id} found."}))
            return

    def disconnect(self, close_code):
        """
        Ends the websocket connection
        """
        self.session.ended = True
        self.session.ended_at = datetime.now()
        self.session.save()

        serialized_session = dict(SessionSerializer(self.session).data)
        self.send(text_data=json.dumps(
                {"message": "Session ended",
                 "session": serialized_session}))
        self.close()
        return

    def receive(self, text_data):
        """
        Receives a message on the websocket for recording an action

        Message format:
        {
            task_attempt_id: [task_attempt_id], (optional)
            data: {
                    [fields of your choosing]
                  }
        }
        """
        if Session.objects.get(id=self.session.id).ended:
            self.send(close=True, text_data=json.dumps({"error": "Session ended unexpectedly.\
                                                               Not processing request."}))
            return

        payload = json.loads(text_data)

        missing_fields, fields = self.check_fields(payload, ["data"])
        if missing_fields:
            self.send(text_data=json.dumps({"error": f"Missing fields: {fields}.\
                                                               Not processing request."}))
            return

        task_attempt = None
        if payload.get("task_attempt_id") is not None:
            task_attempt = TaskAttempt.objects.filter(id=payload.get("task_attempt_id")).first()
            if not task_attempt:
                self.send(text_data=json.dumps(
                    {"error": f"Task attempt with id {payload.get('task_attempt_id')} not found.\
                      Not processing request."}))
                return

        action = Action.objects.create(session=self.session,
                                       task_attempt=task_attempt,
                                       json_blob=payload.get("data")
                                       )
        serialized_action = dict(ActionSerializer(action).data)
        self.send(text_data=json.dumps({"message": "Action recorded",
                                        "action": serialized_action}))

    def check_fields(self, payload, fields):
        fields_missing = []
        missing_fields = False
        output = ""
        for field in fields:
            if payload.get(field) is None:
                missing_fields = True
                fields_missing.append(field)

        if missing_fields:
            output = ", ".join(fields_missing)
        return missing_fields, output
