
function createblock(pos)
{
	var obj = createObject({
			body: 'cube',
			//mesh: 'saronacube.obj',
			texture: 'testgraphics.png',
			mass: 1,
			position: pos,
			bodyScale: 3,
			meshScale: 3
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
	var r = 10;
	var obj1 = createblock([Math.sin(angle)*r,Math.cos(angle)*r, 220]);
	var obj2 = createblock([Math.sin(angle)*-r,Math.cos(angle)*-r, 220]);

	obj1.push(0,0,-100);
	obj2.push(0,0,-100);

	kill_later(obj1, 60 * 1000);
	kill_later(obj2, 60 * 1000);

	angle += Math.PI/180.0 * 30;
	setTimeout(500, loop);
}

function create_car(pos)
{
	pos[2] += 5;
	var body = createObject({
		mesh: 'saronacube.obj',
		body: 'saronacube.obj',
		texture: 'saronacube.png',
		bodyScale: 2.5,
		meshScale: 2.5,
		position: pos,
		mass: 1
	});
	pos[2]-=5;

	var tires = [
		[-2.5,-2.5-1,-2.5, [1 ,0,0,1]],
		[-2.5, 2.5+1,-2.5, [-1,0,0,1]],
		[ 2.5,-2.5-1,-2.5, [1 ,0,0,1]],
		[ 2.5, 2.5+1,-2.5, [-1,0,0,1]]
	];
	for(var i = 0; i < 4; i++)
	{
		tires[i][0] += pos[0];
		tires[i][1] += pos[1];
		tires[i][2] += pos[2];

		tires[i][2] += 5;
		var tire = createObject({
			mesh: 'tire.obj',
			body: 'tire.obj',
			texture: 'tire.png',
			position: tires[i],
			rotation: tires[i][3],
			mass: 1,
			bodyScale: 2,
			meshScale: 2
		});

		try
		{
			createConstraint({
				type: 'hinge',
				objects: [body, tire],
				position: tires[i],
				axis: [0,1,0]
			});
		}
		catch(e)
		{
			print('exception: ' + e);
		}
	}
	return body;
}

function create_ground()
{
	// Create ground

	var ground = createObject({
		body: 'maailma.obj',//'pohja.obj', //'hill_heightmap.png',
		mesh: 'maailma.obj',//'pohja.obj', //'hill_heightmap.png',
		texture: 'maailma.png',//'tekstuuri.png', //'hill_texture.png',
		meshScale: 5,
		bodyScale: 5,
		rotation: [1,0,0,1],
		position: [0,0,0]
	});
}

object = null;

function level_start()
{
	print('Welcome to testimappi!');

/*	for(var i = 0 ; i < 100; i++)
	{
		var obj = createObject({
			position : [0,0,1.5+i*3],
			body : 'cube',//'saronacube.obj',
			//mesh: 'saronacube.obj',
			texture: 'saronacube.png',
			mass : 1,
			bodyScale: 3,
			meshScale: 3
        });
	}
*/
	create_ground();
//	loop();


	for(var i = 0; i < scene.players.length; i++)
	{
		var p = scene.players[i];
		// Bind a key for each player in the scene.

		var obj = create_car([0,0,50]);

		p.cameraFollow(obj, 50);

		p.bind("keydown", "space", function(){
				obj.push(0,0,10);
		});
		p.bind("keydown", "left", function(){
				obj.push(10,0,0);
		});
		p.bind("keydown", "right", function(){
				obj.push(-10,0,0);
		});
		p.bind("keydown", "up", function(){
				obj.push(0,10,0);
		});
		p.bind("keydown", "down", function(){
				obj.push(0,-10,0);
		});
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


				var obj = createObject( {
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




