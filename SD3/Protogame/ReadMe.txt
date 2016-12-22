Assignment Name: Tangent Spaces and Custom Effects
Assignment Number: 05
Name: Benjamin Gibson
Class: Software Development III
Instructor: Chris Forseth


//-----------------------------------------------------------------------------	
Known Issues
	1. Because the quick fix to "toggle" lights is setting powers at min/max to zero, the directional light (using no attenuation factor) can't be toggled off.
	2. Despite attempting to do some custom shader effects, I didn't have enough time to get one in, so only the 4 from the previous assignment can be seen.
		(Though there is one leftover from all those failed attempts, hiding inside the six-cube figure.)
	
Build Notes
	.exe will crash if it cannot find these directories from its working directory $(SolutionDir)Run_$(PlatformName)/ under Project Properties:
	Data/Fonts
	Data/Shaders
	Data/Images

	
//-----------------------------------------------------------------------------
Light Position Method
	Arrayed down the red world-forward x-axis by default, movable with below documented commands and controls.
	
Relevant Console Commands for Testing (use Tab, Up/Down keys to not type it all out each time, will show usage on execution with wrong args)
	PilotLight
		Swaps control schemes as explained below to let you reposition a light (translation follows same camera-angle-relative math as flycam). 
		See indices in A6 Scene Layout for types.
	PilotLight (no arguments)
		Exits the light piloting mode.
	ToggleLight
		Will turn on or off a single light, as well as its rendered orb representation. 
		See indices in A6 Scene Layout for types.
	ToggleLights 
		Will turn on or off all lights, e.g. to see effects of each one.
	SetPilotedLightColor
	SetPilotedLightDirection (note my basis has x-forward, y-left, z-up when using this)
	SetPilotedLightMinDistance
	SetPilotedLightMaxDistance
	SetPilotedLightPowerAtMinDistance
	SetPilotedLightPowerAtMaxDistance
	SetPilotedLightInnerAngleCosine
	SetPilotedLightOuterAngleCosine
	SetPilotedLightPowerInsideInnerAngle
	SetPilotedLightPowerOutsideOuterAngle
	SetPilotedLightIsDirectional
	SetPilotedLightIsSpotLight
	
	
//-----------------------------------------------------------------------------
A6 Scene Layout 
	1. Multiple Lights by Index
		Types
		- 0: Global Point Light, Left
		- 1: Global Directional Light, Backward
		- 2-8: Local Point Lights, in order Forward Backward Left Right Up Down Down+Forward.
		- 9-15: Local Spot Lights, same direction order as local point lights.
		All go from x1.0 full power at min distance/angle to x0.0 at max distance/angle for now (ignored in global cases).
	2. Objects affected by multiple lights: 
		- One sphere along the blue world-up z-axis.
		- The multi-cube structure from lecture demo along the green world-left y-axis.
		- One quad down the red world-forward x-axis.
	
A5 Scene Layout ('O' to see basis)
	1. TBN with Emissive:
		- Quad: straight ahead, on the red +x axis.
		- Cube: left, on the green +y axis.
		- Sphere: up, on the blue +z axis. (Between two smaller spheres for comparisons to shader effects, esp. triplanar.)
	2. Custom Effects:
		- Triplanar: very top of the scene on +z axis, distinguished by RGB diffuse textures applied along XYZ.
		- Parallax: very bottom of the scene, in -z direction. Messy from using a self-made heightmap, but the UVs are being offset by eye pos.
		- Dissolve: right, in the -y direction. Half-completes before the wrapping timer loops back, uses parallax's brick map.
		- Projected Texture: immediately above on +z axis. Can see the image of hexagons on one sphere side, but that's all.
			(Turned the sampler setting off GL_REPEAT so it didn't go all over everywhere.)
			
	
//-----------------------------------------------------------------------------
Flycam Mode Controls
	1. Keyboard
		O: Show/Hide Origin Axes (REMOVE GLUPERSPECTIVE?)
		Tilde: Show/Hide Dev Console
		W: Flycam Forward
		S: Flycam Backward
		A/D: Flycam Strafe
		Spacebar: Flycam Up
		X: Flycam Down
		Hold Shift: Flycam Speed-up (x8, editable as FLYCAM_SPEED_SCALAR in GameCommon.hpp)
		ESC: Exit
		F1: Toggle Debug Info
		1-3: Toggle Fonts (InputSans, Arial, Papyrus)
	2. Mouse: Look
	
Console Mode Controls
	Tab: Auto-complete (use with blank input to cycle through all commands)
	Enter: Execute/Close
	Escape: Clear/Close
	PageUp/PageDown/Scroll: Scroll Output (necessary for 'help' command, some are pushed off-screen now)
	Left/Right: Move Caret
	Home/End: Move Caret
	Delete: Delete
	Up/Down: Command History
	
Light Piloting Controls
	WASD/Space/X: Translate Selected Light
	IJKL/U/O: Translate Flycam
	Begin Mode: Enter "PilotLight <0-15>" command in console mode.
	Leave Mode: Re-enter "PilotLight" command in console mode (no arguments).
	
	
//-----------------------------------------------------------------------------
Resource Attributions
	Fonts		BMFont Software
	Input Font	http://input.fontbureau.com/
	Images		Regan Carver, Anthony Cloudy