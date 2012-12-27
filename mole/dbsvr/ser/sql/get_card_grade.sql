CREATE FUNCTION get_grade (exp  integer) RETURNS int(11)
begin
	if exp<100 then return 0;
	elseif exp<250 then return 1;
	elseif exp<500 then return 2 ;
	elseif exp<1000 then return 3; 
	elseif exp<2000 then return 4; 
	elseif exp<3000 then return 5; 
	elseif exp<4000 then return 6; 
	elseif exp<5000 then return 7; 
	elseif exp<7000 then return 8; 
	elseif exp>7000 then return 9; 
	end if;
end 
