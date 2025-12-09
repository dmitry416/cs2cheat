from rest_framework import status
from rest_framework.exceptions import APIException


class ServiceError(APIException):
    status_code = status.HTTP_400_BAD_REQUEST
    default_detail = "A service error occurred."
    default_code = "service_error"

    def __init__(self, message, status_code=None):
        self.detail = {"status": "error", "message": message}
        if status_code:
            self.status_code = status_code


class SteamIDInvalid(ServiceError):
    pass


class KeyNotFound(ServiceError):
    status_code = status.HTTP_404_NOT_FOUND


class KeyBanned(ServiceError):
    status_code = status.HTTP_403_FORBIDDEN


class KeyMismatch(ServiceError):
    status_code = status.HTTP_403_FORBIDDEN
