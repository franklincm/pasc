program example(input, output);
var
   x, y, z: integer
   r, s: real;
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
   r := 3.14;
   s := 3.14E2;
end.
