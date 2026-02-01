"""
PlatformIO build script to load WiFi credentials from .env file
"""
Import("env")
import os

try:
    # Read .env file
    env_file = os.path.join(env.get("PROJECT_DIR"), ".env")
    if os.path.exists(env_file):
        with open(env_file) as f:
            for line in f:
                line = line.strip()
                if line and not line.startswith('#') and '=' in line:
                    key, value = line.split('=', 1)
                    key = key.strip()
                    value = value.strip()

                    # Add as build flags
                    if key == 'WIFI_SSID':
                        env.Append(CPPDEFINES=[('WIFI_SSID', f'\\"{value}\\"')])
                    elif key == 'WIFI_PASSWORD':
                        env.Append(CPPDEFINES=[('WIFI_PASSWORD', f'\\"{value}\\"')])

        print("✓ Loaded WiFi credentials from .env")
    else:
        print("⚠ Warning: .env file not found. Copy .env.example to .env and add your WiFi password.")
        # Use defaults
        env.Append(CPPDEFINES=[
            ('WIFI_SSID', '\\"www-iot\\"'),
            ('WIFI_PASSWORD', '\\"\\"')
        ])
except Exception as e:
    print(f"Error loading .env: {e}")
    # Use defaults
    env.Append(CPPDEFINES=[
        ('WIFI_SSID', '\\"www-iot\\"'),
        ('WIFI_PASSWORD', '\\"\\"')
    ])
