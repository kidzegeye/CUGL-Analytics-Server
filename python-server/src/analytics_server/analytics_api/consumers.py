"""
Consumer methods to implement the websocket connection

Handles connecting, disconnecting, and receiving messages

Author:  Kidus Zegeye
Version: 12/21/24
"""
import json
from analytics_server.analytics_api.models import Organization, Game, User, Session, Task, TaskAttempt, Action
from analytics_server.analytics_api.serializers import OrganizationSerializer, GameSerializer, UserSerializer, SessionSerializer, TaskSerializer, TaskAttemptSerializer, ActionSerializer
from channels.generic.websocket import WebsocketConsumer
from django.utils.timezone import now
import logging

# Logger to log debug messages to the console
logger = logging.getLogger('django')


class MainConsumer(WebsocketConsumer):
    """
    The main consumer class that handles all websocket connections
    for the analytics server
    """
    def connect(self):
        """
        Starts the websocket connection. The connection is persistent
        until disconnected by the client or this server
        """
        self.accept()
        self.session = None

    def disconnect(self, close_code):
        """
        Ends the websocket connection.

        This ends the current session and any `pending` task attempts.
        Pending task attempts are given the `preempted` status

        :param close_code:  The websocket close code
        :type close_code:   ``int``
        """
        if self.session is not None and not self.session.ended:
            # End active task attempts. 
            TaskAttempt.objects.filter(session=self.session, status=TaskAttempt.Status.PENDING).update(status=TaskAttempt.Status.PREEMPED)

            self.session.ended = True
            self.session.ended_at = now()
            self.session.save()

            serialized_session = dict(SessionSerializer(self.session).data)
            self.send_formatted(text_data=json.dumps(
                    {"message": "Session ended",
                     "session": serialized_session}))
        self.close()
        return

    def receive(self, text_data=None, bytes_data=None):
        """
        Receives a message sent to the websocket

        Messages can be received in text or bytes form. The message should be
        formatted with the following fields, along with specific fields for
        the chosen message type which is defined in later functions.

        Message format:
        {
            "message_type" : <init|task|task_attempt|sync_task_attempt|action>,
            "message_payload": {
                <message_type specific fields>
            }
        }

        :param text_data:    Received data in text form
        :type text_data:     ``string``

        :param bytes_data:   Received data in bytes form
        :type bytes_data:    ``bytes``
        """
        if text_data:
            self.is_bytes = False
            payload = json.loads(text_data)
        else:
            self.is_bytes = True
            payload = json.loads(bytes_data.decode("utf-8").strip())
        logger.info(payload)
        missing_fields, fields = self.check_fields(payload, ["message_type", "message_payload"])
        if missing_fields:
            self.send_formatted(text_data=json.dumps({"error": f"Missing fields: {fields}.\
                                                               Not processing request."}))
            return

        match payload["message_type"]:
            case "init":
                self.handle_init(payload["message_payload"])
            case "task":
                self.handle_task(payload["message_payload"])
            case "task_attempt":
                self.handle_task_attempt(payload["message_payload"])
            case "sync_task_attempt":
                self.handle_sync_task_attempt(payload["message_payload"])
            case "action":
                self.handle_action(payload["message_payload"])

    def handle_init(self, payload):
        """
        Helper function for handling `init` messages

        These messages initialize the organization, game, and user
        in the database. It is fully idempotent, so it is safe to
        send this message on every game startup.

        Payload format:
        {
            "organization_name": str,
            "game_name": str,
            "verison_number": str,
            "vendor_id": str,
            "platform": str
        }

        :param payload:     Dictionary with init-specific fields
        :type payload:      ``dict``
        """
        missing_fields, fields = self.check_fields(payload, ["organization_name", "game_name", "version_number", "vendor_id", "platform"])
        if missing_fields:
            self.send_formatted(text_data=json.dumps({"error": f"Missing fields: {fields}.\
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

        temp_session = Session.objects.filter(user=user, ended=False).last()
        if temp_session is not None:
            self.session = temp_session
        else:
            self.session = Session.objects.create(user=user)
        serialized_session = dict(SessionSerializer(self.session).data)

        self.send_formatted(text_data=json.dumps({"message": "Init recorded",
                                                  "data": {
                                                        "organization": serialized_organization,
                                                        "game": serialized_game,
                                                        "user": serialized_user,
                                                        "session": serialized_session
                                                        }
                                                  }))

    def handle_task(self, payload):
        """
        Helper function for handling `task` messages

        These messages add a new task into the database.
        It is fully idempotent, so it is safe to send this message on every game startup.

        Payload format:
        {
            "organization_name": str,
            "game_name": str,
            "verison_number": str,
            "vendor_id": str,
            "platform": str,
            "task_name": str
        }

        :param payload:     Dictionary with task-specific fields
        :type payload:      ``dict``
        """
        missing_fields, fields = self.check_fields(payload, ["organization_name", "game_name", "version_number", "vendor_id", "platform", "task_name"])
        if missing_fields:
            self.send_formatted(text_data=json.dumps({"error": f"Missing fields: {fields}.\
                                                               Not processing request."}))
            return

        success, val = self.get_org_game_user(payload)
        if not success:
            return
        organization, game, user = val
        task, _ = Task.objects.get_or_create(task_name=payload["task_name"], game=game)
        serialized_task = dict(TaskSerializer(task).data)
        self.send_formatted(text_data=json.dumps({"message": "Task recorded",
                                                  "data": serialized_task}))

    def handle_task_attempt(self, payload):
        """
        Helper function for handling `task_attempt` messages

        These messages add a new task_attempt into the database. Each task_attempt
        includes a statistics field which is a json object. These are miscellaneous
        aggregate data for a task_attempt, and can have their defaults sent here.

        Payload format:
        {
            "organization_name": str,
            "game_name": str,
            "verison_number": str,
            "vendor_id": str,
            "platform": str,
            "task_name": str,
            "task_attempt_uuid": str,
            "status": <"not_started" | "pending" | "suceeded" | "failed" | "preempted">,
            "statistics": <json_blob>,
            "num_failures": int
        }

        :param payload:     Dictionary with task_attempt-specific fields
        :type payload:      ``dict``
        """
        missing_fields, fields = self.check_fields(payload, ["organization_name", "game_name", "version_number", "vendor_id", "platform", "task_name", "task_attempt_uuid", "status", "statistics", "num_failures"])
        if missing_fields:
            self.send_formatted(text_data=json.dumps({"error": f"Missing fields: {fields}.\
                                                               Not processing request."}))
            return

        success, val = self.get_org_game_user(payload)
        if not success:
            return
        organization, game, user = val

        task = Task.objects.filter(task_name=payload["task_name"], game=game).first()
        if not task:
            self.send_formatted(text_data=json.dumps({"error": f"Task does not exist: '{payload['task_name']}'.\
                                                               Not processing request."}))
            return

        if not any(payload["status"] in choice for choice in TaskAttempt.Status.choices):
            self.send_formatted(text_data=json.dumps({"error": f"Invalid status: '{payload['status']}'.\
                                                       Must be one of {TaskAttempt.Status.choices}\
                                                               Not processing request."}))
            return

        temp_session = Session.objects.filter(user=user, ended=False).last()
        if temp_session is not None:
            self.session = temp_session
        else:
            self.session = Session.objects.create(user=user)
        task_attempt, _ = TaskAttempt.objects.get_or_create(
                                                        task=task,
                                                        task_attempt_uuid=payload["task_attempt_uuid"],
                                                        session=self.session,
                                                        status=payload["status"],
                                                        num_failures=payload["num_failures"],
                                                        statistics=payload["statistics"])
        serialized_task_attempt = dict(TaskAttemptSerializer(task_attempt).data)
        self.send_formatted(text_data=json.dumps({"message": "Task Attempt recorded",
                                                  "data": serialized_task_attempt}))

    def handle_sync_task_attempt(self, payload):
        """
        Helper function for handling `sync_task_attempt` messages

        These messages update an existing task_attempt in the database. Does not 
        allow updating the state if the state is terminal. This method is useful
        for updating aggregate statistics, num_failures, and task_attempt states.

        Payload format:
        {
            "task_attempt_uuid": str,
            "status": <"not_started" | "pending" | "suceeded" | "failed" | "preempted">,
            "statistics": <json_blob>,
            "num_failures": int
        }

        :param payload:     Dictionary with task_attempt-specific fields
        :type payload:      ``dict``
        """
        missing_fields, fields = self.check_fields(payload, ["task_attempt_uuid", "status", "statistics", "num_failures"])
        if missing_fields:
            self.send_formatted(text_data=json.dumps({"error": f"Missing fields: {fields}.\
                                                               Not processing request."}))
            return

        if not any(payload["status"] in choice for choice in TaskAttempt.Status.choices):
            self.send_formatted(text_data=json.dumps({"error": f"Invalid status: '{payload['status']}'.\
                                                       Must be one of {TaskAttempt.Status.choices}\
                                                               Not processing request."}))
            return

        task_attempt = TaskAttempt.objects.filter(task_attempt_uuid=payload["task_attempt_uuid"]).first()
        if task_attempt is None:
            self.send_formatted(text_data=json.dumps({"error": f"Task Attempt not found: {payload['task_attempt_uuid']}.\
                                                               Not processing request."}))
            return
        if task_attempt.status in ["succeeded", "failed", "preempted"] and task_attempt.status != payload["status"]:
            self.send_formatted(text_data=json.dumps({"error": f"Can not change already ended Task Attempt status: {task_attempt.status} != {payload['status']}.\
                                                               Not processing request."}))
            return

        task_attempt.status = payload["status"]
        task_attempt.statistics = payload["statistics"]
        task_attempt.num_failures = payload["num_failures"]
        if payload["status"] in ["succeeded", "failed", "preempted"]:
            task_attempt.ended_at = now()
        task_attempt.save()
        serialized_task_attempt = dict(TaskAttemptSerializer(task_attempt).data)
        self.send_formatted(text_data=json.dumps({"message": "Task Attempt synced",
                                                  "data": serialized_task_attempt}))

    def handle_action(self, payload):
        """
        Helper function for handling `action` messages

        These messages add a new action into the database. Each action
        comes with a json object in its data field which details what the 
        action was. Each action can optionally include a list of task_attempt_uuids
        to add context on what the action's goals were.

        Payload format:
        {
            "organization_name": str,
            "game_name": str,
            "verison_number": str,
            "vendor_id": str,
            "platform": str,
            "data": <json_blob>,
            "task_attempt_uuids": [str]
        }

        :param payload:     Dictionary with action-specific fields
        :type payload:      ``dict``
        """
        missing_fields, fields = self.check_fields(payload, ["organization_name", "game_name", "version_number", "vendor_id", "platform", "data", "task_attempt_uuids"])
        if missing_fields:
            self.send_formatted(text_data=json.dumps({"error": f"Missing fields: {fields}.\
                                                               Not processing request."}))
            return

        success, val = self.get_org_game_user(payload)
        if not success:
            return
        organization, game, user = val

        temp_session = Session.objects.filter(user=user, ended=False).last()
        if temp_session is not None:
            self.session = temp_session
        else:
            self.session = Session.objects.create(user=user)

        action = Action.objects.create(session=self.session,
                                       json_blob=payload["data"]
                                       )
        task_attempt_uuids = TaskAttempt.objects.filter(task_attempt_uuid__in=payload["task_attempt_uuids"])
        if task_attempt_uuids:
            action.task_attempts.add(*task_attempt_uuids)

        serialized_action = dict(ActionSerializer(action).data)
        self.send_formatted(text_data=json.dumps({"message": "Action recorded", "data": serialized_action}))

    def check_fields(self, payload, fields):
        """
        Checks that the given payload has all of the required fields

        :param payload:     Dictionary received from the websocket
        :type payload:      ``dict``

        :param fields:     List of required fields
        :type fields:      ``list`` of ``str``

        :return:           Pair of whether the payload was missing fields and the specific missing fields
        :rtype:            ``bool``,``str``
        """
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

    def send_formatted(self, text_data, close=False):
        """
        Sends textual data to a client in bytes form

        :param text_data:   Textual data to be sent
        :type text_data:    ``str``

        :param close:       Whether to close the connection after sending
        :type close:        ``bool``
        """
        if self.is_bytes:
            bytes_data = text_data.encode("utf-8")
            self.send(bytes_data=bytes_data, close=close)
        else:
            self.send(text_data=text_data, close=close)

    def get_org_game_user(self, payload):
        """
        Retrieves the organization, game, and user from a payload. Abstracted
        from the above message functions because its used so much. Payload
        must contain the keys 'organization_name', 'version_number',
        'game_name', 'vendor_id', and 'platform'.

        :param payload:     Dictionary received from the websocket
        :type payload:      ``dict``

        :return:            Pair of whether the retrieval was successful and a triple of the organization, game, and user
        :rtype:             ``bool``,``triple`` of ``Organization``, ``Game``, ``User``
        """
        organization = Organization.objects.filter(organization_name=payload["organization_name"]).first()
        if not organization:
            self.send_formatted(text_data=json.dumps({"error": f"Game Meta Data does not exist: '{payload['organization_name']}'.\
                                                               Not processing request."}))
            return False, None
        game = Game.objects.filter(organization=organization, game_name=payload["game_name"], version_number=payload["version_number"]).first()
        if not game:
            self.send_formatted(text_data=json.dumps({"error": f"Game does not exist: '{payload['game_name']} version {payload['version_number']}'.\
                                                               Not processing request."}))
            return False, None
        user = User.objects.filter(game=game, vendor_id=payload["vendor_id"], platform=payload["platform"]).first()
        if not user:
            self.send_formatted(text_data=json.dumps({"error": f"User does not exist: '{payload['vendor_id']} on {payload['platform']} for {payload['game_name']} version {payload['version_number']}'.\
                                                               Not processing request."}))
            return False, None
        return True, (organization, game, user)
