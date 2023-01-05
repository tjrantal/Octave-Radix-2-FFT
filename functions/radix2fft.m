%Stockham radix-2 https://github.com/miracle2121/hpc12-fp-rl1609

function result = radix2fft(signalIn)
	%Complex multiplication
	function ret = mul(ca,cb)
		ret.x = ca.x*cb.x-ca.y*cb.y;
		ret.y = ca.x*cb.y+ca.y*cb.x;
	end

	%Twiddle
	function c = twiddle(aa,alp)
		sn = sin(alp);
		cs = cos(alp);
		temp.x = cs;
		temp.y = sn;
		atemp.x = aa(1);
		atemp.y = aa(2);
		c = mul(atemp,temp);
	end
	
	%DFT-2
	function [retA, retB] = DFT2(da,db)
		retB.x = da.x-db.x;
		retB.y = da.y-db.y;
		retA.x = da.x+db.x;
		retA.y = da.y+db.y;
	end

	%The butterfly function for RADIX-2
	function [ina, inb] = butterfly(ina, inb, gid, p, t)
		k = bitand(gid,(p - 1));
		j = bitshift(gid-k,1) + k; 
		alpha = -pi * double(k) / double(p);
%         keyboard;
		u0.x = ina.x(gid+1);
		u0.y = ina.y(gid+1);
		u1 = twiddle([ina.x(gid+1+t),ina.y(gid+1+t)],alpha);
		
		[u0,u1] = DFT2(u0,u1);
%         keyboard;
		inb.x(j+1) = u0.x;
		inb.y(j+1) = u0.y;
		inb.x(j+1+p) = u1.x;
		inb.y(j+1+p) = u1.y;
        
%         inc.x(j+1) = gid;
% 		inc.y(j+1) = gid+t;
% 		inc.x(j+1+p) = j;
% 		inc.y(j+1+p) = j+p;
        
%         inc.x(gid+1) = 2*j;
% 		inc.y(gid+1) = 2*j+1;
% 		inc.x(gid+t+1) = 2*j+2*p;
% 		inc.y(gid+t+1) = 2*j+2*p+1;
        
%         inc.x(gid+1) = gid;
% 		inc.y(gid+1) = gid+t;
% 		inc.x(gid+t+1) = j;
% 		inc.y(gid+t+1) = j+p;
        
%         keyboard;
		
	end

	%Handle data as complex values
	a.x = signalIn;
	a.y = zeros(size(signalIn,1),size(signalIn,2));
	b.x = zeros(size(signalIn,1),size(signalIn,2));
	b.y = zeros(size(signalIn,1),size(signalIn,2));
	t = length(a.x)/2;
	p=1;
	%keyboard;
	while p < t*2
%         alpha = zeros(t,1);
%         ind = struct();
		for i = 0:(t-1)
			[a, b] = butterfly(a,b,uint32(i),uint32(p),uint32(t));
            
        end
%         disp(sprintf('P%02d',p));
        ta = [a.x; a.y]';
        tb= [b.x; b.y]';
%         ti= [ind.x; ind.y]';
        

%         res = sqrt(sum([a.x; a.y]'.^2,2))./(length(signalIn/2));
        res = sqrt(sum(ta.^2,2))./(length(signalIn)/2);
        keyboard;
		p = p*2;
		temp = a;
		a = b;
		b = temp;
        
        
        
	end
	result = complex(a.x,a.y);
end