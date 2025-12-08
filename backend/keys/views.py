from django.http import JsonResponse
from django.shortcuts import render
from django.utils import timezone
from django.views.decorators.csrf import csrf_exempt
from django.views.decorators.http import require_POST
import json
from .models import LicenseKey, ActivationLog
from django.core.validators import validate_integer
from django.core.exceptions import ValidationError


def home(request):
    """Главная страница для получения ключа"""
    return render(request, 'index.html')

@csrf_exempt
@require_POST
def verify_key(request):
    try:
        data = json.loads(request.body.decode('utf-8'))
        key = data.get('key', '').strip()
        steamid = data.get('steamid', '').strip()

        try:
            validate_integer(steamid)
        except ValidationError:
            return JsonResponse({
                'status': 'error',
                'message': 'Invalid SteamID format'
            }, status=400)

        try:
            license_key = LicenseKey.objects.get(key=key, is_active=True)
        except LicenseKey.DoesNotExist:
            return JsonResponse({
                'status': 'error',
                'message': 'Invalid or inactive key'
            })

        if license_key.steam_id != steamid:
            return JsonResponse({
                'status': 'error',
                'message': 'Key not assigned to this Steam account'
            })

        license_key.last_used = timezone.now()
        license_key.save()

        ActivationLog.objects.create(
            key=license_key,
            ip_address=request.META.get('REMOTE_ADDR'),
            success=True
        )

        return JsonResponse({
            'status': 'success',
            'message': 'Key verified successfully',
            'created_at': license_key.created_at.isoformat()
        })

    except json.JSONDecodeError:
        return JsonResponse({
            'status': 'error',
            'message': 'Invalid JSON'
        }, status=400)
    except Exception as e:
        return JsonResponse({
            'status': 'error',
            'message': str(e)
        }, status=500)


@csrf_exempt
@require_POST
def request_key(request):
    try:
        data = json.loads(request.body.decode('utf-8'))
        steamid = data.get('steam_id', '').strip()

        try:
            validate_integer(steamid)
        except ValidationError:
            return JsonResponse({
                'status': 'error',
                'message': 'Invalid SteamID format'
            }, status=400)

        if len(steamid) < 15 or len(steamid) > 20:
            return JsonResponse({
                'status': 'error',
                'message': 'Invalid SteamID length'
            }, status=400)

        existing_key = LicenseKey.objects.filter(steam_id=steamid, is_active=True).first()

        if existing_key:
            return JsonResponse({
                'status': 'success',
                'message': 'Key already exists for this SteamID',
                'key': existing_key.key
            })

        new_key = LicenseKey.objects.create(steam_id=steamid)

        return JsonResponse({
            'status': 'success',
            'message': 'Key generated successfully',
            'key': new_key.key
        })

    except json.JSONDecodeError:
        return JsonResponse({
            'status': 'error',
            'message': 'Invalid JSON'
        }, status=400)
    except Exception as e:
        return JsonResponse({
            'status': 'error',
            'message': str(e)
        }, status=500)