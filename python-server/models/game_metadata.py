from sqlalchemy import Column, String
from sqlalchemy.orm import DeclarativeBase


class Base(DeclarativeBase):
    pass


class GameMetaData(Base):
    """
    Metadata for a Game.

    Attributes:
        - `package_name`      The package name of this game.
        - `game_name`         The name of the game.
    """

    __tablename__ = "gamemetadata"

    package_name = Column(String, primary_key=True)
    game_name = Column(String, nullable=False)

    def __init__(self, **kwargs):
        self.package_name = kwargs.get("package_name")
        self.game_name = kwargs.get("game_name")

    def serialize(self):
        return {
            "package_name": self.package_name,
            "game_name": self.game_name,
        }
