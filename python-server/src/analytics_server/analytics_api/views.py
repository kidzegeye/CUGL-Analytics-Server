from rest_framework import viewsets
from analytics_server.analytics_api import models, serializers


class GameMetaDataViewSet(viewsets.ModelViewSet):
    """
    API endpoint that allows GameMetaData to be viewed or edited.
    """
    queryset = models.GameMetaData.objects.all()
    serializer_class = serializers.GameMetaDataSerializer


class GameViewSet(viewsets.ModelViewSet):
    """
    API endpoint that allows Games to be viewed or edited.
    """
    queryset = models.Game.objects.all()
    serializer_class = serializers.GameSerializer


class UserViewSet(viewsets.ModelViewSet):
    """
    API endpoint that allows Users to be viewed or edited.
    """
    queryset = models.User.objects.all()
    serializer_class = serializers.UserSerializer


class SessionViewSet(viewsets.ModelViewSet):
    """
    API endpoint that allows Sessions to be viewed or edited.
    """
    queryset = models.Session.objects.all()
    serializer_class = serializers.SessionSerializer


class TaskViewSet(viewsets.ModelViewSet):
    """
    API endpoint that allows Tasks to be viewed or edited.
    """
    queryset = models.Task.objects.all()
    serializer_class = serializers.TaskSerializer


class TaskAttemptViewSet(viewsets.ModelViewSet):
    """
    API endpoint that allows TaskAttempts to be viewed or edited.
    """
    queryset = models.TaskAttempt.objects.all()
    serializer_class = serializers.TaskAttemptSerializer


class ActionViewSet(viewsets.ModelViewSet):
    """
    API endpoint that allows Actions to be viewed or edited.
    """
    queryset = models.Action.objects.all()
    serializer_class = serializers.ActionSerializer
