-- puppet.lua
-- A simplified puppet without posable joints, but that
-- looks roughly humanoid.

-- Geom = {node=nil, mesh=nil}
--
-- function Geom:new(shape, name)  -- create an instance of class A
--   self.node=gr.node(name + 'Node')
--   self.mesh=gr.mesh(shape, name)
-- end
--
-- function Geom:print()
--   print (self.mesh)
-- end

rootnode = gr.node('root')
rootnode:rotate('y', 0.0)
rootnode:scale( 0.25, 0.25, 0.25 )
rootnode:translate(0.0, 0.0, 0.0)

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)
black = gr.material({0.1, 0.1, 0.1}, {0.1, 0.1, 0.1}, 10)
skin = gr.material({255/255, 204/255, 153/255}, {0.3, 0.3, 0.3}, 30)

yellowHair = gr.material({255/255, 204/255, 0.0}, {0.1, 0.1, 0.1}, 10)

mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25)

scene_root = gr.node('root')

dragon = gr.mesh( 'dragon.obj', 'dragon.obj' )
dragon:set_material(mat4)
scene_root:add_child(dragon)

-- TORSO
-- height, width, depth
uTorsoH = 0.8
uTorsoW = 1.0
uTorsoD = 0.7

uTorsoNode = gr.node('uTorsoNode')
rootnode:add_child(uTorsoNode)

uTorso = gr.mesh('cube', 'uTorso')
uTorsoNode:add_child(uTorso)
uTorso:set_material(black)
uTorso:scale(uTorsoW,uTorsoH,uTorsoD);


lTorsoH = 0.3
lTorsoW = 0.6
lTorsoD = 0.4

lTorsoNode = gr.node('lTorsoNode')
uTorsoNode:add_child(lTorsoNode)
lTorsoNode:translate(0.0,-uTorsoH/2 - lTorsoH/2, 0.0)

lTorso = gr.mesh('cube', 'lTorso')
lTorsoNode:add_child(lTorso)
lTorso:set_material(black)
lTorso:scale(lTorsoW,lTorsoH,lTorsoD);


-- HIPS
hipsJoint = gr.joint('hipsJoint', {0,0,0}, {-30,25,30})
hipsJoint:translate(0.0, -lTorsoH/2, 0.0)
lTorsoNode:add_child(hipsJoint)

hipsH = 0.2
hipsW = 0.6
hipsD = 0.4

hipsNode = gr.node('hipsNode')
hipsNode:translate(0.0, - hipsH/2, 0.0)
hipsJoint:add_child(hipsNode)

hips = gr.mesh('cube', 'hips')
hipsNode:add_child(hips)
hips:set_material(blue)
hips:scale(hipsW,hipsH,hipsD);


-- THIGHS
thighH = 0.35
thighW = 0.25
thighD = 0.25

lLegJoint = gr.joint('lLegJoint', {-45, -30, 45}, {0, 0, 0})
lLegJoint:translate(hipsW/2-thighW/2, -hipsH/2, 0.0)
hipsNode:add_child(lLegJoint)

lThighNode = gr.node('lThighNode')
lLegJoint:add_child(lThighNode)
lThighNode:translate(0.0, -thighH/2, 0.0)

lThigh = gr.mesh('cube', 'lThigh')
lThighNode:add_child(lThigh)
lThigh:set_material(blue)
lThigh:scale(thighW, thighH, thighD)


rLegJoint = gr.joint('rLegJoint', {-45, 0, 45}, {0, 0, 0})
rLegJoint:translate(-hipsW/2+thighW/2, -hipsH/2, 0.0)
hipsNode:add_child(rLegJoint)

rThighNode = gr.node('rThighNode')
rLegJoint:add_child(rThighNode)
rThighNode:translate(0.0, -thighH/2, 0.0)

rThigh = gr.mesh('cube', 'rThigh')
rThighNode:add_child(rThigh)
rThigh:set_material(blue)
rThigh:scale(thighW, thighH, thighD)


-- CALVES
calfH = 0.35
calfW = 0.18
calfD = 0.18

lKneeJoint = gr.joint('lKneeJoint', {0, 50, 70}, {0, 0, 0})
lKneeJoint:translate(0.0, -thighH/2, 0.0)
lThighNode:add_child(lKneeJoint)

lCalfNode = gr.node('lCalfNode')
lCalfNode:translate(0.0, -calfH/2, 0.0)
lKneeJoint:add_child(lCalfNode)

