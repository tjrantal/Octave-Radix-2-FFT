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

		u0.x = ina.x(gid+1);
		u0.y = ina.y(gid+1);
		u1 = twiddle([ina.x(gid+1+t),ina.y(gid+1+t)],alpha);
		
		[u0,u1] = DFT2(u0,u1);

		inb.x(j+1) = u0.x;
		inb.y(j+1) = u0.y;
		inb.x(j+1+p) = u1.x;
		inb.y(j+1+p) = u1.y;
		
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
		for i = 0:(t-1)
			[a, b] = butterfly(a,b,uint32(i),uint32(p),uint32(t));
			
		end
		p = p*2;
		temp = a;
		a = b;
		b = temp;
	end
	result = complex(a.x,a.y);
end