program example(input, output);
var x : integer; var y: integer; var z: integer;
var r : real; var s : real;
function gcd(a, b: integer ): integer;
begin
   if b = 0 then gcd := a
   else gcd := gcd(b, a mod b)
end;

begin
   read(x, y);
   write(gcd(x, y))
end.

begin
   z := 104;
   x := 2;
   z := 21234;
   
   r := 3.14;
   s := 3.14E2;
end.
