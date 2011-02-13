
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
	var ret = {};
	pos[2] += 3.5;
	var body = createObject({
		texture: 'saronacube.png',
		bodyScale: 2.5,
		meshScale: 2.5,
		position: pos,
		mass: 20
	});
	pos[2]-=3.5;

	ret.body = body;

	body.restitution = 0;
	body.friction = 1.0;

	var width = 2;
	var length = 1;
	var tires = [
		[-2.5-length,-2.5-width,-2.5, [1 ,0,0,1], 1],
		[-2.5-length, 2.5+width,-2.5, [-1,0,0,1], -1],
		[ 2.5+length,-2.5-width,-2.5, [1 ,0,0,1], 1],
		[ 2.5+length, 2.5+width,-2.5, [-1,0,0,1], -1]
	];

	ret.tires = [];
	ret.constraints = [];
	for(var i = 0; i < 4; i++)
	{
		tires[i][0] += pos[0];
		tires[i][1] += pos[1];
		tires[i][2] += pos[2];

		tires[i][2] += 5;
		var tire = createObject({
			mesh: 'tire.obj',
			body: 'cylinder',//'tire.obj',
			texture: 'tire.png',
			position: tires[i],
			rotation: tires[i][3],
			mass: 0.1,
			bodyScale: 3,
			meshScale: 2
		});

		tire.restitution = 0.0;
		tire.friction = 1.90;
		tires.push(tire);

		try
		{
			var c = createConstraint({
				type: 'hinge',
				objects: [body, tire],
				position: tires[i],
				axis: [0,1,0]
			});
//			c.setMotor(false, 0, 0);
			ret.constraints.push(c);
		}
		catch(e)
		{
			print('exception: ' + e);
		}
	}
	return ret;
}

function create_ground()
{
	// Create ground

	var ground = createObject({
		body: 'maailma.obj',//'pohja.obj', //'hill_heightmap.png',
		mesh: 'maailma.obj',//'pohja.obj', //'hill_heightmap.png',
		texture: 'maailma.png',//'tekstuuri.png', //'hill_texture.png',
		meshScale: 50,
		bodyScale: 50,
		rotation: [1,0,0,1],
		position: [0,0,0]
	});
}

object = null;

function level_start()
{
	print('Welcome to testimappi!');

	create_ground();
//	loop();


	for(var i = 0; i < scene.players.length; i++)
	{
		var p = scene.players[i];
		// Bind a key for each player in the scene.

		var car = create_car([0,0,310]);

		p.setCamera({
			type: 'chase',
			target: car.body,
			distance: 50,
			height: 30,
			follow_pitch: false
		});

		var states = {
			left : false,
			right: false,
			up   : false,
			down : false,
			Z	 : false
		};

		function key_update(major, minor)
		{
			return function()
			{
				states[minor] = (major=="keydown")?true:false;

				var speed = 50;
				var strength = 1;

				var left = 0;
				var right = 0;

				if(states.up && !states.down)
				{
					left = states.left?0:1;
					right = states.right?0:1;
				}
				else if(states.down && !states.up)
				{
					left = states.left?0:-1;
					right = states.right?0:-1;
				}
				else if(states.left && !states.right)
				{
					left = -1; right = 1;
				}
				else if(states.right && !states.left)
				{
					left = 1; right = -1;
				}

				if(states.Z)
				{
					left = 0;
					right = 0;
				}

				car.constraints[0].setMotor(right != 0 || states.Z, speed * right, strength);
				car.constraints[2].setMotor(right != 0 || states.Z, speed * right, strength);

				car.constraints[1].setMotor(left != 0 || states.Z, speed * left, strength);
				car.constraints[3].setMotor(left != 0 || states.Z, speed * left, strength);
			};
		};

		var states = ["keydown","keyup"];
		var keys = ["down","up","left","right", "Z"];
		for(var b = 0; b < 2; b++)
		{
			for(var a = 0; a < keys.length; a++)
			{
				p.bind(states[b], keys[a], key_update(states[b],keys[a]));
			}
		}
		p.bind("keydown", "space", function(){car.body.push(0,0,200);});

		/*p.bind("keydown", "left", function(){
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
		});*/
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




