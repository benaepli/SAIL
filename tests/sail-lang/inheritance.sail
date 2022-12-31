

class Base
{
    printBase()
    {
        print("Base class");
    }
}

class Derived < Base {}

let instance = Derived();
instance.printBase();