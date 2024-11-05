from django.db import models
from django.utils.timezone import now


class GameMetaData(models.Model):
    package_name = models.TextField(unique=True)
    game_name = models.TextField()


class Game(models.Model):
    gamemetadata = models.ForeignKey(GameMetaData, on_delete=models.CASCADE)
    version_number = models.TextField()


class User(models.Model):
    class Platform(models.TextChoices):
        IOS = "iOS"
        ANDROID = "Android"
        OTHER = "Other"

    game = models.ForeignKey(Game, on_delete=models.CASCADE)
    vendor_id = models.TextField()
    platform = models.TextField(choices=Platform.choices)


class Session(models.Model):
    user = models.ForeignKey(User, on_delete=models.CASCADE)
    started_at = models.DateTimeField(default=now)
    ended_at = models.DateTimeField()
    ended = models.BooleanField()


class Task(models.Model):
    user = models.ForeignKey(User, on_delete=models.CASCADE)
    name = models.TextField()


class TaskAttempt(models.Model):
    class Status(models.TextChoices):
        SUCCEEDED = "Succeeded"
        FAILED = "Failed"
        PENDING = "Pending"
        NOTSTARTED = "NotStarted"
        PREEMPED = "Preempted"

    session = models.ForeignKey(Session, on_delete=models.CASCADE)
    started_at = models.DateTimeField(default=now)
    ended_at = models.DateTimeField()
    status = models.TextField(choices=Status.choices)
    num_failures = models.IntegerField(default=0)
    statistics = models.JSONField(default=dict)


class Action(models.Model):
    task = models.ForeignKey(Task, on_delete=models.CASCADE)
    session = models.ForeignKey(Session, on_delete=models.CASCADE)
    json_blob = models.JSONField(default=dict)
    timestamp = models.DateTimeField(default=now)
