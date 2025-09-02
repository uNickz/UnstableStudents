import os

os.chdir(os.path.dirname(os.path.abspath(__file__)))

def count_lines_of_code(directory, extensions={".c", ".h"}, exclude_empty_lines=True):
    total_lines = 0
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith(tuple(extensions)):
                file_path = os.path.join(root, file)
                with open(file_path, "r", encoding="utf-8", errors="ignore") as f:
                    lines = f.readlines()
                    if exclude_empty_lines:
                        lines = list(filter(lambda line: line.strip(), lines))
                    line_count = len(lines)
                    print(f"{file_path}: {line_count} lines")
                    total_lines += line_count
    print(f"\nTotal lines of code in \"{directory}\": {total_lines}")

if __name__ == "__main__":
    count_lines_of_code("../src/", exclude_empty_lines=False)