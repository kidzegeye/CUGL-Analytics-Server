import enum
from sqlalchemy.dialects.postgresql import JSONB
from sqlalchemy import Column, Integer, Enum, DateTime
from sqlalchemy.orm import relationship
from sqlalchemy.orm import DeclarativeBase
from sqlalchemy.sql.functions import current_timestamp


class Base(DeclarativeBase):
    pass


class Status(enum.Enum):
    succeeded = 1
    failed = 2
    pending = 3
    not_started = 4
    preempted = 5
    other = 6


class TaskAttempt(Base):
    """
    Data for a Task Attempt. A Task Attempt references a 
    Task and a Session. A specific Task Attempt can be played multiple
    times during a session.

    Attributes:
        - `task_attempts_id`    The id of the Task Attempt.
        - `task_id`             Reference to the Task object.
        - `session_id`          Reference to the Session object.
        - `status`              Status of the Task Attempt
        - `started_at`          Timestamp when the Task Attempt started.
        - `ended_at`            Timestamp when the Task Attempt ended.
        - `num_failures`        Number of times this Task Attempt has been failed
        - `statistics`          JSON blob of the statistics recorded for this Task Attempt

    """

    __tablename__ = "taskattempt"

    task_attempts_id = Column(Integer, primary_key=True, autoincrement=True)
    task_id = relationship("Task")
    session_id = relationship("Session")
    status = Column(Enum(Status), nullable=False)
    started_at = Column(DateTime, nullable=False, default=current_timestamp())
    ended_at = Column(DateTime, nullable=False)
    num_failures = Column(Integer, nullable=False, default=0)
    statistics = Column(JSONB, nullable=False, default=lambda: {})

    def __init__(self, **kwargs):
        self.task_attempts_id = kwargs.get("task_attempts_id")
        self.task_id = kwargs.get("task_id")
        self.session_id = kwargs.get("session_id")
        self.status = kwargs.get("status")
        self.started_at = kwargs.get("started_at")
        self.ended_at = kwargs.get("ended_at")
        self.num_failures = kwargs.get("num_failures")
        self.statistics = kwargs.get("statistics")

    def serialize(self):
        return {
            "task_attempts_id": self.task_attempts_id,
            "task_id": self.task_id,
            "session_id": self.session_id,
            "status": self.status,
            "started_at": self.started_at,
            "ended_at": self.ended_at,
            "num_failures": self.num_failures,
            "statistics": self.statistics
        }
