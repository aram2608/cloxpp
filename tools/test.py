from pathlib import Path
import subprocess
from time import sleep

import typer
from typing_extensions import Annotated

app = typer.Typer()


# Helper function to extract files from a path
def extract_files(path: Path) -> list:
    # glob for lox files
    files = path.glob("*.lox")
    return files


def orchestrator(files: list, path: Path, dry: bool) -> None:
    # We loop through each file
    for f in files:
        # We save the command we would like to run as a list
        cmd = ["./build/cloxpp/cloxpp", f]

        # We now run subprocess.run and store the result
        # capture_ouput caputes stderr and stdout
        # text returns output as text instead of bytes
        # check returns an error if the process is not run
        result = subprocess.run(cmd, capture_output=True, text=True, check=True)

        # Write log output
        if not dry:
            # check if not empty
            if result.stdout:
                write_log(input_file=f, output=path, stdout=result.stdout)
            # check if not empty
            if result.stderr:
                write_stderr(input_file=f, output=path, stderr=result.stderr)
        else:
            print(result.stderr)
            sleep(0.5)
        # We increment file count to create a new output file


# Helper function to write output
def write_log(input_file: Path, output: Path, stdout: str) -> None:
    # We create a Path object for the input file
    input_file = Path(input_file)

    # We first need to create a new file using the file count and _expected.tokens extension
    new_file = input_file.stem + ".stdout"

    # We append the new_file to output PosixPath using the / operator
    log = Path(output) / new_file

    # We can now open the new file in write mode and dump the stdout contents
    with open(log, "w") as f:
        f.write(stdout)


# Helper function to write stderr
def write_stderr(input_file: Path, output: Path, stderr: str):
    input_file = Path(input_file)
    # We first need to create a new file using the file count and _expected.tokens extension
    new_file = input_file.stem + ".stderr"
    # We append the new_file to output PosixPath using the / operator
    log = Path(output) / new_file
    # We can now open the new file in write mode and dump the stdout contents
    with open(log, "w") as f:
        f.write(stderr)


@app.command()
def main(path: Path, dry: Annotated[bool, typer.Option("-d")] = False) -> None:
    """
    A helper script to run a series of test cases for the Lox language.

    Example:
        python3 utils/test.py tests

    Output logs are added to the provided directory with the '_expected.tokens'
    extension.

    Options:
        --dry: Will print to console instead of create outfiles.
    """
    # We first need to extract out files
    files = extract_files(path=path)
    # We can now run our list of files using the orchestrator
    orchestrator(files=files, path=path, dry=dry)


if __name__ == "__main__":
    app()
