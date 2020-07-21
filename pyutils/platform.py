import platform
import sys
import os


def is_64b():
    return sys.maxsize > 2**32


def is_osx():
    return sys.platform == 'darwin'


def is_linux():
    return os.name == 'posix' and not is_osx()


def is_windows():
    return platform.system() == 'Windows'
