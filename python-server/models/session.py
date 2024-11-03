from sqlalchemy import Column, DateTime, Boolean, Integer
from sqlalchemy.orm import relationship
from sqlalchemy.orm import DeclarativeBase
from sqlalchemy.sql.functions import current_timestamp


class Base(DeclarativeBase):
    pass


class Session(Base):
    """
    Data for a Session.

    Attributes:
        - `session_id`     The id of the session.
        - `user_id`        Reference to the User object.
        - `started_at`     Timestamp when the session started.
        - `ended_at`       Timestamp when the session ended.
        - `ended`          True if the session has ended.
    """

    __tablename__ = "session"

    session_id = Column(Integer, primary_key=True, autoincrement=True)
    user_id = relationship("User")
    started_at = Column(DateTime, nullable=False, default=current_timestamp())
    ended_at = Column(DateTime, nullable=False)
    ended = Column(Boolean, nullable=False)

    def __init__(self, **kwargs):
        self.session_id = kwargs.get("session_id")
        self.user_id = kwargs.get("user_id")
        self.started_at = kwargs.get("started_at")
        self.ended_at = kwargs.get("ended_at")
        self.ended = kwargs.get("ended")

    def serialize(self):
        return {
            "session_id": self.session_id,
            "user_id": self.user_id,
            "started_at": self.started_at,
            "ended_at": self.ended_at,
            "ended": self.ended
        }
