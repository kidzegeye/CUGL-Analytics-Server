"""
Routing patterns for the websocket connection.

Currently only uses the base url as its endpoint.
"""
from django.urls import path

from . import consumers

websocket_urlpatterns = [
    path("", consumers.MainConsumer.as_asgi()),
]
