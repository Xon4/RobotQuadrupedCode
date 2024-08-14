import numpy as np
import math
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import time

#declare constants
STEP_LENGTH = 50
STEP_HEIGHT = 30
BACK_STEP_DEPTH = 5

SIDE_STEP_LENGTH = 20
SIDE_STEP_HEIGHT = 20
SIDE_BACK_STEP_DEPTH = 5

GROUND_DEPTH = 200
GAIT_SPEED = 80

FR_abadPos = [50, 150, 0]
FL_abadPos = [-50, 150, 0]
BR_abadPos = [50, -150, 0]
BL_abadPos = [-50, -150, 0]


L1 = 120
L2 = 120
L3 = 50

def drawBody():
    ax.plot([0, 0], [-150, 150], [0, 0], color="blue")
    ax.plot([FR_abadPos[0], FL_abadPos[0]], [FR_abadPos[1], FL_abadPos[1]], [FR_abadPos[2], FL_abadPos[2]], color="blue")
    ax.plot([BR_abadPos[0], BL_abadPos[0]], [BR_abadPos[1], BL_abadPos[1]], [BR_abadPos[2], BL_abadPos[2]], color="blue")

class Trajectory:
    def __init__(self, leg_init):
        self.leg = leg_init
        self.dir = "forward"
        if self.leg == "FR" or self.leg == "BL":
            self.phase = "swing"
            self.x = 0
            self.y = 0
            self.z = 0
        elif self.leg == "FL" or self.leg == "BR":
            self.phase = "support"
            self.x = 0
            self.y = STEP_LENGTH
            self.z = 0
        self.interpolations = 20
    def interpolate(self, speed): 
        # speed should be between 0 and 100
        if self.dir == "forward":
            rate = STEP_LENGTH / (self.interpolations - (speed / 100.0 * 18))
            if self.phase == "swing":
                self.y += rate
                if self.y >= STEP_LENGTH:
                    self.y = STEP_LENGTH
                    self.phase = "support"
                self.z = STEP_HEIGHT * math.sin(2 * math.pi / (STEP_LENGTH * 2) * self.y) - GROUND_DEPTH
            elif self.phase == "support":
                self.y -= rate
                if self.y <= 0:
                    self.y = 0
                    self.phase = "swing"
                self.z = -BACK_STEP_DEPTH * math.sin(2 * math.pi / (STEP_LENGTH * 2) * self.y) - GROUND_DEPTH
        
        elif self.dir == "backward":
            rate = STEP_LENGTH / (self.interpolations - (speed / 100.0 * 18))
            if self.phase == "swing":
                self.y -= rate
                if self.y <= 0:
                    self.y = 0
                    self.phase = "support"
                self.z = STEP_HEIGHT * math.sin(2 * math.pi / (STEP_LENGTH * 2) * self.y) - GROUND_DEPTH
            elif self.phase == "support":
                self.y += rate
                if self.y >= STEP_LENGTH:
                    self.y = STEP_LENGTH
                    self.phase = "swing"
                self.z = -BACK_STEP_DEPTH * math.sin(2 * math.pi / (STEP_LENGTH * 2) * self.y) - GROUND_DEPTH
       
        elif self.dir == "right":
            rate = SIDE_STEP_LENGTH / (self.interpolations - (speed / 100.0 * 18))
            if self.phase == "swing":
                self.x += rate
                if self.x >= SIDE_STEP_LENGTH:
                    self.x = SIDE_STEP_LENGTH
                    self.phase = "support"
                self.z = SIDE_STEP_HEIGHT * math.sin(2 * math.pi / (SIDE_STEP_LENGTH * 2) * self.x) - GROUND_DEPTH
            elif self.phase == "support":
                self.x -= rate
                if self.x <= 0:
                    self.x = 0
                    self.phase = "swing"
                self.z = - SIDE_BACK_STEP_DEPTH * math.sin(2 * math.pi / (STEP_LENGTH * 2) * self.x) - GROUND_DEPTH
        
        elif self.dir == "left":
            rate = SIDE_STEP_LENGTH / (self.interpolations - (speed / 100.0 * 18))
            if self.phase == "swing":
                self.x -= rate
                if self.x <= -SIDE_STEP_LENGTH:
                    self.x = -SIDE_STEP_LENGTH
                    self.phase = "support"
                self.z = SIDE_STEP_HEIGHT * math.sin(2 * math.pi / (SIDE_STEP_LENGTH * 2) * (self.x + SIDE_STEP_LENGTH)) - GROUND_DEPTH
            elif self.phase == "support":
                self.x += rate
                if self.x >= 0:
                    self.x = 0
                    self.phase = "swing"
                self.z = - SIDE_BACK_STEP_DEPTH * math.sin(2 * math.pi / (STEP_LENGTH * 2) * (self.x + SIDE_STEP_LENGTH)) - GROUND_DEPTH
        
    def set_dir(self, dir_val):
        self.dir = dir_val
        if self.dir == "forward" or self.dir == "backward":
            if self.leg == "FR" or self.leg == "BL":
                self.phase = "swing"
                self.x = 0
                self.y = 0
                self.z = 0
            elif self.leg == "FL" or self.leg == "BR":
                self.phase = "support"
                self.x = 0
                self.y = STEP_LENGTH
                self.z = 0
        
        elif self.dir == "right":
            if self.leg == "FR" or self.leg == "BL":
                self.phase = "swing"
                self.x = 0
                self.y = 0
                self.z = 0
            elif self.leg == "FL" or self.leg == "BR":
                self.phase = "support"
                self.x = SIDE_STEP_LENGTH
                self.y = 0
                self.z = 0
                
        elif self.dir == "left":
            if self.leg == "FR" or self.leg == "BL":
                self.phase = "swing"
                self.x = 0
                self.y = 0
                self.z = 0
            elif self.leg == "FL" or self.leg == "BR":
                self.phase = "support"
                self.x = -SIDE_STEP_LENGTH
                self.y = 0
                self.z = 0
      
            
    def get_x(self):
        return self.x
    def get_y(self):
        return self.y
    def get_z(self):
        return self.z
    
    
