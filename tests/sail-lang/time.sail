

fn a() 
{
    let now = millis();

    let sum = 0;
    for (let i = 0; i < 10000000; i=i+1)
    {
        if ((i / 2) > 500)
        {
            let b = i;
            sum = sum + b;
        }
    }

    let new = millis();
    return (new - now);
}

print(a());

// One line:  fn a() { let now = millis(); let sum = 0; for (let i = 0; i < 10000000; i=i+1) { if ((i / 2) > 500) { let b = i; sum = sum + b; } } let new = millis(); return (new - now); } print(a());