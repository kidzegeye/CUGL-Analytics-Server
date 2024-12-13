"""
Database models for the analytics server

Each class represents a table in the database with the columns
written as attributes.

Author:  Kidus Zegeye
Version: 12/21/24
"""
from django.db import models
from django.utils.timezone import now
from django.db.models import Q


class Organization(models.Model):
    """
    Represents an organization that creates games, like GDIAC
    """

    class Meta:
        """
        Defines the name of the organization table
        """
        # The name of the organziation table
        db_table = 'analytics_api_organization'

    # The unique name of the organization
    organization_name = models.TextField(unique=True)


class Game(models.Model):
    """
    Represents a game with a specific version number

    It is not possible for two games to have the same name and version for a
    given organization.
    """
    class Meta:
        """
        Defines the name of the game table and data constraints on the table
        """
        # The name of the game table
        db_table = 'analytics_api_game'

        # Constraint that enforces a unique game and version per organization
        constraints = [
            models.UniqueConstraint(fields=['organization', 'game_name', 'version_number'], name='unique game version per organization')
        ]

    # A reference to the organization who developed this game
    organization = models.ForeignKey(Organization, on_delete=models.CASCADE)

    # The name of this game
    game_name = models.TextField()

    # This game's version number
    version_number = models.TextField()


class User(models.Model):
    """
    Represents a user for a specific game

    It is not possible for two users to have the same vendor_id and platform for a
    given game.
    """

    class Meta:
        """
        Defines the name of the user table and data constraints on the table
        """
        # The name of the user table
        db_table = 'analytics_api_user'

        # Constraint that enforces a unique vendor_id and platform per game
        constraints = [
            models.UniqueConstraint(fields=['game', 'vendor_id', 'platform'], name='unique user')
        ]

    # A reference to a game
    game = models.ForeignKey(Game, on_delete=models.CASCADE)

    # The vendor_id that identifies the user's device
    vendor_id = models.TextField()

    # The hardware platform that the user is playing on
    platform = models.TextField()


class Session(models.Model):
    """
    Represents a gameplay session for a given user

    Users can only have one session active at a time. The websocket consumer
    automatically ends sessios upon disconnection.
    """

    class Meta:
        """
        Defines the name of the user table and data constraints on the table
        """
        # The name of the user table
        db_table = 'analytics_api_session'

        # Constraint that enforces one active session per user
        constraints = [
            models.UniqueConstraint(fields=['user', 'ended'],
                                    condition=Q(ended=False),
                                    name='user has only one active session at a time')
        ]

    # A reference to the user that is playing the game
    user = models.ForeignKey(User, on_delete=models.CASCADE)

    # The start time of the session
    started_at = models.DateTimeField(default=now)

    # The end time of the session
    ended_at = models.DateTimeField(null=True, blank=True)

    # If the session has ended or not
    ended = models.BooleanField(default=False)


class Task(models.Model):
    """
    Represents an ingame task for the player to complete

    Tasks are unique by name for a given game.
    """

    class Meta:
        """
        Defines the name of the task table and data constraints on the table
        """
        # The name of the task table
        db_table = 'analytics_api_task'

        # Constraint that enforces a unique task_name for a given game
        constraints = [
            models.UniqueConstraint(fields=['task_name', 'game'], name='unique task')
        ]

    # A reference to the game that the task belongs to
    game = models.ForeignKey(Game, on_delete=models.CASCADE)

    # The name of the task
    task_name = models.TextField(unique=True)


class TaskAttempt(models.Model):
    """
    Represents a player's attempt to complete a specific task

    The websocket consumer automatically preempts all pending task attempts on
    disconnection
    """

    class Meta:
        """
        Defines the name of the task attempts table
        """
        # The name of the task_attempt table
        db_table = 'analytics_api_task_attempt'

    class Status(models.TextChoices):
        """
        An enum representing the current status of a task attempt

        TaskAttempts with a terminal status can not have their status modified
        afterwards. When a session is ended, all pending task attempts become preempted.
        Developers can upload a developer-defined JSON blob for miscellaneous statistics
        collected during a task attempt.
        """

        # Task attempt which has not been started yet
        NOT_STARTED = "not_started"
        # Task attempt which is currently active
        PENDING = "pending"
        # Task attempt where the task was successfully completed (**Terminal**)
        SUCCEEDED = "succeeded"
        # Task attempt where the task has been failed irrevocably (**Terminal**)
        FAILED = "failed"
        # Task attempt where the player's session disconnected midway (**Terminal**)
        PREEMPED = "preempted"

    # A reference to the task being attempted
    task = models.ForeignKey(Task, on_delete=models.CASCADE)

    # A unique id for the task attempt
    task_attempt_uuid = models.TextField(unique=True)

    # A reference to the session where thet task was being attempted
    session = models.ForeignKey(Session, on_delete=models.CASCADE)

    # The time when the task attempt was started (technically when uploaded to the server)
    started_at = models.DateTimeField(default=now)

    # The time when the task attempt was ended (In a terminal state)
    ended_at = models.DateTimeField(null=True, blank=True)

    # The status of the task attempt
    status = models.TextField(choices=Status.choices)

    # The number of non-irrevocable failures during the task attempt
    num_failures = models.IntegerField(default=0)

    # A JSON blob of miscellaneous data recorded during the task attempt
    statistics = models.JSONField(default=dict)


class Action(models.Model):
    """
    Represents player action during gameplay

    The actions are developer defined using JSON blobs. Each action can
    reference many task attempts to give context on the goal of the action.
    """

    class Meta:
        """
        Defines the name of the action table
        """
        # The name of the action table
        db_table = 'analytics_api_action'

    # A reference to the session where the action took place
    session = models.ForeignKey(Session, on_delete=models.CASCADE)

    # References to task attempts that were active when the action took place
    task_attempts = models.ManyToManyField(TaskAttempt, blank=True)

    # A JSON blob of miscellaneous data representing the action a player took
    json_blob = models.JSONField(default=dict)

    # The timestamp when the action was uploaded
    timestamp = models.DateTimeField(default=now)
