from django.db import models


class User(models.Model):
    class Meta:
        db_table = 'User'
    id = models.AutoField(primary_key=True)
    username = models.CharField(max_length=30)
    password = models.CharField(max_length=32)
    times = models.IntegerField()
    Nroot = models.TextField()
    Eroot = models.TextField()


class Record(models.Model):
    class Meta:
        db_table = 'Record'
    id = models.AutoField(primary_key=True)
    lowlimit = models.FloatField(default=0.32)
    uplimit = models.FloatField(default=0.32)
    username = models.CharField(max_length=30)
    secretroot = models.TextField()
    Nuser = models.TextField()
    Euser = models.TextField()
    secretuser = models.TextField()
    message = models.TextField()
    luky = models.IntegerField()