def solveIK(targetPos, abadPos): 
    x = targetPos[0]-abadPos[0]
    y = targetPos[1]-abadPos[1]
    z = targetPos[2]-abadPos[2]
    l = math.sqrt(x**2 + y**2 + z**2)
    d = math.sqrt(l**2 - L3**2)
    alpha = math.acos((L1**2 + d**2 - L2**2) / (2 * L1 * d))
    beta = math.asin(-y / d)
    theta = math.pi/2 - alpha - beta
    row = math.acos((L1**2 + L2**2 - d**2) / (2 * L1 * L2))
    phi = row - theta
    sigma = math.acos(L3/math.sqrt(x**2 + z**2))
    try:
        omega = sigma - math.atan(-z/abs(x))
    except ZeroDivisionError:
        omega = sigma - math.pi/2
    return [omega, theta, phi] #abad angle, hip angle, knee angle

    
def animate(frame):
    ax.cla()
    ax.set_xlim(-300,300)
    ax.set_ylim(-300,300)
    ax.set_zlim(-300,300)
    ax.set_xlabel('X axis')
    ax.set_ylabel('Y axis')
    ax.set_zlabel('Z axis')
    
    drawBody() 
    FR_trajectory.interpolate(GAIT_SPEED)
    FL_trajectory.interpolate(GAIT_SPEED)
    BR_trajectory.interpolate(GAIT_SPEED)
    BL_trajectory.interpolate(GAIT_SPEED)
    
    FR_footPos = translateTrajectory([FR_trajectory.get_x(), FR_trajectory.get_y(), FR_trajectory.get_z()], "FR")
    FL_footPos = translateTrajectory([FL_trajectory.get_x(), FL_trajectory.get_y(), FL_trajectory.get_z()], "FL")
    BR_footPos = translateTrajectory([BR_trajectory.get_x(), BR_trajectory.get_y(), BR_trajectory.get_z()], "BR")
    BL_footPos = translateTrajectory([BL_trajectory.get_x(), BL_trajectory.get_y(), BL_trajectory.get_z()], "BL")
    
    FR_angles = solveIK(FR_footPos, FR_abadPos)
    FL_angles = solveIK(FL_footPos, FL_abadPos)
    BR_angles = solveIK(BR_footPos, BR_abadPos)
    BL_angles = solveIK(BL_footPos, BL_abadPos)
   
    #forward kinematics to find hip joint pos
    FR_hipPos = [FR_abadPos[0] + L3*math.cos(FR_angles[0]), FR_abadPos[1], FR_abadPos[2] + L3*math.sin(FR_angles[0])]
    FL_hipPos = [FL_abadPos[0] - L3*math.cos(FL_angles[0]), FL_abadPos[1], FL_abadPos[2] + L3*math.sin(FL_angles[0])]
    BR_hipPos = [BR_abadPos[0] + L3*math.cos(BR_angles[0]), BR_abadPos[1], BR_abadPos[2] + L3*math.sin(BR_angles[0])]
    BL_hipPos = [BL_abadPos[0] - L3*math.cos(BL_angles[0]), BL_abadPos[1], BL_abadPos[2] + L3*math.sin(BL_angles[0])]
    
    #draw pelvis (bone that connects abad to hip joint)
    ax.plot([FR_abadPos[0], FR_hipPos[0]], [FR_abadPos[1], FR_hipPos[1]], [FR_abadPos[2], FR_hipPos[2]])
    ax.plot([FL_abadPos[0], FL_hipPos[0]], [FL_abadPos[1], FL_hipPos[1]], [FL_abadPos[2], FL_hipPos[2]])
    ax.plot([BR_abadPos[0], BR_hipPos[0]], [BR_abadPos[1], BR_hipPos[1]], [BR_abadPos[2], BR_hipPos[2]])
    ax.plot([BL_abadPos[0], BL_hipPos[0]], [BL_abadPos[1], BL_hipPos[1]], [BL_abadPos[2], BL_hipPos[2]])
    
    #forward kinematics to find knee joint pos
    FR_kneePos = [FR_hipPos[0] + L1*math.sin(FR_angles[1])*math.sin(FR_angles[0]), FR_hipPos[1] - L1*math.cos(FR_angles[1]), FR_hipPos[2] - L1*math.sin(FR_angles[1])*math.cos(FR_angles[0])]
    FL_kneePos = [FL_hipPos[0] - L1*math.sin(FL_angles[1])*math.sin(FL_angles[0]), FL_hipPos[1] - L1*math.cos(FL_angles[1]), FL_hipPos[2] - L1*math.sin(FL_angles[1])*math.cos(FL_angles[0])]
    BR_kneePos = [BR_hipPos[0] + L1*math.sin(BR_angles[1])*math.sin(BR_angles[0]), BR_hipPos[1] - L1*math.cos(BR_angles[1]), BR_hipPos[2] - L1*math.sin(BR_angles[1])*math.cos(BR_angles[0])]
    BL_kneePos = [BL_hipPos[0] - L1*math.sin(BL_angles[1])*math.sin(BL_angles[0]), BL_hipPos[1] - L1*math.cos(BL_angles[1]), BL_hipPos[2] - L1*math.sin(BL_angles[1])*math.cos(BL_angles[0])]
    
    #draw femur (bone that connects hip to knee joint)
    ax.plot([FR_kneePos[0], FR_hipPos[0]], [FR_kneePos[1], FR_hipPos[1]], [FR_kneePos[2], FR_hipPos[2]])
    ax.plot([FL_kneePos[0], FL_hipPos[0]], [FL_kneePos[1], FL_hipPos[1]], [FL_kneePos[2], FL_hipPos[2]])
    ax.plot([BR_kneePos[0], BR_hipPos[0]], [BR_kneePos[1], BR_hipPos[1]], [BR_kneePos[2], BR_hipPos[2]])
    ax.plot([BL_kneePos[0], BL_hipPos[0]], [BL_kneePos[1], BL_hipPos[1]], [BL_kneePos[2], BL_hipPos[2]])
    
    #draw tibia (bone that connects knee to foot joint)
    ax.plot([FR_kneePos[0], FR_footPos[0]], [FR_kneePos[1], FR_footPos[1]], [FR_kneePos[2], FR_footPos[2]])
    ax.plot([FL_kneePos[0], FL_footPos[0]], [FL_kneePos[1], FL_footPos[1]], [FL_kneePos[2], FL_footPos[2]])
    ax.plot([BR_kneePos[0], BR_footPos[0]], [BR_kneePos[1], BR_footPos[1]], [BR_kneePos[2], BR_footPos[2]])
    ax.plot([BL_kneePos[0], BL_footPos[0]], [BL_kneePos[1], BL_footPos[1]], [BL_kneePos[2], BL_footPos[2]])
    
    #draw test line from origin to foot position
    # ax.plot([0, FR_footPos[0]], [0, FR_footPos[1]], [0, FR_footPos[2]])
    # ax.plot([0, FL_footPos[0]], [0, FL_footPos[1]], [0, FL_footPos[2]])
    # ax.plot([0, BR_footPos[0]], [0, BR_footPos[1]], [0, BR_footPos[2]])
    # ax.plot([0, BL_footPos[0]], [0, BL_footPos[1]], [0, BL_footPos[2]])
    
    
def translateTrajectory(pos, leg):
    if leg == "FR":
        return [pos[0]+FR_abadPos[0]+50, pos[1]+FR_abadPos[1]-STEP_LENGTH/2, pos[2]]
    elif leg == "FL":
        return [pos[0]+FL_abadPos[0]-50, pos[1]+FL_abadPos[1]-STEP_LENGTH/2, pos[2]]
    elif leg == "BR":
        return [pos[0]+BR_abadPos[0]+50, pos[1]+BR_abadPos[1]-STEP_LENGTH/2, pos[2]]
    elif leg == "BL":
        return [pos[0]+BL_abadPos[0]-50, pos[1]+BL_abadPos[1]-STEP_LENGTH/2, pos[2]]
    return

#main program below
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

FR_trajectory = Trajectory("FR")
FL_trajectory = Trajectory("FL")
BR_trajectory = Trajectory("BR")
BL_trajectory = Trajectory("BL")
FR_trajectory.set_dir("left")
FL_trajectory.set_dir("left")
BR_trajectory.set_dir("left")
BL_trajectory.set_dir("left")


ani = animation.FuncAnimation(fig, animate, frames=200, interval=50)

# Show the plot
plt.show()
    
