from setuptools import setup, Extension

module = Extension('vibration', sources=['vibration.c'])

setup(
    name='vibration',
    version='1.0',
    description='Industrial Vibration Analytics C Extension',
    ext_modules=[module]
)