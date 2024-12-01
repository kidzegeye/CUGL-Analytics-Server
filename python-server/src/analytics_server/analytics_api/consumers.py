import json
from analytics_server.analytics_api.models import GameMetaData, Game, User, Session, Task, TaskAttempt, Action
from analytics_server.analytics_api.serializers import GameMetaDataSerializer, GameSerializer, UserSerializer, SessionSerializer, TaskSerializer, TaskAttemptSerializer, ActionSerializer
from channels.generic.websocket import WebsocketConsumer
from django.utils.timezone import now


class MainConsumer(WebsocketConsumer):
    session = None

    def connect(self):
        """
        Starts the websocket connection
        """
        self.accept()
        #session_id = self.scope["url_route"]["kwargs"]["session_id"]

        #self.session = Session.objects.filter(id=session_id, ended=False).first()
        # if not self.session:
        #     self.send(close=True, text_data=json.dumps(
        #         {"error": f"No active sessions for session_id {session_id} found."}))
        #     return

    def disconnect(self, close_code):
        """
        Ends the websocket connection
        """
        if self.session is not None and not self.session.ended:
            self.session.ended = True
            self.session.ended_at = now()
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
        {   "message_type" : <game_meta_data|game|user|session|task|task_attempt|action>
            "message_payload": {
                "task_attempt_id": <(optional) task_attempt_id>,
                "data": {
                    <fields of your choosing>
                 }
            }
            
        }
        """
        payload = json.loads(text_data)
        missing_fields, fields = self.check_fields(payload, ["message_type", "message_payload"])
        if missing_fields:
            self.send(text_data=json.dumps({"error": f"Missing fields: {fields}.\
                                                               Not processing request."}))
            return

        match payload["message_type"]:
            case "game_meta_data":
                self.handle_game_meta_data(payload["message_payload"])
            case "game":
                self.handle_game(payload["message_payload"])
            case "user":
                self.handle_user(payload["message_payload"])
            case "session":
                self.handle_session(payload["message_payload"])
            case "task":
                self.handle_task(payload["message_payload"])
            case "task_attempt":
                self.handle_task_attempt(payload["message_payload"])
            case "sync_task_attempt":
                self.handle_sync_task_attempt(payload["message_payload"])
            case "action":
                self.handle_action(payload["message_payload"])

    def handle_game_meta_data(self, payload):
        missing_fields, fields = self.check_fields(payload, ["package_name", "game_name"])
        if missing_fields:
            self.send(text_data=json.dumps({"error": f"Missing fields: {fields}.\
                                                               Not processing request."}))
            return
        game_meta_data = GameMetaData.objects.create(package_name=payload["package_name"],
                                                     game_name=payload["game_name"]
                                                     )
        serialized_game_meta_data = dict(GameMetaDataSerializer(game_meta_data).data)
        self.send(text_data=json.dumps({"message": "Game Metadata recorded",
                                        "data": serialized_game_meta_data}))

    def handle_game(self, payload):
        missing_fields, fields = self.check_fields(payload, ["gamemetadata", "version_number"])
        if missing_fields:
            self.send(text_data=json.dumps({"error": f"Missing fields: {fields}.\
                                                               Not processing request."}))
            return
        game = Game.objects.create(gamemetadata=payload["gamemetadata"],
                                   version_number=payload["version_number"]
                                   )
        serialized_game = dict(GameSerializer(game).data)
        self.send(text_data=json.dumps({"message": "Game recorded",
                                        "data": serialized_game}))

    def handle_user(self, payload):
        missing_fields, fields = self.check_fields(payload, ["game", "vendor_id", "platform"])
        if missing_fields:
            self.send(text_data=json.dumps({"error": f"Missing fields: {fields}.\
                                                               Not processing request."}))
            return
        if payload["platform"] not in ["ios", "android", "other"]:
            self.send(text_data=json.dumps({"error": f"Invalid platform: {payload["platform"]}.\
                                                               Not processing request."}))
            return
        user = User.objects.create(game=payload["game"],
                                   vendor_id=payload["vendor_id"],
                                   platform=payload["platform"]
                                   )
        serialized_user = dict(UserSerializer(user).data)
        self.send(text_data=json.dumps({"message": "User recorded",
                                        "data": serialized_user}))

    def handle_session(self, payload):
        missing_fields, fields = self.check_fields(payload, ["user"])
        if missing_fields:
            self.send(text_data=json.dumps({"error": f"Missing fields: {fields}.\
                                                               Not processing request."}))
            return
        self.session = Session.objects.create(user=payload["user"])
        serialized_session = dict(SessionSerializer(self.session).data)
        self.send(text_data=json.dumps({"message": "Session started",
                                        "data": serialized_session}))

    def handle_task(self, payload):
        missing_fields, fields = self.check_fields(payload, ["user","name"])
        if missing_fields:
            self.send(text_data=json.dumps({"error": f"Missing fields: {fields}.\
                                                               Not processing request."}))
            return
        task = Task.objects.create(user=payload["user"], name=payload["name"])
        serialized_task = dict(TaskSerializer(task).data)
        self.send(text_data=json.dumps({"message": "Task recorded",
                                        "data": serialized_task}))

    def handle_task_attempt(self, payload):
        missing_fields, fields = self.check_fields(payload, ["task", "session", "statistics"])
        if missing_fields:
            self.send(text_data=json.dumps({"error": f"Missing fields: {fields}.\
                                                               Not processing request."}))
            return
        task_attempt = TaskAttempt.objects.create(task=payload["task"],
                                                  session=payload["session"],
                                                  status="notstarted",
                                                  statistics=payload["statistics"])
        serialized_task_attempt = dict(TaskAttemptSerializer(task_attempt).data)
        self.send(text_data=json.dumps({"message": "Task Attempt recorded",
                                        "data": serialized_task_attempt}))

    def handle_sync_task_attempt(self, payload):
        missing_fields, fields = self.check_fields(payload, ["task_attempt", "status", "statistics", "num_failures"])
        if missing_fields:
            self.send(text_data=json.dumps({"error": f"Missing fields: {fields}.\
                                                               Not processing request."}))
            return

        if payload["status"] not in ["succeeded", "failed", "pending", "notstarted", "preempted"]:
            self.send(text_data=json.dumps({"error": f"Invalid status: {payload["platform"]}.\
                                                               Not processing request."}))
            return

        task_attempt = TaskAttempt.filter(id=payload["task_attempt"]).first()
        if not task_attempt.exists():
            self.send(text_data=json.dumps({"error": f"Task Attempt not found: {payload["task_attempt"]}.\
                                                               Not processing request."}))
            return

        task_attempt.status = payload["status"]
        task_attempt.statistics = payload["statistics"]
        task_attempt.num_failure = payload["num_failures"]
        if payload["status"] in ["succeeded", "failed", "preempted"]:
            task_attempt.ended_at(now())
        task_attempt.save()
        serialized_task_attempt = dict(TaskAttemptSerializer(task_attempt).data)
        self.send(text_data=json.dumps({"message": "Task Attempt synced",
                                        "data": serialized_task_attempt}))

    def handle_action(self, payload):
        if Session.objects.get(id=self.session.id).ended:
            self.send(close=True, text_data=json.dumps({"error": "Session ended unexpectedly.\
                                                               Not processing request."}))
            return

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
                                        "data": serialized_action}))

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
