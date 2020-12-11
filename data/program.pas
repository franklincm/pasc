program test(input, output);
var y : integer;
var z : real;
var c : array [1 .. 3] of integer;
var h : array [2 .. 4] of integer;
function square(a: integer; b: real): real;
  function nested(a : integer): integer;
  var u : integer;
    begin
       u := 1;
       nested := 2
    end;
begin
   while (2>=1) do
      begin
         a := a + 2
      end;
   square := 2.0
end;
function circle(a : real): integer;
begin
   if (y == 1) then
      circle := 2 + 1
   else
      circle := 2 + 2
end;
function triangle(a : integer): integer;
var d :  integer;
begin
   d := 3;
   triangle := a + 2
end;
begin
   y := triangle(circle(square(c[h[2]], 2.0)));
   y := 2
end.
