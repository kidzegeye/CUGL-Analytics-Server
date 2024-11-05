from analytics_server.analytics_api.models import GameMetaData, Game, User, Session, Task, TaskAttempt, Action
from rest_framework import serializers


class GameMetaDataSerializer(serializers.HyperlinkedModelSerializer):
    class Meta:
        model = GameMetaData
        fields = '__all__'


class GameSerializer(serializers.HyperlinkedModelSerializer):
    class Meta:
        model = Game
        fields = '__all__'


class UserSerializer(serializers.HyperlinkedModelSerializer):
    class Meta:
        model = User
        fields = '__all__'


class SessionSerializer(serializers.HyperlinkedModelSerializer):
    class Meta:
        model = Session
        fields = '__all__'


class TaskSerializer(serializers.HyperlinkedModelSerializer):
    class Meta:
        model = Task
        fields = '__all__'


class TaskAttemptSerializer(serializers.HyperlinkedModelSerializer):
    class Meta:
        model = TaskAttempt
        fields = '__all__'


class ActionSerializer(serializers.HyperlinkedModelSerializer):
    class Meta:
        model = Action
        fields = '__all__'