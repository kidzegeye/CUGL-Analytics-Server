"""
Serializers for each of the models

These are used for taking the objects for an instance of one of these models
and serializing them into a format that is fit to send on a network. Each model
can have multiple serializers that format the data in different ways. Serializers
can also use the serializers of some of the fields that are being serialized. The simple
serializers defined in this file typically don't serialize any foreign fields, as
to prevent deeply nested payloads.

Author:  Kidus Zegeye
Version: 12/21/24
"""
from analytics_server.analytics_api.models import Organization, Game, User, Session, Task, TaskAttempt, Action
from rest_framework import serializers


class OrganizationSerializer(serializers.ModelSerializer):
    """
    Serializer for Organizations
    """
    class Meta:
        model = Organization
        fields = ('organization_name',)


class GameSerializer(serializers.ModelSerializer):
    """
    Serializer for Games
    """
    organization = OrganizationSerializer()

    class Meta:
        model = Game
        fields = ('game_name', 'version_number', 'organization')


class GameSerializerSimple(serializers.ModelSerializer):
    """
    Simplified serializer for Games
    """
    class Meta:
        model = Game
        fields = ('game_name', 'version_number')


class UserSerializer(serializers.ModelSerializer):
    """
    Serializer for Users
    """
    game = GameSerializerSimple()

    class Meta:
        model = User
        fields = ('vendor_id', 'platform', 'game')


class UserSerializerSimple(serializers.ModelSerializer):
    """
    Simplified serializer for Users
    """
    class Meta:
        model = User
        fields = ('vendor_id', 'platform')


class SessionSerializer(serializers.ModelSerializer):
    """
    Serializer for Sessions
    """
    user = UserSerializerSimple()

    class Meta:
        model = Session
        fields = ('user', 'ended', 'started_at', 'ended_at')


class SessionSerializerSimple(serializers.ModelSerializer):
    """
    Simplified serializer for sessions
    """
    class Meta:
        model = Session
        fields = ('ended', 'started_at', 'ended_at')


class TaskSerializer(serializers.ModelSerializer):
    """
    Serializer for Tasks
    """
    game = GameSerializerSimple()

    class Meta:
        model = Task
        fields = ('task_name', 'game')


class TaskSerializerSimple(serializers.ModelSerializer):
    """
    Simplified serializer for Tasks
    """
    class Meta:
        model = Task
        fields = ('task_name',)


class TaskAttemptSerializer(serializers.ModelSerializer):
    """
    Serializer for TaskAttempts
    """
    task = TaskSerializerSimple()
    session = SessionSerializerSimple()

    class Meta:
        model = TaskAttempt
        fields = ('task_attempt_uuid', 'task', 'session', 'started_at', 'ended_at', 'status', 'num_failures', 'statistics')


class TaskAttemptSerializerSimple(serializers.ModelSerializer):
    """
    Simplified Serializer for TaskAttempts

    This serializer doesn't serialize the num_failures or statistics fields along with its foreign fields
    """
    class Meta:
        model = TaskAttempt
        fields = ('task_attempt_uuid', 'started_at', 'ended_at', 'status')


class ActionSerializer(serializers.ModelSerializer):
    """
    Serializer for Actions
    """
    task_attempts = TaskAttemptSerializerSimple(many=True)
    session = SessionSerializerSimple()

    class Meta:
        model = Action
        fields = ('json_blob', 'timestamp', 'task_attempts', 'session')
