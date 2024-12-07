from django.urls import path

from . import consumers

websocket_urlpatterns = [
    path("", consumers.MainConsumer.as_asgi()),
]
