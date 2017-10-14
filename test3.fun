function Inc(x) returns xx
begin
  xx := Plus(x,1);
end function;

function MyTimes(x, y) returns times 
variables i
begin
  times := 0;
  i := 0;
  while LessEq(i,y) loop
    times := Plus(x,times);
    i := Inc(i);
  end loop;
  write times;
end function;

function Main() 
variables a, b, x, y
begin
  read a;
  read b;
  x := Times(a,b);
  y := MyTimes(a,b);
  if Eq(x,y)
   then write 1;
   else write 0;
  end if;
end function; 
