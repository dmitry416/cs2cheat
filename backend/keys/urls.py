from django.urls import path
from . import views

urlpatterns = [
    path('', views.home, name='home'),
    path('api/verify', views.verify_key, name='verify_key'),
    path('api/request-key', views.request_key, name='request_key'),
]