function result = naive_fft(signalIn)
	%Set constants
	n = length(signalIn);
	ph = -2*pi/length(n);

	%Handle data as complex values
	a.x = signalIn;
	a.y = zeros(size(signalIn,1),size(signalIn,2));
	b.x = zeros(size(signalIn,1),size(signalIn,2));
	b.y = zeros(size(signalIn,1),size(signalIn,2));
	for gid = 1:n
		res.x = 0;
		res.y = 0;
		for k = 1:n
			t.x = a.x(k);
			t.y = a.y(k);
			val = ph*(k-1)*(gid-1);
			cs = cos(val);
			sn = sin(val);
			res.x = res.x+t.x*cs-t.y*sn;
			res.y = res.y+t.y*cs+t.x*sn;
		end
		b.x(gid) = res.x;
		b.y(gid) = res.y;
	end
	result = complex(b.x,b.y);	
end
