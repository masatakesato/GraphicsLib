import ctypes


def MAKELPARAM( low, high ):
    return ctypes.wintypes.LPARAM( ( (high & 0xFFFF) << 16 ) | (low & 0xFFFF) )