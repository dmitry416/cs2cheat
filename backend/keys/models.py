import uuid

from django.db import models
from django.utils import timezone


def generate_uuid_hex():
    return uuid.uuid4().hex


class LicenseKey(models.Model):
    key = models.CharField(
        max_length=32,
        unique=True,
        default=generate_uuid_hex,
        verbose_name="Лицензионный ключ",
    )
    steam_id = models.CharField(
        max_length=20,
        unique=True,
        verbose_name="Steam ID (64)",
        help_text="Уникальный идентификатор Steam аккаунта",
    )
    created_at = models.DateTimeField(auto_now_add=True, verbose_name="Дата создания")
    is_active = models.BooleanField(default=True, verbose_name="Активен")
    last_used = models.DateTimeField(
        null=True, blank=True, verbose_name="Последнее использование"
    )

    def mark_used(self):
        self.last_used = timezone.now()
        self.save(update_fields=["last_used"])

    class Meta:
        verbose_name = "Лицензия"
        verbose_name_plural = "Лицензии"
        ordering = ["-created_at"]

    def __str__(self):
        return f"{self.steam_id} ({self.key[:4]}...)"


class ActivationLog(models.Model):
    key = models.ForeignKey(
        LicenseKey, on_delete=models.CASCADE, related_name="logs", verbose_name="Ключ"
    )
    ip_address = models.GenericIPAddressField(verbose_name="IP Адрес")
    timestamp = models.DateTimeField(auto_now_add=True, verbose_name="Время запроса")
    success = models.BooleanField(verbose_name="Успешно")

    class Meta:
        verbose_name = "Лог активации"
        verbose_name_plural = "Логи активаций"
        ordering = ["-timestamp"]

    def __str__(self):
        status = "УСПЕХ" if self.success else "ОШИБКА"
        return f"[{self.timestamp.strftime('%H:%M:%S')}] {status} - {self.ip_address}"
