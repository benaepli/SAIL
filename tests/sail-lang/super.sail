
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


class FirstBase
{
    printMethod()
    {
        print("First base");
    }
}

class SecondBase < FirstBase
{
    printMethod()
    {
        super.printMethod();
        print("Second base");
    }
}

class ThirdBase < SecondBase
{
    printMethod()
    {
        super.printMethod();
        print("Third base."); // should print all three bases
    }
}