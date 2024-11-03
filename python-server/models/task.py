from sqlalchemy import Column, String, Integer
from sqlalchemy.orm import relationship
from sqlalchemy.orm import DeclarativeBase


class Base(DeclarativeBase):
    pass


class Task(Base):
    """
    Data for a Task.

    Attributes:
        - `task_id`        The id of the task.
        - `user_id`        Reference to the User object.
        - `task_name`           Name of the task.
    """

    __tablename__ = "task"

    task_id = Column(Integer, primary_key=True, autoincrement=True)
    user_id = relationship("User")
    task_name = Column(String, nullable=False)

    def __init__(self, **kwargs):
        self.session_id = kwargs.get("session_id")
        self.user_id = kwargs.get("user_id")
        self.task_name = kwargs.get("task_name")

    def serialize(self):
        return {
            "session_id": self.session_id,
            "user_id": self.user_id,
            "task_name": self.task_name
        }
