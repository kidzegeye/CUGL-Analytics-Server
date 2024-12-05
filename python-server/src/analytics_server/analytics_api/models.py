from django.db import models
from django.utils.timezone import now
from django.db.models import Q


class Organization(models.Model):
    organization_name = models.TextField(unique=True)


class Game(models.Model):
    class Meta:
        db_table = 'analytics_api_game'
        constraints = [
            models.UniqueConstraint(fields=['organization', 'game_name', 'version_number'], name='unique game version per organization')
        ]

    organization = models.ForeignKey(Organization, on_delete=models.CASCADE)
    game_name = models.TextField()
    version_number = models.TextField()


class User(models.Model):

    class Meta:
        db_table = 'analytics_api_user'
        constraints = [
            models.UniqueConstraint(fields=['game', 'vendor_id', 'platform'], name='unique user')
        ]

    game = models.ForeignKey(Game, on_delete=models.CASCADE)
    vendor_id = models.TextField()
    platform = models.TextField()


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
            models.UniqueConstraint(fields=['user', 'task_name'], name='unique task')
        ]

    user = models.ForeignKey(User, on_delete=models.CASCADE)
    task_uuid = models.TextField(unique=True)
    task_name = models.TextField()


class TaskAttempt(models.Model):
    class Status(models.TextChoices):
        SUCCEEDED = "succeeded"
        FAILED = "failed"
        PENDING = "pending"
        NOT_STARTED = "not_started"
        PREEMPED = "preempted"

    task = models.ForeignKey(Task, on_delete=models.CASCADE)
    task_attempt_uuid = models.TextField()
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
