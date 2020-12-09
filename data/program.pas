program test(input, output);
var y : integer;
var z : real;
var c : array [1 .. 3] of real;
function square(a: integer; b: real): real;
  function square2(a : integer): integer;
    begin
       square := a * a
    end;
begin
   square := a * a
end;

function addTwo(a : integer): integer;
begin
   addTwo := a + 2
end;
begin
   y := addTwo(square(4))
end.
