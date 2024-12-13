"""analytics_server URL Configuration

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/4.1/topics/http/urls/

Author:  Kidus Zegeye
Version: 12/21/24
"""
from django.contrib import admin
from django.urls import path, include
from rest_framework import routers

from analytics_server.analytics_api import views
from drf_spectacular.views import SpectacularAPIView, SpectacularRedocView, SpectacularSwaggerView

router = routers.DefaultRouter()
router.register(r'organization', views.OrganizationViewSet)
router.register(r'game', views.GameViewSet)
router.register(r'user', views.UserViewSet)
router.register(r'session', views.SessionViewSet)
router.register(r'task', views.TaskViewSet)
router.register(r'taskattempt', views.TaskAttemptViewSet)
router.register(r'action', views.ActionViewSet)


urlpatterns = [
    path('admin/', admin.site.urls),
    path('rest/', include(router.urls)),
    path('api/schema/', SpectacularAPIView.as_view(), name='schema'),
    path('api/schema/swagger-ui/', SpectacularSwaggerView.as_view(url_name='schema'), name='swagger-ui'),
    path('api/schema/redoc/', SpectacularRedocView.as_view(url_name='schema'), name='redoc'),
]
