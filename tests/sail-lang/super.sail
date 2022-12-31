

class Base
{
    printMethod()
    {
        print("Base class");
    }
}

class Derived < Base 
{
    printMethod()
    {
        super.printMethod();
        print("Derived class");
    }
}

let instance = Derived();
instance.printMethod();