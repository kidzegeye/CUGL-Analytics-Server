from analytics_server.analytics_api.models import Organization, Game, User, Session, Task, TaskAttempt, Action
from rest_framework import serializers


class OrganizationSerializer(serializers.ModelSerializer):
    class Meta:
        model = Organization
        fields = ('organization_name',)


class GameSerializer(serializers.ModelSerializer):
    organization = OrganizationSerializer()

    class Meta:
        model = Game
        fields = ('game_name', 'version_number', 'organization')


class GameSerializerSimple(serializers.ModelSerializer):
    class Meta:
        model = Game
        fields = ('game_name', 'version_number')


class UserSerializer(serializers.ModelSerializer):
    game = GameSerializerSimple()

    class Meta:
        model = User
        fields = ('vendor_id', 'platform', 'game')


class UserSerializerSimple(serializers.ModelSerializer):
    class Meta:
        model = User
        fields = ('vendor_id', 'platform')


class SessionSerializer(serializers.ModelSerializer):
    user = UserSerializerSimple()

    class Meta:
        model = Session
        fields = ('user', 'ended', 'started_at', 'ended_at')


class SessionSerializerSimple(serializers.ModelSerializer):
    class Meta:
        model = Session
        fields = ('ended', 'started_at', 'ended_at')


class TaskSerializer(serializers.ModelSerializer):
    game = GameSerializerSimple()

    class Meta:
        model = Task
        fields = ('task_name', 'game')


class TaskSerializerSimple(serializers.ModelSerializer):
    class Meta:
        model = Task
        fields = ('task_name',)


class TaskAttemptSerializer(serializers.ModelSerializer):
    task = TaskSerializerSimple()
    session = SessionSerializerSimple()

    class Meta:
        model = TaskAttempt
        fields = ('task_attempt_uuid', 'task', 'started_at', 'ended_at', 'status', 'num_failures', 'statistics')


class TaskAttemptSerializerSimple(serializers.ModelSerializer):
    class Meta:
        model = TaskAttempt
        fields = ('task_attempt_uuid', 'started_at', 'ended_at', 'status')


class ActionSerializer(serializers.ModelSerializer):
    task_attempts = TaskAttemptSerializerSimple(many=True)
    session = SessionSerializerSimple()

    class Meta:
        model = Action
        fields = ('json_blob', 'timestamp', 'task_attempts', 'session')
