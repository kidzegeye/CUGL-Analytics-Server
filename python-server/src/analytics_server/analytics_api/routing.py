from django.urls import path

from . import consumers

websocket_urlpatterns = [
    path("ws/record_action/", consumers.RecordActionConsumer.as_asgi()),
]