lCalf = gr.mesh('cube', 'lCalf')
lCalfNode:add_child(lCalf)
lCalf:set_material(blue)
lCalf:scale(calfW, calfH, calfD)


rKneeJoint = gr.joint('rKneeJoint', {0, 0, 75}, {0, 0, 0})
rKneeJoint:translate(0.0, -thighH/2, 0.0)
rThighNode:add_child(rKneeJoint)

rCalfNode = gr.node('rCalfNode')
rCalfNode:translate(0.0, -calfH/2, 0.0)
rKneeJoint:add_child(rCalfNode)

rCalf = gr.mesh('cube', 'rCalf')
rCalfNode:add_child(rCalf)
rCalf:set_material(blue)
rCalf:scale(calfW, calfH, calfD)

-- FEET
footH = 0.12
footW = 0.18
footD = 0.30

lFootJoint = gr.joint('lFootJoint', {-75, 0, 0}, {0, 0, 0})
lFootJoint:translate(0.0, -thighH/2, 0.0)
lCalfNode:add_child(lFootJoint)

lFootNode = gr.node('lFootNode')
lFootNode:translate(0.0, -footH/2, footD/2-calfD/2)
lFootJoint:add_child(lFootNode)

lFoot = gr.mesh('cube', 'lFoot')
lFootNode:add_child(lFoot)
lFoot:set_material(black)
lFoot:scale(footW, footH, footD)


rFootJoint = gr.joint('rFootJoint', {-75, 0, 0}, {0, 0, 0})
rFootJoint:translate(0.0, -thighH/2, 0.0)
rCalfNode:add_child(rFootJoint)

rFootNode = gr.node('rFootNode')
rFootNode:translate(0.0, -footH/2, footD/2-calfD/2)
rFootJoint:add_child(rFootNode)

rFoot = gr.mesh('cube', 'rFoot')
rFootNode:add_child(rFoot)
rFoot:set_material(black)
rFoot:scale(footW, footH, footD)


-- NECK
neckJoint = gr.joint('neckJoint', {-30, 7, 30}, {-90, 30, -90})
neckJoint:translate(0.0,uTorsoH/2,0.0)
uTorsoNode:add_child(neckJoint)

neckH = 0.25
neckW = 0.25
neckD = 0.25

neckNode = gr.node('neckNode')
neckJoint:add_child(neckNode)
neckNode:translate(0.0, neckH/2, 0.0)

neck = gr.mesh('cube', 'neck')
neckNode:add_child(neck)
neck:set_material(skin)
neck:scale(neckW, neckH, neckD)


-- HEAD
headH = 0.5
headW = 0.5
headD = 0.5

headNode = gr.node('headNode')
neckNode:add_child(headNode)
headNode:translate(0.0, neckH/2 + headH/2, 0.0)

head = gr.mesh('cube', 'head')
headNode:add_child(head)
head:scale(headW,headH,headD)
head:set_material(skin)


-- HAIR
hairH = 0.4
hairW = 0.6
hairD = 0.9

hairNode = gr.node('hairNode')
headNode:add_child(hairNode)
hairNode:translate(0.0, headH/2 + hairH/2, hairD/2-headD/2)

hair = gr.mesh('cube', 'hair')
hairNode:add_child(hair)
hair:scale(hairW,hairH,hairD)
hair:set_material(yellowHair)


-- EYES
leftEye = gr.mesh('cube', 'leftEye')
head:add_child(leftEye)
leftEye:scale(0.35, 0.3, 0.1)
leftEye:translate(-0.23, 0.1, 0.5)
leftEye:set_material(black)

rightEye = gr.mesh('cube', 'rightEye')
head:add_child(rightEye)
rightEye:scale(0.35, 0.3, 0.1)
rightEye:translate(0.23, 0.1, 0.5)
rightEye:set_material(black)


-- SHOULDERS
shoulderH = 0.35
shoulderW = 0.35
shoulderD = 0.35

lShoulderJoint = gr.joint('lShoulderJoint', {-90, -70, 90}, {0, 0, 0})
lShoulderJoint:translate(uTorsoW/2, uTorsoH/2-shoulderH/2, 0.0)
uTorsoNode:add_child(lShoulderJoint)

lShoulderNode = gr.node('lShoulderNode')
lShoulderJoint:add_child(lShoulderNode)
lShoulderNode:translate(shoulderW/2, 0.0, 0.0)

