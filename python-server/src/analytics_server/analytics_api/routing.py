from django.urls import path

from . import consumers

websocket_urlpatterns = [
    path("ws/record_action/session/<int:session_id>/", consumers.RecordActionConsumer.as_asgi()),
]
