"""
This file defines the AnalyticsApi app config

Author:  Kidus Zegeye
Version: 12/21/24
"""
from django.apps import AppConfig


class AnalyticsApiConfig(AppConfig):
    """
    This class stores config data for the AnalyticsApi application
    """
    # The default primary key type for the database
    default_auto_field = 'django.db.models.BigAutoField'

    # The full path name of the application
    name = 'analytics_server.analytics_api'
