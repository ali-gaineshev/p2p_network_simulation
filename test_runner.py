import subprocess
import sys
import os

TESTFILE = 'TESTING_SPECIFICS.md'
NS3 = "../ns3"


def main():
    num_tests = int(sys.argv[1])

    commands = read_testing_file()

    cwd = os.getcwd().replace("/scratch", "")

    # clear logs before printing them
    open('app.log', 'w').close()

    for command in commands:
        command = "export CMAKE_SUPPRESS_DEV_WARNINGS=1; " + command
        for n in range(num_tests):
            run_command(command, cwd)


def run_command(command, cwd):
    try:
        with open("app.log", "a") as app:
            subprocess.run(
                command,
                check=True,
                cwd=cwd,
                shell=True,
                stdout=subprocess.DEVNULL,
                stderr=app
            )
    except subprocess.CalledProcessError as e:
        print(f"Command failed with error: {e}")
        sys.exit(1)


def read_testing_file():
    """Reads test file and extracts ns3 commands from code blocks."""
    with open(TESTFILE, 'r') as f:
        lines = f.readlines()

    commands = []
    in_code_block = False

    for line in lines:
        stripped_line = line.strip()

        if line.startswith("```\n"):
            in_code_block = not in_code_block  # Toggle code block state
            continue

        if in_code_block and stripped_line.startswith("./ns3"):
            commands.append(stripped_line)

    return commands


main()
