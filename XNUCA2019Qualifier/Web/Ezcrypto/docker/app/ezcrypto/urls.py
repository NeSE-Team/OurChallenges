from django.urls import path
import ezcrypto.views as views

urlpatterns = [
    path('', views.index),
    path('login', views.login),
    path('change', views.change),
]
