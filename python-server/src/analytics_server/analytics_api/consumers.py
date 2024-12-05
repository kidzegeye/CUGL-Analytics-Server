import json
from analytics_server.analytics_api.models import Organization, Game, User, Session, Task, TaskAttempt, Action
from analytics_server.analytics_api.serializers import OrganizationSerializer, GameSerializer, UserSerializer, SessionSerializer, TaskSerializer, TaskAttemptSerializer, ActionSerializer
from channels.generic.websocket import WebsocketConsumer
from django.utils.timezone import now


class MainConsumer(WebsocketConsumer):

    def connect(self):
        """
        Starts the websocket connection
        """
        self.accept()
        self.session = None

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

    def receive(self, bytes_data):
        pass
    
    def receive(self, text_data):
        """
        Receives a message on the websocket for recording an action

        Message format:
        {   "message_type" : <init|session|task|task_attempt|sync_task_attempt|action>
            "message_payload": {
                <message_type specific fields>
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
            case "init":
                self.handle_init(payload["message_payload"])
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

    def handle_init(self, payload):
        missing_fields, fields = self.check_fields(payload, ["organization_name", "game_name", "version_number", "vendor_id", "platform"])
        if missing_fields:
            self.send(text_data=json.dumps({"error": f"Missing fields: {fields}.\
                                                               Not processing request."}))
            return

        organization, _ = Organization.objects.get_or_create(organization_name=payload["organization_name"]
                                                             )
        serialized_organization = dict(OrganizationSerializer(organization).data)

        game, _ = Game.objects.get_or_create(organization=organization,
                                             game_name=payload["game_name"],
                                             version_number=payload["version_number"]
                                             )
        serialized_game = dict(GameSerializer(game).data)

        user, _ = User.objects.get_or_create(game=game,
                                             vendor_id=payload["vendor_id"],
                                             platform=payload["platform"]
                                             )
        serialized_user = dict(UserSerializer(user).data)
        self.send(text_data=json.dumps({"message": "Init recorded",
                                        "data": {
                                            "organization": serialized_organization,
                                            "game": serialized_game,
                                            "user": serialized_user
                                            }
                                        }))

    def handle_session(self, payload):
        missing_fields, fields = self.check_fields(payload, ["organization_name", "game_name", "version_number", "vendor_id", "platform"])
        if missing_fields:
            self.send(text_data=json.dumps({"error": f"Missing fields: {fields}.\
                                                               Not processing request."}))
            return
        
        organization = Organization.objects.filter(organization_name=payload["organization_name"]).first()
        if not organization:
            self.send(text_data=json.dumps({"error": f"Game Meta Data does not exist: '{payload['package_name']}'.\
                                                               Not processing request."}))
            return
        game = Game.objects.filter(organization=organization, game_name=payload["game_name"], version_number=payload["version_number"]).first()
        if not game:
            self.send(text_data=json.dumps({"error": f"Game does not exist: '{payload['package_name']} version {payload['version_number']}'.\
                                                               Not processing request."}))
            return
        user = User.objects.filter(game=game, vendor_id=payload["vendor_id"], platform=payload["platform"]).first()
        if not user:
            self.send(text_data=json.dumps({"error": f"User does not exist: '{payload['vendor_id']} on {payload['platform']} for {payload['package_name']} version {payload['version_number']}'.\
                                                               Not processing request."}))
            return
        temp_session = Session.objects.filter(user=user, ended=False).last()
        if temp_session is not None:
            self.session = temp_session
        self.session = Session.objects.create(user=user)
        serialized_session = dict(SessionSerializer(self.session).data)
        self.send(text_data=json.dumps({"message": "Session started",
                                        "data": serialized_session}))

    def handle_task(self, payload):
        missing_fields, fields = self.check_fields(payload, ["organization_name","game_name", "version_number", "vendor_id", "platform", "task_uuid", "task_name"])
        if missing_fields:
            self.send(text_data=json.dumps({"error": f"Missing fields: {fields}.\
                                                               Not processing request."}))
            return

        organization = Organization.objects.filter(organization_name=payload["organization_name"]).first()
        if not organization:
            self.send(text_data=json.dumps({"error": f"Game Meta Data does not exist: '{payload['organization_name']}'.\
                                                               Not processing request."}))
            return
        game = Game.objects.filter(organization=organization, game_name=payload["game_name"], version_number=payload["version_number"]).first()
        if not game:
            self.send(text_data=json.dumps({"error": f"Game does not exist: '{payload['package_name']} version {payload['version_number']}'.\
                                                               Not processing request."}))
            return
        user = User.objects.filter(game=game, vendor_id=payload["vendor_id"], platform=payload["platform"]).first()
        if not user:
            self.send(text_data=json.dumps({"error": f"User does not exist: '{payload['vendor_id']} on {payload['platform']} for {payload['package_name']} version {payload['version_number']}'.\
                                                               Not processing request."}))
            return
        task, _ = Task.objects.get_or_create(user=user, task_uuid=payload["task_uuid"], task_name=payload["task_name"])
        serialized_task = dict(TaskSerializer(task).data)
        self.send(text_data=json.dumps({"message": "Task recorded",
                                        "data": serialized_task}))

    def handle_task_attempt(self, payload):
        missing_fields, fields = self.check_fields(payload, ["task_uuid", "task_attempt_uuid", "statistics"])
        if missing_fields:
            self.send(text_data=json.dumps({"error": f"Missing fields: {fields}.\
                                                               Not processing request."}))
            return
        task = Task.objects.filter(task_uuid=payload["task_uuid"]).first()
        if not task:
            self.send(text_data=json.dumps({"error": f"Task does not exist: '{payload['task_uuid']}'.\
                                                               Not processing request."}))
            return
        session = Session.objects.filter(user=task.user, ended=False).last()
        if not session:
            self.send(text_data=json.dumps({"error": "Active session does not exist. Not processing request."}))
            return
        task_attempt, _ = TaskAttempt.objects.get_or_create(
                                                        task=task,
                                                        task_attempt_uuid=payload["task_attempt_uuid"],
                                                        session=session,
                                                        status="not_started",
                                                        statistics=payload["statistics"])
        serialized_task_attempt = dict(TaskAttemptSerializer(task_attempt).data)
        self.send(text_data=json.dumps({"message": "Task Attempt recorded",
                                        "data": serialized_task_attempt}))

    def handle_sync_task_attempt(self, payload):
        missing_fields, fields = self.check_fields(payload, ["task_attempt_uuid", "status", "statistics", "num_failures"])
        if missing_fields:
            self.send(text_data=json.dumps({"error": f"Missing fields: {fields}.\
                                                               Not processing request."}))
            return

        if not any(payload["status"] in choice for choice in TaskAttempt.Status.choices):
            self.send(text_data=json.dumps({"error": f"Invalid status: '{payload['status']}'.\
                                                       Must be one of {TaskAttempt.Status.choices}\
                                                               Not processing request."}))
            return

        task_attempt = TaskAttempt.objects.filter(task_attempt_uuid=payload["task_attempt_uuid"]).first()
        if task_attempt is None:
            self.send(text_data=json.dumps({"error": f"Task Attempt not found: {payload['task_attempt_uuid']}.\
                                                               Not processing request."}))
            return
        if task_attempt.status in ["succeeded", "failed", "preempted"] and task_attempt.status != payload["status"]:
            self.send(text_data=json.dumps({"error": f"Can not change already ended Task Attempt status: {task_attempt.status} != {payload['status']}.\
                                                               Not processing request."}))
            return

        task_attempt.status = payload["status"]
        task_attempt.statistics = payload["statistics"]
        task_attempt.num_failures = payload["num_failures"]
        if payload["status"] in ["succeeded", "failed", "preempted"]:
            task_attempt.ended_at = now()
        task_attempt.save()
        serialized_task_attempt = dict(TaskAttemptSerializer(task_attempt).data)
        self.send(text_data=json.dumps({"message": "Task Attempt synced",
                                        "data": serialized_task_attempt}))

    def handle_action(self, payload):
        if self.session is None:
            self.send(close=True, text_data=json.dumps({"error": "No active session found.\
                                                               Not processing request."}))
            return
        if Session.objects.get(id=self.session.id).ended:
            self.send(close=True, text_data=json.dumps({"error": "Latest session is not active, please start a new one.\
                                                               Not processing request."}))
            return

        missing_fields, fields = self.check_fields(payload, ["data"])
        if missing_fields:
            self.send(text_data=json.dumps({"error": f"Missing fields: {fields}.\
                                                               Not processing request."}))
            return

        task_attempt = None
        if payload.get("task_attempt_uuid") is not None:
            task_attempt = TaskAttempt.objects.filter(task_attempt_uuid=payload["task_attempt_uuid"]).first()
            if not task_attempt:
                self.send(text_data=json.dumps(
                    {"error": f"Task attempt with id '{payload['task_attempt_uuid']}' not found.\
                      Not processing request."}))
                return

        action = Action.objects.create(session=self.session,
                                       task_attempt=task_attempt,
                                       json_blob=payload["data"]
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
