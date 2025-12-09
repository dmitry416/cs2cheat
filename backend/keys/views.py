import logging

from django.core.exceptions import ValidationError
from django.core.validators import validate_integer
from django.shortcuts import render
from rest_framework.decorators import api_view
from rest_framework.response import Response

from .exceptions import KeyBanned, KeyMismatch, KeyNotFound, SteamIDInvalid
from .models import ActivationLog, LicenseKey

logger = logging.getLogger("keys")


def home(request):
    return render(request, "index.html")


def validate_steamid(steamid):
    if not steamid:
        raise SteamIDInvalid("SteamID is required")
    try:
        validate_integer(steamid)
    except ValidationError:
        raise SteamIDInvalid("Invalid SteamID format")
    if len(steamid) < 15 or len(steamid) > 20:
        raise SteamIDInvalid("Invalid SteamID length")


@api_view(["POST"])
def verify_key(request):
    key = request.data.get("key", "").strip()
    steamid = request.data.get("steamid", "").strip()
    client_ip = request.META.get("REMOTE_ADDR")

    logger.info(f"Incoming verify request from IP: {client_ip}")
    logger.info(f"Received DATA -> Key: '{key}', SteamID: '{steamid}'")

    validate_steamid(steamid)

    license_key = LicenseKey.objects.filter(key=key).first()

    if not license_key:
        logger.warning(f"Failed: Key '{key}' not found in DB.")
        raise KeyNotFound("Invalid key")

    if not license_key.is_active:
        logger.warning(f"Failed: Key '{key}' is BANNED.")
        ActivationLog.objects.create(
            key=license_key, ip_address=client_ip, success=False
        )
        raise KeyBanned("Key is banned or inactive")

    if license_key.steam_id != steamid:
        logger.error(
            f"MISMATCH! Key owner: '{license_key.steam_id}' vs Request SteamID: '{steamid}'"
        )
        ActivationLog.objects.create(
            key=license_key, ip_address=client_ip, success=False
        )
        raise KeyMismatch("Key not assigned to this Steam account")

    license_key.mark_used()
    ActivationLog.objects.create(key=license_key, ip_address=client_ip, success=True)

    logger.info(f"SUCCESS! Key verified for SteamID: {steamid}")

    return Response(
        {
            "status": "success",
            "message": "Key verified successfully",
            "created_at": license_key.created_at,
        }
    )


@api_view(["POST"])
def request_key(request):
    steamid = request.data.get("steam_id", "").strip()
    client_ip = request.META.get("REMOTE_ADDR")

    logger.info(f"Requesting NEW key for SteamID: '{steamid}' from {client_ip}")

    validate_steamid(steamid)

    existing_key = LicenseKey.objects.filter(steam_id=steamid).first()
    if existing_key:
        if not existing_key.is_active:
            logger.warning(f"Request denied: SteamID '{steamid}' is banned.")
            raise KeyBanned("Your key has been banned")

        logger.info(f"Returning EXISTING key for '{steamid}'")
        return Response(
            {
                "status": "success",
                "message": "Key already exists",
                "key": existing_key.key,
            }
        )

    new_key = LicenseKey.objects.create(steam_id=steamid)
    logger.info(f"Generated NEW key for '{steamid}'")

    return Response(
        {
            "status": "success",
            "message": "Key generated successfully",
            "key": new_key.key,
        },
        status=201,
    )
