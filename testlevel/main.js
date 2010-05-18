
var oPallo = {
	shapes : [{
		shape: 'sphere',
		radius: 1,
		mass: 1
	}]
}

var oTaso = {
	shapes : [{
		shape: 'plane',
		normal: [0,0,1]
	}]
}

var oKuutio = {
	shapes : [{
		shape: 'cube',
		length: 5,
		mass: 1
	}]
}


function level_start()
{
	print('Welcome to testimappi!');

//	scene.gravity = [0,0,-9.80665];
	
//	var pallo = scene.create(oPallo, [0,0,10]);
	var taso = scene.create(oTaso, [0,0,0]);
	
	
	for(var kerros = 0 ; kerros < 20; kerros++)
	{
		for(var x = 0 ; x < 10; x++)
		for(var y = 0 ; y < 10; y++)
		{
			if((x == 0 || x == 9 || y == 0 || y == 9))
			{
				var xd = 0;
				var yd = 0;
				
				if(kerros % 2 == 0)
				{
					if(x == 0)
						yd = 0.5;
					if(x == 9)
						yd = -0.5;
					if(y == 0)
						xd = -0.5;
					if(y == 9)
						xd = 0.5;
				}
				
				
				scene.create( {
					shapes : [{
						shape: 'cube',
						length: 5,
						mass: 1
					}]
				}, [(0.5+x+xd)*5, (0.5+y+yd)*5, (kerros+0.5)*5]);
			}
		}
	}
	scene.create( {
		shapes : [{
			shape: 'cube',
			length: 50,
			mass: 6000
		}]
	}, [25,25,125]);
}

function level_end()
{
	printf('Thanks for playing!');
}