lShoulder = gr.mesh('cube', 'lShoulder')
lShoulderNode:add_child(lShoulder)
lShoulder:scale(shoulderW,shoulderH,shoulderD)
lShoulder:set_material(black)


rShoulderJoint = gr.joint('rShoulderJoint', {-90, 0, 90}, {0, 0, 0})
rShoulderJoint:translate(-uTorsoW/2, uTorsoH/2-shoulderH/2, 0.0)
uTorsoNode:add_child(rShoulderJoint)

rShoulderNode = gr.node('rShoulderNode')
rShoulderJoint:add_child(rShoulderNode)
rShoulderNode:translate(-shoulderW/2, 0.0, 0.0)

rShoulder = gr.mesh('cube', 'rShoulder')
rShoulderNode:add_child(rShoulder)
rShoulder:scale(shoulderW,shoulderH,shoulderD)
rShoulder:set_material(black)


-- UPPER ARMS
upperArmH = 0.35
upperArmW = 0.35
upperArmD = 0.35

lUpperArmNode = gr.node('lUpperArmerNode')
lShoulderNode:add_child(lUpperArmNode)
lUpperArmNode:translate(0.0, -shoulderH/2-upperArmH/2, 0.0)

lUpperArm = gr.mesh('cube', 'lUpperArm')
lUpperArmNode:add_child(lUpperArm)
lUpperArm:scale(upperArmW,upperArmH,upperArmD)
lUpperArm:set_material(skin)


rUpperArmNode = gr.node('rUpperArmerNode')
rShoulderNode:add_child(rUpperArmNode)
rUpperArmNode:translate(0.0, -shoulderH/2-upperArmH/2, 0.0)

rUpperArm = gr.mesh('cube', 'rUpperArm')
rUpperArmNode:add_child(rUpperArm)
rUpperArm:scale(upperArmW,upperArmH,upperArmD)
rUpperArm:set_material(skin)


-- LOWER ARMS
lowerArmH = 0.62
lowerArmW = 0.27
lowerArmD = 0.27

lElbowJoint = gr.joint('lElbowJoint', {-130, -120, 0}, {0, 0, 0})
lElbowJoint:translate(0.0, -upperArmH/2, 0.0)
lUpperArmNode:add_child(lElbowJoint)

lLowerArmNode = gr.node('lLowerArmNode')
lElbowJoint:add_child(lLowerArmNode)
lLowerArmNode:translate(0.0, -lowerArmH/2, 0.0)

lLowerArm = gr.mesh('cube', 'lLowerArm')
lLowerArmNode:add_child(lLowerArm)
lLowerArm:scale(lowerArmW,lowerArmH,lowerArmD)
lLowerArm:set_material(skin)


rElbowJoint = gr.joint('rElbowJoint', {-90, 0, 90}, {0, 0, 0})
rElbowJoint:translate(0.0, -upperArmH/2, 0.0)
rUpperArmNode:add_child(rElbowJoint)

rLowerArmNode = gr.node('rLowerArmNode')
rElbowJoint:add_child(rLowerArmNode)
rLowerArmNode:translate(0.0, -lowerArmH/2, 0.0)

rLowerArm = gr.mesh('cube', 'rLowerArm')
rLowerArmNode:add_child(rLowerArm)
rLowerArm:scale(lowerArmW,lowerArmH,lowerArmD)
rLowerArm:set_material(skin)


-- HANDS
handH = 0.3
handW = 0.05
handD = 0.27

lHandJoint = gr.joint('lHandJoint', {-70, 0, 70}, {0, 0, 0})
lHandJoint:translate(0.0, -lowerArmH/2, 0.0)
lLowerArmNode:add_child(lHandJoint)

lHandNode = gr.node('lHandNode')
lHandJoint:add_child(lHandNode)
lHandNode:translate(0.0, -handH/2, 0.0)

lHand = gr.mesh('cube', 'lHand')
lHandNode:add_child(lHand)
lHand:scale(handW,handH,handD)
lHand:set_material(skin)


rHandJoint = gr.joint('rHandJoint', {-70, 0, 70}, {0, 0, 0})
rHandJoint:translate(0.0, -lowerArmH/2, 0.0)
rLowerArmNode:add_child(rHandJoint)

rHandNode = gr.node('rHandNode')
rHandJoint:add_child(rHandNode)
rHandNode:translate(0.0, -handH/2, 0.0)

rHand = gr.mesh('cube', 'rHand')
rHandNode:add_child(rHand)
rHand:scale(handW,handH,handD)
rHand:set_material(skin)


return rootnode
