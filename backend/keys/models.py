from django.db import models
import uuid
from django.utils import timezone

def generate_uuid_hex():
    return uuid.uuid4().hex

class LicenseKey(models.Model):
    key = models.CharField(max_length=32, unique=True, default=generate_uuid_hex)
    steam_id = models.CharField(max_length=32, unique=True)
    created_at = models.DateTimeField(auto_now_add=True)
    is_active = models.BooleanField(default=True)
    last_used = models.DateTimeField(null=True, blank=True)

    def __str__(self):
        return f"{self.key[:8]}... - {self.steam_id}"


class ActivationLog(models.Model):
    key = models.ForeignKey(LicenseKey, on_delete=models.CASCADE)
    ip_address = models.GenericIPAddressField()
    timestamp = models.DateTimeField(auto_now_add=True)
    success = models.BooleanField()

    class Meta:
        ordering = ['-timestamp']