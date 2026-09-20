# WARNING-----------------
# This script uses hardcoded names
# This script works on relative directories

import os
import sys
import subprocess

script_dir = os.path.dirname(os.path.abspath(__file__))
tantrums_path = None

test_exe = os.path.join(script_dir, "TEST.exe")
test_obj = os.path.join(script_dir, "TEST.obj")


def check_exists(test_file_name, test_file_expected_output):
    test_file_path = os.path.join(script_dir, test_file_name)
    test_file_expected_output_path = os.path.join(script_dir, test_file_expected_output)

    if not os.path.isfile(test_file_path):
        print(f"The test file '{test_file_path}' does not exist!")
        sys.exit(1)
    elif not os.path.isfile(test_file_expected_output_path):
        print(f"The expected output file '{test_file_expected_output_path}' does not exist!")
        sys.exit(1)


def run_test(test_file_name, test_file_expected_output):
    test_file_path = os.path.join(script_dir, test_file_name)
    test_file_expected_output_path = os.path.join(script_dir, test_file_expected_output)

    temp_result = subprocess.run(
        [tantrums_path, test_file_path, "-o", test_obj, "-l", test_exe],
        capture_output=True, text=True
    )

    result = subprocess.run(
        [test_exe],
        capture_output=True, text=True
    )

    with open(test_file_expected_output_path, "r", encoding="UTF-8") as file:
        expected_content = file.read()

    if result.stdout in expected_content:
        print(f"Test '{test_file_name}' passed!")
    else:
        print(f"Test '{test_file_name}' failed!")
        print(f"  Expected: {expected_content.strip()!r}")
        print(f"  Got:      {result.stdout.strip()!r}")


def main():
    global tantrums_path
    raw = input("Enter the relative path to your tantrums executable: ")
    tantrums_path = os.path.abspath(os.path.join(script_dir, raw))

    if not os.path.isfile(tantrums_path):
        print(f"Executable '{tantrums_path}' does not exist!")
        sys.exit(1)

    core_suite = {
        "core/helloWorld.tnt": "core/helloWorld.expectedOutput"
    }

    test_sets = [core_suite]

    for suite in test_sets:
        for test in suite:
            check_exists(test, suite[test])
            run_test(test, suite[test])

if __name__ == "__main__":
    main()
