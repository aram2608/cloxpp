from pathlib import Path
import subprocess

import typer

app = typer.Typer()

# Helper function to extract files from a path
def extract_files(path: Path) -> list:
    # glob for lox files
    files = path.glob("*.lox")
    return files

def orchestrator(files: list, path: Path) -> None:
    file_count = 1
    # We loop through each file
    for f in files:
        # We save the command we would like to run as a list
        cmd = ["./build/cloxpp", f]

        # We now run subprocess.run and store the result
        # capture_ouput caputes stderr and stdout
        # text returns output as text instead of bytes
        # check returns an error if the process is not run
        result = subprocess.run(cmd, capture_output=True, text=True, check=True)

        # Write log output
        write_log(file_count=file_count, output=path, stdout=result.stdout)
        # We increment file count to create a new output file
        file_count += 1

# Helper function to write out 
def write_log(file_count: int, output: Path, stdout: str) -> None:
    # We first need to create a new file using the file count and _expected.tokens extension
    new_file = str(file_count) + "_expected.tokens"
    # We append the new_file to output PosixPath using the / operator
    log = output / new_file
    # We can now open the new file in write mode and dump the stdout contents
    with open(log, "w") as f:
        f.write(stdout)

@app.command()
def main(path: Path):
    """
    A helper script to run a series of test cases for the Lox language.
    Usage:
        python3 utils/test.py path/to/tests
    Example:
        python3 utils/test.py tests

    Output logs are added to the provided directory with the '_expected.tokens'
    extension.
    """
    # We first need to extract out files
    files = extract_files(path=path)
    # We can now run our list of files using the orchestrator
    orchestrator(files=files, path=path)


if __name__ == "__main__":
    app()