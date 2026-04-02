import vibration
import time

data = [1.2, 3.4, -2.1, 5.5, 0.1]

print("--- Testing Vibration Module ---")
print(f"Peak-to-Peak: {vibration.peak_to_peak(data)}")
print(f"RMS: {vibration.rms(data)}")
print(f"Summary: {vibration.summary(data)}")

# Edge Case: Empty List
try:
    print(f"Empty List Test: {vibration.peak_to_peak([])}")
except ValueError as e:
    print(f"Caught expected error: {e}")