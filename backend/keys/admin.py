from django.contrib import admin
from django.contrib.auth.models import Group, User
from django.utils.html import format_html

from .models import ActivationLog, LicenseKey


@admin.register(LicenseKey)
class LicenseKeyAdmin(admin.ModelAdmin):
    list_display = (
        "steam_id",
        "key_display",
        "is_active",
        "created_at",
        "last_used",
        "status_badge",
    )
    list_filter = ("is_active", "created_at")
    search_fields = ("steam_id", "key")
    readonly_fields = ("key", "created_at", "last_used")
    actions = ["ban_keys", "activate_keys"]

    @admin.display(description="Ключ")
    def key_display(self, obj):
        return format_html('<code style="font-size: 12px;">{}</code>', obj.key)

    @admin.display(description="Статус")
    def status_badge(self, obj):
        color = "green" if obj.is_active else "red"
        text = "Активен" if obj.is_active else "Забанен"
        return format_html(
            '<span style="color: {}; font-weight: bold;">{}</span>', color, text
        )

    @admin.action(description="Забанить выбранные ключи")
    def ban_keys(self, request, queryset):
        queryset.update(is_active=False)

    @admin.action(description="Активировать выбранные ключи")
    def activate_keys(self, request, queryset):
        queryset.update(is_active=True)


@admin.register(ActivationLog)
class ActivationLogAdmin(admin.ModelAdmin):
    list_display = ("timestamp", "key_link", "ip_address", "success_icon")
    list_filter = ("success", "timestamp")
    search_fields = ("key__steam_id", "ip_address")
    readonly_fields = ("key", "ip_address", "timestamp", "success")

    def has_add_permission(self, request):
        return False

    @admin.display(description="Ключ/SteamID")
    def key_link(self, obj):
        return obj.key

    @admin.display(description="Результат")
    def success_icon(self, obj):
        return "✅" if obj.success else "❌"


try:
    admin.site.unregister(Group)
    admin.site.unregister(User)
except admin.sites.NotRegistered:
    pass
