import dis

def greet(name: str) -> None:
    print(name)

dis.dis(greet)