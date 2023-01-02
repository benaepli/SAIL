{
  let a = "global";
  {
    fn showA() {
      print(a);
    }

    showA();
    let a = "block";
    showA();
  }
}

let f = 0;
{
  fn loop(n)
  {
    while (n < 2)
    {
      print(n);
      n=n+1;
    }
  }

  loop(f);
  let f = -2;
  loop(f);
  f = 0;
  {
    let f = 3;
    loop(f);

    fn loop(i)
    {
      print("Second loop");
      if (i < 3)
      {
        i=i+1;
        loop(i);
      }
    }
    loop(0);
  }
}