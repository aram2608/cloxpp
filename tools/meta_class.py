class AttributeInitType(type):
    def __call__(self, *args, **kwargs):
        """Create a new instance."""

        # First, create the object in the normal default way.
        obj = type.__call__(self, *args)

        # Additionally, set attributes on the new object.
        for name, value in kwargs.items():
            setattr(obj, name, value)

        # Return the new object.
        return obj
    
class Car(metaclass=AttributeInitType):
    @property
    def description(self) -> str:
        """Return a description of this car."""
        return " ".join(str(value) for value in self.__dict__.values())
    
new_car = Car(make='Toyota', model='Prius', year=2005, color='Green', engine='Hybrid')

print(new_car.description)

class Python:
    def __init__(self):
        self.name = "Python"

    @staticmethod
    def get_class_name():
        print("Python")

    @property
    def set_name(self):
        return self.name
    
    @set_name.setter
    def set_name(self, name):
        print(f"Setting {self.name} to {name}")
        self.name = name

    @set_name.deleter
    def set_name(self):
        del self.name

    def say_name(self):
        print(self.name)

Python.get_class_name

p = Python()

p.say_name()

p.set_name = "Mamba"

p.say_name()
