from sqlalchemy import Column, String
from sqlalchemy.orm import relationship
from sqlalchemy.orm import DeclarativeBase


class Base(DeclarativeBase):
    pass


class Game(Base):
    """
    Version data for a Game

    Attributes:
        - `game_version_id`      The ID of this game version entry.
        - `package_name`         The reference to the game's metadata.
        - `version_number`       The version number of the game.
    """

    __tablename__ = "game"

    game_version_id = Column(String, primary_key=True)
    package_name = relationship("GameMetaData")
    version_number = Column(String, nullable=False)

    def __init__(self, **kwargs):
        self.game_version_id = kwargs.get("game_version_id")
        self.package_name = kwargs.get("package_name")
        self.version_number = kwargs.get("version_number")

    def serialize(self):
        return {
            "game_version_id": self.game_version_id,
            "package_name": self.package_name,
            "version_number": self.version_number
        }
