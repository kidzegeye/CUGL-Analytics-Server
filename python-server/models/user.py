import enum
from sqlalchemy import Column, String, Enum, Integer
from sqlalchemy.orm import relationship
from sqlalchemy.orm import DeclarativeBase


class Base(DeclarativeBase):
    pass


class Platform(enum.Enum):
    ios = 1
    android = 2
    other = 3


class User(Base):
    """
    Data for a User.

    Attributes:
        - `user_id`           The user id of the user.
        - `game_version_id`   Reference to the Game version object.
        - `vendor_id`         The vendor id of the user's device.
        - `platform`          The platform of the user's device.
    """

    __tablename__ = "user"

    user_id = Column(Integer, primary_key=True, autoincrement=True)
    game_version_id = relationship("Game")
    vendor_id = Column(String, nullable=False)
    platform = Column(Enum(Platform), nullable=False)

    def __init__(self, **kwargs):
        self.user_id = kwargs.get("user_id")
        self.game_version_id = kwargs.get("game_version_id")
        self.vendor_id = kwargs.get("vendor_id")
        self.platform = kwargs.get("platform")

    def serialize(self):
        return {
            "user_id": self.user_id,
            "game_version_id": self.game_version_id,
            "vendor_id": self.vendor_id,
            "platform": self.platform,
        }
