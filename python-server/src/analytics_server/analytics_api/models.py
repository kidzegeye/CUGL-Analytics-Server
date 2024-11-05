from django.db import models
from django.utils.timezone import now
from django.db.models import Q


class GameMetaData(models.Model):
    package_name = models.TextField(unique=True)
    game_name = models.TextField()


class Game(models.Model):
    class Meta:
        db_table = 'analytics_api_game'
        constraints = [
            models.UniqueConstraint(fields=['gamemetadata', 'version_number'], name='unique version')
        ]

    gamemetadata = models.ForeignKey(GameMetaData, on_delete=models.CASCADE)
    version_number = models.TextField()


class User(models.Model):
    class Platform(models.TextChoices):
        IOS = "ios"
        ANDROID = "android"
        OTHER = "other"

    class Meta:
        db_table = 'analytics_api_user'
        constraints = [
            models.UniqueConstraint(fields=['game', 'vendor_id', 'platform'], name='unique user')
        ]

    game = models.ForeignKey(Game, on_delete=models.CASCADE)
    vendor_id = models.TextField()
    platform = models.TextField(choices=Platform.choices)


class Session(models.Model):
    class Meta:
        db_table = 'analytics_api_session'
        constraints = [
            models.UniqueConstraint(fields=['user', 'ended'],
                                    condition=Q(ended=False),
                                    name='user has only one active session at a time')
        ]

    user = models.ForeignKey(User, on_delete=models.CASCADE)
    started_at = models.DateTimeField(default=now)
    ended_at = models.DateTimeField(null=True, blank=True)
    ended = models.BooleanField(default=False)


class Task(models.Model):
    class Meta:
        db_table = 'analytics_api_task'
        constraints = [
            models.UniqueConstraint(fields=['user', 'name'], name='unique task')
        ]

    user = models.ForeignKey(User, on_delete=models.CASCADE)
    name = models.TextField()


class TaskAttempt(models.Model):
    class Status(models.TextChoices):
        SUCCEEDED = "succeeded"
        FAILED = "failed"
        PENDING = "pending"
        NOTSTARTED = "notstarted"
        PREEMPED = "preempted"

    task = models.ForeignKey(Task, on_delete=models.CASCADE)
    session = models.ForeignKey(Session, on_delete=models.CASCADE)
    started_at = models.DateTimeField(default=now)
    ended_at = models.DateTimeField(null=True, blank=True)
    status = models.TextField(choices=Status.choices)
    num_failures = models.IntegerField(default=0)
    statistics = models.JSONField(default=dict)


class Action(models.Model):
    session = models.ForeignKey(Session, on_delete=models.CASCADE)
    task_attempt = models.ForeignKey(TaskAttempt, null=True, blank=True, on_delete=models.CASCADE)
    json_blob = models.JSONField(default=dict)
    timestamp = models.DateTimeField(default=now)
