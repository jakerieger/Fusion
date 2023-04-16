import os

total_lines = 0


def count_lines(directory, extensions):
    global total_lines
    for root, dirs, files in os.walk(directory):
        for file in files:
            if any(file.endswith(ext) for ext in extensions):
                filepath = os.path.join(root, file)
                with open(filepath, 'r') as f:
                    lines = f.readlines()
                    total_lines += len(lines)


extensions = ['.c', '.h']
count_lines('include', extensions)
count_lines('src', extensions)

print("Total lines: " + str(total_lines) + "\n")
