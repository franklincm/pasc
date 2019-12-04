program example(input, output);
var
   x, y, z : integer
   r     : real;
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
   r := 3.14;
   z := 12345678901;
end.
