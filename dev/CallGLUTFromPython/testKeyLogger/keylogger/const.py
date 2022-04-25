﻿
class Const:

    # Window Messages
    WM_MOUSEFIRST       = 0x0200
    WM_MOUSEMOVE        = 0x0200
    WM_LBUTTONDOWN      = 0x0201
    WM_LBUTTONUP        = 0x0202
    WM_LBUTTONDBLCLK    = 0x0203
    WM_RBUTTONDOWN      = 0x0204
    WM_RBUTTONUP        = 0x0205
    WM_RBUTTONDBLCLK    = 0x0206
    WM_MBUTTONDOWN      = 0x0207
    WM_MBUTTONUP        = 0x0208
    WM_MBUTTONDBLCLK    = 0x0209
    WM_MOUSEWHEEL       = 0x020A
    WM_MOUSELAST        = 0x020A

    WM_KEYFIRST         = 0x0100
    WM_KEYDOWN          = 0x0100
    WM_KEYUP            = 0x0101
    WM_CHAR             = 0x0102
    WM_DEADCHAR         = 0x0103
    WM_SYSKEYDOWN       = 0x0104
    WM_SYSKEYUP         = 0x0105
    WM_SYSCHAR          = 0x0106
    WM_SYSDEADCHAR      = 0x0107
    # WM_UNICHAR        = 0x0109 windows xp
    WM_KEYLAST          = 0x0109



    # Check flag for GetKeyState/GetAsyncKeyState result
    KEYSTATE_DOWN = 0x8000
    KEYSTATE_TOGGLE = 0x0001



    # Virtual-Key Codes https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes

    VK_LBUTTON              = 0x01# Left mouse button
    VK_RBUTTON              = 0x02# Right mouse button
    VK_CANCEL               = 0x03# Control-break processing
    VK_MBUTTON              = 0x04# Middle mouse button (three-button mouse)
    VK_XBUTTON1             = 0x05# X1 mouse button
    VK_XBUTTON2             = 0x06# X2 mouse button
    #-	0x07	Undefined
    VK_BACK                 = 0x08# BACKSPACE key
    VK_TAB                  = 0x09# TAB key
    #-	0x0A-0B	Reserved
    VK_CLEAR                = 0x0C# CLEAR key
    VK_RETURN               = 0x0D# ENTER key
    #-	0x0E-0F	Undefined
    VK_SHIFT                = 0x10# SHIFT key
    VK_CONTROL              = 0x11# CTRL key
    VK_MENU                 = 0x12# ALT key
    VK_PAUSE                = 0x13# PAUSE key
    VK_CAPITAL              = 0x14# CAPS LOCK key
    VK_KANA                 = 0x15# IME Kana mode
    VK_HANGUEL              = 0x15# IME Hanguel mode (maintained for compatibility; use VK_HANGUL)
    VK_HANGUL               = 0x15# IME Hangul mode
    VK_IME_ON               = 0x16# IME On
    VK_JUNJA                = 0x17# IME Junja mode
    VK_FINAL                = 0x18# IME final mode
    VK_HANJA                = 0x19# IME Hanja mode
    VK_KANJI                = 0x19# IME Kanji mode
    VK_IME_OFF              = 0x1A# IME Off
    VK_ESCAPE               = 0x1B# ESC key
    VK_CONVERT              = 0x1C# IME convert
    VK_NONCONVERT           = 0x1D# IME nonconvert
    VK_ACCEPT               = 0x1E# IME accept
    VK_MODECHANGE           = 0x1F# IME mode change request
    VK_SPACE                = 0x20# SPACEBAR
    VK_PRIOR                = 0x21# PAGE UP key
    VK_NEXT                 = 0x22# PAGE DOWN key
    VK_END                  = 0x23# END key
    VK_HOME                 = 0x24# HOME key
    VK_LEFT                 = 0x25# LEFT ARROW key
    VK_UP                   = 0x26# UP ARROW key
    VK_RIGHT                = 0x27# RIGHT ARROW key
    VK_DOWN                 = 0x28# DOWN ARROW key
    VK_SELECT               = 0x29# SELECT key
    VK_PRINT                = 0x2A# PRINT key
    VK_EXECUTE              = 0x2B# EXECUTE key
    VK_SNAPSHOT             = 0x2C# PRINT SCREEN key
    VK_INSERT               = 0x2D# INS key
    VK_DELETE               = 0x2E# DEL key
    VK_HELP                 = 0x2F# HELP key
    VK_LWIN                 = 0x5B# Left Windows key (Natural keyboard)
    VK_RWIN                 = 0x5C# Right Windows key (Natural keyboard)
    VK_APPS                 = 0x5D# Applications key (Natural keyboard)
    #-	0x5E	Reserved
    VK_SLEEP                = 0x5F# Computer Sleep key
    VK_NUMPAD0              = 0x60# Numeric keypad 0 key
    VK_NUMPAD1              = 0x61# Numeric keypad 1 key
    VK_NUMPAD2              = 0x62# Numeric keypad 2 key
    VK_NUMPAD3              = 0x63# Numeric keypad 3 key
    VK_NUMPAD4              = 0x64# Numeric keypad 4 key
    VK_NUMPAD5              = 0x65# Numeric keypad 5 key
    VK_NUMPAD6              = 0x66# Numeric keypad 6 key
    VK_NUMPAD7              = 0x67# Numeric keypad 7 key
    VK_NUMPAD8              = 0x68# Numeric keypad 8 key
    VK_NUMPAD9              = 0x69# Numeric keypad 9 key
    VK_MULTIPLY             = 0x6A# Multiply key
    VK_ADD                  = 0x6B# Add key
    VK_SEPARATOR            = 0x6C# Separator key
    VK_SUBTRACT             = 0x6D# Subtract key
    VK_DECIMAL              = 0x6E# Decimal key
    VK_DIVIDE               = 0x6F# Divide key
    VK_F1                   = 0x70# F1 key
    VK_F2                   = 0x71# F2 key
    VK_F3                   = 0x72# F3 key
    VK_F4                   = 0x73# F4 key
    VK_F5                   = 0x74# F5 key
    VK_F6                   = 0x75# F6 key
    VK_F7                   = 0x76# F7 key
    VK_F8                   = 0x77# F8 key
    VK_F9                   = 0x78# F9 key
    VK_F10                  = 0x79# F10 key
    VK_F11                  = 0x7A# F11 key
    VK_F12                  = 0x7B# F12 key
    VK_F13                  = 0x7C# F13 key
    VK_F14                  = 0x7D# F14 key
    VK_F15                  = 0x7E# F15 key
    VK_F16                  = 0x7F# F16 key
    VK_F17                  = 0x80# F17 key
    VK_F18                  = 0x81# F18 key
    VK_F19                  = 0x82# F19 key
    VK_F20                  = 0x83# F20 key
    VK_F21                  = 0x84# F21 key
    VK_F22                  = 0x85# F22 key
    VK_F23                  = 0x86# F23 key
    VK_F24                  = 0x87# F24 key
    #-	0x88-8F	Unassigned
    VK_NUMLOCK              = 0x90#	NUM LOCK key
    VK_SCROLL               = 0x91#	SCROLL LOCK key
    #0x92-96	OEM specific
    #-	0x97-9F	Unassigned
    VK_LSHIFT               = 0xA0# Left SHIFT key
    VK_RSHIFT               = 0xA1# Right SHIFT key
    VK_LCONTROL             = 0xA2# Left CONTROL key
    VK_RCONTROL             = 0xA3# Right CONTROL key
    VK_LMENU                = 0xA4# Left MENU key
    VK_RMENU                = 0xA5# Right MENU key
    VK_BROWSER_BACK	        = 0xA6# Browser Back key
    VK_BROWSER_FORWARD      = 0xA7# Browser Forward key
    VK_BROWSER_REFRESH      = 0xA8# Browser Refresh key
    VK_BROWSER_STOP         = 0xA9# Browser Stop key
    VK_BROWSER_SEARCH       = 0xAA# Browser Search key
    VK_BROWSER_FAVORITES    = 0xAB# Browser Favorites key
    VK_BROWSER_HOME         = 0xAC# Browser Start and Home key
    VK_VOLUME_MUTE          = 0xAD# Volume Mute key
    VK_VOLUME_DOWN          = 0xAE# Volume Down key
    VK_VOLUME_UP            = 0xAF# Volume Up key
    VK_MEDIA_NEXT_TRACK     = 0xB0# Next Track key
    VK_MEDIA_PREV_TRACK     = 0xB1# Previous Track key
    VK_MEDIA_STOP           = 0xB2# Stop Media key
    VK_MEDIA_PLAY_PAUSE     = 0xB3# Play/Pause Media key
    VK_LAUNCH_MAIL          = 0xB4# Start Mail key
    VK_LAUNCH_MEDIA_SELECT  = 0xB5# Select Media key
    VK_LAUNCH_APP1          = 0xB6# Start Application 1 key
    VK_LAUNCH_APP2          = 0xB7# Start Application 2 key
    #-	0xB8-B9	Reserved
    VK_OEM_1                = 0xBA# Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ';:' key
    VK_OEM_PLUS             = 0xBB# For any country/region, the '+' key
    VK_OEM_COMMA            = 0xBC# For any country/region, the ',' key
    VK_OEM_MINUS            = 0xBD# For any country/region, the '-' key
    VK_OEM_PERIOD           = 0xBE# For any country/region, the '.' key
    VK_OEM_2                = 0xBF# Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '/?' key
    VK_OEM_3                = 0xC0# Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '`~' key
    #-	0xC1-D7	Reserved
    #-	0xD8-DA	Unassigned
    VK_OEM_4                = 0xDB# Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '[{' key
    VK_OEM_5                = 0xDC# Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '\|' key
    VK_OEM_6                = 0xDD# Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ']}' key
    VK_OEM_7                = 0xDE# Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the 'single-quote/double-quote' key
    VK_OEM_8                = 0xDF# Used for miscellaneous characters; it can vary by keyboard.
    #-	0xE0	Reserved
    #0xE1	OEM specific
    VK_OEM_102              = 0xE2# The <> keys on the US standard keyboard, or the \\| key on the non-US 102-key keyboard
    #0xE3-E4	OEM specific
    VK_PROCESSKEY           = 0xE5# IME PROCESS key
    #0xE6	OEM specific
    VK_PACKET               = 0xE7# Used to pass Unicode characters as if they were keystrokes. The VK_PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods. For more information, see Remark in KEYBDINPUT, SendInput, WM_KEYDOWN, and WM_KEYUP
    #-	0xE8	Unassigned
    #0xE9-F5	OEM specific
    VK_ATTN                 = 0xF6# Attn key
    VK_CRSEL                = 0xF7# CrSel key
    VK_EXSEL                = 0xF8# ExSel key
    VK_EREOF                = 0xF9# Erase EOF key
    VK_PLAY                 = 0xFA# Play key
    VK_ZOOM                 = 0xFB# Zoom key
    VK_NONAME               = 0xFC# Reserved
    VK_PA1                  = 0xFD# PA1 key
    VK_OEM_CLEAR            = 0xFE# Clear key

    KEY_0                   = 0x30# 0 key
    KEY_1                   = 0x31# 1 key
    KEY_2                   = 0x32# 2 key
    KEY_3                   = 0x33# 3 key
    KEY_4                   = 0x34# 4 key
    KEY_5                   = 0x35# 5 key
    KEY_6                   = 0x36# 6 key
    KEY_7                   = 0x37# 7 key
    KEY_8                   = 0x38# 8 key
    KEY_9                   = 0x39# 9 key
    #0x3A-40	Undefined
    KEY_A                   = 0x41# A key
    KEY_B                   = 0x42# B key
    KEY_C                   = 0x43# C key
    KEY_D                   = 0x44# D key
    KEY_E                   = 0x45# E key
    KEY_F                   = 0x46# F key
    KEY_G                   = 0x47# G key
    KEY_H                   = 0x48# H key
    KEY_I                   = 0x49# I key
    KEY_J                   = 0x4A# J key
    KEY_K                   = 0x4B# K key
    KEY_L                   = 0x4C# L key
    KEY_M                   = 0x4D# M key
    KEY_N                   = 0x4E# N key
    KEY_O                   = 0x4F# O key
    KEY_P                   = 0x50# P key
    KEY_Q                   = 0x51# Q key
    KEY_R                   = 0x52# R key
    KEY_S                   = 0x53# S key
    KEY_T                   = 0x54# T key
    KEY_U                   = 0x55# U key
    KEY_V                   = 0x56# V key
    KEY_W                   = 0x57# W key
    KEY_X                   = 0x58# X key
    KEY_Y                   = 0x59# Y key
    KEY_Z                   = 0x5A# Z key



    MsgToName = {
        WM_MOUSEMOVE :      "mouse move",
        WM_LBUTTONDOWN :    "mouse left down", 
        WM_LBUTTONUP :      "mouse left up",
        WM_LBUTTONDBLCLK :  "mouse left double", 
        WM_RBUTTONDOWN :    "mouse right down",
        WM_RBUTTONUP :      "mouse right up", 
        WM_RBUTTONDBLCLK :  "mouse right double",
        WM_MBUTTONDOWN :    "mouse middle down", 
        WM_MBUTTONUP :      "mouse middle up",
        WM_MBUTTONDBLCLK :  "mouse middle double", 
        WM_MOUSEWHEEL :     "mouse wheel",
        WM_KEYDOWN :        "key down", 
        WM_KEYUP :          "key up",
        WM_CHAR :           "key char",
        WM_DEADCHAR :       "key dead char", 
        WM_SYSKEYDOWN :     "key sys down",
        WM_SYSKEYUP :       "key sys up", 
        WM_SYSCHAR :        "key sys char",
        WM_SYSDEADCHAR :    "key sys dead char"
    }
