
function level_start()
{
	//print('Welcome to testimappi!');

/*

	var cubeBody = new CubeBody({
		length : 5
	});
	var cubeMesh = new CubeMesh({
		length : 5,
		texture: 'testgraphics.png'
	});
	var obj = new Object({
		body: [cubeBody],
		mesh: [cubeMesh],
		mass: 1,
		
		position	: [0,0,0],
		rotation	: [0,0,0],
		velocity	: [0,0,10],
		omega		: [0,0,0],
		
		class: 'foobox'
	});
*/	

for (var i = 0; i < 10; i++)
{
	var obj = new Object({
		body: 'saronacube.obj',
		mesh: 'saronacube.obj',
		texture: 'saronacube.png',
		mass: 1,
		position: [0,0,i*8+5]
	});
	
	obj.push(0,0,0);
}
	
	
	
	var maxkerroksia = 5;
	for(var kerros = 0 ; kerros < maxkerroksia; kerros++)
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
				
				/*
				var obj = new Object( {
					shapes : [{
						shape: 'cube',
						length: 5,
						mass: 1
					}]
				}, [(0.5+x+xd)*5, (0.5+y+yd)*5, (kerros+0.5)*5]);
				
				obj.push(0,0,1); */
			}
		}
	}
	/*
	for (var i = 0 ; i < 200; i ++)
	{
		scene.create({shapes:[{shape:'cube',length:5,mass:1}]},[0,0,(i+0.5)*5]);
	} */
	
	/*
	scene.create( {
		shapes : [{
			shape: 'cube',
			length: 50,
			mass: 500
		}]
	}, [25,25,200]);
	*/
}

function level_end()
{
	printf('Thanks for playing!');
}




