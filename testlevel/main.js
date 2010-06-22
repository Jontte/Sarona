

function createblock(pos)
{
	return new Object({
//			body: 'saronacube.obj',
			body: 'cube',
			mesh: 'saronacube.obj',
			texture: 'testgraphics.png',
			mass: 1,
			position: pos
	});	
}

object = null;

function level_start()
{
	// Create ground
	
	var ground = new Object({
		body: 'maailma.obj',
		mesh: 'maailma.obj',
		texture: 'testgraphics.png',
		position: [0,0,0],
		meshScale: 8,
		bodyScale: 8, 
		rotation: [90,0,0]
	});

	print('Welcome to testimappi!');

/*	for (var i = 0; i < 10; i++)
	{
		var obj = createblock(
			new Vector(0,0,i*8+5)
		);
		
		obj.push(0,0,0);
	}*/
	
	for(var i = 0; i < scene.players.length; i++)
	{
		var p = scene.players[i];
		// Bind a key for each player in the scene.
		
		object = createblock(
			new Vector(0,0,500)
		);
		
		p.bind("keydown", "space", function(){			
			//p.cameraFollow(object, 50);
			object.push(0,0,10);
		})
		p.bind("keydown", "left", function(){			
			//p.cameraFollow(object, 50);
			object.push(10,0,0);
		})
		p.bind("keydown", "right", function(){			
			//p.cameraFollow(object, 50);
			object.push(-10,0,0);
		})
		p.bind("keydown", "up", function(){			
			//p.cameraFollow(object, 50);
			object.push(0, 10,0);
		})
		p.bind("keydown", "down", function(){			
			//p.cameraFollow(object, 50);
			object.push(0, -10,0);
		})
	}
	
	
	/*
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
				
				
				var obj = new Object( {
					shapes : [{
						shape: 'cube',
						length: 5,
						mass: 1
					}]
				}, [(0.5+x+xd)*5, (0.5+y+yd)*5, (kerros+0.5)*5]);
				
				obj.push(0,0,1); 
			}
		}
	}*/
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




