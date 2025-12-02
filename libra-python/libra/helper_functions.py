import os
import difflib

def compare_images_with_tolerance(dir1, dir2, tolerance_percent=5):
    # Ensure both paths are directories
    #if not os.path.isdir(dir1) or not os.path.isdir(dir2):
    #    raise ValueError("Both inputs must be valid directories.")
    if not os.path.isdir(dir1):
        raise ValueError("Dir1 must be valid directory.")

    if not os.path.isdir(dir2):
        raise ValueError("Dir2 must be valid directory.")

    # Compare directories recursively
    dir1_files = get_files_recursive(dir1)
    dir2_files = get_files_recursive(dir2)

    diffs = []

    # Compare files in both directories
    for file1, file2 in zip(dir1_files, dir2_files):
        size1 = os.path.getsize(file1)
        size2 = os.path.getsize(file2)
        
        # Allow a percentage difference in size
        size_diff_percent = abs(size1 - size2) / max(size1, size2) * 100
        
        if size_diff_percent > tolerance_percent:
            diffs.append(f"File size difference exceeds tolerance: {file1} vs {file2}")
        else:
            # Compare file contents if sizes are within tolerance
            with open(file1, 'rb') as f1, open(file2, 'rb') as f2:
                content1 = f1.read()
                content2 = f2.read()

            # Example: Compare contents as bytes
            if content1 != content2:
                diffs.append(f"File contents differ beyond tolerance: {file1} vs {file2}")

    # Return True if no significant differences found
    return not diffs

def get_files_recursive(directory):
    file_list = []
    for root, _, files in os.walk(directory):
        for file in files:
            file_list.append(os.path.join(root, file))
    return file_list