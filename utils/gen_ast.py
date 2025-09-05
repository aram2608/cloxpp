import typer

app = typer.Typer()

nodes = [
    "Binary   : Expr left, Token operator, Expr right",
    "Grouping : Expr expression",
    "Literal  : Object value",
    "Unary    : Token operator, Expr right",
]


@app.command()
def main():
    print("Hello world")


if __name__ == "__main__":
    app()
