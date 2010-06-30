

function createblock(pos)
{
	var obj = new Object({
			body: 'cube',
			mesh: 'saronacube.obj',
			texture: 'testgraphics.png',
			mass: 1,
			position: pos,
			bodyScale: 2.5
	});	

	return obj;
}

function kill_later(object, timeout)
{
	setTimeout(timeout, function(o){
		return function(){
			o.kill();
		}
	}(object));
}

angle = 0;
function loop()
{
	var obj1 = createblock([Math.sin(angle)*50,Math.cos(angle)*50, 200])
	var obj2 = createblock([Math.sin(angle)*-50,Math.cos(angle)*-50, 200])

	kill_later(obj1, 20000);
	kill_later(obj2, 20000);

	angle += Math.PI/180.0 * 10;
	setTimeout(500, loop);
}

function create_ground()
{
	// Create ground
	
	var ground = new Object({
		body: 'trainmap_mask.obj',
		mesh: 'trainmap.obj',
		texture: 'maailma.png',
		meshScale: 64,
		bodyScale: 64, 
		rotation: [0,0,0,1],
		position: [0,0,50]
	});
}

object = null;

function level_start()
{
	create_ground();
	loop();

	print('Welcome to testimappi!');
	return;
	for(var i = 0; i < scene.players.length; i++)
	{
		var p = scene.players[i];
		// Bind a key for each player in the scene.
		
		var object = createblock(
			new Vector(0,0,500)
		);

		p.cameraFollow(object, 50);
		
		p.bind("keydown", "space", function(obj){return function(){			
				obj.push(0,0,10);
			};}(object)
		);
		p.bind("keydown", "a", function(obj){return function(){			
				obj.push(0,0,-10);
			};}(object)
		);
		p.bind("keydown", "left", function(obj){return function(){			
				obj.push(10,0,0);
			};}(object)
		);
		p.bind("keydown", "right", function(obj){return function(){			
				obj.push(-10, 0,0);
			};}(object)
		);
		p.bind("keydown", "up", function(obj){return function(){			
				obj.push(0, 10,0);
			};}(object)
		);
		p.bind("keydown", "down", function(obj){return function(){			
				obj.push(0, -10,0);
			};}(object)
		);
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




